#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#include "includes/includes.h"
extern ESP8266WebServer server;

void HTTP_server_init();
void handle_Root();
void handle_Save();
void handle_debugInfo();
String getRSSIQuality(long rssi);
String getUpTime();

#endif
