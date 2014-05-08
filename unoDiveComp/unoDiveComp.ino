/***************************************************
  This is an example sketch for the Adafruit 2.2" SPI display.
  This library works with the Adafruit 2.2" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/1480
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
 
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include "Time.h"

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

// Using software SPI is really not suggested, its incredibly slow
//Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _mosi, _sclk, _rst, _miso);
// Use hardware SPI
Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

// Dive variables
unsigned int depth_dm;  // Depth in decameters
unsigned long dive_time_s;  // Dive time in seconds


// Display variables
enum prev_display_enum {DIVEMODEDISP, SURFMODEDISP, NONE} prev_display = NONE;
char s1[4], s2[2];
unsigned short prev_display_time[] = {0, 0, 0, 0, 0};
unsigned short new_display_time[] = {0, 0, 0, 0, 0};
unsigned short prev_display_depth[] = {0, 0, 0, 0};
unsigned short new_display_depth[] = {0, 0, 0, 0};
time_t current_time;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println("Adafruit 2.2\" SPI TFT Test!"); 
 
  // Setup the display
  tft.begin();
  tft.setRotation(1);  // Want a landscape display

  // Initialise dive variables
  depth_dm = 9999;
  dive_time_s = 6051;
  
  // Initialise the time to about now-ish
  setTime(17, 57, 0, 8, 05, 2014);

  Serial.print(F("Text                     "));
  drawDiveDisplay();
  delay(500);
}

void loop(void) {
    /*if(depth_dm == 5) depth_dm = 4995;
    else depth_dm = 5;
    drawDiveDisplay();*/
    drawDiveDisplay();
    delay(1000);
    dive_time_s++;
    depth_dm -= 1;
}


unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(ILI9340_BLACK);
  tft.fillScreen(ILI9340_RED);
  tft.fillScreen(ILI9340_GREEN);
  tft.fillScreen(ILI9340_BLUE);
  tft.fillScreen(ILI9340_BLACK);
  return micros() - start;
}

void drawDiveDisplay() {
  if(prev_display != DIVEMODEDISP){  // Wipe the screen if we don't know what's there
    tft.fillScreen(ILI9340_BLACK);
  }
  
  // Display dive time
  new_display_time[0] = (dive_time_s/(60*100))%10;
  new_display_time[1] = (dive_time_s/(60*10))%10;
  new_display_time[2] = (dive_time_s/60)%10;
  new_display_time[3] = (dive_time_s/10)%6;
  new_display_time[4] = dive_time_s%10;
  if(prev_display == DIVEMODEDISP){
    if(prev_display_time[0] != new_display_time[0]){
      tft.fillRect(190, 0, 20, 28, ILI9340_BLACK);
      if(new_display_time[0]) {
        tft.setCursor(190, 0); tft.setTextSize(4); tft.print(new_display_time[0]);
      }
      prev_display_time[0] = new_display_time[0];
    }
    if(prev_display_time[1] != new_display_time[1]){
      tft.fillRect(214, 0, 20, 28, ILI9340_BLACK);
      if(new_display_time[1] || new_display_time[0]) {
        tft.setCursor(214, 0); tft.setTextSize(4); tft.print(new_display_time[1]);
      }
      prev_display_time[1] = new_display_time[1];
    }
    if(prev_display_time[2] != new_display_time[2]){
      tft.fillRect(238, 0, 20, 28, ILI9340_BLACK);
      tft.setCursor(238, 0); tft.setTextSize(4); tft.print(new_display_time[2]);
      prev_display_time[2] = new_display_time[2];
    }
    if(prev_display_time[3] != new_display_time[3]){
      tft.fillRect(270, 6, 15, 22, ILI9340_BLACK);
      tft.setCursor(270, 6); tft.setTextSize(3); tft.print(new_display_time[3]);
      prev_display_time[3] = new_display_time[3];
    }
    if(prev_display_time[4] != new_display_time[4]){
      tft.fillRect(288, 6, 15, 22, ILI9340_BLACK);
      tft.setCursor(288, 6); tft.setTextSize(3); tft.print(new_display_time[4]);
      prev_display_time[4] = new_display_time[4];
    }
  } else {
    // Screen was cleared, no need for rectangle blanking
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9340_WHITE);  tft.setTextSize(3);
    tft.print("Dive time: ");
    if(new_display_time[0]) {
      tft.setCursor(190, 0); tft.setTextSize(4); tft.print(new_display_time[0]);
    }
    prev_display_time[0] = new_display_time[0];
    if(new_display_time[1] || new_display_time[0]) {
      tft.setCursor(214, 0); tft.setTextSize(4); tft.print(new_display_time[1]);
    }
    prev_display_time[1] = new_display_time[1];
    tft.setCursor(238, 0); tft.setTextSize(4); tft.print(new_display_time[2]);
    prev_display_time[2] = new_display_time[2];
    tft.setCursor(253, 0); tft.setTextSize(4); tft.print(":");
    tft.setCursor(270, 6); tft.setTextSize(3); tft.print(new_display_time[3]);
    prev_display_time[3] = new_display_time[3];
    tft.setCursor(288, 6); tft.setTextSize(3); tft.print(new_display_time[4]);
    prev_display_time[4] = new_display_time[4];
  }
  
  // Display depth in meters
  new_display_depth[0] = (depth_dm/(1000))%10;
  new_display_depth[1] = (depth_dm/(100))%10;
  new_display_depth[2] = (depth_dm/(10))%10;
  new_display_depth[3] =  depth_dm%10;
  if(prev_display == DIVEMODEDISP){
    if(new_display_depth[0] != prev_display_depth[0]){
      tft.fillRect(95, 40, 35, 49, ILI9340_BLACK);
      if(new_display_depth[0]){
        tft.setCursor(95, 40); tft.setTextSize(7); tft.print(new_display_depth[0]);
      }
      prev_display_depth[0] = new_display_depth[0];
    }
    if(new_display_depth[1] != prev_display_depth[1]){
      tft.fillRect(137, 40, 35, 49, ILI9340_BLACK);
      if(new_display_depth[1] || new_display_depth[0]){
        tft.setCursor(137, 40); tft.setTextSize(7); tft.print(new_display_depth[1]);
      }
      prev_display_depth[1] = new_display_depth[1];
    }
    if(new_display_depth[2] != prev_display_depth[2]){
      tft.fillRect(179, 40, 35, 49, ILI9340_BLACK);
      tft.setCursor(179, 40); tft.setTextSize(7); tft.print(new_display_depth[2]);
      prev_display_depth[2] = new_display_depth[2];
    }
    if(new_display_depth[3] != prev_display_depth[3]){
      tft.fillRect(230, 54, 25, 35, ILI9340_BLACK);
      tft.setCursor(230, 54); tft.setTextSize(5); tft.print(new_display_depth[3]);
      prev_display_depth[3] = new_display_depth[3];
    }
  } else {
    // Screen was cleared, no need for rectangle blanking
    tft.setCursor(0, 40);
    tft.setTextColor(ILI9340_WHITE);  
    tft.setCursor(0, 40); tft.setTextSize(3); tft.println("Depth: ");
    if(new_display_depth[0]){
      tft.setCursor(95, 40); tft.setTextSize(7); tft.print(new_display_depth[0]);
    }
    prev_display_depth[0] = new_display_depth[0];
    if(new_display_depth[1] || new_display_depth[0]){
      tft.setCursor(137, 40); tft.setTextSize(7); tft.print(new_display_depth[1]);
    }
    prev_display_depth[1] = new_display_depth[1];
    tft.setCursor(179, 40); tft.setTextSize(7); tft.print(new_display_depth[2]);
    prev_display_depth[2] = new_display_depth[2];
    tft.setCursor(205, 54); tft.setTextSize(5); tft.print(".");
    tft.setCursor(230, 54); tft.setTextSize(5); tft.print(new_display_depth[3]);
    prev_display_depth[3] = new_display_depth[3];
    tft.setCursor(258, 68); tft.setTextSize(3); tft.print("msw");
  }
  
  // Update display variables
  prev_display = DIVEMODEDISP;
}

void drawSurfaceDisplay(){
  // FIXME - make this faster like the drawDiveDisplay() function!
  tft.fillScreen(ILI9340_BLACK);
  current_time = now();
  // Display current date
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);  tft.setTextSize(3);
  tft.print(year(current_time));
  tft.print("-");
  sprintf(s1, "%02d", month(current_time));
  tft.print(s1);
  tft.print("-");
  sprintf(s1, "%02d", day(current_time));
  tft.println(s1);
  tft.setCursor(20 ,40); tft.setTextSize(5);
  sprintf(s1, "%02d", hour(current_time));
  tft.print(s1);
  tft.print(":");
  sprintf(s1, "%02d", minute(current_time));
  tft.print(s1);
  tft.print(":");
  sprintf(s1, "%02d", second(current_time));
  tft.print(s1);
}

unsigned long testText() {
  tft.fillScreen(ILI9340_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9340_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9340_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9340_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9340_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(ILI9340_BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  tft.fillScreen(ILI9340_BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(ILI9340_BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(ILI9340_BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(ILI9340_BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(ILI9340_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(ILI9340_BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.Color565(0, 0, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.Color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.Color565(i, i, 0));
  }

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.Color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9340_BLACK);
  start = micros();
  for(i=min(tft.width(), tft.height()); i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.Color565(0, i, 0));
  }

  return micros() - start;
}
