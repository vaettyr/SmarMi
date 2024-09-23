
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

#define USE_SD_CARD
#define SD_CS    10  // SD card select pin

// Use dedicated hardware SPI pins
Adafruit_ST7789 primary = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
// write to the external display
Adafruit_ST7789 secondary = Adafruit_ST7789(T5, T6, T9);

SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys

Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;

csd_t csd;

void setup() {
  

  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }
  Serial.println(F("SmarMi initializing"));
  initDisplays();
  initSD();

  Serial.println(F("Initialized"));

  // be able to read the directory from sd
  // be able to load images from the sd and display them
  // be able to communicate over bluetooth

}

void initDisplays() {
  // turn on backlite
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
  // initialize the second tft
  secondary.init(135, 240); // Init secondary display
  secondary.setRotation(2);
  secondary.fillScreen(ST77XX_BLACK);
}

void initSD() {
  ImageReturnCode stat; // Status from image-reading functions
  if(!SD.begin(SD_CS, SD_SCK_MHZ(10))) { // Breakouts require 10 MHz limit due to longer wires
    Serial.println(F("SD begin() failed"));
    for(;;); // Fatal error, do not continue
  }
  Serial.println(F("OK!"));
  SD.card()->readCSD(&csd);
  switch (SD.card()->type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println(F("SD1"));
      break;

    case SD_CARD_TYPE_SD2:
      Serial.println(F("SD2"));
      break;

    case SD_CARD_TYPE_SDHC:
      if (csd.capacity() < 70000000) {
        Serial.println(F("SDHC"));
      } else {
        Serial.println(F("SDXC"));
      }
      break;

    default:
      Serial.println(F("OOPS"));
  }


  Serial.print(F("Loading minerva.bmp to screen..."));
  stat = reader.drawBMP("/minerva.bmp", primary, 0, 0);
  reader.printStatus(stat);   // How'd we do?
  stat = reader.drawBMP("/minerva.bmp", secondary, 0, 0);
  reader.printStatus(stat);

}

void loop() {
  // put your main code here, to run repeatedly:

}
