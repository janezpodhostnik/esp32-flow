#include <WiFi.h>
#include <HTTPClient.h>
// examples for json get https://randomnerdtutorials.com/esp32-http-get-post-arduino/
#include <Arduino_JSON.h>
#include <arduino_base64.hpp>
#include <string>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
//mine
#include "wifi_credentials.h"

//values from from TFT_eSPI, User_Setup_Select.h 
Adafruit_GC9A01A tft(9, 8, 11, 10, 14, 12);

const String flowRestAccess = "http://access-002.devnet52.nodes.onflow.org:8070";

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

            //Serial.print("block: ");
            //Serial.println(result);
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

void setup() {
    Serial.begin(115200);
    //set monitor backlight
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    //init monitor
    tft.begin();
    tft.fillScreen(GC9A01A_BLACK);
    tft.setCursor(50, 110);
    tft.setTextColor(GC9A01A_GREEN);
    tft.setTextSize(3);
    tft.println("Flow IoT");

    ensure_wifi_connected();
    delay(3000);
}

void loop() {
    tft.fillScreen(GC9A01A_BLACK);
    tft.setRotation(0);
    ensure_wifi_connected();
    
    //Serial.println("Hello Jan");
    tft.setCursor(40, 110);
    tft.setTextColor(GC9A01A_GREEN);
    tft.setTextSize(3);
    unsigned long block = get_latest_sealed_block();
    tft.println(block);
    Serial.printf("latest sealed block: %d\n", block);

    delay(1000);
}