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
#include "flow.h"

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

FlowClient client("http://access-002.devnet52.nodes.onflow.org:8070");
/*
import MicrocontrollerTest from 0x0d3c8d02b02ceb4c
  access(all) fun main(): Int64 {
  return MicrocontrollerTest.ControlValue
}
*/
const String script = "aW1wb3J0IE1pY3JvY29udHJvbGxlclRlc3QgZnJvbSAweDBkM2M4ZDAyYjAyY2ViNGMKYWNjZXNzKGFsbCkgZnVuIG1haW4oKTogSW50NjQgewpyZXR1cm4gTWljcm9jb250cm9sbGVyVGVzdC5Db250cm9sVmFsdWUKfQ==";

void get_value(unsigned long block) {
    Serial.println("running script");

    JSONVar script_result = client.run_script(script, block);
        
    Serial.println("script done");
    String tmp = JSON.stringify(script_result);
    Serial.print("Value: ");
    Serial.println(tmp); 

    tft.setCursor(10, 120);
    tft.setTextSize(2);
    tft.println(tmp);
}

void loop() {
    tft.fillScreen(GC9A01A_BLACK);
    tft.setRotation(0);
    ensure_wifi_connected();
    
    //Serial.println("Hello Jan");
    tft.setCursor(40, 80);
    tft.setTextColor(GC9A01A_GREEN);
    tft.setTextSize(3);
    unsigned long block = get_latest_sealed_block();
    tft.println(block);
    Serial.printf("latest sealed block: %d\n", block);

    if(block > 0)
    {
        get_value(block);
    }

    delay(1000);
}