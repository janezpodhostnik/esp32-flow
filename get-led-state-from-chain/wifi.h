#pragma once

#include <WiFi.h>
#include <HTTPClient.h>
// examples for json get https://randomnerdtutorials.com/esp32-http-get-post-arduino/
#include <Arduino_JSON.h>
#include "arduino_base64.hpp"

#include "WiFiCredentials.h"

String flowRestAccess = "http://access-002.devnet52.nodes.onflow.org:8070";

void ensure_wifi_connected() {
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

unsigned long get_latest_sealed_block() {
  unsigned long result = 0;

  HTTPClient http;
  http.begin(flowRestAccess + "/v1/blocks?height=sealed");
  int httpResponseCode = http.GET();

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

String postBody = R"({
    "script": "aW1wb3J0IE1pY3JvY29udHJvbGxlclRlc3QgZnJvbSAweDRhZjI0YTQ5Njg4ZWFhOTIKCmFjY2VzcyhhbGwpIGZ1biBtYWluKCk6IEJvb2wgewogIHJldHVybiBNaWNyb2NvbnRyb2xsZXJUZXN0LkxlZE9uCn0=",
    "arguments": []
})";

bool current_led_state = false;

bool get_led_state_at_block(unsigned long  block) {
  HTTPClient http;
  http.begin(flowRestAccess + "/v1/scripts?block_height="+block);

  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(postBody);
  if (httpResponseCode > 0) {
    String  input = http.getString();
    input.remove(input.length()-1,1);
    input.remove(0,1);

    const char* input2 = input.c_str();
    uint8_t output[base64::decodeLength(input2)];
    base64::decode(input2, output);

    JSONVar myObject = JSON.parse((const char*)output);


    current_led_state = (bool) myObject["value"];
    Serial.print("current_led_state: ");
    Serial.println(current_led_state);
  } else {
    Serial.print("Get led state error code: ");
    Serial.println(httpResponseCode);
  }
  
  // Free resources
  http.end();

  return current_led_state;
}