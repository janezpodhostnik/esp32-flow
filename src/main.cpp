#include <uECC.h>

#include "display.h"
#include "wifi.h"
#include "button.h"
#include "flow.h"
#include "rlp.h"

#define LED_PIN 10
#define EXTERNAL_LED_PIN 0
#define BUTTON_PIN 1

Button button(BUTTON_PIN);
FlowClient client("http://access-002.devnet52.nodes.onflow.org:8070");

void IRAM_ATTR isr() {
    button.handle_pressed();
}

static int RNG(uint8_t *p_dest, unsigned p_size) {
    while (p_size) {
        long v = random();
        unsigned l_amount = min(p_size, sizeof(long));
        memcpy(p_dest, &v, l_amount);
        p_size -= l_amount;
        p_dest += l_amount;
    }
    return 1;
}

void setup() {
    Serial.begin(115200);

    randomSeed(analogRead(0));
    uECC_set_rng(&RNG);

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

FlowTX create_transaction() {
    FlowTX tx;
    tx.script = "transaction { execute { } }";
    tx.gas_limit = 9999;
    tx.proposal_key.address = "0x0d3c8d02b02ceb4c";
    tx.proposal_key.key_index = 0;
    tx.proposal_key.sequence_number = 15;
    tx.payer = "0x0d3c8d02b02ceb4c";
    tx.authorizers.emplace_back("0x0d3c8d02b02ceb4c");

    return tx;
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
    Serial.print(latest_block.height);
    Serial.print(" id: ");
    Serial.println(latest_block.id);
    lcd_display_info(latest_block.height);

    const bool led_state = get_led_state_at_block(&client, latest_block.height);
    digitalWrite(EXTERNAL_LED_PIN, led_state ? HIGH : LOW);

    if (button.pop_pressed()) {
        Serial.println("Button has been pressed");
        FlowTX tx = create_transaction();
        auto sig = tx.sign_envelope();
        tx.reference_block_id = latest_block.id;
        tx.envelope_signatures.emplace_back(FlowTX::Signature{
            0, 0, sig
        });

        client.send_tx(tx);

        auto envelope = tx.to_envelope_rlp();
        Serial.println(RLP::VectorToString(&envelope));
        auto hash = tx.to_hashed_envelope();
        Serial.println(RLP::ConvertBytesToHex(hash.data(), hash.size()));
        Serial.println(RLP::ConvertBytesToHex(sig.data(), sig.size()));
        //
        // Serial.println(tx.to_json());
    }

    delay(100);
}
