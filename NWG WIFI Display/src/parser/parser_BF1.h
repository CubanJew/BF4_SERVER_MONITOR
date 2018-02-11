#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include "includes/include_DataStructs.h"
#include "includes/string_table.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <elapsedMillis.h>

#define K_BF1_MAP         1
#define K_BF1_PLAYERS     2
#define K_BF1_PLAYERSMAX  3
#define K_BF1_QUEUE       4
#define K_BF1_MODE        5

//enum bf1_HTTP_SERVER {s1 = 1, s2 = 2};
#define   BF1_TEAM_NA   11
#define   BF1_TEAM_1    12
#define   BF1_TEAM_2    13

class parser_BF1: public JsonListener {

  public:
    parser_BF1(BF1_GAME_DATA *d, elapsedMillis *timer);
    void update();
    virtual void whitespace(char c);
    virtual void startDocument();
    virtual void key(String key);
    virtual void value(String value);
    virtual void endArray();
    virtual void endObject();
    virtual void endDocument();
    virtual void startArray();
    virtual void startObject();



  private:
    uint8_t curTeam = BF1_TEAM_NA;

    elapsedMillis *_timer;
    //BF1_GAME_DATA *lGame;
    //uint8_t _key;
};
