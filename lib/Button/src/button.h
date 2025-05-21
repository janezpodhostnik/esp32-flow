#pragma once
#include <stdint.h>
#include <atomic>

class Button {
public:
    uint8_t PIN;

    explicit Button(uint8_t pin);

    ~Button();

    void init() const;

    void handle_pressed();

    bool pop_pressed();

private:
    unsigned long last_button_time;
    std::atomic<int16_t> pressed;
};
