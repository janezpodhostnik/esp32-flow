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

inline unsigned long get_latest_sealed_block() {
    unsigned long result = 0;

    HTTPClient http;
    http.begin(flowRestAccess + "/v1/blocks?height=sealed");
    const int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
        String payload = http.getString();

        JSONVar myObject = JSON.parse(payload);

        // JSON.typeof(jsonVar) can be used to get the type of the var
        if (JSON.typeof(myObject) != "undefined") {
            result = atol(myObject[0]["header"]["height"]);

            Serial.print("block: ");
            Serial.println(result);
        } else {
            Serial.println("Parsing input failed!");
        }
    } else {
        Serial.print("Get sealed block error code: ");
        Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();

    return result;
}

const String postBody = R"({
    "script": "aW1wb3J0IE1pY3JvY29udHJvbGxlclRlc3QgZnJvbSAweDBkM2M4ZDAyYjAyY2ViNGMKCmFjY2VzcyhhbGwpIGZ1biBtYWluKCk6IEludDY0IHsKICByZXR1cm4gTWljcm9jb250cm9sbGVyVGVzdC5Db250cm9sVmFsdWUKfQ==",
    "arguments": []
})";

bool current_led_state = false;

inline bool get_led_state_at_block(FlowClient *client, unsigned long block_height) {
    JSONVar script_result = client->run_script(script, block_height);

    if (script_result.hasOwnProperty("value")) {
        current_led_state = static_cast<bool>(script_result["value"]);
        Serial.print("current_led_state: ");
        Serial.println(current_led_state);
    } else {
        Serial.print("Get led state error code: ");
        Serial.println(client->httpResponseCode);
    }

    return current_led_state;
}
