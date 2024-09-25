
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

#include "classes/button.h" // custom button object

#define USE_SD_CARD
#define SD_CS    10  // SD card select pin

// displays
Adafruit_ST7789 primary = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ST7789 secondary = Adafruit_ST7789(T5, T6, T9);
// onboard flash
Adafruit_FlashTransport_ESP32 flashTransport;
Adafruit_SPIFlash onboardFlash(&flashTransport);
// external SD card
SdFat                SD;         // SD card filesystem
// image file reader (might need to read from different places. Create as needed?)
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys

Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;

csd_t csd;

int d0State = 1, d0LastState = 1, d1State = 0, d1LastState = 0, d2State = 0, d2LastState = 0;

void setup() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }
  // entire boot sequence should be
  // turn on both displays, turn off backlights, load initialization image, turn on backlight
  // that means the initialization image needs to be stored as a variable somehow. We'll figure it out
  // then we initialize the onboard flash
  // then we check the root directory of the onboard flash for a file (smarmi.txt?)
  // if we have one, read it and look for the images to load
  // if we don't have one we'll check to see if we can go into file sd card browser mode
  Serial.print(F("SmarMi initializing"));
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT);
  pinMode(2, INPUT);
  initDisplays();
  Serial.print(F(" ."));
  initFlash();
  Serial.print(F(" ."));
  initSD();
  Serial.print(F(" ."));
  Serial.print("\r\n");

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

void initFlash() {
  // checking to see if we have access to on-board flash memory
  Serial.print("Starting up onboard QSPI Flash...");
  onboardFlash.begin();
  Serial.println("Done");
  Serial.println("Onboard Flash information");
  Serial.print("JEDEC ID: 0x");
  Serial.println(onboardFlash.getJEDECID(), HEX);
  Serial.print("Flash size: ");
  Serial.print(onboardFlash.size() / 1024);
  Serial.println(" KB");
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

void getButtonState() {
  d0State = digitalRead(0);
  if (d0State != d0LastState) {
    // d0 button was toggled
    if (d0State == 0) {
      Serial.println(F("d0 button is pressed down"));
    } else {
      Serial.println(F("d0 button is released"));
    }
  }
  d1State = digitalRead(1);
  if (d1State != d1LastState) {
    // d0 button was toggled
    if (d1State == 1) {
      Serial.println(F("d1 button is pressed down"));
    } else {
      Serial.println(F("d1 button is released"));
    }
  }
  d2State = digitalRead(2);
  if (d2State != d2LastState) {
    // d0 button was toggled
    if (d2State == 1) {
      Serial.println(F("d2 button is pressed down"));
    } else {
      Serial.println(F("d2 button is released"));
    }
  }
  d0LastState = d0State;
  d1LastState = d1State;
  d2LastState = d2State;
  // d1State = digitalRead(1);
  // d2State = digitalRead(2);
  // states this can return
  // click
  // long click
  // long hold
}

void loop() {
  // put your main code here, to run repeatedly:
  getButtonState();
}
