// https://apigee.com/resources/wunderground?apig_cc=1
// https://github.com/squix78/esp8266-weather-station/blob/master/WundergroundClient.cpp
// http://api.wunderground.com/api/e2a7dc6f79c3cc0b/conditions/q/autoip.json
#pragma once

#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include "includes/string_table.h"
#include "includes/include_DataStructs.h"
#include "includes/includes.h"
#include <Arduino.h>
#include <elapsedMillis.h>


extern const String WUNDERGRROUND_API_KEY;

// Current Conditions keys
#define K_2C_ICON               20
#define K_2C_TEMP_F             21
#define K_2C_RELATIVE_HUMIDITY  22
#define K_2C_FEELSLIKE_F        23
#define K_2C_PRECIP_1HR_IN      24
#define K_2C_PRECIP_TODAY_IN    25
#define K_2C_WEATHER            26

// Forecast keys
#define K_2F_SIMPLEFORECAST     27
#define K_3F_FORECAST_DAY_IDENT 30

#define K_4F_POP                40
#define K_4F_AVE_HUMIDITY       41
#define K_4F_HIGH               42
#define K_4F_LOW                43
#define K_4F_QPF_ALLDAY         44
#define K_4F_ICON               45
#define K_4F_PERIOD             46
#define K_4F_DATE               47

#define K_5F_HIGH_FAHRENHEIT    50
#define K_5F_LOW_FAHRENHEIT     51
#define K_5F_QPF_ALLDAY         52
#define K_5F_WEEKDAY_SHORT      53


class WundergroundClient: public JsonListener {
  private:
    void doUpdate(String url);
    uint8_t dayShort_to_uint8t(String s);

    uint8_t OBJECT_DEPTH_INDEX = 0;
    int curForecastDay = -1;
    uint8_t dKeys [6] = {KEY_DUMMYPAD, KEY_DUMMYPAD, INVALID, INVALID, INVALID, INVALID};
    bool nextValGrab = false;
    bool isForecast = false;
    bool isCurrent = false;
    Weather *w;
    elapsedMillis *_timer;


  public:
    WundergroundClient(Weather *_w, elapsedMillis *timer);
    void update();
    //void updateConditionsAutoLocation(String apiKey);
    //void updateForecastAutoLocation(String apiKey);
    //void initMetric(boolean isMetric);			// Added by fowlerk, 12/22/16, as an option to change metric setting other than at instantiation
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

    String getMeteoconIcon(String iconText);



/*
  FORECAST: http://api.wunderground.com/api/e2a7dc6f79c3cc0b/forecast/q/autoip.json
      response {3},
      forecast {2}
        -> txt_forecast {2}
        -> simpleforecast {1}
            -> forecastday {4}
                -> 0 / 1 / 2 / 3 {20}
                    -> period
                    -> high {2} / low {2}
                        -> fahrenheit
                        -> celcius
                    -> icon
                    -> pop
                    -> qpf_allday {2}
                        -> in
                        -> mm

  CONDITIONS:    http://api.wunderground.com/api/e2a7dc6f79c3cc0b/conditions/q/autoip.json
    response {3}
    current_observation {56}
      -> ...
      -> weather
      -> temp_f / temp_c
      -> relative_humidity
      -> feelslike_f / feelslike_c
      -> precip_1hr_in / precip_1hr_metric
      -> precip_today_in / precip_today_metric


*/
