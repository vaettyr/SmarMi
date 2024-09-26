#ifndef button_h
#define button_h
#include "Arduino.h"

#define BOUNCE_DELAY = 50UL;
#define CLICK_THRESHOLD = 500UL;
#define HOLD_THRESHOLD = 2000UL;

enum ButtonAction {
  NO_ACTION,
  SHORT_CLICK,
  LONG_CLICK,
  LONG_HOLD
};

class Button {
// class code goes here
  public:
    Button(bool, int);
    ButtonAction GetState(void);
    void init(void);
  private:
    bool _invert;
    int _pin;
    bool _isHeld;
    int _currentState;
    int _lastState;
    unsigned long _lastDebounceTime;
};


#endif