#include "button.h"
#include <Arduino.h>

Button::Button(uint8_t pin) {
    this->last_button_time = 0;
    this->pressed = false;
    this->PIN = pin;
}

Button::~Button() {
}

void Button::init() const {
    pinMode(this->PIN, INPUT_PULLUP);
}

void Button::handle_pressed() {
    const unsigned long button_time = millis();
    if (button_time - last_button_time > 100) {
        this->pressed.store(true);
        last_button_time = button_time;
    }
}

bool Button::pop_pressed() {
    return this->pressed.exchange(false);
}
