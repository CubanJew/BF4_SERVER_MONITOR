#include "includes/includes.h"

ADC_MODE(ADC_VCC); // added for v1.2.01 (5/18/17) for diagnostics page  //#include "user_interface.h"

// ESP8266-12E NWG PCB:
SSD1306   display(0x3C, 14, 12);
OLEDDisplayUi ui     ( &display );

char str_buffer[STRING_TABLE_BUFFER_SIZE];
// Timers ...
elapsedMillis timeElapsed_BF4;  // Bf4 refresh interval
elapsedMillis timeElapsed_weather = REFRESH_WEATHER;
elapsedMillis timeElapsed_fwAutoUpdateCheck = 0; // check for new FW version (if setting enabled)
elapsedMillis t_frame = 0;

// Global data ...
CFG_NEW cfg_n { {"5c154149-dd96-4001-8b19-bd2197782f37", 30000, false},  {false, AUTO, "00000.29.WCXMP"},  235, 5000, B00010011, BUILD_TIME};   // default config
GameData game;  //BF4 game info
Stats_Delta stats;  // BF4 game stats
Weather weather;  // Weather    // TODO: LEGACY (DELETE) ??
int curFrm = 0; // current Widget frame displayed

/*
TODO BEFORE RELEASE 1.2.3:
  3. Add rain prediction to current conditions & forecast screen

Other:
  7. COMBINE UPDATE REQUESTS TO ECONOMIZE API CALLS: https://www.wunderground.com/weather/api/d/docs?d=data/index
  8. Verify WUNDERGROUND_APIKEY_TABLE actually in PROGMEM


*************************
Finished:
  - Save config to EEPROM
  - Weather config save page - handle saving
  - Weather config page - handle loading values
  - wunderground API key bank (to roll through)
  - Clean-up old settings (delete)
  - Implement BF4 auto-hide if server empty setting
*/
void setup(void) {
  Serial.begin(115200);  Serial.println("\nBooting Sketch...");
  oled_init();
  EEPROM_loadSettings();
  WiFiManager wifiManager;  // connect to WiFi network
  wifiManager.setAPCallback(configModeCallback);  // set callback to update screen w/ instructions on failed auto connect
  wifiManager.autoConnect("NWG Server");
  HTTP_server_init(); // start HTTP server
  display.clear(); display.drawString(0, 0, "Config IP:"); display.drawString(0, 20, WiFi.localIP().toString()); display.display();

  // force first FW auto-update check (if enabled) in 5 minutes
  if(_MB(M_FW_AUTOUPDATE_EN))
    timeElapsed_fwAutoUpdateCheck = REFRESH_FW_AUTO_UPDATE - 300000;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  oled_draw_noWiFiNetwork(myWiFiManager->getConfigPortalSSID());
}

// checks whether target frame # is enabled   // TODO: ignore BF4 auto hide option if both weather widgets disabled
bool isFrameEn(int fNum) {
  switch(fNum) {
    case 0:   return (_MB(M_DSP_BF4_AUTO_HIDE) && _MB(M_DSP_BF4_GAME_EN)  && game.players == 0) ? false : _MB(M_DSP_BF4_GAME_EN);         //if (_MB(M_DSP_BF4_AUTO_HIDE) && _MB(M_DSP_BF4_GAME_EN) && game.players == 0) return false;         //else return _MB(M_DSP_BF4_GAME_EN);
    case 1:   return (_MB(M_DSP_BF4_AUTO_HIDE) && _MB(M_DSP_BF4_STATS_EN) && game.players == 0) ? false : _MB(M_DSP_BF4_STATS_EN);        //if (_MB(M_DSP_BF4_AUTO_HIDE) && _MB(M_DSP_BF4_STATS_EN) && game.players == 0) return false;          //else return _MB(M_DSP_BF4_STATS_EN);
    case 2:   return _MB(M_DSP_WEATHER_CUR_EN);
    case 3:   return _MB(M_DSP_WEATHER_FRCST_EN);
    default:  return false;
  }
}

void loop() {
  // TODO: ??? WHY CHANGING FRAMES AROUND??? check initial conditions
  // refresh BF4 data
  if ((_MB(B_DSP_BF4_GAME_EN) || _MB(B_DSP_BF4_STATS_EN)) && timeElapsed_BF4 > cfg_n.bf4.refresh) {                //displayGameData((int)fetch_json());
      bf4_update();     //curFrm=-1; //t_frame=cfg_n.scrollSpeed;
  }
  // refresh weather data
  if ((_MB(B_DSP_WEATHER_CUR_EN) || _MB(B_DSP_WEATHER_FRCST_EN)) && timeElapsed_weather > REFRESH_WEATHER) {
      weather_update();
  }
  // Auto FW update check every 24 hrs (if enabled), and w/in first 5 min of power-up.
  if(_MB(M_FW_AUTOUPDATE_EN) && timeElapsed_fwAutoUpdateCheck >= REFRESH_FW_AUTO_UPDATE) { if (firmware_update_check()) {firmware_update();}   timeElapsed_fwAutoUpdateCheck = 0; }

  // progress to next display frame
  if (t_frame >= cfg_n.scrollSpeed) {
    do {
      curFrm = (curFrm >= FRAME_COUNT) ? 0 : curFrm + 1;
    } while (!isFrameEn(curFrm));
    if (curFrm == 0)      frames[0]();
    else if (curFrm == 1) frames[1]();
    else if (curFrm == 2) frames[2]();
    else if (curFrm == 3) frames[3]();
    t_frame = 0;
  }

  delay(100);   // added 10/5/16
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
  timeElapsed_BF4 = cfg_n.bf4.refresh; // force immediate BF4 data refresh
  EEPROM.end();
}
  // weather improve: https://github.com/squix78/esp8266-weather-station/tree/master/examples/WeatherStationDemoExtendedDST
