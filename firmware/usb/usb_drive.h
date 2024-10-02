#ifndef usb_drive_h
#define usb_drive_h

#include <Adafruit_SPIFlash.h>
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <SdFat.h>
#include <SPI.h>

class USB_Drive {
  public:
    USB_Drive(Adafruit_SPIFlash* flash);
    USB_Drive(){};
    ~USB_Drive(){};
    bool init(void);
  private:
    Adafruit_SPIFlash* _flash;
    Adafruilt_USBD_MSC _usb_msc;
    // bool _fs_changed;
    uint32_t _read(uint32_t lba, void* buffer, uint32_t bufsize);
    uint32_t _write(uint32_t lba, uint8_t* buffer, uint32_t bufsize);
    void _flush(void);
}

#endif