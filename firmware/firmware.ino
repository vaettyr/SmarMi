
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions

#include <Fonts/FreeSansBold12pt7b.h> // mono font

#include "button.h" // custom button object

#define USE_SD_CARD
#define SD_CS    13  // SD card select pin


// PIN 13 IS SD CS
#define EXT_TFT_DC 12
// 12 is TFT EXT DC
#define EXT_TFT_RST 11
// 11 is EXT TFT RST
#define EXT_TFT_CS 9
// 9 is EXT TFT CS
// A5 is EXT TFT Brightness
// A4 is enable the entire external TFT/SD
// displays
// it's a shitty breadboard. If something's not working, check the wires
Adafruit_ST7789 primary = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
// Adafruit_ST7789 secondary = Adafruit_ST7789(10, 6, 9); // old pins
Adafruit_ST7789 secondary = Adafruit_ST7789(EXT_TFT_CS, EXT_TFT_DC, EXT_TFT_RST);
// onboard flash
Adafruit_FlashTransport_ESP32 flashTransport;
Adafruit_SPIFlash onboardFlash(&flashTransport);
// File Volume on the onboard flash
FatVolume onboardFs; // Onboard flash file system;
Adafruit_ImageReader flashReader(onboardFs);

SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys

Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;

csd_t csd;

Button interface[] = {Button(true, 0), Button(false, 1), Button(false, 2)};
ButtonAction state[] = {NO_ACTION, NO_ACTION, NO_ACTION};

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


  interface[0].init();
  interface[1].init();
  interface[2].init();
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
  pinMode(A5, OUTPUT); // External TFT brightness
  digitalWrite(A5, HIGH);

  pinMode(A4, OUTPUT); // External TFT and SD Enable
  digitalWrite(A4, HIGH);

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
  // I bet this is because I'm not setting the pin modes?
  secondary.init(135, 240); // Init secondary display
  secondary.setRotation(2); // flipping for now because of how it's mounted on protoboard
  secondary.fillScreen(ST77XX_BLACK);
}

void initFlash() {
  // checking to see if we have access to on-board flash memory
  ImageReturnCode stat;
  Serial.print("Starting up onboard QSPI Flash...");
  onboardFlash.begin();
  Serial.println("Done");
  Serial.println("Onboard Flash information");
  Serial.print("JEDEC ID: 0x");
  Serial.println(onboardFlash.getJEDECID(), HEX);
  Serial.print("Flash size: ");
  Serial.print(onboardFlash.size() / 1024);
  Serial.println(" KB");
  if(!onboardFs.begin(&onboardFlash)) {
    Serial.println("Error initializing onboard flash filesystem");
  } else {
    Serial.print(F("Loading logo.bmp to second screen..."));
    stat = flashReader.drawBMP("/logo.bmp", secondary, 23, 23);
    flashReader.printStatus(stat);
  }
}

void initSD() {
  ImageReturnCode stat; // Status from image-reading functions
  if(!SD.begin(SD_CS, SD_SCK_MHZ(10))) { // Breakouts require 10 MHz limit due to longer wires
    Serial.println(F("SD begin() failed"));
  } else {
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
    Serial.print(F("Loading logo.bmp to screen..."));
    stat = reader.drawBMP("/logo.bmp", primary, 23, 23);
    reader.printStatus(stat);   // How'd we do?
    primary.setFont(&FreeSansBold12pt7b);
    primary.setCursor(22, 152);
    primary.println("SmarMi");
  }

  
  // stat = reader.drawBMP("/minerva.bmp", secondary, 0, 0);
  // reader.printStatus(stat);
}

void getButtonState() {
  state[0] = interface[0].GetState();
  state[1] = interface[1].GetState();
  state[2] = interface[2].GetState();
  printButtonState(state[0], '0');
  printButtonState(state[1], '1');
  printButtonState(state[2], '2');
}

void printButtonState(ButtonAction state, char button) {
  switch (state) {
    case PRESS:
      Serial.print(button);
      Serial.print(" pressed\r\n");
      break;
    case RELEASE:
      Serial.print(button);
      Serial.print(" released\r\n");
      break;
    case LONG_HOLD_START:
      Serial.print(button);
      Serial.print(" long hold start\r\n");
      break;
    case LONG_HOLD_RELEASE:
      Serial.print(button);
      Serial.print(" long hold release\r\n");
      break;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  getButtonState();
  if (state[0] == PRESS) {
    ImageReturnCode stat;
    Serial.print(F("Loading minerva to screen 1"));
    stat = reader.drawBMP("/minerva.bmp", primary, 0, 0);
    reader.printStatus(stat);
    Serial.print(F("Loading minerva to screen 2"));
    stat = reader.drawBMP("/minerva.bmp", secondary, 0, 0);
    reader.printStatus(stat);
  }
}
