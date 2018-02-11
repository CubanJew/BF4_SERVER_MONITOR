#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "includes/HTTP_SERVER.h"
#include "includes/HTTP_FW_OTA.h"
#include <ESP8266httpUpdate.h> // for remote HTTP update

#include "includes/OLED.h"

/**
Checks if firmware update available
* @ return TRUE if firmware update available.
*/
bool firmware_update_check () {
  const char* FW_check_host = "cubanjew.a2hosted.com";
  WiFiClient client;
  if (!client.connect(FW_check_host, 80)){ client.flush(); client.stop();  return false;}
  //char buffer[30];  //strcpy_P(buffer, (char*)pgm_read_word(FW_check_host)); // Necessary casts and dereferencing, just copy.

  String url = "/WIFI_OLED/FW/FW_check.php?mac_id=" + WiFi.macAddress() + "&FW_v=" + String(FIRMWARE_VERSION);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + FW_check_host + "\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) { if (millis() - timeout > 3000) { client.flush(); client.stop(); return false; }}

  client.readStringUntil('*');
  String response = client.readStringUntil('*');
  client.flush(); client.stop();
  if (response == "YES") return true;
  return false;
}

/**
Updates firmware. firmware_update_check() should be called prior.
*/
void firmware_update() {
  oled_FW_update_msg();
  t_httpUpdate_return ret = ESPhttpUpdate.update("http://cubanjew.a2hosted.com/WIFI_OLED/FW/FW_check.php?mac_id=" + WiFi.macAddress() + "&FW_v=" + String(FIRMWARE_VERSION) + "&dl=1");
}
