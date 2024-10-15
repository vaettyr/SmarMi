#include <Arduino.h>
#include "menu.h"
#include <Fonts/FreeSansBold12pt7b.h> // mono font

const char* Menu::_options[] = {
  "pair", // or unpair. This actually shouldn't be a const, it can change
  "SD", // file browsing mode
  "USB", // mount as a usb storage device. Needed?
  "Wifi" // does this actually do anything?
};

Menu::Menu(Adafruit_ST7789* display, Interface* input) {
  _display = display;
  _input = input;
  _isActive = false;
  _index = 0;
}

bool Menu::begin() {
  _isActive = true;
  refresh();
  return true;
}

bool Menu::end() {
  _isActive = false;
  return true;
}

void Menu::tick() {
  if (!_isActive) return;

  int lastIndex = _index;
  ButtonAction b1 = _input->State(1);
  ButtonAction b2 = _input->State(2);
  if(b2 == PRESS && _index < 4) {
    _index++;
  }
  if(b1 == PRESS && _index > 0) {
    _index--;
  }
  if (_index != lastIndex) {
    refresh();
  }
}

void Menu::refresh() {
  _display->fillScreen(ST77XX_BLACK);
  _display->setFont(&FreeSansBold12pt7b); // font is 16px tall
  // _display->setTextSize(2);
  for(int x; x < 5; x++) {
    _display->setCursor(5, x * 24 + 20);
    _display->setTextColor(_index != x ? ST77XX_WHITE : ST77XX_BLACK);
    if(_index == x) {
      _display->fillRect(5, x * 24 + 2, 125, 22, ST77XX_WHITE);
    }
    _display->println(_options[x]);
  }
}