#include "display.h"
#include "wifi.h"
#include "button.h"
#include "flow.h"

#define LED_PIN 10
#define EXTERNAL_LED_PIN 0
#define BUTTON_PIN 1

Button button(BUTTON_PIN);
FlowClient client("http://access-002.devnet52.nodes.onflow.org:8070");

void IRAM_ATTR isr() {
    button.handle_pressed();
}

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);
    pinMode(EXTERNAL_LED_PIN, OUTPUT);

    button.init();
    attachInterrupt(button.PIN, isr, FALLING);

    // set led to high so that we know its on!
    digitalWrite(LED_PIN, HIGH);
    // ensure external leds start low
    digitalWrite(EXTERNAL_LED_PIN, LOW);

    ensure_wifi_connected();
    display_begin();
}

void loop() {
    ensure_wifi_connected();

    Block latest_block = client.get_latest_sealed_block();
    if (latest_block.is_zero()) {
        // failed getting latest block
        Serial.print("Failed getting latest block. http error code:");
        Serial.println(client.httpResponseCode);

        delay(1000);
        return;
    }
    Serial.print("latest sealed block: ");
    Serial.println(latest_block.height);
    lcd_display_info(latest_block.height);

    const bool led_state = get_led_state_at_block(&client, latest_block.height);
    digitalWrite(EXTERNAL_LED_PIN, led_state ? HIGH : LOW);

    if (button.pop_pressed()) {
        Serial.println("Button has been pressed");
        // TODO: send transaction
    }

    delay(100);
}
