
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "parser/Weather_parser.h"

const String  WUNDERGRROUND_API_KEY = "e2a7dc6f79c3cc0b";

// PROGMEM:   http://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html    https://arduino.stackexchange.com/questions/13545/using-progmem-to-store-array-of-structs


/*

WEATHER ZMV LOOKUP:   http://autocomplete.wunderground.com/aq?query=canada&c=CA
https://www.wunderground.com/weather/api/d/docs?d=autocomplete-api#city_api_request
*/



// WUNDERGROUND API; max: 500/day;  10/min
void WundergroundClient::key(String key) {
  switch (OBJECT_DEPTH_INDEX) {
    // DETERMINE IF FORECAST OR CURRENT CONDITIONS JSON
    case 1:
      if(key == "forecast") {
          isForecast = true;  //dKeys[1]=KEY_1_FORECAST;
      } else if (key == "current_observation") isCurrent = true;
      break;

    case 2:
      if(isForecast) {
        if(key=="simpleforecast") {
            dKeys[2]=K_2F_SIMPLEFORECAST; nextValGrab=true;
        }
      }
      if(isCurrent) {
          // CURRENT CONDITIONS
          if(key=="icon") {                     dKeys[2]=K_2C_ICON;  nextValGrab = true;
          } else if (key=="temp_f"){            dKeys[2]=K_2C_TEMP_F; nextValGrab = true;
          } else if (key=="relative_humidity"){ dKeys[2]=K_2C_RELATIVE_HUMIDITY; nextValGrab = true;
          } else if (key=="feelslike_f"){       dKeys[2]=K_2C_FEELSLIKE_F; nextValGrab = true;
          } else if (key=="precip_1hr_in"){     dKeys[2]=K_2C_PRECIP_1HR_IN;  nextValGrab = true;
          } else if (key=="precip_today_in"){   dKeys[2]=K_2C_PRECIP_TODAY_IN;  nextValGrab = true;
          } else if (key=="weather"){           dKeys[2]=K_2C_WEATHER; nextValGrab = true;
          }
        }
        break;

    case 3:
      if(isForecast) {
        // set day of current forecast
          //if(dKeys[2]==K_2F_SIMPLEFORECAST) {
            dKeys[3]=K_3F_FORECAST_DAY_IDENT; nextValGrab=true;
          //}
      }
      break;

    case 4:
      if(isForecast && dKeys[2]==K_2F_SIMPLEFORECAST) {
        if      (key=="period")     {dKeys[4]=K_4F_PERIOD;      nextValGrab=true;}
        else if (key=="high")       {dKeys[4]=K_4F_HIGH;}
        else if (key=="low")        {dKeys[4]=K_4F_LOW;}
        else if (key=="icon")       {dKeys[4]=K_4F_ICON;        nextValGrab=true;}
        else if (key=="pop")        {dKeys[4]=K_4F_POP;         nextValGrab=true;}
        else if (key=="qpf_allday") {dKeys[4]=K_4F_QPF_ALLDAY;  }
        else if (key=="avehumidity"){dKeys[4]=K_4F_AVE_HUMIDITY;nextValGrab=true;}
        else if (key=="date")       {dKeys[4]=K_4F_DATE; curForecastDay++;}   //DATE IS ALWAYS 1ST IN ORDER
      }
      break;

    case 5:
      if(isForecast && dKeys[2]==K_2F_SIMPLEFORECAST) {
        if      (key=="fahrenheit" && dKeys[4]==K_4F_HIGH)     {dKeys[5]=K_5F_HIGH_FAHRENHEIT;  nextValGrab=true;}
        else if (key=="fahrenheit" && dKeys[4]==K_4F_LOW)      {dKeys[5]=K_5F_LOW_FAHRENHEIT;   nextValGrab=true;}
        else if (key=="in" && dKeys[4]==K_4F_QPF_ALLDAY)       {dKeys[5]=K_5F_QPF_ALLDAY;       nextValGrab=true;}
        else if (key=="weekday_short" && dKeys[4]==K_4F_DATE)  {dKeys[5]=K_5F_WEEKDAY_SHORT;    nextValGrab=true;}
      }
      break;


    }
}

void WundergroundClient::value(String value) {
  if (nextValGrab) {
      nextValGrab=false;
      if(isForecast) {
        if(OBJECT_DEPTH_INDEX==4) {
          if (dKeys[4]==K_4F_PERIOD)            {curForecastDay = value.toInt() - 1;}
          else if (dKeys[4]==K_4F_ICON)         {w->forecast.icon[curForecastDay]=value; }
          else if (dKeys[4]==K_4F_POP)          {w->forecast.pop[curForecastDay]=value.toInt();}
          else if (dKeys[4]==K_4F_AVE_HUMIDITY) {w->forecast.ave_humidity[curForecastDay]=value.toInt();}
        }
        if(OBJECT_DEPTH_INDEX==5) {
          switch(dKeys[5]) {
            case K_5F_HIGH_FAHRENHEIT:
              w->forecast.high_f[curForecastDay] = value.toInt();
              break;
            case K_5F_LOW_FAHRENHEIT:
              w->forecast.low_f[curForecastDay] = value.toInt();
              break;
            case K_5F_QPF_ALLDAY:
              w->forecast.qpf_allday_in[curForecastDay] = atof(value.c_str());
              break;
            case K_5F_WEEKDAY_SHORT:
              w->forecast.day[curForecastDay] = String(value);
              //Serial.print("Day: "); Serial.print(w->forecast.day[curForecastDay]); Serial.print(" Value: "); Serial.print(value); Serial.print(" curForecastDay="); Serial.println(curForecastDay); //dayShort_to_uint8t(value);
              break;
          }
        }
      }
      if(isCurrent) {
        // CURRENT CONDITIONS
          if(OBJECT_DEPTH_INDEX==2) {
            String s;
            switch (dKeys[2]) {
                case K_2C_ICON:
                  w->now.icon = value;
                  break;
                case K_2C_TEMP_F:
                  w->now.temp_f = value.toInt();
                  break;
                case K_2C_RELATIVE_HUMIDITY:
                  s = String(value);
                  s.replace('%',' ');
                  w->now.relative_humidity = s.toInt();
                  break;
                case K_2C_FEELSLIKE_F:
                  w->now.feelslike_f = value.toInt();
                  break;
                case K_2C_PRECIP_1HR_IN:
                  w->now.precip_1hr_in = atof(value.c_str());
                  break;
                case K_2C_PRECIP_TODAY_IN:
                  w->now.precip_today_in = atof(value.c_str());
                  break;
                case K_2C_WEATHER:
                  w->now.weather = String(value);
                  break;
             }
          }
        }
      }
  }

String getMeteoconIcon(String iconText) {
  if (iconText == "chanceflurries") return "F";
  if (iconText == "chancerain") return "Q";
  if (iconText == "chancesleet") return "W";
  if (iconText == "chancesnow") return "V";
  if (iconText == "chancetstorms") return "S";
  if (iconText == "clear") return "B";
  if (iconText == "cloudy") return "Y";
  if (iconText == "flurries") return "F";
  if (iconText == "fog") return "M";
  if (iconText == "hazy") return "E";
  if (iconText == "mostlycloudy") return "Y";
  if (iconText == "mostlysunny") return "H";
  if (iconText == "partlycloudy") return "H";
  if (iconText == "partlysunny") return "J";
  if (iconText == "sleet") return "W";
  if (iconText == "rain") return "R";
  if (iconText == "snow") return "W";
  if (iconText == "sunny") return "B";
  if (iconText == "tstorms") return "0";

  if (iconText == "nt_chanceflurries") return "F";
  if (iconText == "nt_chancerain") return "7";
  if (iconText == "nt_chancesleet") return "#";
  if (iconText == "nt_chancesnow") return "#";
  if (iconText == "nt_chancetstorms") return "&";
  if (iconText == "nt_clear") return "2";
  if (iconText == "nt_cloudy") return "Y";
  if (iconText == "nt_flurries") return "9";
  if (iconText == "nt_fog") return "M";
  if (iconText == "nt_hazy") return "E";
  if (iconText == "nt_mostlycloudy") return "5";
  if (iconText == "nt_mostlysunny") return "3";
  if (iconText == "nt_partlycloudy") return "4";
  if (iconText == "nt_partlysunny") return "4";
  if (iconText == "nt_sleet") return "9";
  if (iconText == "nt_rain") return "7";
  if (iconText == "nt_snow") return "#";
  if (iconText == "nt_sunny") return "4";
  if (iconText == "nt_tstorms") return "&";
}
WundergroundClient::WundergroundClient(Weather *_w, boolean _isMetric) {
  w = _w;
  //w->isMetric = _isMetric;
}
void WundergroundClient::update(String apiKey) {

  //WUNDERGROUND_APIKEY_TABLE[random(0,2)];
  doUpdate("/api/" + apiKey + "/conditions/q/autoip.json");
  doUpdate("/api/" + apiKey + "/forecast/q/autoip.json");
}

void WundergroundClient::doUpdate(String url) {
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient client;
  if (!client.connect("api.wunderground.com", 80)) {
    Serial.println("connection failed");
    return;
  }

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: api.wunderground.com\r\n" +
               "Connection: close\r\n\r\n");
  int retryCounter = 0;
  while(!client.available()) {
    delay(1000);
    retryCounter++;
    if (retryCounter > 10) {
      return;
    }
  }

  int pos = 0;
  boolean isBody = false;
  char c;

  int size = 0;
  client.setNoDelay(false);
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
}
void WundergroundClient::whitespace(char c) {}
void WundergroundClient::startDocument() {OBJECT_DEPTH_INDEX=0;}
void WundergroundClient::endArray() {}
void WundergroundClient::startObject() { OBJECT_DEPTH_INDEX++;}
void WundergroundClient::endObject() {  OBJECT_DEPTH_INDEX--;}
void WundergroundClient::endDocument() {}
void WundergroundClient::startArray() {}
