#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266httpUpdate.h> // for remote HTTP update
#include "includes/HTTP_FW_OTA.h"
#include "includes/includes.h"
#include "includes/include_DataStructs.h"
#include "includes/OLED.h"  // ONLY NEED THIS FOR TESTING oled_frame_2_teamStats()

extern ESP8266WebServer server;
extern const char FIRMWARE_VERSION[] PROGMEM;

void HTTP_server_init();


void h_root_new();
void h_root_new_save();
void h_bf4_config();
void h_bf4_config_save();
void h_weather();
void h_weather_save();
void h_test();

void h_diagnostics();
void h_admin();
void handle_Root(); // old
void handle_Save(); // old
//void handle_advanced_settings();
//void handle_advanced_settings_save();
void handle_test_fw_check();
void handle_test_http_fw_update();
void handle_testbed();//void handle_test_weather();void handle_display_weather_current();void handle_display_weather_forecast();

String getRSSIQuality(long rssi);
String getUpTime();


// css tips:  https://webdesign.tutsplus.com/articles/quick-tip-easy-css3-checkboxes-and-radio-buttons--webdesign-8953
#endif
