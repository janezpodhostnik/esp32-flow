#include "display.h"
#include "wifi.h"

#define LED_PIN 10
#define EXTERNAL_LED_PIN 0

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(EXTERNAL_LED_PIN, OUTPUT);

    // ensure leds start low
    digitalWrite(LED_PIN, LOW);
    digitalWrite(EXTERNAL_LED_PIN, LOW);

    ensure_wifi_connected();
    display_begin();
}

void loop() {
    ensure_wifi_connected();

    const unsigned long latest_block = get_latest_sealed_block();
    if (latest_block == 0) {
        // failed getting latest block
        delay(1000);
        return;
    }

    lcd_display_info(latest_block);

    const bool led_state = get_led_state_at_block(latest_block);
    digitalWrite(EXTERNAL_LED_PIN, led_state ? HIGH : LOW);

    delay(100);
}
