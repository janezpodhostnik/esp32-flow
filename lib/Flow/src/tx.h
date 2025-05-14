#pragma once

#include <WString.h>

class FlowTX
{
public:
    // base64 encoded
    String script;
    // base64 encoded
    String* arguments;
    // hex encoded
    String reference_block_id;
    String gas_limit;
    // hex encoded
    String payer;
//    String proposal_key;
//    String* authorizers;
//    String* payload_signatures;
//    String* envelope_signatures;
private:
};
