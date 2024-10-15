#ifndef menu_h
#define menu_h

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "interface.h" // interface object

class Menu {
  public:
    Menu(Adafruit_ST7789* display, Interface* input);
    bool begin(void);
    bool end(void);
    void tick(void);
    bool active() { return _isActive; };
  private:
    Adafruit_ST7789* _display;
    Interface* _input;
    bool _isActive;
    int _index;
    void refresh(void);
    // how do we track what our options are?
    // how do we also track where we are on those options?
    static const char* _options[];
};

#endif