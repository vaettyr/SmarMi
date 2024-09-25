#ifndef button_h
#define button_h
#include "Arduino.h"

enum ButtonAction {
  NO_ACTION,
  SHORT_CLICK,
  LONG_CLICK,
  LONG_HOLD
}

class Button {
// class code goes here
  public:
    Button(bool, int, unsigned long, unsigned long, unsigned long);
    ButtonAction GetState(void);
  private:
    bool _invert;
    int _pin;
    int _currentState;
    int _lastState;
    unsigned long _lastDebounceTime;
    unsigned long _bounceDelay;
    unsigned long _clickThreshold;
    unsigned long _holdThreshold;
};


#endif