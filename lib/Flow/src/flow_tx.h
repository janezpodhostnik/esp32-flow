#pragma once

#include <array>
#include <WString.h>
#include <vector>

class FlowTX
{
public:
    String script;
    String reference_block_id;
    unsigned long gas_limit;
    String payer;

    struct ProposalKey {
        String address;
        unsigned long key_index;
        unsigned long sequence_number;
    } proposal_key;

    std::vector<String> arguments;
    std::vector<String> authorizers;

    struct Signature {
        unsigned long index;
        unsigned long key_id;
        std::array<uint8_t, 64> signature;
    };

    std::vector<Signature> payload_signatures;
    std::vector<Signature> envelope_signatures;


    FlowTX();
    ~FlowTX();

    String to_json() const;
    std::vector<uint8_t> to_payload_rlp() const;
    std::vector<uint8_t> to_envelope_rlp() const;
    std::array<uint8_t, 32> to_hashed_envelope() const;
    std::array<uint8_t, 64>  sign_envelope() const;
private:
    std::vector<uint8_t> rlp_arguments() const;
    std::vector<uint8_t> rlp_authorizers() const;
    std::vector<uint8_t> encode_signatures() const;
};
