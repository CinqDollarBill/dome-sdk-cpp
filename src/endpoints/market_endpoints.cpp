#include "dome_api_sdk/market_endpoints.hpp"

namespace dome {

MarketEndpoints::MarketEndpoints(const DomeSDKConfig& config)
    : BaseEndpoint(config) {}

MarketPriceResponse MarketEndpoints::get_market_price(const GetMarketPriceParams& params) {
    std::string endpoint = "/polymarket/market-price/" + params.token_id;
    
    std::map<std::string, std::string> query_params;
    add_param_if_present(query_params, "at_time", params.at_time);
    
    auto json = http_client_->get(endpoint, query_params);
    
    MarketPriceResponse response;
    response.price = json.value("price", 0.0);
    response.at_time = json.value("at_time", 0LL);
    
    return response;
}

CandlesticksResponse MarketEndpoints::get_candlesticks(const GetCandlesticksParams& params) {
    std::string endpoint = "/polymarket/candlesticks/" + params.condition_id;
    
    std::map<std::string, std::string> query_params;
    query_params["start_time"] = std::to_string(params.start_time);
    query_params["end_time"] = std::to_string(params.end_time);
    if (params.interval.has_value()) {
        query_params["interval"] = std::to_string(params.interval.value());
    }
    
    auto json = http_client_->get(endpoint, query_params);
    
    CandlesticksResponse response;
    
    if (json.contains("candlesticks") && json["candlesticks"].is_array()) {
        for (const auto& item : json["candlesticks"]) {
            CandlestickData candle;
            candle.end_period_ts = item.value("end_period_ts", 0LL);
            candle.open_interest = item.value("open_interest", 0LL);
            candle.volume = item.value("volume", 0LL);
            
            // Parse price
            if (item.contains("price")) {
                const auto& p = item["price"];
                candle.price.open = p.value("open", 0.0);
                candle.price.high = p.value("high", 0.0);
                candle.price.low = p.value("low", 0.0);
                candle.price.close = p.value("close", 0.0);
                candle.price.open_dollars = p.value("open_dollars", "");
                candle.price.high_dollars = p.value("high_dollars", "");
                candle.price.low_dollars = p.value("low_dollars", "");
                candle.price.close_dollars = p.value("close_dollars", "");
                candle.price.mean = p.value("mean", 0.0);
                candle.price.mean_dollars = p.value("mean_dollars", "");
                candle.price.previous = p.value("previous", 0.0);
                candle.price.previous_dollars = p.value("previous_dollars", "");
            }
            
            response.candlesticks.push_back(candle);
        }
    }
    
    return response;
}

MarketsResponse MarketEndpoints::get_markets(const GetMarketsParams& params) {
    std::string endpoint = "/polymarket/markets";
    
    std::map<std::string, std::string> query_params;
    add_param_if_present(query_params, "status", params.status);
    add_param_if_present(query_params, "limit", params.limit);
    add_param_if_present(query_params, "offset", params.offset);
    add_param_if_present(query_params, "min_volume", params.min_volume);
    add_param_if_present(query_params, "market_slug", params.market_slug);
    add_param_if_present(query_params, "tags", params.tags);
    
    auto json = http_client_->get(endpoint, query_params);
    
    MarketsResponse response;
    
    // Parse pagination
    if (json.contains("pagination")) {
        const auto& p = json["pagination"];
        response.pagination.total = p.value("total", 0);
        response.pagination.limit = p.value("limit", 0);
        response.pagination.offset = p.value("offset", 0);
        response.pagination.has_more = p.value("has_more", false);
    }
    
    if (json.contains("markets") && json["markets"].is_array()) {
        for (const auto& item : json["markets"]) {
            Market market;
            market.market_slug = item.value("market_slug", "");
            market.condition_id = item.value("condition_id", "");
            market.title = item.value("title", "");
            market.start_time = item.value("start_time", 0LL);
            market.end_time = item.value("end_time", 0LL);
            market.status = item.value("status", "");
            market.volume_total = item.value("volume_total", 0.0);
            market.volume_1_week = item.value("volume_1_week", 0.0);
            market.volume_1_month = item.value("volume_1_month", 0.0);
            market.volume_1_year = item.value("volume_1_year", 0.0);
            market.resolution_source = item.value("resolution_source", "");
            market.image = item.value("image", "");
            
            // Parse side_a
            if (item.contains("side_a")) {
                market.side_a.id = item["side_a"].value("id", "");
                market.side_a.label = item["side_a"].value("label", "");
            }
            
            // Parse side_b
            if (item.contains("side_b")) {
                market.side_b.id = item["side_b"].value("id", "");
                market.side_b.label = item["side_b"].value("label", "");
            }
            
            if (item.contains("tags") && item["tags"].is_array()) {
                for (const auto& tag : item["tags"]) {
                    market.tags.push_back(tag.get<std::string>());
                }
            }
            
            response.markets.push_back(market);
        }
    }
    
    return response;
}

OrderbooksResponse MarketEndpoints::get_orderbooks(const GetOrderbooksParams& params) {
    std::string endpoint = "/polymarket/orderbooks";
    
    std::map<std::string, std::string> query_params;
    query_params["token_id"] = params.token_id;
    query_params["start_time"] = std::to_string(params.start_time);
    query_params["end_time"] = std::to_string(params.end_time);
    add_param_if_present(query_params, "limit", params.limit);
    
    auto json = http_client_->get(endpoint, query_params);
    
    OrderbooksResponse response;
    
    // Parse pagination
    if (json.contains("pagination")) {
        const auto& p = json["pagination"];
        response.pagination.limit = p.value("limit", 0);
        response.pagination.count = p.value("count", 0);
        response.pagination.has_more = p.value("has_more", false);
        if (p.contains("pagination_key") && !p["pagination_key"].is_null()) {
            response.pagination.pagination_key = p["pagination_key"].get<std::string>();
        }
    }
    
    if (json.contains("snapshots") && json["snapshots"].is_array()) {
        for (const auto& item : json["snapshots"]) {
            OrderbookSnapshot snapshot;
            snapshot.timestamp = item.value("timestamp", 0LL);
            snapshot.hash = item.value("hash", "");
            snapshot.minOrderSize = item.value("minOrderSize", "");
            snapshot.negRisk = item.value("negRisk", false);
            snapshot.assetId = item.value("assetId", "");
            snapshot.tickSize = item.value("tickSize", "");
            snapshot.indexedAt = item.value("indexedAt", 0LL);
            snapshot.market = item.value("market", "");
            
            if (item.contains("bids") && item["bids"].is_array()) {
                for (const auto& bid : item["bids"]) {
                    std::map<std::string, std::string> level;
                    level["price"] = bid.value("price", "");
                    level["size"] = bid.value("size", "");
                    snapshot.bids.push_back(level);
                }
            }
            
            if (item.contains("asks") && item["asks"].is_array()) {
                for (const auto& ask : item["asks"]) {
                    std::map<std::string, std::string> level;
                    level["price"] = ask.value("price", "");
                    level["size"] = ask.value("size", "");
                    snapshot.asks.push_back(level);
                }
            }
            
            response.snapshots.push_back(snapshot);
        }
    }
    
    return response;
}

}  // namespace dome