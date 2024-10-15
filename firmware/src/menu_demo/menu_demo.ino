
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "interface.h"
#include "menu.h"

Adafruit_ST7789 primary = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Button b0 = Button(0,true);
Button b1 = Button(1);
Button b2 = Button(2);
Interface inputs(b0,b1,b2);
Menu menu = Menu(&primary, &inputs);

void setup() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }

  Serial.print(F("Menu demo initializing"));
  inputs.init();

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  // set this for secondary backlite also

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  // without a transistor I don't think we can turn off the secondary display?
  // maybe hook one up if there's room. Otherwise we can just toggle the backlight and write black
  // can use an analog pin for this since it takes PWM?
  delay(10);
  // initialize the onboard tft
  primary.init(135, 240); // Init ST7789 240x135  
  primary.setRotation(0);
  primary.fillScreen(ST77XX_BLACK);
  menu.begin();
}

void loop() {
  menu.tick();
}