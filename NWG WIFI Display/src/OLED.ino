#include <Arduino.h>

// xbm image conversion   http://blog.squix.ch/2015/05/esp8266-nodemcu-how-to-create-xbm.html   // https://github.com/squix78/esp8266-oled-ssd1306
void oled_init() {
  display.init();
  display.flipScreenVertically();
  display.setContrast(0x00);
  display.drawXbm(0,  0, NWG_LOGO_width, NWG_LOGO_height, NWG_LOGO_bits);
  display.display();
  display.setContrast(settings.contrast);
  //delay(2000);
}

void oled_displayError(uint8_t err) {
  timeElapsed = settings.refresh - 3000;  // force next refresh interval in 3 seconds.
  display.clear();
  display.drawString(0, 0, String("Error: " + String(err)));
  display.display();
}

#define GFX_MAX_X           128  // OLED max dimensions
#define GFX_MAX_Y           64
#define GFX_CENTER_X        64

#define GFX_T_BOX_X          19  // TOP progress bar
#define GFX_T_BOX_Y          20
#define GFX_B_BOX_X          19  // BOTTOM progress bar
#define GFX_B_BOX_Y          38
#define GFX_BOX_WIDTH       85  // progress bar dimensins
#define GFX_BOX_HEIGHT      12

#define GFX_FOOTER_TIME_X   1   // footer:  round time
#define GFX_FOOTER_Y        54
#define GFX_FACTION_LABEL_X 1

#define GFX_CIRCLE_RADIUS   7  // for mcom/flag icons


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
  /*  strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_MODES[game.mode])));
  String strFooter = String(str_buffer) + " | " + String((int)game.players) + " | " + String(game.roundTime / 60) + "m";
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(GFX_FOOTER_TIME_X, GFX_FOOTER_Y, strFooter);*/

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

  // ... team stats comparison (kill difference & average rank)
  // NEED TO CHECK THAT WE HAVE VALID GAME MODE GOING??
/*
  int killDiff = game.stats.killSum.T1 - game.stats.killSum.T2;
  int rankDiff = ((double)game.stats.rankSum.T1 / (double)game.stats.playerCount.T1) - ((double)game.stats.rankSum.T2 / (double)game.stats.playerCount.T2);
  String str = "K";
  if (killDiff > 0)
    str += "+" + String(killDiff);
  else if (killDiff = 0)
    str += "=";
  else
    str += String(killDiff);
  str += " R";

  if (rankDiff > 0)
    str += "+" + String(rankDiff);
  else if (rankDiff = 0)
    str += "=";
  else
    str += String(rankDiff);

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(GFX_MAX_X, GFX_FOOTER_Y, str);

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);    // NOT NEEDED?*/

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
// TODO:  MOVE THE FACTION PRINTS OUT TO BE COMMON
void displayGameData(int statusCode) {
  timeElapsed = 0;
  display.clear();

  if (statusCode == STATUS_JSON_SUCCESS) {  // if obtained valid BF4 round information from battlelog...
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
      oled_displayError(statusCode);
}
