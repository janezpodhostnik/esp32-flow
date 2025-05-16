#include "flow.h"

#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <arduino_base64.hpp>

#include "block.h"

FlowClient::FlowClient(const String &url) {
    this->url = url;
    this->httpResponseCode = 0;
}

FlowClient::~FlowClient() = default;

Block FlowClient::get_latest_sealed_block() {
    HTTPClient http;
    http.begin(this->url + "/v1/blocks?height=sealed");
    this->httpResponseCode = http.GET();

    if (this->httpResponseCode <= 0) {
        http.end();
        return {};
    }

    const String payload = http.getString();
    JSONVar myObject = JSON.parse(payload);

    if (JSON.typeof(myObject) == "undefined") {
        // Parsing input failed!
        http.end();
        return {};
    }

    const unsigned long height = atol(myObject[0]["header"]["height"]);
    const String id = myObject[0]["header"]["id"];


    // Free resources
    http.end();

    return {height, id};
}

JSONVar FlowClient::run_script(const String &script, const unsigned long block_height) {
    String postBody = R"({
        "script": "%s",
        "arguments": []
    })";
    postBody.replace("%s", script);

    HTTPClient http;
    http.begin(this->url + "/v1/scripts?block_height=" + block_height);
    http.addHeader("Content-Type", "application/json");
    this->httpResponseCode = http.POST(postBody);
    if (this->httpResponseCode <= 0) {
        http.end();
        return {};
    }

    // for some reason the returned json is in quotes and base64 encoded
    String input = http.getString();
    input.remove(input.length() - 1, 1);
    input.remove(0, 1);

    const char *input2 = input.c_str();
    uint8_t output[base64::decodeLength(input2)];
    base64::decode(input2, output);

    JSONVar result = JSON.parse(reinterpret_cast<const char *>(output));

    // Free resources
    http.end();

    return result;
}


String FlowClient::send_tx(const FlowTX &tx) {
    HTTPClient http;
    http.begin(this->url + "/v1/transactions");
    http.addHeader("Content-Type", "application/json");

    Serial.println("request");
    Serial.println(tx.to_json());
    this->httpResponseCode = http.POST(tx.to_json());
    if (this->httpResponseCode <= 0) {
        http.end();
        return {};
    }

    String input = http.getString();
    Serial.println("response");
    Serial.println(input);

    return "";
}
