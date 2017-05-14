#ifndef _INCLUDES_H
#define _INCLUDES_H

// http SERVER INCLUDES
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <elapsedMillis.h>

#include <EEPROM.h>

/* required for WIFI MANAGER: */
#include <DNSServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

// OLED INCLUDES
#include <Wire.h>
#include "SSD1306.h"
#include "OLEDDisplayUi.h"

#include "includes/string_table.h"
#include "include_DataStructs.h"
#include "HTTP_SERVER.h"


// MAIN SETTINGS PROTOTYPES
extern GameData game;  // Store pertinent returned game data
extern elapsedMillis timeElapsed;  // Track game data refresh interval

#define SETTINGS_T_SIZE 44

typedef struct {
  char serverID[37];
  uint8_t contrast;
  unsigned int refresh;
} settings_t __attribute__ ((packed));


// main.ino - PROTOTYPES
void EEPROM_loadSettings();
//settings
extern settings_t settings;


// OLED PROTOTYPES:
extern SSD1306  display;
void displayGameData(int statusCode);
void draw_headerFooter();
void testPrint();
void oled_init();

// JSON PARSER PROTOTYPES:
byte   fetch_json();

#endif
