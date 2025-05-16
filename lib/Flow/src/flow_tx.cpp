#include "flow_tx.h"
#include <Arduino_JSON.h>
#include <arduino_base64.hpp>
#include "rlp.h"
#include <SHA3.h>
#include <SHA1.h>
#include <uECC.h>



String base64_encode(String s) {
    char encoded_script[base64::encodeLength(s.length())];
    base64::encode(reinterpret_cast<const uint8_t *>(s.c_str()), s.length(), encoded_script);
    return {encoded_script};
}

String base64_encode(const std::array<unsigned char, 64> &arr) {
    char encoded_signature[base64::encodeLength(arr.size())];
    base64::encode(arr.data(), arr.size(), encoded_signature);
    return {encoded_signature};
}

String FlowTX::to_json() const {
    String json = "{";


    // Encode the script to Base64
    json += R"("script":")" + base64_encode(script) + "\",";

    json += R"("arguments":[)";
    for (size_t i = 0; i < arguments.size(); i++) {
        json += "\"" + base64_encode(arguments[i]) + "\"";
        if (i < arguments.size() - 1) json += ",";
    }
    json += "],";

    json += R"("reference_block_id":")" + reference_block_id + "\",";
    json += R"("gas_limit":")" + String(gas_limit) + "\",";
    json += R"("payer":")" + String(payer) + "\",";

    json += R"("proposal_key":{)";
    json += R"("address":")" + String(proposal_key.address) + "\",";
    json += R"("key_index":")" + String(proposal_key.key_index) + "\",";
    json += R"("sequence_number":")" + String(proposal_key.sequence_number) + "\"";
    json += "},";

    json += "\"authorizers\":[";
    for (size_t i = 0; i < authorizers.size(); i++) {
        json += "\"" + String(authorizers[i]) + "\"";
        if (i < authorizers.size() - 1) json += ",";
    }
    json += "],";

    json += "\"payload_signatures\":[";
    for (size_t i = 0; i < payload_signatures.size(); i++) {
        auto address = String(authorizers[payload_signatures[i].index]);

        json += "{";
        json += R"("address":")" + address + "\",";
        json += R"("key_index":")" + String(payload_signatures[i].key_id) + "\",";
        json += R"("signature":")" + base64_encode(std::array<unsigned char, 64>(payload_signatures[i].signature)) + "\"";
        json += "}";
        if (i < payload_signatures.size() - 1) json += ",";
    }
    json += "],";

    json += "\"envelope_signatures\":[";
    for (size_t i = 0; i < envelope_signatures.size(); i++) {
        auto address = String(authorizers[envelope_signatures[i].index]);

        json += "{";
        json += R"("address":")" + address + "\",";
        json += R"("key_index":")" + String(envelope_signatures[i].key_id) + "\",";
        json += R"("signature":")" + base64_encode(std::array<unsigned char, 64>(envelope_signatures[i].signature)) + "\"";
        json += "}";
        if (i < envelope_signatures.size() - 1) json += ",";
    }
    json += "]";

    json += "}";

    return json;
}

std::vector<uint8_t> FlowTX::rlp_arguments() const {
    vector<vector<uint8_t> > outputArgs;
    uint32_t sumSize = 0;

    for (auto &arg: this->arguments) {
        auto outputArg = RLP::RlpEncodeString(&arg);
        outputArgs.push_back(outputArg);
        sumSize += outputArg.size();
    }
    vector<uint8_t> encoded = RLP::RlpEncodeWholeHeaderWithVector(
        sumSize);

    for (auto &arg: outputArgs) {
        encoded.insert(encoded.end(), arg.begin(), arg.end());
    }
    return encoded;
}

std::vector<uint8_t> FlowTX::rlp_authorizers() const {
    vector<vector<uint8_t> > authorizers;
    uint32_t sumSize = 0;

    for (auto &auth: this->authorizers) {
        auto outputArg = RLP::RlpEncodeItemWithVector(
            RLP::ConvertHexToVector(&auth));;
        authorizers.push_back(outputArg);
        sumSize += outputArg.size();
    }
    vector<uint8_t> encoded = RLP::RlpEncodeWholeHeaderWithVector(
        sumSize);

    for (auto &arg: authorizers) {
        encoded.insert(encoded.end(), arg.begin(), arg.end());
    }
    return encoded;
}

std::vector<uint8_t> FlowTX::to_payload_rlp() const {
    vector<uint8_t> script = RLP::RlpEncodeString(&this->script);
    vector<uint8_t> args = rlp_arguments();
    // reference block is already a hex
    vector<uint8_t> reference_block = RLP::RlpEncodeItemWithVector(RLP::ConvertHexToVector(&this->reference_block_id));
    vector<uint8_t> gas_limit = RLP::RlpEncodeItemWithVector(RLP::ConvertNumberToVector(this->gas_limit));
    vector<uint8_t> proposal_key_address = RLP::RlpEncodeItemWithVector(
        RLP::ConvertHexToVector(&this->proposal_key.address));
    vector<uint8_t> proposal_key_id = RLP::RlpEncodeItemWithVector(
        RLP::ConvertNumberToVector(this->proposal_key.key_index));
    vector<uint8_t> proposal_key_sequence = RLP::RlpEncodeItemWithVector(
        RLP::ConvertNumberToVector(this->proposal_key.sequence_number));
    vector<uint8_t> payer = RLP::RlpEncodeItemWithVector(
        RLP::ConvertHexToVector(&this->payer));
    vector<uint8_t> authorizers = rlp_authorizers();

    vector<uint8_t> encoded = RLP::RlpEncodeWholeHeaderWithVector(
        script.size() +
        args.size() +
        reference_block.size() +
        gas_limit.size() +
        proposal_key_address.size() +
        proposal_key_id.size() +
        proposal_key_sequence.size() +
        payer.size() +
        authorizers.size()
    );


    encoded.insert(encoded.end(), script.begin(), script.end());
    encoded.insert(encoded.end(), args.begin(), args.end());
    encoded.insert(encoded.end(), reference_block.begin(), reference_block.end());
    encoded.insert(encoded.end(), gas_limit.begin(), gas_limit.end());
    encoded.insert(encoded.end(), proposal_key_address.begin(), proposal_key_address.end());
    encoded.insert(encoded.end(), proposal_key_id.begin(), proposal_key_id.end());
    encoded.insert(encoded.end(), proposal_key_sequence.begin(), proposal_key_sequence.end());
    encoded.insert(encoded.end(), payer.begin(), payer.end());
    encoded.insert(encoded.end(), authorizers.begin(), authorizers.end());


    return encoded;
}

std::vector<uint8_t> encode_signature(const FlowTX::Signature &signature) {
    vector<uint8_t> signer_index = RLP::RlpEncodeItemWithVector(
        RLP::ConvertNumberToVector(signature.index));
    vector<uint8_t> key_id = RLP::RlpEncodeItemWithVector(
        RLP::ConvertNumberToVector(signature.key_id));
    vector<uint8_t> sig = RLP::RlpEncodeItemWithVector(vector<uint8_t>(signature.signature.begin(), signature.signature.end()));

    vector<uint8_t> encoded = RLP::RlpEncodeWholeHeaderWithVector(
        signer_index.size() +
        key_id.size() +
        sig.size());

    encoded.insert(encoded.end(), signer_index.begin(), signer_index.end());
    encoded.insert(encoded.end(), key_id.begin(), key_id.end());
    encoded.insert(encoded.end(), sig.begin(), sig.end());

    return encoded;
}

std::vector<uint8_t> FlowTX::encode_signatures() const {
    vector<vector<uint8_t> > signatures;
    uint32_t sumSize = 0;

    for (auto &sig: this->payload_signatures) {
        auto outputArg = encode_signature(sig);
        signatures.push_back(outputArg);
        sumSize += outputArg.size();
    }
    vector<uint8_t> encoded = RLP::RlpEncodeWholeHeaderWithVector(
        sumSize);

    for (auto &arg: signatures) {
        encoded.insert(encoded.end(), arg.begin(), arg.end());
    }
    return encoded;
}

std::vector<uint8_t> FlowTX::to_envelope_rlp() const {
    vector<uint8_t> body = to_payload_rlp();
    vector<uint8_t> signatures = encode_signatures();

    vector<uint8_t> encoded = RLP::RlpEncodeWholeHeaderWithVector(
        body.size() +
        signatures.size()
    );

    encoded.insert(encoded.end(), body.begin(), body.end());
    encoded.insert(encoded.end(), signatures.begin(), signatures.end());

    return encoded;
}


FlowTX::FlowTX() {
    this->gas_limit = 9999;
}

FlowTX::~FlowTX() = default;

std::array<uint8_t, 32> FlowTX::to_hashed_envelope() const {
    SHA3_256 sha3_256;
    sha3_256.reset();
    auto envelope = to_envelope_rlp();
    uint8_t tag[32];
    RLP::ConvertHexToBytes(tag, "464c4f572d56302e302d7472616e73616374696f6e0000000000000000000000", 32);
    sha3_256.update(tag, 32);
    sha3_256.update(envelope.data(), envelope.size());
    std::array<uint8_t, 32> hash{};
    sha3_256.finalize(hash.data(), hash.size());

    return hash;
}

typedef struct {
    uECC_HashContext uECC;
    SHA1 sha1;
} SHA1_HashContext;

void init_SHA1(const uECC_HashContext *base) {
    SHA1_HashContext *context = (SHA1_HashContext *)base;
    context->sha1.reset();
}

void update_SHA1(const uECC_HashContext *base, const uint8_t *message, unsigned message_size) {
    SHA1_HashContext *context = (SHA1_HashContext *)base;
    context->sha1.update(message, message_size);
}

void finish_SHA1(const uECC_HashContext *base, uint8_t *hash_result) {
    SHA1_HashContext *context = (SHA1_HashContext *)base;
    context->sha1.finalize(hash_result, sizeof(hash_result));
}

std::array<uint8_t, 64> FlowTX::sign_envelope() const {
    uint8_t private_key[32] = {
        //
    };

    uint8_t public_compressed[64] = {};
    RLP::ConvertHexToBytes(public_compressed,
                           "7af4f022050c4f9d450d521b05154ea198a8028d2912e18b701991c38ed6e8b0b03748a2fcc068f635d461cc4440e6c2becae1a9112a7e6d6ff3e9c57b5f9cdd",
                           64);
    std::array<uint8_t, 32> hash = to_hashed_envelope();
    std::array<uint8_t, 64> sig{};
    uECC_Curve curve = uECC_secp256r1();


    uint8_t public_key[64];
    uECC_decompress(public_compressed, public_key, curve);

    int isValid = uECC_valid_public_key(public_compressed, curve);
    if (!isValid) {
        Serial.println("Invalid public key");
        Serial.println(RLP::ConvertBytesToHex(public_key, 64));
        return {};
    }


    uint8_t tmp[64 + 20 + 20]; // block_size + 2 * result_size
    SHA1_HashContext sha1_ctx = {
        {
            init_SHA1,
            update_SHA1,
            finish_SHA1,
            64,  // SHA1 block size
            20,  // SHA1 digest size
            tmp
        },
        SHA1() // Initialize SHA1 instance}};
        };

    //if (!uECC_sign_deterministic(private_key, hash.data(), hash.size(), &sha1_ctx.uECC, sig.data(), curve)) {
    if (!uECC_sign(private_key, hash.data(), hash.size(), sig.data(), curve)) {
        Serial.println("Failed to sign envelope");
        Serial.println(RLP::ConvertBytesToHex(hash.data(), hash.size()));
        Serial.println(RLP::ConvertBytesToHex(private_key, 32));
        return {};
    }

    if (!uECC_verify(public_compressed, hash.data(), hash.size(), sig.data(), curve)) {
        Serial.println("Failed to verify envelope signature");
    }

    return sig;
}
