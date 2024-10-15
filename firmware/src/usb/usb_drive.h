#ifndef usb_drive_h
#define usb_drive_h

#include <Adafruit_SPIFlash.h>
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <SdFat.h>
#include <SPI.h>

// these elements need to be static in order to be referenced by the tinyusb library

class USB_Drive {
  public:
    USB_Drive(Adafruit_SPIFlash* flash);
    USB_Drive(){};
    ~USB_Drive(){};
    void init(void);
  private:
    static Adafruit_SPIFlash* _flash;
    Adafruit_USBD_MSC _usb_msc;
    // bool _fs_changed;
    static int32_t _read(uint32_t lba, void* buffer, uint32_t bufsize);
    static int32_t _write(uint32_t lba, uint8_t* buffer, uint32_t bufsize);
    static void _flush(void);
};

#endif