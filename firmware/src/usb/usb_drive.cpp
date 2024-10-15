#include "usb_drive.h"

USB_Drive::USB_Drive(Adafruit_SPIFlash* flash) {
  _flash = flash;
  // bool fs_changed = false;
  _usb_msc = Adafruit_USBD_MSC();
};

Adafruit_SPIFlash* USB_Drive::_flash;

void USB_Drive::init(void) {
  _usb_msc.setID("Boltcave", "SmarMi", "1.0"); // these should be passed in to mount the SD also
  _usb_msc.setReadWriteCallback(USB_Drive::_read, USB_Drive::_write, USB_Drive::_flush);
  _usb_msc.setUnitReady(false);
  _usb_msc.begin();
  // Should we re-mount when the device restarts?
  // maybe this can be an explicit function
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  int block_count = _flash->sectorCount();
  _usb_msc.setCapacity(block_count, 512);
  _usb_msc.setUnitReady(true);
};

int32_t USB_Drive::_read(uint32_t lba, void* buffer, uint32_t bufsize) {
  bool rc;
  rc = _flash->readSectors(lba, (uint8_t*) buffer, bufsize/512);
  return rc ? bufsize : -1;
};

int32_t USB_Drive::_write(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
  bool rc;  
  // digitalWrite(LED_BUILTIN, HIGH);
  rc = _flash->writeSectors(lba, buffer, bufsize/512);
  return rc ? bufsize : -1;
};

void USB_Drive::_flush(void) {
  _flash->syncDevice();
  if (_flash->isCached()) {
    _flash->end();
    _flash->begin();
  }
  // fs_changed = true;
  // digitalWrite(LED_BUILTIN, LOW);
};