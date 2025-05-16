#pragma once

#include <Arduino_JSON.h>

#include "block.h"
#include "flow_tx.h"

/**
 * Represents a client used to interact with a Flow blockchain network.
 * Provides methods for retrieving blockchain data and executing scripts.
 */
class FlowClient {
public:
    int httpResponseCode;

    explicit FlowClient(const String &url);

    ~FlowClient();

    Block get_latest_sealed_block();

    /**
     * Executes a provided script within the FlowClient environment.
     *
     * @param script The script to be executed, provided as a base64 encoded script.
     * @param block_height The block height at which to run the script
     * @return A JSONVar object representing the result or output of the executed script.
     */
    JSONVar run_script(const String &script, unsigned long block_height);

    String send_tx(const FlowTX &tx);

    String envelope_rlp() const;
private:
    String url;
};
