#include <Arduino.h>
//#define JSON_HOST   "keeper.battlelog.com" //  "192.168.1.100"  //#define JSON_URL   "5c154149-dd96-4001-8b19-bd2197782f37"

byte fetch_json()
{
  game.validData = false;

  // verify connected to AP
  if(WiFi.status() != WL_CONNECTED)
  {
    if (WiFi.status() == WL_DISCONNECTED)
      return STATUS_JSON_ERROR_WIFI_DISCONNECTED;
    else
      return STATUS_JSON_ERROR_WIFI_ERROR;
  }

  JsonStreamingParser parser;
  ExampleListener listener = ExampleListener(&game);

  const char * jsonHOST = "keeper.battlelog.com";   //JSON_HOST;
  const char * localBF4ServerID =  settings.serverID;   //const char * localBF4ServerID =  BF4ServerID.c_str();  //JSON_URL;

  // Use WiFiClient class to create TCP connections (7 attempts)
  WiFiClient client;
  delay(10);
  for (int i=0; i<4; i++) {
    if (client.connect(jsonHOST, 80))  break;
    Serial.println("connection failed");
    delay(100);
    if (i == 3) return STATUS_JSON_ERROR_HTTP_CONNECT_FAIL;
  }

  // This will send the request to the server
  client.print(String("GET /snapshot/") + localBF4ServerID + " HTTP/1.1\r\n" + "Host: " + jsonHOST + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    delay(10);
    if (millis() - timeout > 10000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return STATUS_JSON_ERROR_HTTP_REQUEST_TIMEOUT;
    }
  }
  Serial.println("got http resp");
  // Read all the lines of the reply from server and print them to Serial
  timeout = millis();
  while (client.available()) {
    if (millis() - timeout > 10000) {
        client.stop();
        return STATUS_JSON_ERROR_HTTP_REQUEST_LONG_RESPONSE;
    }

    // discard http response headers and skip to the http response data...
    if(client.find("\r\n\r\n"))
    {
      parser.setListener(&listener);

      // unfortunateluy need to store entire HTTP response in string initially
      // otherwise ESP8266 buffer cant keep up w/ parsing of each individual char one at a time... :(
      //while (client.available())   {   parser.parse(client.read());      char c = client.read();      if (c != '\r')        parser.parse(c);      }
      Serial.println("*"); Serial.println("Pre:\t\t" + String(ESP.getFreeHeap()));
      String s = client.readString();     // Serial.println(s);  // debug!

      if (s.length() < 20 && s.indexOf("No such game") != -1)
        return STATUS_JSON_ERROR_INVALID_SERVER_URL;

      if(s.length() < 20)
        return STATUS_JSON_ERROR_INVALID_RESPONSE;

      Serial.println("valid http resp...");
      for (int i = 0; i < s.length(); i++)
          parser.parse(s.charAt(i));
      Serial.println("post:\t\t" + String(ESP.getFreeHeap()));
      game.validData = true;    // use depreciated ?

    } else {
      client.stop();
      return STATUS_JSON_ERROR_INVALID_RESPONSE;
    }
  }
  client.stop();  // likely not necessary as this should be called implicitly when local WifiClient goes out of scope & is destroyed.
  return STATUS_JSON_SUCCESS;
}

// BELOW WORKS NOT CURRENTLY USED
/*
// Team Stat Comparison
// ... kills
int killDelta = listener.getTeamStat(STAT_KILLS_T1) - listener.getTeamStat(STAT_KILLS_T2);
String killDelta_s = String(killDelta);
if (killDelta > 0)
  killDelta_s = "+" + killDelta_s;
Serial.println("Kill delta=" + killDelta_s);
// ... rank (avg.)
// if empty server, don't divide by 0
int rankDelta = 0;
if (listener.getTeamStat(STAT_PLAYERCOUNT_T1)) != 0 && listener.getTeamStat(STAT_PLAYERCOUNT_T2)) != 0)
    rankDelta = ((double)listener.getTeamStat(STAT_RANK_T1) / listener.getTeamStat(STAT_PLAYERCOUNT_T1)) - ((double)listener.getTeamStat(STAT_RANK_T2) / listener.getTeamStat(STAT_PLAYERCOUNT_T2));
String rankDelta_s = String(rankDelta);
if (rankDelta > 0)
  rankDelta_s = "+" + rankDelta_s;
Serial.println("Rank delta=" + rankDelta_s);

// .. # players
int playerDelta = listener.getTeamStat(STAT_PLAYERCOUNT_T1) - listener.getTeamStat(STAT_PLAYERCOUNT_T2);
String playerDelta_s = String(playerDelta);
if (playerDelta > 0)
  playerDelta_s = "+" + playerDelta_s;
Serial.println("Player delta=" + playerDelta_s);

Serial.println("----------------------------");
strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_MAPS[game.map])));
Serial.print(str_buffer); Serial.print("\t");
strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_MODES[game.mode])));
Serial.println(str_buffer);

strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_FACTIONS[game.faction.T1])));
Serial.print(str_buffer); Serial.print(": "); Serial.print(game.score.T1);  Serial.print(" [");  Serial.print(game.scoreMax.T1); Serial.print("]\t\t");


strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_FACTIONS[game.faction.T2])));
Serial.print(str_buffer); Serial.print(": "); Serial.print(game.score.T2);  Serial.print(" [");  Serial.print(game.scoreMax.T2);  Serial.println("]");

// players
Serial.print(game.players); Serial.print("[");  Serial.print(game.playersQueue);  Serial.print("]\t");      Serial.println(game.roundTime);

Serial.println("----------------------------");
//listener.resetStats();
*/
