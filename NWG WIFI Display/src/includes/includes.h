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
#include <Wire.h>
#include "SSD1306.h"
#include "OLEDDisplayUi.h"
#include "includes/string_table.h"
#include "include_DataStructs.h"
#include "HTTP_SERVER.h"
#include <Arduino.h>
#include "stdint.h"
#include <elapsedMillis.h>
#include "JsonStreamingParser.h"
#include "JsonListener.h"
#include "parser/parser_BF4.h"      // BF4 parser
#include "parser/parser_weather.h"  // Weather
#include "parser/parser_BF1.h"  // BF1 parser

#include "includes/HTTP_SERVER.h"
#include "includes/OLED.h"
#include "includes/HTTP_FW_OTA.h"

#define DEBUG 1
#ifdef DEBUG
  #define DEBUG_P(x)  Serial.println(x)
#else
  #define DEBUG_P(x)
#endif


// INTERVALS
#define REFRESH_WEATHER         3600000 // hourly
#define REFRESH_BF1             300000  // every 5 minutes
#define REFRESH_FW_AUTO_UPDATE  86400000  // daily

// -----------------------------------
enum WEATHER_MODE {US, CA, AUTO};

typedef struct CFG_bf4 CFR_bf4;
typedef struct CFG_weather CFG_weather;
typedef struct CFG_NEW CFG_NEW;

struct CFG_bf4{
  char          serverID[37];
  int           refresh;
  bool          autoHide;
};
struct CFG_weather{
  bool          isMetric;
  WEATHER_MODE  mode;
  char          zipzmw[16];    // zmw # chars: 5:3*:5 + 1 newline + 2 periods = 16     3*=assumed
};
struct CFG_NEW{
  CFG_bf4       bf4;
  CFG_weather   weather;
  uint8_t       contrast;
  unsigned int  scrollSpeed;
  byte          options;        // module display & fetch toggles, firmware update enable
  unsigned long eeprom_build_time;    // FW build date to know when previuosly saved EEPROM config data may no longer be in valid format.
};

// MAIN SETTINGS PROTOTYPES
extern CFG_NEW cfg_n;
extern GameData game;
extern BF1_GAME_DATA BF1_GAME;
extern Stats_Delta stats;
extern Weather weather;
extern elapsedMillis timeElapsed_BF4;
extern elapsedMillis timeElapsed_BF1;
extern elapsedMillis t_frame;
extern elapsedMillis timeElapsed_weather;
extern elapsedMillis timeElapsed_fwAutoUpdateCheck;

extern SSD1306  display;

// main.ino - PROTOTYPES
void EEPROM_loadSettings();


// OLED PROTOTYPES
void testPrint();
void oled_init();

// BF4 prototypes:
void draw_bf4_data();
void fetch_json();
void draw_headerFooter();

// "OPTIONS" bit value
#define _MB(mask) (bool)(mask & cfg_n.options)

#define B_FW_AUTOUPDATE_EN				  0
#define B_DSP_BF4_GAME_EN				    1
#define B_DSP_BF4_STATS_EN				  2
#define B_DSP_WEATHER_CUR_EN			  3
#define B_DSP_WEATHER_FRCST_EN			4
#define B_DSP_BF4_AUTO_HIDE         5
#define B_DSP_BF1                   6
/***

  "options"  REGISTER:   [x] [6] [5] [4] [3] [2] [1] [0]
        [0]: B_FW_AUTOUPDATE_EN:          Firmware auto-update enable
        [1]: B_DSP_BF4_GAME_EN            Display BF4 live scoreboard info
        [2]: B_DSP_BF4_STATS_EN           Display BF4 differential stats
        [3]: B_DSP_WEATHER_CUR_EN         Display current weather conditions
        [4]: B_DSP_WEATHER_FRCST_EN       Display weather forecast
        [5]: B_DSP_BF4_AUTO_HIDE          Auto-hide bf4 display if server empty
        [6]: B_DSP_BF1                    Display BF1 server info
        SETTING:
        	        Clear:	^= _BV(B_FW_AUTOUPDATE_EN)
        	        Set:	|= _BV(B_FW_AUTOUPDATE_EN)
        CHECKING:
        	       Check:	_MB(M_FW_AUTOUPDATE_EN)
  **/
// "OPTIONS" bitmasks
#define M_FW_AUTOUPDATE_EN				  1
#define M_DSP_BF4_GAME_EN				    2
#define M_DSP_BF4_STATS_EN				  4
#define M_DSP_WEATHER_CUR_EN			  8
#define M_DSP_WEATHER_FRCST_EN			16
#define M_DSP_BF4_AUTO_HIDE		      32
#define M_DSP_BF1                   64
//#define _not_used6              		128

#endif
