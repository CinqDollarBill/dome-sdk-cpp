#ifndef DOME_MARKET_ENDPOINTS_HPP
#define DOME_MARKET_ENDPOINTS_HPP

#include "base_endpoint.hpp"
#include "types.hpp"

namespace dome {

class MarketEndpoints : public BaseEndpoint {
public:
    explicit MarketEndpoints(const DomeSDKConfig& config);

    // Get current or historical market price
    // Endpoint: /polymarket/market-price/{token_id}
    MarketPriceResponse get_market_price(const GetMarketPriceParams& params);

    // Get historical candlestick data
    // Endpoint: /polymarket/candlesticks/{condition_id}
    CandlesticksResponse get_candlesticks(const GetCandlesticksParams& params);

    // Get markets with filtering
    // Endpoint: /polymarket/markets
    MarketsResponse get_markets(const GetMarketsParams& params = {});

    // Get historical orderbook snapshots
    // Endpoint: /polymarket/orderbooks
    OrderbooksResponse get_orderbooks(const GetOrderbooksParams& params);
};

}  // namespace dome

#endif  // DOME_MARKET_ENDPOINTS_HPP