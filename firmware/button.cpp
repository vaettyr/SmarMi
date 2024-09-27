#include <Arduino.h>
#include "button.h"

Button::Button(bool invert, int pin) {
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