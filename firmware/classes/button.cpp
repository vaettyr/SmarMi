#include "Arduino.h”
#include "button.h"

Button::Button(bool invert, int pin) {
  _invert = invert;
  _pin = pin;

  _currentState = _invert ? HIGH : LOW;
  _lastState = _invert ? HIGH : LOW;
  _lastDebounceTime = 0;
  _isHeld = false;
}

Button::init() {
  pinMode(_pin, _invert ? INPUT_PULLUP : INPUT);
}

Button::GetState() {
  int reading = digitalRead(_pin);

  // If the switch changed, due to noise or pressing:
  if (reading != _lastState) {
    // reset the debouncing timer
    _lastDebounceTime = millis();
  }

  if ((millis() - _lastDebounceTime) > BOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != _currentState) {
      _currentState = reading;
      _isHeld = false;
      // depending on if it's inverted, we may return an event when this happens
      if ((_invert && _currentState == HIGH) || (!_invert && _currentState == LOW)) {
        // button was just released. check the time to see what event we should return
        return (millis() - _lastDebounceTime) < CLICK_THRESHOLD ? SHORT_CLICK : LONG_CLICK;
      }
    } else if (millis() - _lastDebounceTime > HOLD_THRESHOLD && !_isHeld) {
      _isHeld = true;
      return LONG_HOLD;
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  _lastState = reading;
  return NO_ACTION;
}