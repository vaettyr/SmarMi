#ifndef button_h
#define button_h

#include <Arduino.h>

#define BOUNCE_DELAY 50UL
#define HOLD_THRESHOLD 2000UL

enum ButtonAction {
  NO_ACTION,
  PRESS,
  RELEASE,
  LONG_HOLD_START,
  LONG_HOLD_RELEASE
};

class Button {
  public:
    Button(bool, int);
    ButtonAction GetState(void);
    void init(void);
    bool isHeld(void) { return _isHeld; }
  private:
    bool _invert;
    int _pin;
    bool _isHeld;
    int _currentState;
    int _lastState;
    unsigned long _lastDebounceTime;
};

#endif