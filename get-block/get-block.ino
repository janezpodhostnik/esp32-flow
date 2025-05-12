#include <WiFi.h>
#include <HTTPClient.h>
// examples for json get https://randomnerdtutorials.com/esp32-http-get-post-arduino/
#include <Arduino_JSON.h>

#include "WiFiCredentials.h"

int led = 10;
void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  WiFi.begin(SSID, PASSWORD);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(750);
    Serial.print(".");
  }

  Serial.println("connected");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(led, HIGH);

    HTTPClient http;
    http.begin("https://rest-testnet.onflow.org/v1/blocks?height=final");
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
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    digitalWrite(led, LOW);
  } else {
    Serial.println("WiFi Disconnected");
  }
}
