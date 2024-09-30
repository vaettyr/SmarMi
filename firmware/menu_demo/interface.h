#ifndef interface_h
#define interface_h

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
    Button(int, bool = false);
    Button(){};
    ~Button(){};
    ButtonAction GetState(void);
    void init(void);
    bool isHeld(void) { return _isHeld; };
  private:
    bool _invert;
    int _pin;
    bool _isHeld;
    int _currentState;
    int _lastState;
    unsigned long _lastDebounceTime;
};

class Interface {
  public:
    Interface(Button b0, Button b1, Button b2);
    void init(void);
    void update(void);
    bool isHeld(int);
    ButtonAction State(int);
  private:
    Button _b0;
    Button _b1;
    Button _b2;
    ButtonAction _state0;
    ButtonAction _state1;
    ButtonAction _state2;
    void printState(ButtonAction, char);
};

#endif