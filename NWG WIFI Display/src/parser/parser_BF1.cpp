#include "parser_BF1.h"

// TODO: MOVE bf1G AND _KEY AS PRIVATE INSTANCE VARIABLE. SAME WITH OTHER CLASSES

BF1_GAME_DATA *bf1G;
uint8_t _key = -1;

parser_BF1::parser_BF1(BF1_GAME_DATA *d, elapsedMillis *timer) {
  bf1G = d;
  bf1G->g1.valid = false;
  bf1G->g2.valid = false;
  _timer = timer;  //timer = 0;
}

void parser_BF1::update() {
  Serial.println(F("Updating BF1..."));
  _timer->operator=(0);

  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient client;

  unsigned long timeOut = millis() + 5000;
  while(!client.connect("cubanjew.a2hosted.com", 80)) {  //if (!client.connect("cubanjew.a2hosted.com", 80)) {  Serial.println("connection failed");  return; }
    delay(300);
    if(millis() > timeOut) {
      //client.stop();
      DEBUG_P("BF1 FAILED ON CONNECT");
      return;
    }
  }

  // This will send the request to the server
  client.print(String("GET /WIFI_OLED/tools/BF1/BF1_serverQ.php") + " HTTP/1.1\r\n" +
               "Host: cubanjew.a2hosted.com\r\n" +
               "Connection: close\r\n\r\n");
  timeOut = millis() + 10000;
  while(!client.available()) {
    delay(300);
    if (millis() > timeOut) {
      //client.stop();
      DEBUG_P("BF1 AVAILABLE TIMEOUT");
      return;
    }
  }

  int pos = 0;
  boolean isBody = false;
  char c;

  int size = 0;
  client.setNoDelay(true);    //https://www.bountysource.com/issues/32550162-found-a-way-to-upload-data-very-fast-by-wifi
  while(client.connected()) {
    while((size = client.available()) > 0) {
      c = client.read();
      if (c == '{' || c == '[') {
        isBody = true;
      }
      if (isBody) {
        parser.parse(c);
      }
    }
  }
  client.stop();
}

void parser_BF1::key(String key) {
  int sLen = key.length();
  char serverNum = key.charAt(sLen-1);
  // NOTE:  "team" refers to server # (1 or 2)
  if (sLen > 0) {
    if (serverNum == '1') {
        this->curTeam = BF1_TEAM_1;
    } else if (serverNum == '2') {
      this->curTeam = BF1_TEAM_2;
    } else {
      this->curTeam = BF1_TEAM_NA;
      return;
    }
    key[sLen-1] = '\0'; // remove # at the end

    if     (key == "map")    _key = K_BF1_MAP;
    else if(key == "p")      _key = K_BF1_PLAYERS;
    else if(key == "pMax")   _key = K_BF1_PLAYERSMAX;
    else if(key == "q")      _key = K_BF1_QUEUE;
    else if(key == "mode")   _key = K_BF1_MODE;
  }

}

void parser_BF1::value(String value) {
  int iVal = value.toInt();

  switch (_key){
    case K_BF1_MAP:
      if (iVal >= 0 && iVal < 255) {
          if(this->curTeam == BF1_TEAM_1) {
            bf1G->g1.map = iVal;
          } else if (this->curTeam == BF1_TEAM_2) {
            bf1G->g2.map = iVal;
          }
      }// else return;
      break;
    case K_BF1_PLAYERS:
        if(this->curTeam == BF1_TEAM_1) {
          bf1G->g1.players = iVal;
        } else if (this->curTeam == BF1_TEAM_2) {
          bf1G->g2.players = iVal;
        }
      break;
    case K_BF1_PLAYERSMAX:
        if(this->curTeam == BF1_TEAM_1) {
            bf1G->g1.playersMax = iVal;
        } else if (this->curTeam == BF1_TEAM_2) {
            bf1G->g2.playersMax = iVal;
        }
      break;
    case K_BF1_QUEUE:
        if(this->curTeam == BF1_TEAM_1) {
            bf1G->g1.queue = iVal;
        } else if (this->curTeam == BF1_TEAM_2) {
            bf1G->g2.queue = iVal;
        }
      break;
    case K_BF1_MODE:
        if(this->curTeam == BF1_TEAM_1) {
          bf1G->g1.mode = iVal;
          // mode is last JSON element so we can check validity now
          if((bf1G->g1.map <= BF1_NUM_MAPS) && (bf1G->g1.mode <= BF1_NUM_MODES))
              bf1G->g1.valid = true;
        } else if (this->curTeam == BF1_TEAM_2) {
          bf1G->g2.mode = iVal;
          if((bf1G->g2.map <= BF1_NUM_MAPS) && (bf1G->g2.mode <= BF1_NUM_MODES))
              bf1G->g2.valid = true;
        }
      break;
  }
}

void parser_BF1::startObject() {}
void parser_BF1::endObject() {}
void parser_BF1::startDocument() {}
void parser_BF1::endDocument() {}
void parser_BF1::whitespace(char c) {}
void parser_BF1::startArray() {}
void parser_BF1::endArray() {}
