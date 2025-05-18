#pragma once

#include <WiFi.h>

#include "flow.h"
#include "wifi_credentials.h"

inline void ensure_wifi_connected() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(SSID, PASSWORD);
        Serial.println("Connecting");
        while (WiFi.status() != WL_CONNECTED) {
            delay(750);
            Serial.print(".");
        }
        Serial.println("");
        Serial.println("connected");
    }
}

const String script =
        "aW1wb3J0IE1pY3JvY29udHJvbGxlclRlc3QgZnJvbSAweDBkM2M4ZDAyYjAyY2ViNGMKCmFjY2VzcyhhbGwpIGZ1biBtYWluKCk6IEludDY0IHsKICByZXR1cm4gTWljcm9jb250cm9sbGVyVGVzdC5Db250cm9sVmFsdWUKfQo=";

bool current_led_state = false;

inline bool get_led_state_at_block(FlowClient *client, unsigned long block_height) {
    JSONVar script_result = client->run_script(script, block_height);

    if (script_result.hasOwnProperty("value")) {
        current_led_state = atol(script_result["value"]) > 0;
        Serial.print("current_led_state: ");
        Serial.println(current_led_state);
    } else {
        Serial.print("Get led state error code: ");
        Serial.println(client->httpResponseCode);
        Serial.println(script_result);
    }

    return current_led_state;
}
