#include <Arduino.h>
#include "stdint.h"
#include <elapsedMillis.h>
#include "includes/includes.h"
#include "JsonStreamingParser.h"
#include "JsonListener.h"
#include "parser/ExampleParser.h"
#include "includes/HTTP_SERVER.h"

// ESP8266-12E NWG PCB:
SSD1306   display(0x3C, 14, 12);
OLEDDisplayUi ui     ( &display );

GameData game;  // Store pertinent returned game data
elapsedMillis timeElapsed;  // Track game data refresh interval

settings_t settings {"5c154149-dd96-4001-8b19-bd2197782f37", 49, 30000};

char str_buffer[STRING_TABLE_BUFFER_SIZE];

void setup(void) {
  Serial.begin(115200);  Serial.println("\nBooting Sketch...");
  oled_init();
  EEPROM_loadSettings();
  timeElapsed = settings.refresh;
  WiFiManager wifiManager;  // connect to WiFi network
  wifiManager.autoConnect("NWG Server");    //wifiManager.autoConnect("NWG Server", "NWG12345");
  HTTP_server_init(); // start HTTP server
  display.clear(); display.drawString(0, 0, "Config IP:"); display.drawString(0, 20, WiFi.localIP().toString()); display.display();
}

void loop() {
  if (timeElapsed > settings.refresh) // was ... > updateInterval
    displayGameData((int)fetch_json());
  delay(100);   // added 10/5/16
  server.handleClient();

  // auto launch disabled for now...
  /*delay(100);
    if (digitalRead(3) == LOW)
    {
    wifi_TCP_launchGame();
    while (digitalRead(3) == LOW) {
      delay(10);
    }
    }*/

  // Auto launch/join BF4 NWG server & launch Teamspeak.
  // Launches 192.168.1.100/BF4/BF4_AUTO_LAUNCH.php
  // Script is hosted via XAMPP running APACHE, which executes windows batch script that launches BF4.EXE & TS.EXE
  /*
    if (digitalRead(16) == HIGH) // GPIO16
    {

      wifi_TCP_launchGame();
      // debounce / make sure user has let go of button
      while (digitalRead(16) == HIGH) delay(100);
      unsigned long timePressed = millis();
      while (millis() - timePressed <= 2000) {
        delay(1);
      }
    }*/
}

// validate argument values [serverID(36 char), refresh(>0), contrast(1-100%)]
  void EEPROM_loadSettings() {
    EEPROM.begin(SETTINGS_T_SIZE);
    settings_t tmpCfg;
    EEPROM.get(0, tmpCfg);
    // check EEPROM setting integrity/validity
    if((tmpCfg.refresh > 0) && (tmpCfg.contrast > 0 && tmpCfg.contrast < 256) && (tmpCfg.serverID[36] == '\0'))
    {
      // make sure # of dashes ('-') in servrID is 4. NOT sure if this should ever be other than 4.
      int dashCount = 0;
      for (int i=0; i<36; i++) {  // note: checking last index unnecessary
        if(tmpCfg.serverID[i] == '-') dashCount++;
      }
      // valid EEPROM settings; let's use them!    TODO:  easier way to just copy the entire settings_t struct?
      if(dashCount == 4) {
        strncpy(settings.serverID, tmpCfg.serverID, sizeof(settings.serverID));
        settings.contrast = tmpCfg.contrast;
        settings.refresh = tmpCfg.refresh;
        EEPROM.end();
        display.setContrast(settings.contrast);
        return;
      }
    }
    // corrupt EEPROM settings. just store the default info into EEPROM
    EEPROM.put(0, settings);
    EEPROM.commit();
    EEPROM.end();
    display.setContrast(settings.contrast);
  }
