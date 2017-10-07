#include <Arduino.h>
#include "includes/OLED.h"

Demo frames[] = {draw_bf4_data, draw_bf4_stats, draw_weather_current, draw_weather_forecast};

void oled_init() {
  display.init();
  display.flipScreenVertically();
  display.setContrast(0x00);
  display.drawXbm(0,  0, NWG_LOGO_width, NWG_LOGO_height, NWG_LOGO_bits);
  display.display();
  display.setContrast(cfg_n.contrast);
}

void draw_bf4_stats () {
  display.clear();
  char faction[3] = {' ', ' ', '\0'};

  strcpy_P(faction, (char *)pgm_read_dword(&(STR_TABLE_FACTIONS[game.faction.T1])));
  display.drawString(60, 0, faction);
  strcpy_P(faction, (char *)pgm_read_dword(&(STR_TABLE_FACTIONS[game.faction.T2])));
  display.drawString(100, 0, faction);

  display.drawString(0, 16, "Kills");
  display.drawString(0, 32, "K/D (avg)");
  display.drawString(0, 48, "Rank (avg)");

  display.drawHorizontalLine(0,11,128);
  display.drawString((stats.kills.team == TEAM_1 ? 60 : 100), 16, "+" + String(abs(stats.kills.val)));
  if(stats.kd.team != TEAM_NA)     display.drawString((stats.kd.team == TEAM_1 ? 60 : 100), 32, "+" + String(stats.kd.val));
  if(stats.rank.team != TEAM_NA)   display.drawString((stats.rank.team == TEAM_1 ? 60 : 100), 48, "+" + String(stats.rank.val));
  display.display();
}

void draw_weather_current() {
  int x,y=10;
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(50 + x, 5 + y, weather.now.weather); //display.drawString(60 + x, 5 + y, weather.now.icon); //wunderground.getWeatherText()

  display.setFont(ArialMT_Plain_24);
  String temp;
  if(cfg_n.weather.isMetric) {  //if(weather.isMetric) {
    temp = String(fToC(weather.now.feelslike_f)) + "°C";
  } else {
    temp = String(weather.now.feelslike_f) + "°F";
  }
  display.drawString(50 + x, 15 + y, temp);
  int tempWidth = display.getStringWidth(temp);

  display.setFont(Meteocons_Plain_42);
  String weatherIcon = getMeteoconIcon(weather.now.icon);
  int weatherIconWidth = display.getStringWidth(weatherIcon);
  display.drawString(22 + x - weatherIconWidth / 2, 05 + y, weatherIcon);
  display.display();
  display.setFont(ArialMT_Plain_10);
}

void draw_weather_forecast() {
  int x=0;
  int y=4;
  display.clear();
  drawForecastDetails(x, y, 0);
  drawForecastDetails(x + 44, y, 1);
  drawForecastDetails(x + 88, y, 2);

  // draw footer
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);   //display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawHorizontalLine(0, 51, 128);
  String tempNow;
  if(cfg_n.weather.isMetric) {//if(weather.isMetric) {
    tempNow = String(fToC(weather.now.feelslike_f)) + " °C";
  } else {
    tempNow = String(weather.now.feelslike_f) + " °F";
  }
  display.drawString(128, 52, tempNow);
  int tempNowLength = display.getStringWidth(tempNow);

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(1, 52, String(weather.now.relative_humidity) + " %RH");

  display.setTextAlignment(TEXT_ALIGN_CENTER); // was center
  display.setFont(Meteocons_Plain_10);
  String weatherIcon = getMeteoconIcon(weather.now.icon);
  int weatherIconWidth = display.getStringWidth(weatherIcon);
  display.drawString(128-tempNowLength-weatherIconWidth-2, 53, weatherIcon); // x was 77

  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.display();
}

void drawForecastDetails(int x, int y, int dayIndex) {
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  String day = weather.forecast.day[dayIndex];  //wunderground.getForecastTitle(dayIndex).substring(0, 3);
  day.toUpperCase();
  display.drawString(x + 20, y, day);

  display.setFont(Meteocons_Plain_21);
  display.drawString(x + 20, y + 12, getMeteoconIcon(weather.forecast.icon[dayIndex]) ); //wunderground.getForecastIcon(dayIndex)

  display.setFont(ArialMT_Plain_10);
  String hiLo;
  if(cfg_n.weather.isMetric) {//if(weather.isMetric) {
    hiLo = String(fToC(weather.forecast.high_f[dayIndex])) + "|" + String(fToC(weather.forecast.low_f[dayIndex]));
  } else {
    hiLo = String(weather.forecast.high_f[dayIndex]) + "|" + String(weather.forecast.low_f[dayIndex]);
  }
  display.drawString(x + 20, y + 34, hiLo);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void oled_displayError(uint8_t err) {
  timeElapsed_BF4 = cfg_n.bf4.refresh - 3000; //settings.refresh - 3000;  // force next refresh interval in 3 seconds.
  display.clear();
  display.drawString(0, 0, String("Error: " + String(err)));
  display.display();
}

void oled_FW_update_msg() {
  display.clear();
  display.drawStringMaxWidth(1, 1, 128, F("Updating firmware."));
  display.drawStringMaxWidth(1, 20, 128, F("Do NOT power off device!"));
  display.display();
}
void oled_display_msg(String msg, int delayMS) {
  display.clear();
  display.drawStringMaxWidth(1, 1, 128, msg);
  delay(delayMS);
}

void oled_draw_noWiFiNetwork(String pSSID) {
  display.clear();
  display.drawStringMaxWidth(1, 1, 128, F("Failed to join WiFi network, please configure WiFi settings. Join WiFi AP: "));
  display.drawString(10, 40, pSSID);
  display.drawString(1, 52, F("Address: 192.168.4.1"));
  display.display();
}

//  display FOOTER: time, mode, # players
// todo: OPTMIZE font change commands
void draw_headerFooter() {
  // HEADER
  // ... map
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Dialog_plain_12);   // not needed?
  strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_MAPS[game.map])));
  display.drawString(64, 1, str_buffer);    //Serial.print("Map="); Serial.println(str_buffer);
  display.setFont(ArialMT_Plain_10);

  // FOOTER
  // ... time ...
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(GFX_FOOTER_TIME_X, GFX_FOOTER_Y, (game.roundTime % 60 < 10) ? (String((game.roundTime / 60)) + ":0" + String((game.roundTime % 60))) : String((game.roundTime / 60)) + ":" + String((game.roundTime % 60))); // was 35
  // ... mode ...
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_MODES[game.mode])));
  display.drawString(64,  GFX_FOOTER_Y,  str_buffer);

  // ... # players / queued ...
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  if ((int)game.playersQueue > 0)
      display.drawString(GFX_MAX_X, GFX_FOOTER_Y, String(String((int)game.players) + "[" + String((int)game.playersQueue) + "]"));    // was 127
  else
      display.drawString(GFX_MAX_X, GFX_FOOTER_Y, String((int)game.players));

  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void draw_factionLabels() {
  display.setTextAlignment(TEXT_ALIGN_LEFT);   // not needed?
  // team 1
  strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_FACTIONS[game.faction.T1])));
  display.drawString(GFX_FACTION_LABEL_X,  GFX_T_BOX_Y,  str_buffer);  // was 2 23
  // team 2
  strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_FACTIONS[game.faction.T2])));
  display.drawString(GFX_FACTION_LABEL_X,  GFX_B_BOX_Y,  str_buffer);  // was 2 38
}

void dispCQ() {   // tickets/ticketsmax
  display.setTextAlignment(TEXT_ALIGN_RIGHT);

  // team 1
  display.drawProgressBar(GFX_T_BOX_X, GFX_T_BOX_Y, GFX_BOX_WIDTH,  GFX_BOX_HEIGHT, (game.score.T1 * 100.0 / game.scoreMax.T1));   // was 30,25,95,8, then 20 25 80 8
  display.drawString(GFX_MAX_X-1, GFX_T_BOX_Y, String(game.score.T1)); // was 127 23

  // team 2
  display.drawProgressBar(GFX_B_BOX_X, GFX_B_BOX_Y, GFX_BOX_WIDTH,  GFX_BOX_HEIGHT, (game.score.T2 * 100.0 / game.scoreMax.T2));   // was 30, 40, 95, 8, then 20 40 80 8
  display.drawString(GFX_MAX_X-1, GFX_B_BOX_Y, String(game.score.T2)); // was 127 38
}


void dispCTF() { // flags/flagsMax
  display.setTextAlignment(TEXT_ALIGN_CENTER);

  display.drawString(GFX_CENTER_X, GFX_T_BOX_Y, String(String(game.score.T1) + " / " + String(game.scoreMax.T1)));
  display.drawString(GFX_CENTER_X, GFX_B_BOX_Y, String(String(game.score.T2) + " / " + String(game.scoreMax.T2)));
}


void dispCA() {   // destroyedCrates (score) / carrierHealth (scoreMax)
  display.setTextAlignment(TEXT_ALIGN_RIGHT);

  // team 1
  display.drawProgressBar(GFX_T_BOX_X, GFX_T_BOX_Y, GFX_BOX_WIDTH, GFX_BOX_HEIGHT, (game.scoreMax.T1));
  display.drawString(GFX_MAX_X, GFX_T_BOX_Y, String(2 - game.score.T1) + "/2");   // show # of MCOMs remaining (instead of # destroyed)

  // team 2
  display.drawProgressBar(GFX_B_BOX_X, GFX_B_BOX_Y, GFX_BOX_WIDTH, GFX_BOX_HEIGHT, (game.scoreMax.T2));
  display.drawString(GFX_MAX_X, GFX_B_BOX_Y, String(2 - game.score.T2) + "/2");  // show # of MCOMs remaining (instead of # destroyed)
}


void dispR() {     // bases/basesMax & tickets/ticketsMax
// if 'rush' JSON object was returned
if(game.rushStarted) {
  // team 1 (defenders) - MCOM symbols
  for (int i=0; i < game.scoreMax.T2;  i++) {
      int x = 20 + GFX_CIRCLE_RADIUS + (i * GFX_CIRCLE_RADIUS*2.8);
      int y = GFX_B_BOX_Y + GFX_CIRCLE_RADIUS*0.9;

        // TODO: FIGURE OUT WHY CAN'T DRAW PERFECTLY OVERLAPPING RECTANGLE OVER CIRCLE. EITHER SLIGTLY TOO SHORT OR TOO LONG. SOMETHING TO DO W/ OFFSET LOSS OF PRECISION?
        if(game.scoreMax.T2 - i <= game.score.T2) {
            display.drawCircle(x, y, GFX_CIRCLE_RADIUS);
            display.setTextAlignment(TEXT_ALIGN_CENTER);
            display.drawString(x, GFX_B_BOX_Y - GFX_CIRCLE_RADIUS/4+1, "M");
        } else {
          display.fillCircle(x, y, GFX_CIRCLE_RADIUS);
        }
    }

    // team 2 (attackers) - progress bar
    display.drawProgressBar(GFX_T_BOX_X, GFX_T_BOX_Y, GFX_BOX_WIDTH, GFX_BOX_HEIGHT, (game.score.T1 * 100.0 / game.scoreMax.T1));   // was 20 25 85 8
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(GFX_MAX_X, GFX_T_BOX_Y, String(game.score.T1));   // was 128 23
  }
}

void dispDM() {   // kills/killsMax
  display.setTextAlignment(TEXT_ALIGN_RIGHT);

  // team 1
  display.drawProgressBar(GFX_T_BOX_X, GFX_T_BOX_Y, GFX_BOX_WIDTH, GFX_BOX_HEIGHT, (game.score.T1 * 100.0 / game.scoreMax.T1)); // was 20 25 85 8
  display.drawString(GFX_MAX_X, GFX_T_BOX_Y, String(game.score.T1)); // was 128 23

  // team 2
  display.drawProgressBar(GFX_B_BOX_X, GFX_B_BOX_Y, GFX_BOX_WIDTH, GFX_BOX_HEIGHT, (game.score.T2 * 100.0 / game.scoreMax.T2));   // was 20 40 85 8
  display.drawString(GFX_MAX_X, GFX_B_BOX_Y, String(game.score.T2));   // was 128 38
}


// display Game Data (mode specific)
void draw_bf4_data() {
  display.clear();

  if (game.return_status == STATUS_JSON_SUCCESS) {  // if obtained valid BF4 round information from battlelog...
    // draw map, mode, time, players
    draw_headerFooter();

    // if round not started...
    if ((game.scoreMax.T1 == 0 && game.scoreMax.T2 == 0) || (game.mode == MODE_R && !game.rushStarted)) {   // need to take closer look. this doesnt look right...
      if (game.roundTime == 0)
        display.drawString(20, 30, "Round not started");
      else
        display.drawString(20, 30, "Round over");

      display.display();
      return;
    }

    // draw faction labels
    draw_factionLabels();

    // .. otherwise display mode-specific info
    if (game.mode == MODE_OB || game.mode == MODE_CQ || game.mode == MODE_AS || game.mode == MODE_DOM || game.mode == MODE_CL)  dispCQ();
    else if (game.mode == MODE_CTF) dispCTF();
    else if (game.mode == MODE_CA)  dispCA();
    else if (game.mode == MODE_R)   dispR();
    else if (game.mode == MODE_TDM || game.mode == MODE_SD)     dispDM();
    else if (game.mode == MODE_GM)  display.drawString(64, 30, "GM unsupported");
    else if (game.mode == MODE_DE)  display.drawString(64, 30, "DEFUSE unsupported");
    display.display();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
  } else
      oled_displayError(game.return_status);
}

// convert fahrenheit to celcius
int fToC(double f) {
  return (f - 32) * (5.0/9);
}
