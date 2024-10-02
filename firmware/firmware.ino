
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions
#include <Adafruit_TinyUSB.h>     // USB functionalty to register as MSC

#include <Fonts/FreeSansBold12pt7b.h> // mono font

#include ".\\menu\\interface.h" // custom interface object
#include ".\\usb\\usb_drive.h" // usb mass storage object

#define USE_SD_CARD
#define SD_CS    13  // SD card select pin
#define EXT_TFT_DC 12
#define EXT_TFT_RST 11
#define EXT_TFT_CS 9
#define EXT_TFT_BACKLITE A5 // actual value is 8
#define EXT_TFT_I2C_POWER A4 // actual value is 14

Adafruit_ST7789 primary = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ST7789 secondary = Adafruit_ST7789(EXT_TFT_CS, EXT_TFT_DC, EXT_TFT_RST);

Adafruit_FlashTransport_ESP32 flashTransport;
Adafruit_SPIFlash onboardFlash(&flashTransport);

USBDrive onboardPartition = USBDrive(&onboardFlash);

FatVolume onboardFs; // Onboard flash file system;
Adafruit_ImageReader flashReader(onboardFs);

SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
bool hasSD = false;

csd_t csd;

void setup() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }
  Serial.print(F("SmarMi initializing"));

  // entire boot sequence should be
  // turn on both displays, turn off backlights, load initialization image, turn on backlight
  // that means the initialization image needs to be stored as a variable somehow. We'll figure it out
  // then we initialize the onboard flash
  // then we check the root directory of the onboard flash for a file (smarmi.txt?)
  // if we have one, read it and look for the images to load
  // if we don't have one we'll check to see if we can go into file sd card browser mode
  
  initDisplays();

  if (!onboardPartition.init()) {
    Serial.println(F("Failed to mount device as USB Mass Storage Device"));
    Serial.print(F("Are you sure the onboard flash is formatted?"));
    primary.fillScreen(ST77XX_RED);
    secondary.fillScreen(ST77XX_RED);
    while(1);
  }

  if (flashReader.drawBMP("/logo.bmp", primary, 23, 23)) {
    Serial.println(F("Failed to load initialization logo"));
    Serial.println(F("Are you sure the necessary boot files are loaded?"));
  }
  reader.drawBMP("/logo.bmp", secondary, 23, 23);
  primary.setFont(&FreeSansBold12pt7b);
  primary.setCursor(22, 152);
  primary.println("SmarMi");

  hasSD = SD.begin(SD_CS, SD_SCK_MHZ(10));

  Serial.println(F("Initialized"));

  // be able to read the directory from sd
  // be able to load images from the sd and display them
  // be able to communicate over bluetooth

}

void initDisplays() {
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  pinMode(EXT_TFT_BACKLITE, OUTPUT);
  digitalWrite(EXT_TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  pinMode(EXT_TFT_I2C_POWER, OUTPUT);
  digitalWrite(EXT_TFT_I2C_POWER, HIGH);

  delay(10);
  // initialize the onboard tft
  primary.init(135, 240); // Init ST7789 240x135  
  primary.setRotation(0);
  primary.fillScreen(ST77XX_BLACK);
  // initialize the second tft
  secondary.init(135, 240); // Init secondary display
  secondary.setRotation(2); // flipping for now because of how it's mounted on protoboard
  secondary.fillScreen(ST77XX_BLACK);
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
