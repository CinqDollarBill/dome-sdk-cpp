#ifndef DOME_POLYMARKET_CLIENT_HPP
#define DOME_POLYMARKET_CLIENT_HPP

#include <string>

#include "types.hpp"

// Include all endpoint headers
#include "market_endpoints.hpp"
#include "orders_endpoints.hpp"
#include "wallet_endpoints.hpp"
#include "activity_endpoints.hpp"
#include "polymarket_websocket.hpp"

namespace dome {

class PolymarketClient {
public:
    // Constructor with configuration
    explicit PolymarketClient(const DomeSDKConfig& config);

    // Public sub-clients for accessing different API endpoints
    MarketEndpoints markets;
    OrdersEndpoints orders;
    WalletEndpoints wallet;
    ActivityEndpoints activity;
    PolymarketWebSocketClient websocket;

private:
    DomeSDKConfig config_;
    
    // Resolve API key from config or environment
    static std::string resolve_api_key(const DomeSDKConfig& config);
};

}  // namespace dome

#endif  // DOME_POLYMARKET_CLIENT_HPP