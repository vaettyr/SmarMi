#include "Arduino.h”
#include "button.h"

Button::Button(bool invert, int pin, unsigned long bounceDelay, unsigned long clickThreshold, unsigned long holdThreshold) {
  _invert = invert;
  _pin = pin;
  _bounceDelay = bounceDelay;
  _clickThreshold = clickThreshold;
  _holdThreshold = holdThreshold;

  _currentState = _invert ? HIGH : LOW;
  _lastState = _invert ? HIGH : LOW;
  _lastDebounceTime = 0;

  pinMode(_pin, _invert ? INPUT_PULLUP : INPUT);
}

Button::GetState() {
  int reading = digitalRead(_pin);


  // If the switch changed, due to noise or pressing:
  if (reading != _lastState) {
    // reset the debouncing timer
    _lastDebounceTime = millis();
  }

  if ((millis() - _lastDebounceTime) > _bounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != _currentState) {
      _currentState = reading;
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  _lastState = reading;
}