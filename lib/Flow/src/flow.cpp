#include "flow.h"

#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <arduino_base64.hpp>

#include "block.h"

FlowClient::FlowClient(String url) {
    this->url = url;
    this->httpResponseCode = 0;
}

FlowClient::~FlowClient() {
}

Block FlowClient::get_latest_sealed_block() {
    HTTPClient http;
    http.begin(this->url + "/v1/blocks?height=sealed");
    this->httpResponseCode = http.GET();

    if (this->httpResponseCode <= 0) {
        http.end();
        return Block();
    }

    String payload = http.getString();
    JSONVar myObject = JSON.parse(payload);

    if (JSON.typeof(myObject) == "undefined") {
        // Parsing input failed!
        http.end();
        return Block();
    }

    unsigned long height = atol(myObject[0]["header"]["height"]);
    String id = myObject[0]["header"]["id"];


    // Free resources
    http.end();

    return Block(height, id);
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
        return JSONVar();
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


String FlowClient::send_tx(FlowTX tx) {
    /*{

    "script": "string",
    "arguments":
[

    "string"

],
"reference_block_id": "string",
"gas_limit": "string",
"payer": "string",
"proposal_key":
{

    "address": "string",
    "key_index": "string",
    "sequence_number": "string"

},
"authorizers":
[

    "string"

],
"payload_signatures":
[

    {
        "address": "string",
        "key_index": "string",
        "signature": "string"
    }

],
"envelope_signatures":
[

        {
            "address": "string",
            "key_index": "string",
            "signature": "string"
        }
    ]

}
     */

   return "";
}
