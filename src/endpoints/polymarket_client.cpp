#include "dome_api_sdk/polymarket_client.hpp"
#include <cstdlib>

namespace dome {

std::string PolymarketClient::resolve_api_key(const DomeSDKConfig& config) {
    if (!config.api_key.empty()) {
        return config.api_key;
    }
    
    const char* env_key = std::getenv("DOME_API_KEY");
    return (env_key != nullptr) ? std::string(env_key) : "";
}

PolymarketClient::PolymarketClient(const DomeSDKConfig& config)
    : config_(config),
      markets(config),
      orders(config),
      wallet(config),
      activity(config),
      websocket(resolve_api_key(config)) {}

}  // namespace dome