#ifndef INCLUDE_OLED_H
#define INCLUDE_OLED_H
#include <Arduino.h>
#include "includes/include_DataStructs.h"
#include "includes/includes.h"
#include "parser/parser_weather.h"  // need for getMeteoconIcon() function
#include "includes/fonts_imgs.h"
#define FRAME_COUNT    5

#define GFX_MAX_X           128  // OLED max dimensions
#define GFX_MAX_Y           64
#define GFX_CENTER_X        64

#define GFX_T_BOX_X          19  // TOP progress bar
#define GFX_T_BOX_Y          20
#define GFX_B_BOX_X          19  // BOTTOM progress bar
#define GFX_B_BOX_Y          38
#define GFX_BOX_WIDTH       85  // progress bar dimensins
#define GFX_BOX_HEIGHT      12

#define GFX_FOOTER_TIME_X   1   // footer:  round time
#define GFX_FOOTER_Y        54
#define GFX_FACTION_LABEL_X 1

#define GFX_CIRCLE_RADIUS   7  // for mcom/flag icons

void oled_FW_update_msg();
void oled_draw_noWiFiNetwork(String pSSID);
void oled_display_msg(String msg, int delayMS);

// FRAMES
void draw_bf4_data();
void draw_bf4_stats();
void draw_BF1();
void draw_weather_current(); //void oled_frame_3_curr_weather();
void draw_weather_forecast();  // oled_frame_3_curr_forecast();

void drawForecastDetails(int x, int y, int dayIndex);

// t/s: http://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html.
// xbm image conversion   http://blog.squix.ch/2015/05/esp8266-nodemcu-how-to-create-xbm.html   // https://github.com/squix78/esp8266-oled-ssd1306
// https://github.com/squix78/esp8266-oled-ssd1306/blob/master/examples/SSD1306UiDemo/SSD1306UiDemo.ino
extern OLEDDisplayUi ui;
extern int frameCount;
typedef void (*Demo)(void);
extern Demo frames[];
#endif
