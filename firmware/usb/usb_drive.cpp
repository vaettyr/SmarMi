#include "usb_drive.h"

USBDrive::USB_Drive(Adafruit_SPIFlash* flash) {
  _flash = flash;
  // bool fs_changed = false;
}

bool USBDrive::init(void) {
  _usb_msc.SetID("Boltcave", "SmarMi", "1.0"); // these should be passed in to mount the SD also
  _usb_msc.setReadWriteCallback(_read, _write, _flush);
  _usb_msc.setUnitReady(false);
  _usb.msc.begin();
  // Should we re-mount when the device restarts?
  // maybe this can be an explicit function
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
  if (!_flash->begin()) {
    return false;
  }
  unit32_t block_count _flash->sectorCount();
  _usb_msc.setCapacity(block_count, 512);
  _usb_msc.setUnitReady(true);
  return true;
}

uint32_t USBDrive::_read(uint32_t lba, void* buffer, uint32_t bufsize) {
  bool rc;
  rc = _flash->readSectors(lba, (uint8_t*) buffer, bufsize/512);
  return rc ? bufsize : -1;
}

uint32_t USBDrive::_write(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
  bool rc;  
  // digitalWrite(LED_BUILTIN, HIGH);
  rc = _flash->writeSectors(lba, buffer, bufsize/512);
  return rc ? bufsize : -1;
}

void USBDrive::_flush(void) {
onboardFlash.syncDevice();
  if (_flash->isCached()) {
    _flash->end();
    _flash->onboardFlash.begin();
  }
  // fs_changed = true;
  // digitalWrite(LED_BUILTIN, LOW);
}