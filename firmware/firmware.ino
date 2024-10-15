
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SdFat.h>                // SD card & FAT filesystem library
#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library
#include <Adafruit_ImageReader.h> // Image-reading functions
// #include <Adafruit_TinyUSB.h>     // USB functionalty to register as MSC

#include <Fonts/FreeSansBold12pt7b.h> // mono font

#include <WebServer.h>
#include <WiFi.h>

#include "src/menu_demo/menu.h" // custom interface object
#include "src/usb/usb_drive.h" // usb mass storage object

// actual value is 8 - A5
// actual value is 14 - A4
#define USE_SD_CARD
#define SD_CS    13
#define EXT_TFT_BACKLITE A5 
#define EXT_TFT_I2C_POWER A4 
#define EXT_TFT_DC 12
#define EXT_TFT_RST 11
#define EXT_TFT_CS 9

// Adafruit_ST7789 primary = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
// Adafruit_ST7789 secondary = Adafruit_ST7789(EXT_TFT_CS, EXT_TFT_DC, EXT_TFT_RST);

Adafruit_FlashTransport_ESP32 flashTransport;
Adafruit_SPIFlash onboardFlash(&flashTransport);
FatVolume onboardFs; // Onboard flash file system;
// this usb drive is going to need some work still
USB_Drive onboardPartition = USB_Drive(&onboardFlash);

// Adafruit_ImageReader flashReader(onboardFs);

// SdFat                SD;         // SD card filesystem
// Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys
// bool hasSD = false;

csd_t csd;

Interface interface = Interface(Button(0, true), Button(1), Button(2));
// Menu menu = Menu(&primary, &interface);

WebServer server(80);

void setup() {
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  }
  Serial.println(F("SmarMi initializing"));
  
  // initDisplays();
  onboardFlash.begin();
  onboardFs.begin(&onboardFlash);
  initWifi();

/*
  ImageReturnCode stat = flashReader.drawBMP("/sys/logo.bmp", primary, 23, 23);
  if ( stat != IMAGE_SUCCESS) {
    flashReader.printStatus(stat);
    Serial.println(F("Failed to load initialization logo"));
    Serial.println(F("Are you sure the necessary boot files are loaded?"));
  }

  flashReader.drawBMP("/sys/logo.bmp", secondary, 23, 23);
  primary.setFont(&FreeSansBold12pt7b);
  primary.setCursor(22, 152);
  primary.println("SmarMi");

  hasSD = SD.begin(SD_CS, SD_SCK_MHZ(10));
 */
  interface.init();

  onboardPartition.init();

  Serial.println(F("Initialized"));

  // be able to read the directory from sd
  // be able to load images from the sd and display them
  // be able to communicate over bluetooth

}

void initWifi() {
  File32 config = onboardFs.open("wifi.txt");
  char line[41]; // 32 char max for ssid and password + 9 for "password="
  String parsed;
  String password;
  String ssid;
  if (!config) {
    Serial.println("No config file found");
    return;
  }
  while (config.available()) {
    Serial.print("Reading line: ");
    int n = config.fgets(line, sizeof(line));
    if (n <= 0) {
      Serial.println(F("Failed to read line"));
    } else if (line[n-1] != '\n' && n == (sizeof(line) - 1)) {
      Serial.println(F("Line too long"));
    } else {
      parsed = line;
      if (parsed.startsWith("ssid=")) {
        ssid = parsed.substring(5);
        Serial.print(F("SSID: "));
        Serial.println(ssid);
      } else if(parsed.startsWith("password=")) {
        password = parsed.substring(9);
        Serial.print(F("Password: "));
        Serial.println(password);
      }
    }
  }
  config.close();
  if (sizeof(password) == 0 || sizeof(ssid) == 0) {
    return;
  }
  WiFi.setHostname("smarmi");
  Serial.print("Connecting to wifi...");
  WiFi.begin(ssid, password);
  uint8_t status = WiFi.waitForConnectResult();
  Serial.println(status);
  if (status == WL_CONNECTED) {
    server.on("/", HTTP_GET, handleRequest);
    server.begin();
  }
}

void handleRequest() {
  String response = "OK";
  server.send(200, "text/html; charset=utf-8", response);
}

/*
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
*/
void loop() {
  interface.update();
  server.handleClient();
  /*
  if (!menu.active()){
    if (interface.State(0) == LONG_HOLD_START) {
      menu.begin();
    }
  }
  menu.tick();
  */
}
