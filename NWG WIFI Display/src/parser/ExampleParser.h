#pragma once

#include "JsonListener.h"
#include "includes/include_DataStructs.h"

class ExampleListener: public JsonListener {

  public:
    ExampleListener(GameData *d);
    int getTeamStat(uint8_t stat);
    void resetStats();
    virtual void whitespace(char c);
    virtual void startDocument();
    virtual void key(String key);
    virtual void value(String value);
    virtual void endArray();
    virtual void endObject();
    virtual void endDocument();
    virtual void startArray();
    virtual void startObject();
};
