#include <WiFi.h>
#include <HTTPClient.h>
// examples for json get https://randomnerdtutorials.com/esp32-http-get-post-arduino/
#include <Arduino_JSON.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "WiFiCredentials.h"
#include "display.h"

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

int led = 10;
void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  ensure_wifi_connected();
  display_begin();

  delay(100);
}


void loop() {
  ensure_wifi_connected();
  digitalWrite(led, HIGH);

  HTTPClient http;
  http.begin("http://access-003.mainnet26.nodes.onflow.org:8070/v1/blocks?height=sealed");
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String payload = http.getString();

    JSONVar myObject = JSON.parse(payload);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }
    JSONVar value = myObject[0]["header"]["height"];

    Serial.print("block: ");
    Serial.println(value);

    lcd_display_info(value);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  digitalWrite(led, LOW);

  delay(100);
}
