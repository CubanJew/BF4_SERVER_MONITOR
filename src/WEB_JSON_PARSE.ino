#include <Arduino.h>

void bf4_computeStats() {
  stats.kills.val = game.stats.killSum.T1 - game.stats.killSum.T2;  // for some stupid reason doing abs() here doesn't work
  stats.kills.team = (game.stats.killSum.T1 > game.stats.killSum.T2) ? TEAM_1 : TEAM_2;
  if (game.stats.playerCount.T1 != 0 && game.stats.playerCount.T2 != 0) {
    int rank1=((double)game.stats.rankSum.T1 / game.stats.playerCount.T1);
    int rank2=((double)game.stats.rankSum.T2 / game.stats.playerCount.T2);
    stats.rank.val = abs(rank1 - rank2);
    stats.rank.team = (rank1 > rank2) ? TEAM_1 : TEAM_2;
  } else stats.rank.team = TEAM_NA;

  if(game.stats.deathSum.T1 != 0 && game.stats.deathSum.T2 !=0) {
    double kd1= (double)game.stats.killSum.T1/game.stats.deathSum.T1;
    double kd2= (double)game.stats.killSum.T2/game.stats.deathSum.T2;
    stats.kd.val = abs(kd1-kd2)  ;
    stats.kd.team = (kd1>kd2) ? TEAM_1 : TEAM_2;
  } else stats.kd.team = TEAM_NA;
}

void weather_update() {
  Serial.println(F("Updating weather..."));
  timeElapsed_weather = 0;
  WundergroundClient wunderground(&weather, false);

// use random API key to spread out API call rate
  switch (random(0,3)) {
    case 0:
        wunderground.update("e2a7dc6f79c3cc0b");
        break;
    case 1:
        wunderground.update("2965f06f6aa56310");
        break;
    case 2:
        wunderground.update("a82c1f435e08c1f1");
        break;
    break;
  }
}


void bf4_update() {
  timeElapsed_BF4 = 0;
  game.validData = false;
  Serial.println(F("Updating BF4..."));
  // verify connected to AP
  if(WiFi.status() != WL_CONNECTED)
  {
    if (WiFi.status() == WL_DISCONNECTED) {game.return_status = STATUS_JSON_ERROR_WIFI_DISCONNECTED; return;}
    else                                  {game.return_status = STATUS_JSON_ERROR_WIFI_ERROR; return;}
  }

  JsonStreamingParser parser;
  ExampleListener listener = ExampleListener(&game);

  const char * jsonHOST = "keeper.battlelog.com";   //JSON_HOST;
  const char * localBF4ServerID =  cfg_n.bf4.serverID;     //settings.serverID;   //const char * localBF4ServerID =  BF4ServerID.c_str();  //JSON_URL;

  // Use WiFiClient class to create TCP connections (3 attempts)
  WiFiClient client;
  delay(10);
  for (int i=0; i<4; i++) {
    if (client.connect(jsonHOST, 80))  break;
    Serial.println("connection failed");
    delay(100);
    if (i == 3) {game.return_status = STATUS_JSON_ERROR_HTTP_CONNECT_FAIL; return;}
  }

  // This will send the request to the server
  client.print(String("GET /snapshot/") + localBF4ServerID + " HTTP/1.1\r\n" + "Host: " + jsonHOST + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    delay(10);
    if (millis() - timeout > 10000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      game.return_status =  STATUS_JSON_ERROR_HTTP_REQUEST_TIMEOUT; return;
    }
  }
  Serial.println("got http resp");
  // Read all the lines of the reply from server and print them to Serial
  timeout = millis();
  while (client.available()) {
    if (millis() - timeout > 10000) {
        client.stop();
        game.return_status = STATUS_JSON_ERROR_HTTP_REQUEST_LONG_RESPONSE; return;
    }

    // discard http response headers and skip to the response content.
    if(client.find("\r\n\r\n"))
    {
      parser.setListener(&listener);

      // unfortunateluy need to store entire HTTP response in string initially
      // otherwise ESP8266 buffer cant keep up w/ parsing of each individual char one at a time... :(
      //while (client.available())   {   parser.parse(client.read());      char c = client.read();      if (c != '\r')        parser.parse(c);      }
      Serial.println("*"); Serial.println("Pre:\t\t" + String(ESP.getFreeHeap()));
      String s = client.readString();     // Serial.println(s);  // debug!

      if (s.length() < 20 && s.indexOf("No such game") != -1) {game.return_status = STATUS_JSON_ERROR_INVALID_SERVER_URL; return;}

      if(s.length() < 20) {game.return_status = STATUS_JSON_ERROR_INVALID_RESPONSE; return;}

      Serial.println("valid http resp...");
      for (int i = 0; i < s.length(); i++)
          parser.parse(s.charAt(i));
      Serial.println("post:\t\t" + String(ESP.getFreeHeap()));
      game.validData = true;    // use depreciated ?
      bf4_computeStats();

    } else {
      client.stop();
      game.return_status = STATUS_JSON_ERROR_INVALID_RESPONSE; return;
    }
  }
  client.stop();  // likely not necessary as this should be called implicitly when local WifiClient goes out of scope & is destroyed.
  game.return_status = STATUS_JSON_SUCCESS;
}
