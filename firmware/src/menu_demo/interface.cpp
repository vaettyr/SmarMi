#include <Arduino.h>
#include "interface.h"

Button::Button(int pin, bool invert) {
  _invert = invert;
  _pin = pin;

  _currentState = _invert ? HIGH : LOW;
  _lastState = _invert ? HIGH : LOW;
  _lastDebounceTime = 0;
  _isHeld = false;
}

void Button::init() {
  pinMode(_pin, _invert ? INPUT_PULLUP : INPUT);
}

ButtonAction Button::GetState() {
  int reading = digitalRead(_pin);
  ButtonAction response = NO_ACTION;

  if (reading != _lastState) {
    _lastDebounceTime = millis();
  }

  unsigned long timeSinceDebounce = millis() - _lastDebounceTime;

  if (timeSinceDebounce > BOUNCE_DELAY) {
    bool isDown = !_invert ? reading == HIGH : reading == LOW;
    if (reading != _currentState) {
      _currentState = reading;
      if (!isDown) {
        if (_isHeld) {
          response = LONG_HOLD_RELEASE;
        } else {
          response = RELEASE;
        }
      } else {
        response = PRESS;
      }
      _isHeld = false;
    } else if (timeSinceDebounce > HOLD_THRESHOLD && !_isHeld && isDown) {
      _isHeld = true;
      response = LONG_HOLD_START;
    }
  }
  _lastState = reading;
  return response;
}

Interface::Interface(Button b0, Button b1, Button b2) {
  _b0 = b0;
  _b1 = b1;
  _b2 = b2;
  
  _state0 = NO_ACTION;
  _state1 = NO_ACTION;
  _state2 = NO_ACTION;
};

void Interface::init() {
  _b0.init();
  _b1.init();
  _b2.init();
};

void Interface::update() {
  _state0 = _b0.GetState();
  _state1 = _b1.GetState();
  _state2 = _b2.GetState();
  printState(_state0, '0');
  printState(_state1, '1');
  printState(_state2, '2');
};

ButtonAction Interface::State(int button) {
  switch(button) {
    case 0:
      return _state0;
    case 1:
      return _state1;
    case 2:
      return _state2;
    default:
      return NO_ACTION;
  }
};

bool Interface::isHeld(int button) {
  switch (button) {
    case 0:
      return _b0.isHeld();
    case 1:
      return _b1.isHeld();
    case 2:
      return _b2.isHeld();
    default:
      return false;
  }
};

void Interface::printState(ButtonAction state, char button) {
  switch (state) {
    case PRESS:
      Serial.print(button);
      Serial.print(" pressed\r\n");
      break;
    case RELEASE:
      Serial.print(button);
      Serial.print(" released\r\n");
      break;
    case LONG_HOLD_START:
      Serial.print(button);
      Serial.print(" long hold start\r\n");
      break;
    case LONG_HOLD_RELEASE:
      Serial.print(button);
      Serial.print(" long hold release\r\n");
      break;
  }
};