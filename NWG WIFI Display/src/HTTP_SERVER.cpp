#include "includes/HTTP_SERVER.h"
#include "includes/includes.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

static const char FIRMWARE_VERSION[] PROGMEM   = "1.2";

static const char SERVER_PAGE_OTA[] PROGMEM    = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";      //const char* SERVER_PAGE_OTA = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

static const char SERVER_PAGE_ROOT_1[] PROGMEM = "<head><meta name='viewport' content='width=device-width, initial-scale=1'></head><h1> BF4 SERVER MONITOR CONFIGURATION</h1><br><form action='/save' method='POST'><b>Server ID:</b> &nbsp; <input type='text' name='serverID' value='";
static const char SERVER_PAGE_ROOT_2[] PROGMEM = "' maxlength='36' size='30'><br><b>Refresh:</b> &nbsp; <input type='text' name='updateInt' value='";
static const char SERVER_PAGE_ROOT_3[] PROGMEM = "' maxlength='3' size='5'>&nbsp;<i>seconds</i><br> <b>Contrast:</b> &nbsp; <input type='text' name='contrast' value='";
static const char SERVER_PAGE_ROOT_4[] PROGMEM = "' maxlength='3' size='5'>&nbsp;<i>%</i><br><input type='submit'value='Save Settings'><br> </form><br><a href='/OTA'><button>Update Firmware</button></a><br>FW v";

static const char SERVER_PAGE_SAVE_SUCCESS[] PROGMEM    = "<meta http-equiv='refresh' content='1;url=/' />Settings saved! <br><br> Redirecting in 1 second...";
static const char SERVER_PAGE_SAVE_ERROR[] PROGMEM      = "Invalid setting values! \n\t- Server ID must be exactly 32 characters.\n\t- Update interval must be >= 30 seconds.\n\t= Contrast must be 1-100%";
static const char SERVER_PAGE_SAVE_FORM_ERROR[] PROGMEM = "Form submit error";

ESP8266WebServer server ( 80 );

// -----------------------------
//        HTTP INITIALIZATION
// -----------------------------
void HTTP_server_init() {
  server.on("/", handle_Root);
  server.on("/save", handle_Save);

  server.on("/OTA", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send_P(200, "text/html", SERVER_PAGE_OTA);
  });

  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");    //erver.send_P(200, "text/text", (Update.hasError()) ? SERVER_PAGE_OTA_FAIL : SERVER_PAGE_OTA_SUCCESS);
    ESP.restart();
  }, []() {
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

  server.onNotFound([]() {server.send(200, "text/plain", "Page not found.");  });
  server.begin();
}

// -----------------------------
//        ROOT PAGE
// -----------------------------
void handle_Root() {
  settings_t tmpSettings;

  EEPROM.get(0, tmpSettings);

  //EEPROM.put(0,settings);
  //EEPROM.commit();
  //EEPROM.end();

  server.sendContent_P(SERVER_PAGE_ROOT_1);
  server.sendContent(String(settings.serverID));
  server.sendContent_P(SERVER_PAGE_ROOT_2);
  server.sendContent(String(settings.refresh/1000));
  server.sendContent_P(SERVER_PAGE_ROOT_3);
  server.sendContent(String(constrain(map(settings.contrast, 1,255,1,101), 1, 100))); //server.sendContent(String(map(settings.contrast, 1,255,1,100)+1));  // BUG: THE +1 IS MESSED UP. WORKS FOR LOW #S, BUT NOT FOR 100% (RETURNS AS 101%)
  server.sendContent_P(SERVER_PAGE_ROOT_4);
  server.sendContent_P(FIRMWARE_VERSION);
  //server.sendContent(String(sizeof(settings)));

  //server.sendContent(String(String(tmpSettings.refresh) + "<br>" + String(tmpSettings.contrast) + "<br>" + String(tmpSettings.serverID)));


}

// -----------------------------
//        SAVE (SETTINGS) PAGE
// -----------------------------
void handle_Save()
{
  // Validate form submission & correct # of arguments passed
  if (server.args() != 3 ||  server.argName(0) != "serverID" || server.argName(1) != "updateInt" || server.argName(2) != "contrast")  {
    server.send_P(200, "text/text", SERVER_PAGE_SAVE_FORM_ERROR);    return;
  }
  // validate argument values [serverID(36 char), refresh(>0), contrast(1-100%)]
  if (server.arg(0).length() != 36 || (server.arg(1).toInt() < 30 || (server.arg(2).toInt() <= 0 || server.arg(2).toInt() > 100)))  {
    server.send_P(200, "text/text", SERVER_PAGE_SAVE_ERROR);    return;
  }

  // update settings ...
  strncpy(settings.serverID, server.arg(0).c_str(), sizeof(settings.serverID));
  settings.serverID[sizeof(settings.serverID) -1] = '\0';

  settings.refresh = ((unsigned int)server.arg(1).toInt()) * 1000;
  timeElapsed = settings.refresh;

  settings.contrast = map(server.arg(2).toInt(), 1, 100, 1, 255);
  display.setContrast(settings.contrast);

  // save new settings to EEPROM ...
  EEPROM.begin(SETTINGS_T_SIZE);
  EEPROM.put(0,settings);
  EEPROM.commit();
  EEPROM.end();

  server.send_P(200, "text/html", SERVER_PAGE_SAVE_SUCCESS);
}
