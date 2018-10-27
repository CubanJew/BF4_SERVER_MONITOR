#include "includes/HTTP_SERVER.h"

const char FIRMWARE_VERSION[] PROGMEM   = "1.2.32"; // formerly w/out the static

// root:  /updateInt, /contrast
static const char SERVER_PAGE_OTA[] PROGMEM    = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";      //const char* SERVER_PAGE_OTA = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

static const char SERVER_PAGE_ROOT_1[] PROGMEM = "<head><meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='cache-control' content='no-cache' /></head><h1> BF4 SERVER MONITOR CONFIGURATION</h1><br><form action='/save' method='POST'>";
static const char SERVER_PAGE_ROOT_2[] PROGMEM = "<b>Refresh Interval:</b> &nbsp; <input type='text' name='updateInt' value='";
static const char SERVER_PAGE_ROOT_3[] PROGMEM = "' maxlength='3' size='5'>&nbsp;<i>seconds</i><br> <b>Display Contrast:</b> &nbsp; <input type='text' name='contrast' value='";
static const char SERVER_PAGE_ROOT_4[] PROGMEM = "' maxlength='3' size='5'>&nbsp;<i>%</i><br><input type='submit' value='Save Settings'><br> </form><br><a href='/advanced'><button>Advanced Settings</button></a> <br>FW v";

static const char SERVER_PAGE_SAVE_ERROR[] PROGMEM      = "Invalid setting values! \n\t- Update interval must be >= 30 seconds.\n\t= Contrast must be 1-100%";  // \n\t- Server ID must be exactly 32 characters.
static const char SERVER_PAGE_SAVE_FORM_ERROR[] PROGMEM = "Form submit error";

// TODO: INEFFICIENT DUPLICATION
const char SERVER_HTTP_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>Advanced Configuration</title>";
static const char SERVER_HTTP_STYLE[] PROGMEM           = "<style>.c{text-align: center;} div,input{padding:5px;} input[type='submit']{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
static const char SERVER_HTTP_HEAD_END[] PROGMEM        = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";

ESP8266WebServer server ( 80 );

// ROOT (NEW) CONSTANTS

//static const char PAGE_ROOT_NEW_1[] PROGMEM = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='cache-control' content='no-cache'/></head><style>.floating-box{display: inline-block;margin:10px;border:3px solid #73AD21;width:300px;}.st{display:inline-block;width:170px;}</style><h1>NWG WIFI MONITOR</h1><br><form action='/root_save' method='POST'><div class='floating-box'><b><u>Global Settings</u></b><br><div class='st'>Display Contrast:</div>";
//static const char PAGE_ROOT_NEW_3[] PROGMEM = "&nbsp; seconds&nbsp;&nbsp;[1-999]<br><a href='/edit-bf4'>Customize BF4</a><br><a href='weather-edit'>Customize Weather</a><br></div><br><input type='submit' value='Save Settings'><br></form><a href='/advanced'>Admin Tools/Help</a><br><h5>FW ";
//static const char PAGE_ROOT_NEW_5[] PROGMEM = "</h5></html>";
//BF4 WIDGET CONFIG
//static const char PAGE_BF4_1[] PROGMEM = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='cache-control' content='no-cache' /></head><h1> BF4 Widget - Config</h1><br><form action='/save-bf4' method='POST'><b><u>BF4 Widget Settings</u></b><br>";
//static const char PAGE_BF4_3[] PROGMEM = "<br><br><input type='submit' value='Save Settings'><br></form></html>";


// VALID constants:....
static const char SERVER_PAGE_SAVE_SUCCESS[] PROGMEM    = "<meta http-equiv='refresh' content='1;url=/' />Settings saved! <br><br> Redirecting in 1 second...";

// -------- LONG STRING LITERALS: http://www.esp8266.com/viewtopic.php?f=29&t=2417 CHECK OUT!!!!    http://www.esp8266.com/viewtopic.php?f=28&t=5549

// SAVE SETTINGS INTO EEPROM
void eeprom_saveCfg() {
  EEPROM.begin(sizeof(CFG_NEW));
  EEPROM.put(0,cfg_n);
  EEPROM.commit();
  EEPROM.end();
}

// -----------------------------
//        HTTP INITIALIZATION
// -----------------------------
void HTTP_server_init() {
  server.on("/", h_root_new);
  server.on("/_save", h_root_new_save);
  server.on("/bf4-edit", h_bf4_config);
  server.on("/bf4-save", h_bf4_config_save);
  server.on("/weather-edit", h_weather);
  server.on("/weather-save", h_weather_save);
  server.on("/admin", h_admin);
  server.on("/diag", h_diagnostics);
  server.on("/fw_check", handle_test_fw_check);
  server.on("/test", h_test);
  server.on("/fw_update", handle_test_http_fw_update);

  server.on("/OTA", HTTP_GET, []() {
    // set all timers to 0 incase theres some sort of critical bug, so user can quickly re-flash upon boot
    timeElapsed_BF4=0;
    timeElapsed_weather=0;
    timeElapsed_fwAutoUpdateCheck= 0;

    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send_P(200, "text/html", SERVER_PAGE_OTA);
  });

  server.on("/update", HTTP_POST, []() {
    oled_FW_update_msg();
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");    //erver.send_P(200, "text/text", (Update.hasError()) ? SERVER_PAGE_OTA_FAIL : SERVER_PAGE_OTA_SUCCESS);
    ESP.restart();
  }, []() {
    oled_FW_update_msg();
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });

  server.onNotFound([]() {server.send(200, "text/plain", F("Page not found."));  });
  server.begin();
}

// SEE CHEAT SHEET: https://woodsgood.ca/projects/2015/02/06/accessing-arduino-program-memory/
void h_test() {

//  int num = random(0,3);
   //char buf[17];
   //strcpy_P(buf,	(char *)pgm_read_dword(WUNDERGROUND_APIKEY_1));
   //buf[16] = '\0';
   //  strcpy_P(faction, (char *)pgm_read_dword(&(STR_TABLE_FACTIONS[game.faction.T2])));

    //String s = String(buf.c_str());

    unsigned long hello = BUILD_TIME;
    server.send(200, "text/text", String(hello));


/*
    char mapName[20];// = {' ', ' ', '\0'};
    strcpy_P(mapName, (char *)pgm_read_dword(&(STR_TABLE_BF1_MAPS[BF1_GAME.map])));
    mapName[19] = '\0';

    String s = String(BF1_GAME.map) + "..." + String(BF1_GAME.players) + "..." + String(BF1_GAME.playersMax) + "..." + String(BF1_GAME.queue) + "..." + String(BF1_GAME.mode) + "...";
    s += mapName;
    strcpy_P(mapName, (char *)pgm_read_dword(&(STR_TABLE_BF1_MODES[BF1_GAME.mode])));
    s += mapName;
    server.send(200, "text/text", s);
    */
    //server.send(200, "text/text", String(hello));


  //  String s = BUILD_TIME;
  //server.send(200, "text/text", BUILD_TIME);
   //server.send_P(200, "text/text", buf);

   //char data = pgm_rear_word(&(WUNDERGROUND_APIKEY_TABLE[num]));
  //server.send(200, "text/text", num + String("\n\n") + buffer.c_str());
}

String cb2varS(bool b) {
  if(b)  return "checked";
  return "";
}
// -----------------------------
//        PAGE - ROOT (NEW)
//      memory cost: ~600bytes
// -----------------------------
void h_root_new() {
  //uint32_t memStart =  system_get_free_heap_size();uint32_t memEnd;unsigned long tStart, tEnd;tStart = millis();
  //TODO: FIX: input['submit'] {width:90%;}

 /*
 String str_frm = F("<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='cache-control' content='no-cache'/></head><style>.floating-box{display: inline-block;margin:10px;border:3px solid #73AD21;width:300px;}.st{display:inline-block;width:170px;}.input[type='submit']{width:95%;}</style><h1>NWG WIFI MONITOR</h1><br><form action='/_save' method='POST'><div class='floating-box'><b><u>Global Settings</u></b><br><div class='st'>Display Contrast:</div><input type='text' name='contrast' value='%vCnst%' maxlength='3' size='1'>&nbsp;% [1-100]<div class='st'>Firmware Auto-Update:</div><input type='checkbox' name='fwAU' %efwAU%></div><br><div class='floating-box'><b><u>Enabled Widgets</u></b><br><input type='checkbox' name='e_BF4_sb' %ebf4sb%>BF4 Scoreboard<br><input type='checkbox' name='e_BF4_st' %ebf4st%>BF4 Stats<br><input type='checkbox' name='e_BF1' %ebf1%>BF1 Server Info<br><input type='checkbox' name='e_w-c' %ewc%>Weather - Now<br><input type='checkbox' name='e_w-f' %ewf%>Weather - Forecast<br><br>Scroll Speed:&nbsp; <input type='textbox' value='%vSS%' name='SS' maxlength='3' size='1'>&nbsp; seconds&nbsp;&nbsp;[1-999]<br><a href='/bf4-edit'>Customize BF4</a><br><a href='weather-edit'>Customize Weather</a><br></div><br><input type='submit' value='Save Settings'><br></form><a href='/admin'>Admin Tools/Help</a><br><h5>FW %fwF%</h5></html>");
*/

String str_frm = F("<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='cache-control' content='no-cache'/></head><style>.floating-box{display: inline-block;margin:10px;border:3px solid #73AD21;width:300px;}.st{display:inline-block;width:170px;} .input[type='submit']{width:95%;} .wh{padding-bottom:10px;}</style> <h1>NWG WIFI MONITOR</h1><br><form action='/_save' method='POST'><div class='floating-box'><b><u>Global Settings</u></b><br><div class='st'>Display Contrast:</div><input type='text' name='contrast' value='%vCnst%' maxlength='3' size='1'>&nbsp;% [1-100]<div class='st'>Firmware Auto-Update:</div><input type='checkbox' name='fwAU' %efwAU%><div class='st'>Widget Scroll Speed:</div><input type='textbox' value='%vSS%' name='SS' maxlength='3' size='1'>&nbsp; sec.</div><br><div class='floating-box'><div class='wh'><b><u>Enabled Widgets</u></b></div><div class='wh'><b>BF4</b> - <a href='/bf4-edit'>config</a><br><input type='checkbox' name='e_BF4_sb' %ebf4sb%>BF4 Scoreboard<br><input type='checkbox' name='e_BF4_st' %ebf4st%>BF4 Stats</div><div class='wh'><b>BF1</b><br> <input type='checkbox' name='e_BF1' %ebf1%>BF1 Server Info</div><div class='wh'><b>Weather</b> - <a href='/weather-edit'>config</a><br><input type='checkbox' name='e_w-c' %ewc%>Weather - Now<br><input type='checkbox' name='e_w-f' %ewf%>Weather - Forecast</div></div><br><input type='submit' value='Save Settings'><br></form><a href='/admin'>Admin Tools/Help</a><br><h5>FW %fwF%</h5></html>");

  str_frm.replace("%vCnst%",  String(constrain(map(cfg_n.contrast, 1,255,1,101), 1, 100)));               // display contrast (text)
  str_frm.replace("%efwAU%",  cb2varS(_MB(M_FW_AUTOUPDATE_EN)));     // FW auto-update (cb)
  str_frm.replace("%ebf4sb%", cb2varS(_MB(M_DSP_BF4_GAME_EN)));      // BF4 MODULE (cb)
  str_frm.replace("%ebf4st%", cb2varS(_MB(M_DSP_BF4_STATS_EN)));     // BF4 STATS MODULE (cb)
  str_frm.replace("%ebf1%",   cb2varS(_MB(M_DSP_BF1)));               // BF1 SERVER INFO (cb)
  str_frm.replace("%ewc%",    cb2varS(_MB(M_DSP_WEATHER_CUR_EN)));   // WEATHER CURRENT MODULE (cb)
  str_frm.replace("%ewf%",    cb2varS(_MB(M_DSP_WEATHER_FRCST_EN))); // WEATHER FORECAST MODULE (cb)
  str_frm.replace("%vSS%",    String(cfg_n.scrollSpeed/1000));       // MODULE SCROLL SPEED (text)
  str_frm.replace("%fwF%",    String(FIRMWARE_VERSION));

  server.sendContent(str_frm);  //memEnd = system_get_free_heap_size();  tEnd = millis();  server.sendContent( String(memStart-memEnd) + "<br>" + String(tEnd-tStart));
}
// -----------------------------
//        PAGE - ROOT (NEW) - SAVE
// -----------------------------
void h_root_new_save() {
  // ERROR CHECKS
  // ... 1+ modules enabled; contrast value 1-100; scroll speed > 0
  if ((server.arg("e_BF4_sb").length() == 0) && (server.arg("e_BF4_st").length() == 0) && (server.arg("e_w-c").length() == 0) && (server.arg("e_w-f").length() == 0) && (server.arg("e_BF1").length() == 0) ) {      server.send(200, "text/text", F("ERROR: at least one widget must be enabled")); return;  }
  uint8_t tContrast = server.arg("contrast").toInt();
  if(tContrast > 100 || tContrast <= 0) {    server.send(200, "text/text", F("ERROR: contrast value must be between 1 and 100")); return;  }
  if(server.arg("SS").toInt() <= 0) {    server.send(200, "text/text", F("ERROR: scroll speed must be greater than 0 seconds")); return;  }

  cfg_n.contrast =    map(server.arg("contrast").toInt(), 1, 100, 1, 255);

  // TODO: NEEDS FIXED. OVERWRITTING SOME SHIT??
  cfg_n.options= B00000000 | _BV(B_DSP_BF4_AUTO_HIDE);

  // FW auto-update
  if(server.arg("fwAU").length() > 0)      cfg_n.options |= _BV(B_FW_AUTOUPDATE_EN);      else cfg_n.options &= 0xFF - _BV(B_FW_AUTOUPDATE_EN);
  // BF4 live score board
  if(server.arg("e_BF4_sb").length() > 0)  cfg_n.options |= _BV(B_DSP_BF4_GAME_EN);       else cfg_n.options &= 0xFF - _BV(B_DSP_BF4_GAME_EN);
  // BF4 stats
  if(server.arg("e_BF4_st").length() > 0)  cfg_n.options |= _BV(B_DSP_BF4_STATS_EN);      else cfg_n.options &= 0xFF - _BV(B_DSP_BF4_STATS_EN);
  // BF1 server info
  if(server.arg("e_BF1").length() > 0)     cfg_n.options |= _BV(B_DSP_BF1);               else cfg_n.options &= 0xFF - _BV(B_DSP_BF1);
  // weather - current conditions
  if(server.arg("e_w-c").length() > 0)     cfg_n.options |= _BV(B_DSP_WEATHER_CUR_EN);    else cfg_n.options &= 0xFF - _BV(B_DSP_WEATHER_CUR_EN);
  // weather - forecast
  if(server.arg("e_w-f").length() > 0)     cfg_n.options |= _BV(B_DSP_WEATHER_FRCST_EN);  else cfg_n.options &= 0xFF - _BV(B_DSP_WEATHER_FRCST_EN);
  cfg_n.scrollSpeed = (unsigned int)server.arg("SS").toInt()*1000;

  server.send_P(200, "text/html",SERVER_PAGE_SAVE_SUCCESS);
  display.setContrast(cfg_n.contrast);

  // advance widget frames (in case current one was disbaled)
  t_frame = cfg_n.scrollSpeed;

  // save settings to EEPROM
  eeprom_saveCfg();
}

// -----------------------------
//        PAGE - BF4 SETTINGS
// -----------------------------
void h_bf4_config(){
    //server.sendContent_P(PAGE_BF4_1);
    String str_frm = F("<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='cache-control' content='no-cache' /></head><h1> BF4 Widget - Config</h1><br><form action='/bf4-save' method='POST'><b><u>BF4 Widget Settings</u></b><br><input type='checkbox' name='c-SB' %eSB%> Live Scoreboard Display Enabled<br><input type='checkbox' name='c-ST' %eST%> Stats Display Enabled<br><input type='checkbox' name='c-AH' %eAH%> Show only if server not empty<br>BF4 Server ID:<input type='text' name='t-ID' value='%tID%' maxlength='36' size='30'><br>Update Interval:<input type='text' name='t-UI' value='%tUI%' maxlength='3' size='5'>&nbsp; seconds [30-999]<br><br><input type='submit' value='Save Settings'><br></form></html>");
    str_frm.replace("%tID%", String(cfg_n.bf4.serverID));
    str_frm.replace("%tUI%", String(cfg_n.bf4.refresh/1000));
    str_frm.replace("%eSB%", cb2varS(_MB(M_DSP_BF4_GAME_EN)));
    str_frm.replace("%eST%", cb2varS(_MB(M_DSP_BF4_STATS_EN)));
    str_frm.replace("%eAH%", cb2varS(_MB(M_DSP_BF4_AUTO_HIDE)));
    server.sendContent(str_frm);    //server.sendContent_P(PAGE_BF4_3);
}
void h_bf4_config_save() {
  // error handling ...

  // ... refresh interval ....
  if(server.arg("t-UI").toInt() < 30) {server.send(200, "text/html", F("ERROR: update interval must be 30 seconds or greater"));  return;}
  // ... BF4 server ID
  int dashCnt = 0;
  for (int i=0; i<36; i++) {  if((server.arg("t-ID"))[i] == '-') dashCnt++; }
  if(dashCnt != 4 || server.arg("t-ID").length() != 36) {server.send(200, "text/html", F("ERROR: invalid BF4 server ID"));  return;}


  // copy checkboxes ...
  if(server.arg("c-SB").length() > 0) cfg_n.options |= _BV(B_DSP_BF4_GAME_EN);    else cfg_n.options &= 0xFF - _BV(B_DSP_BF4_GAME_EN);
  if(server.arg("c-ST").length() > 0) cfg_n.options |= _BV(B_DSP_BF4_STATS_EN);   else cfg_n.options &= 0xFF - _BV(B_DSP_BF4_STATS_EN);
  if(server.arg("c-AH").length() > 0) cfg_n.options |= _BV(B_DSP_BF4_AUTO_HIDE);  else cfg_n.options &= 0xFF - _BV(B_DSP_BF4_AUTO_HIDE);

  // copy update interval...
  cfg_n.bf4.refresh = server.arg("t-UI").toInt()*1000;

  // copy BF4 Server ID ...
  strncpy(cfg_n.bf4.serverID, server.arg("t-ID").c_str(), sizeof(cfg_n.bf4.serverID));
  cfg_n.bf4.serverID[sizeof(cfg_n.bf4.serverID) -1] = '\0';

  // immediately refresh ...
  timeElapsed_BF4 = cfg_n.bf4.refresh;
  server.send_P(200, "text/html",SERVER_PAGE_SAVE_SUCCESS);

  // save settings to EEPROM
  eeprom_saveCfg();
}

void h_weather(){
  //String str_frm = F("<html><body><script>function changeDisplay(field, val) {if(val == 1)document.getElementById(field).style.display = 'block';else document.getElementById(field).style.display = 'none';}function selCountry(txt){changeDisplay(\"countryMethod\", 1);if(txt==\"US\")document.getElementById(\"countrySelTxt\").innerHTML = \"Zip-Code:\";else if (txt==\"CA\")document.getElementById(\"countrySelTxt\").innerHTML = \"ZMW: <br> <i>(use <a href='http://cubanjew.a2hosted.com/WIFI_OLED/tools/CanadaZMWLookup.php' target='_blank'><b><font color='red'>this</font></b></a> tool to find your ZMW)</i>\";document.getElementById(\"loc\").value = \"\";}</script><h1> Weather Widget Setup</h1><form action='/weather-save'><u>Enabled Widgets:</u><br><input type='checkbox' name='enF' %eF%> Forecast<br><input type='checkbox' name='enC' %eC%> Current Conditions  <br><br><u>Degree Unit:</u><br><input type='radio' name=\"unit\" value=\"F\" %uF%> Fahrenheit<br><input type='radio' name=\"unit\" value=\"C\" %uC%> Celsius<br><br><u>Location:</u><br><input type='radio' name='acc' value='auto' %lA% onclick='changeDisplay(\"country\", 0)'>Auto by IP <i>(least accurate)</i><br><input type='radio' name='acc' value='sel' %lS% onclick='changeDisplay(\"country\", 1)'>Specific City<br><div id='country' style='padding-left:3em; padding-top:5px; display:%cD%;'><b>Country:</b>&nbsp;<input type='radio' name='country' value='US' %cU% onclick='selCountry(\"US\")'> US&nbsp;<input type='radio' name='country' value='CA' %cC% onClick='selCountry(\"CA\")'> Canada<br><div id='countryMethod' style='display:none;'><div id='countrySelTxt'></div><input type='textbox' name='loc' value='%l%' id='loc'></div></div></div><br><input type='submit' value='save settings'></form><br><i>refresh interval = 1 hour</i></body></html>");

  String str_frm = F("<html><body onload='selCountry(%sC%)'><script>function changeDisplay(field, val) {if(val == 1)document.getElementById(field).style.display = 'block';else document.getElementById(field).style.display = 'none';}function selCountry(txt, clr){changeDisplay(\"countryMethod\", 1);if(txt==\"US\")document.getElementById(\"countrySelTxt\").innerHTML = \"Zip-Code:\";else if (txt==\"CA\")document.getElementById(\"countrySelTxt\").innerHTML = \"ZMW: <br> <i>(use <a href='http://cubanjew.a2hosted.com/WIFI_OLED/tools/CanadaZMWLookup.php' target='_blank'><b><font color='red'>this</font></b></a> tool to find your ZMW)</i>\";if (typeof(clr)==='undefined') document.getElementById(\"loc\").value = \"\";}</script><h1> Weather Widget Setup</h1><form action='/weather-save'><u>Enabled Widgets:</u><br><input type='checkbox' name='enF' %eF%> Forecast<br><input type='checkbox' name='enC' %eC%> Current Conditions  <br><br><u>Degree Unit:</u><br><input type='radio' name=\"unit\" value=\"F\" %uF%> Fahrenheit<br><input type='radio' name=\"unit\" value=\"C\" %uC%> Celsius<br><br><u>Location:</u><br><input type='radio' name='acc' value='auto' %lA% onclick='changeDisplay(\"country\", 0)'>Auto by IP <i>(least accurate)</i><br><input type='radio' name='acc' value='sel' %lS% onclick='changeDisplay(\"country\", 1)'>Specific City<br><div id='country' style='padding-left:3em; padding-top:5px; display:%cD%;'><b>Country:</b>&nbsp;<input type='radio' name='country' value='US' %cU% onclick='selCountry(\"US\")'> US&nbsp;<input type='radio' name='country' value='CA' %cC% onClick='selCountry(\"CA\")'> Canada<br><div id='countryMethod' style='display:none;'><div id='countrySelTxt'></div><input type='textbox' name='loc' value='%l%' id='loc'></div></div></div><br><input type='submit' value='save settings'></form><br><i>refresh interval = 1 hour</i></body></html>");

  // TODO: MORE EFFICIENT WAY FOR   str_frm.replace("%cD%", F("block")); VIA BODY ONLOAD
  str_frm.replace("%eF%", cb2varS(_MB(M_DSP_WEATHER_FRCST_EN)));  // forecast enabled (cb)
  str_frm.replace("%eC%", cb2varS(_MB(M_DSP_WEATHER_CUR_EN)));    // current conditions enabled (cb)
  str_frm.replace("%uF%", cb2varS(!cfg_n.weather.isMetric));      // degree unit - fahrenheit (rd)
  str_frm.replace("%uC%", cb2varS(cfg_n.weather.isMetric));       // degree unit - celcius (rd)
  if(cfg_n.weather.mode == AUTO) {                                // location mode & country (rd & rd))
      str_frm.replace("%lA%", F("checked"));                      // check 'auto IP' accuracy/location mode (rd)
      str_frm.replace("%cD%", F("none"));                         // hide country select DIV (via CSS attribute)
  } else if (cfg_n.weather.mode == US) {
      str_frm.replace("%lS%", F("checked"));                      // check 'manual' accuracy mode (rd)
      str_frm.replace("%cU%", F("checked"));                      // check 'US' country (rd)
      str_frm.replace("%cD%", F("block"));                        // show country select DIV (via CSS attribute)
      str_frm.replace("%sC%", F("\"US\", 1"));                       // show zip textbox (via body onLoad function), pass additional dummy arg so zipzmw field not auto cleared
  } else if (cfg_n.weather.mode == CA) {
      str_frm.replace("%lS%", F("checked"));                      // check 'manual' accuracy mode(rd)
      str_frm.replace("%cC%", F("checked"));                      // check 'CA' country (rd)
      str_frm.replace("%cD%", F("block"));                        // show country select DIV (via CSS attribute)
      str_frm.replace("%sC%", F("\"CA\", 1"));                       // show zmw textbox (via body onLoad function), pass additional dummy arg so zipzmw field not auto cleared
  }
  str_frm.replace("%l%", cfg_n.weather.zipzmw);                   // zip/zmw (txt)
  server.send(200, "text/html", str_frm);
}

void h_weather_save(){

  // current conditions and forecast frame enables
  if(server.arg("enF").length() > 0)     cfg_n.options |= _BV(B_DSP_WEATHER_FRCST_EN);  else cfg_n.options &= 0xFF - _BV(B_DSP_WEATHER_FRCST_EN);
  if(server.arg("enC").length() > 0)     cfg_n.options |= _BV(B_DSP_WEATHER_CUR_EN);  else cfg_n.options &= 0xFF - _BV(B_DSP_WEATHER_CUR_EN);

  // metric or imperial units.
  if (server.arg("unit") == "F") cfg_n.weather.isMetric = false;  else if (server.arg("unit") == "C") cfg_n.weather.isMetric = true;

  // accuracy mode (AUTO or country (US/CA))
  if(server.arg("acc") == "auto") {
    cfg_n.weather.mode = AUTO;
  } else if (server.arg("acc") == "sel") {
      if(server.arg("country") == "US") {
        cfg_n.weather.mode = US;
        strncpy(cfg_n.weather.zipzmw, server.arg("loc").c_str(), sizeof(cfg_n.weather.zipzmw));
        cfg_n.weather.zipzmw[5] = '\0';   // 5 digit zip code
      } else if (server.arg("country") == "CA") {
        cfg_n.weather.mode = CA;
        strncpy(cfg_n.weather.zipzmw, server.arg("loc").c_str(), sizeof(cfg_n.weather.zipzmw));
        cfg_n.weather.zipzmw[sizeof(cfg_n.weather.zipzmw)-1] = '\0';   // ZNW takes up full length
      }
    timeElapsed_weather = REFRESH_WEATHER;
  }


  server.send_P(200, "text/html",SERVER_PAGE_SAVE_SUCCESS);

  // save settings to EEPROM
  eeprom_saveCfg();

    // TODO: RESET WEATHER TIMER
}

void h_admin(){
  String str_frm = F("<html><head><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/><title>Advanced Configuration</title><style>.c{text-align: center;} div,input{padding:5px;} input[type='submit']{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;}</style></head><body><div style='text-align:left;display:inline-block;min-width:260px;'><html><h1>Admin/Help</h1><b>Tools:</b><br><a href='/diag'>Diagnostics Info</a><br><a href='/OTA'>Update Firmware</a><br><br><b>Help:</b><br><a href='http://newworldgamingbf4.enjin.com' target='_blank'>NWG Forums</a><br><a href='https://github.com/CubanJew/BF4_SERVER_MONITOR' target='_blank'>Source Code</a><br><a href='http://cubanjew.a2hosted.com/WIFI_OLED/DOCS/WIFI_OLED_Manual_Rev.2.pdf'>User Manual</a></html>");
  server.send(200, "text/html", str_frm);
}


/*
// -----------------------------
//        PAGE - ADVANCED SETTINGS
// -----------------------------
void handle_advanced_settings() {
  // hyperlink not change color after click : https://stackoverflow.com/questions/8188060/how-to-make-link-not-change-color-after-visited
  String page = FPSTR(SERVER_HTTP_HEAD);
  page += FPSTR(SERVER_HTTP_STYLE);
  page += FPSTR(SERVER_HTTP_HEAD_END);
  page += "<html><h1>Advanced Settings</h1><form action='/advanced_save'> <b>BF4 Server ID:</b> &nbsp; <input type='text' name='serverID' id='BF4_ID' value='";
  page += String(settings.serverID);
  page += "' maxlength='36' size='30'><br>";
  page += "<b>Auto FW Update:</b>&nbsp;<input type='checkbox' name='fwAutoUpdate'";
  page += (settings.fwAutoUpdate == true ? " checked>" : ">");
  page += "<br><br><input type='submit'value='Save Settings'></form>";
  page += "<br><br><b>Tools:</b><br><a href='/diag'>Diagnostics Info</a><br><a href='/OTA'>Update Firmware</a><br><br><b>Help:</b><br><a href='http://newworldgamingbf4.enjin.com' target='_blank'>NWG Forums</a><br><a href='https://github.com/CubanJew/BF4_SERVER_MONITOR' target='_blank'>Source Code</a><br>User Manual";

  page += "<br><br><b><u>Enabled Modules:</u></b><br><input type='checkbox' name='e_BF4_sc'> BF4 Scoreboard<br><input type='checkbox' name='e_BF4_st'> BF4 Stats<br><input type='checkbox' name='e_wh'> Weather<br><input type='checkbox' name='e_cl'> Clock";
  page += "</html>";
  server.send(200, "text/html",page);
}*/

/*
void handle_advanced_settings_save(){
  // ARGS:  0=serverID  1*=fwAutoUpdate
    if(server.argName(0) != "serverID" || server.arg(0).length() != 36) {
      server.send(200, "text/text", "Invalid form");    return;
    }
    // Auto FW Update ...
    if (server.argName(1) == "fwAutoUpdate" && strcmp(server.arg(1).c_str(), "on") == 0)
      settings.fwAutoUpdate = true;
    else
      settings.fwAutoUpdate = false;

    // BF4 Server ID ...
    strncpy(settings.serverID, server.arg(0).c_str(), sizeof(settings.serverID));
    settings.serverID[sizeof(settings.serverID) -1] = '\0';

    //immediately refresh
    timeElapsed_BF4 = settings.refresh;

    // save new settings to EEPROM ...
    EEPROM.begin(SETTINGS_T_SIZE);
    EEPROM.put(0,settings);
    EEPROM.commit();
    EEPROM.end();

    server.send_P(200, "text/html", SERVER_PAGE_SAVE_SUCCESS);
}*/

// -----------------------------
//        PAGE - ROOT
// -----------------------------
/*
void handle_Root() {
  settings_t tmpSettings;
  EEPROM.get(0, tmpSettings);     //EEPROM.put(0,settings);  //EEPROM.commit();  //EEPROM.end();
  server.sendContent_P(SERVER_PAGE_ROOT_1);  //server.sendContent(String(settings.serverID));
  server.sendContent_P(SERVER_PAGE_ROOT_2);
  server.sendContent(String(settings.refresh/1000));
  server.sendContent_P(SERVER_PAGE_ROOT_3);
  server.sendContent(String(constrain(map(settings.contrast, 1,255,1,101), 1, 100))); //server.sendContent(String(map(settings.contrast, 1,255,1,100)+1));  // BUG: THE +1 IS MESSED UP. WORKS FOR LOW #S, BUT NOT FOR 100% (RETURNS AS 101%)
  server.sendContent_P(SERVER_PAGE_ROOT_4);
  server.sendContent_P(FIRMWARE_VERSION);
}*/

// -----------------------------
//       PAGE -  SAVE (SETTINGS)
// -----------------------------
/*
void handle_Save()
{
  // Validate form submission & correct # of args
  if (server.args() != 2 ||   server.argName(0) != "updateInt" || server.argName(1) != "contrast")  { //server.argName(0) != "serverID" ||
    server.send_P(200, "text/text", SERVER_PAGE_SAVE_FORM_ERROR);    return;
  }
  // validate arg values [serverID(36 char), refresh(>0), contrast(1-100%)]
  if ((server.arg(0).toInt() < 30 || (server.arg(1).toInt() <= 0 || server.arg(1).toInt() > 100)))  { server.arg(0).length() != 36 ||
    server.send_P(200, "text/text", SERVER_PAGE_SAVE_ERROR);    return;
  }

  // update settings ...
  //strncpy(settings.serverID, server.arg(0).c_str(), sizeof(settings.serverID));
  //settings.serverID[sizeof(settings.serverID) -1] = '\0';

  settings.refresh = ((unsigned int)server.arg(0).toInt()) * 1000;
  timeElapsed_BF4 = settings.refresh;

  settings.contrast = map(server.arg(1).toInt(), 1, 100, 1, 255);
  display.setContrast(settings.contrast);

  // save new settings to EEPROM ...
  EEPROM.begin(sizeof(settings_t)); //EEPROM.begin(SETTINGS_T_SIZE);
  EEPROM.put(0,settings);
  EEPROM.commit();
  EEPROM.end();

  server.send_P(200, "text/html", SERVER_PAGE_SAVE_SUCCESS);
}*/




// -----------------------------
//        NEW FW VERSION CHECK
// -----------------------------
void handle_test_fw_check() {
    server.send(200, "text/plain", (firmware_update_check()) ? "UPDATE AVAILABLE" : "NO UPDATE");
}

void handle_test_http_fw_update() {
  firmware_update();
}



// -----------------------------
//       PAGE -  DEBUG INFO PAGE
// -----------------------------
void h_diagnostics() {
  String page = F("<html>");

  page += F("<b>Firmware</b><ul>");
  page += F("<li>Espressif SDK:\t");		          page += ESP.getSdkVersion();
  page += F("</li><li>Boot Version:\t");			    page += ESP.getBootVersion();
  page += F("</li><li>FW Version:\t");			      page += String(FIRMWARE_VERSION);
  page += F("</li><li>FW md5 sum:\t");            page += ESP.getSketchMD5();
  page += F("</li><li>FW Build Time:\t");         String s = String(BUILD_TIME);  page += s.substring(2,4) + String("/") + s.substring(4,6) + String("/20") + s.substring(0,2) + String(" ") + s.substring(6,8) + String(":") + s.substring(8,10);

  //  page += F("</li><li>Compile Time:\t");
  // page += F(FIRMWARE_VERSION);

  page += F("</li></ul><b>Memory Management</b><ul>");
  page += F("<li>Free RAM:\t");				             page += ((uint32_t)system_get_free_heap_size()/1024); page += F(" KB");
  page += F("</li><li>Free Sketch Space:\t");		   page += ESP.getFreeSketchSpace()/1024;                page += F(" KB");
  page += F("</li><li>Sketch File Size:\t");		   page += ESP.getSketchSize()/1024;                     page += F(" KB");

  page += F("</li></ul><b>Hardware</b><ul>");
  page += F("<li>ESP8266 Chip ID:\t");			       page += ESP.getChipId();
  page += F("</li><li>Flash Chip ID:\t");				   page += ESP.getFlashChipId();
  page += F("</li><li>Flash Chip Size by ID:\t");	 page += ESP.getFlashChipRealSize()/1024;            page += F(" KB");

  page += F("</li></ul><b>Configurable</b><ul>");
  page += F("<li>ESP8266 CPU Speed:\t");	         page += ESP.getCpuFreqMHz();                        page += F(" MHz");
  page += F("</li><li>Flash Speed:\t");			       page += ESP.getFlashChipSpeed()/1000000;            page += F(" MHz");
  page += F("</li><li>Flash Size (IDE):\t");			 page += ESP.getFlashChipSize()/1024;                page += F(" KB");
  page += F("</li><li>Flash Chip Mode:\t");        FlashMode_t ideMode = ESP.getFlashChipMode();       page += (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");

  page += F("</li></ul><b>Operational</b><ul>");
  page += F("<li>System VCC:\t");                  float voltage = ESP.getVcc();  page += (voltage/1024.00f);         page += F(" V");
  page += F("</li><li>System Up Time:\t");         page += getUpTime();
  //page += F("</li><li>Last System Restart Reason:\t (NOT IMPLEMENTED)");  //page += ESP.getResetInfo();                                              

  page += F("</li></ul><b>Wi-Fi Network</b><ul>"); long rssi = WiFi.RSSI();
  page += F("<li>RSSI:\t");				                 page += rssi;  page += F(" dBm (");  page += getRSSIQuality(rssi);
  page += F("</li><li>MAC Address:\t");				     page += WiFi.macAddress();

  page += F("</li></ul><b>Real-time Values</b><ul>");
  page += F("<li>Timer-BF4:\t");                  page += String(timeElapsed_BF4);
  page += F("<li>Timer-BF1:\t");                  page += String(timeElapsed_BF1);
  page += F("<li>Timer-WEATHER:\t");              page += String(timeElapsed_weather);
  page += F("<li>Timer-FW update:\t");            page += String(timeElapsed_fwAutoUpdateCheck);
  page += F("<li>Timer-FRAME:\t");                page += String(t_frame);
  page += F("<li>Config Register:\t");            page += String(cfg_n.options);

  page += F("</li></ul></html>");

  server.send(200, "text/html", page);
}

// Generate uptime in nice string format (HH:MM:SS)
String getUpTime() {
  String duration1 = " ";
  int hr,mn,st;
  st = millis() / 1000;
  mn = st / 60;
  hr = st / 3600;
  st = st - mn * 60;
  mn = mn - hr * 60;
  if (hr<10) {duration1 += ("0");}
  duration1 += (hr);
  duration1 += (":");
  if (mn<10) {duration1 += ("0");}
  duration1 += (mn);
  duration1 += (":");
  if (st<10) {duration1 += ("0");}
  return duration1 += (st);
}

// return string description of RSSI quality
String getRSSIQuality(long rssi) {
  if (rssi > -50) return "fantastic)";
  if (rssi > -75) return "great)";
  if (rssi > -85) return "average)";
  if (rssi > -95) return "poor)";
  return "unusuable";
}
