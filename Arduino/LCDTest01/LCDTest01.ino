/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     6
#define TFT_RST    8  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     7
#define TFT_SCLK   4   // set these to be whatever pins you like!
#define TFT_MOSI   5   // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


float p = 3.1415926;

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! ST7735 TFT Test");

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  Serial.println("Initialized");

  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  // large block of text
  tft.fillScreen(ST7735_BLACK);
  delay(1000);
  tftPrintTest();
  delay(4000);
}

void loop() {
  tftPrintTest();
  delay(1000);
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setCursor(15, 20);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(12);
  tft.println("TV");
  delay(1000);
  
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setCursor(15, 30);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(8);
  tft.println("MP3");
  
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setCursor(15, 30);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(8);
  tft.println("VOL");
  
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  
  tft.setCursor(15, 30);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(8);
  tft.println("CHL");
  
  for(int i=0;i<200;i+=31) {
  tft.setRotation(1);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(15, 30);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(8);
  tft.print(i);
  delay(200);
  }
}

void mediabuttons() {
  // play
  tft.fillScreen(ST7735_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST7735_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_GREEN);
}

