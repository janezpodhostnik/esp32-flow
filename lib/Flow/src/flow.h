#pragma once

#include <Arduino_JSON.h>

#include "block.h"
#include "tx.h"

/**
 * Represents a client used to interact with a Flow blockchain network.
 * Provides methods for retrieving blockchain data and executing scripts.
 */
class FlowClient {
public:
    int httpResponseCode;

    explicit FlowClient(String url);

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

    String send_tx(FlowTX tx);
private:
    String url;
};
