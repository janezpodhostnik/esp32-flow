#include "display.h"
#include "wifi.h"

#define LED_PIN 10
#define EXTERNAL_LED_PIN 0

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(EXTERNAL_LED_PIN, OUTPUT);
    // ensure led starts low
    digitalWrite(LED_PIN, LOW);
    digitalWrite(EXTERNAL_LED_PIN, LOW);

    ensure_wifi_connected();
    display_begin();

    delay(100);
}
unsigned long latest_block = 0;

void loop() {
    ensure_wifi_connected();

    latest_block = get_latest_sealed_block();
    if (latest_block != 0) {
        lcd_display_info(latest_block);
    }

    bool led_state = get_led_state_at_block(latest_block);
    if (led_state) {
        digitalWrite(EXTERNAL_LED_PIN, HIGH);
    } else {
        digitalWrite(EXTERNAL_LED_PIN, LOW);
    }
    Serial.print("test");

    delay(100);
}
