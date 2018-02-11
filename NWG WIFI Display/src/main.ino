#include "includes/includes.h"

ADC_MODE(ADC_VCC); // added for v1.2.01 (5/18/17) for diagnostics page  //#include "user_interface.h"

// ESP8266-12E NWG PCB:
SSD1306   display(0x3C, 14, 12);
OLEDDisplayUi ui     ( &display );

char str_buffer[STRING_TABLE_BUFFER_SIZE];
// Timers ...
elapsedMillis timeElapsed_BF4;      // Bf4 refresh interval
elapsedMillis timeElapsed_BF1;      // BF1 refresh interval
elapsedMillis timeElapsed_weather;  // weather refresh interval
elapsedMillis timeElapsed_fwAutoUpdateCheck = 0; // check for new FW version (if setting enabled)
elapsedMillis t_frame = 0;

// Global data ...
CFG_NEW cfg_n { {"5c154149-dd96-4001-8b19-bd2197782f37", 30000, false},  {false, AUTO, "00000.29.WCXMP"},  125, 5000, B01011001, BUILD_TIME};   // default config
GameData game;  //BF4 game info
Stats_Delta stats;  // BF4 game stats
BF1_GAME_DATA BF1_GAME; // BF1 game data
Weather weather;  // Weather    // TODO: LEGACY (DELETE) ??
int curFrm = -1; // 0; // current Widget frame displayed

void setup(void) {
  pinMode(13, INPUT); // used as input to safe mode. short "SW" pins on PCB

  Serial.begin(115200);  Serial.println("\nBooting Sketch...");
  oled_init();

  EEPROM_loadSettings();
  WiFiManager wifiManager;  // connect to WiFi network
  wifiManager.setAPCallback(configModeCallback);  // set callback to update screen w/ instructions on failed auto connect
  wifiManager.autoConnect("NWG Server");
  HTTP_server_init(); // start HTTP server

  // safe-mode
  if(!digitalRead(13))
    safeMode();

  display.clear(); display.drawString(0, 0, "Config IP:"); display.drawString(0, 20, WiFi.localIP().toString()); display.display();

  // force first FW auto-update check (if enabled) in 5 minutes
  if(_MB(M_FW_AUTOUPDATE_EN))
    timeElapsed_fwAutoUpdateCheck = REFRESH_FW_AUTO_UPDATE - 300000;
}

void safeMode() {
  oled_display_msg("SAFE MODE", 100);
  while(1) {
    server.handleClient();
    delay(100);
  }
}
void configModeCallback (WiFiManager *myWiFiManager) {
  oled_draw_noWiFiNetwork(myWiFiManager->getConfigPortalSSID());
}

// checks whether target frame # is enabled
bool isFrameEn(int fNum) {
  switch(fNum) {
    case 0:   return (_MB(M_DSP_BF4_AUTO_HIDE) && _MB(M_DSP_BF4_GAME_EN)  && game.players == 0) ? false : _MB(M_DSP_BF4_GAME_EN);         //if (_MB(M_DSP_BF4_AUTO_HIDE) && _MB(M_DSP_BF4_GAME_EN) && game.players == 0) return false;         //else return _MB(M_DSP_BF4_GAME_EN);
    case 1:   return (_MB(M_DSP_BF4_AUTO_HIDE) && _MB(M_DSP_BF4_STATS_EN) && game.players == 0) ? false : _MB(M_DSP_BF4_STATS_EN);        //if (_MB(M_DSP_BF4_AUTO_HIDE) && _MB(M_DSP_BF4_STATS_EN) && game.players == 0) return false;          //else return _MB(M_DSP_BF4_STATS_EN);
    case 2:   return _MB(M_DSP_WEATHER_CUR_EN);
    case 3:   return _MB(M_DSP_WEATHER_FRCST_EN);
    case 4:   return _MB(M_DSP_BF1);
    default:  return false;
  }
}

void loop() {

  // BF4 - refresh data
  if ((_MB(M_DSP_BF4_GAME_EN) || _MB(M_DSP_BF4_STATS_EN)) && timeElapsed_BF4 > cfg_n.bf4.refresh) {
      bf4_update();
  }
  // Weather - refresh data
  if ((_MB(M_DSP_WEATHER_CUR_EN) || _MB(M_DSP_WEATHER_FRCST_EN)) && timeElapsed_weather > REFRESH_WEATHER) {
      WundergroundClient wunderground = WundergroundClient(&weather, &timeElapsed_weather);
      wunderground.update();
  }

  // BF1 - refresh data
  if (_MB(M_DSP_BF1) &&  timeElapsed_BF1 > REFRESH_BF1) {
    parser_BF1 bf1 = parser_BF1(&BF1_GAME, &timeElapsed_BF1);
    bf1.update();

  }

  // Auto FW update check every 24 hrs (if enabled), and w/in first 5 min of power-up.
  if(_MB(M_FW_AUTOUPDATE_EN) && timeElapsed_fwAutoUpdateCheck >= REFRESH_FW_AUTO_UPDATE) { if (firmware_update_check()) {firmware_update();}   timeElapsed_fwAutoUpdateCheck = 0; }


  // progress to next display frame
  bool newFrameAvail = false;
  int checks = FRAME_COUNT*2;
  if (t_frame >= cfg_n.scrollSpeed) {
    do {
      curFrm = (curFrm >= FRAME_COUNT) ? 0 : curFrm + 1;
      if(isFrameEn(curFrm))
          newFrameAvail = true;
      if (--checks <= 0) {  // prevent infinite loop
          oled_display_msg("No widget data found", 1000);
          break;
      }

    } while (!isFrameEn(curFrm));
    if(newFrameAvail) {
      if (curFrm == 0)      frames[0]();  // draw_bf4_data()
      else if (curFrm == 1) frames[1]();  // draw_bf4_stats()
      else if (curFrm == 2) frames[2]();  // draw_weather_current()
      else if (curFrm == 3) frames[3]();  // draw_weather_forecast()
      else if (curFrm == 4) frames[4]();  // draw_BF1()
      t_frame = 0;
    }
  }

  delay(100);
  server.handleClient();
}


void EEPROM_loadSettings() {
  // load EEPROM config into temporary struct and only assign to global CFG if FW build time matches (i.e.: structure of CFG is unchanged in code)
  EEPROM.begin(sizeof(CFG_NEW));
  CFG_NEW tmpCFG;
  EEPROM.get(0, tmpCFG);
  // FW build time matches; use EEPROM-stored settings
  if((unsigned long)tmpCFG.eeprom_build_time == (unsigned long)BUILD_TIME) {
      cfg_n = tmpCFG; // shallow copy, ok???
  }
  display.setContrast(cfg_n.contrast);
  timeElapsed_BF4 = cfg_n.bf4.refresh; // force immediate BF4 data refresh.
  timeElapsed_weather = REFRESH_WEATHER;
  timeElapsed_BF1 = REFRESH_BF1;
  EEPROM.end();
}
  // weather improve: https://github.com/squix78/esp8266-weather-station/tree/master/examples/WeatherStationDemoExtendedDST
