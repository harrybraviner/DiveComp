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

#define SERIAL_DEBUG

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

char serialString[201];

void setup() {
  Serial.begin(9600);
  while (!Serial);
 
  // Setup the display
  tft.begin();
  tft.setRotation(1);  // Want a landscape display

  // Initialise dive variables
  depth_dm = 0;
  dive_time_s = 0;
  
  // Initialise the time to about now-ish
  setTime(17, 57, 0, 8, 05, 2014);

  drawDiveDisplay();
  delay(500);
}

void loop(void) {
    /*if(depth_dm == 5) depth_dm = 4995;
    else depth_dm = 5;
    drawDiveDisplay();*/
    drawSurfaceDisplay();
    delay(100);
    dive_time_s++;
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

void serialEvent(){
  int i=0;
  while(Serial.available() && i <200){
    serialString[i] = (char)Serial.read();
    i++;  // Make sure we don't run past te end of the serialString
  }
  serialString[i] = '\0';

#ifdef SERIAL_DEBUG
  if(i>0 && serialString[0] == 'D'){
      sscanf(serialString, "D %ud", &depth_dm);
  }
#endif
  if(i>0 && serialString[0] == 'T'){
    int hour, minute, second, date, month, year;
    sscanf(serialString, "T %d %d %d %d %d %d", &hour, &minute, &second, &date, &month, &year);
    setTime(hour, minute, second, date, month, year);
  }
  
  serialString[0] = '\0';
}
