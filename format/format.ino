#include <Adafruit_SPIFlash.h>    // SPI / QSPI flash library, should include sdfat I believe

Adafruit_FlashTransport_ESP32 flashTransport;
Adafruit_SPIFlash flash(&flashTransport); // onboard flash
FatVolume fatfs;
uint8_t  sectorBuffer[512];

bool formatFlash() {
  FatFormatter fatFormatter;
  bool rtn = fatFormatter.format(&flash, sectorBuffer, &Serial);
  Serial.print(F("formatting the drive was "));
  Serial.print(F(rtn ? "Successful\r\n" : "Unsuccessful\r\n"));
  return rtn;
}

void setup() {
  // Initialize serial port and wait for it to open before continuing.
  Serial.begin(115200);
  while (!Serial) delay(100);
  
  Serial.println(F("SmarMi onBoard flash formatting utility"));

  // Initialize flash library and check its chip ID.
  if (!flash.begin()) {
    Serial.println(F("Error, failed to initialize flash chip!"));
    while(1) yield();
  }

  Serial.print(F("Flash chip JEDEC ID: 0x")); Serial.println(flash.getJEDECID(), HEX);
  Serial.print(F("Flash size: ")); Serial.print(flash.size() / 1024); Serial.println(F(" KB"));
  Serial.print(F("Sector count: ")); Serial.print(flash.sectorCount()); Serial.println(F(" Sectors"));

  // Uncomment to flash LED while writing to flash
  flash.setIndicator(LED_BUILTIN, true);

  // Wait for user to send OK to continue.
  Serial.setTimeout(30000);  // Increase timeout to print message less frequently.
  do {
    Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    Serial.println(F("This sketch will ERASE ALL DATA on the flash chip and format it with a new filesystem!"));
    Serial.println(F("Type OK (all caps) and press enter to continue."));
    Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
  } while ( !Serial.find((char*) "OK"));

  // Call fatfs begin and passed flash object to initialize file system
  Serial.println(F("Creating and formatting FAT filesystem (this takes ~60 seconds)..."));

  bool formatted = formatFlash();

  if (formatted) {
    Serial.println(F("Flash chip successfully formatted with new empty filesystem!"));
    bool started = fatfs.begin(&flash);
    if (!started) {
      Serial.println(F("Failed to start new file partition"));
    } else {
      Serial.println(F("Successfully accessed new fat file system"));
      fatfs.end();
    }
  } else {
    Serial.println(F("Flash chip formatting failed"));
  }
}

void loop() {}