#ifndef DOME_TYPES_HPP
#define DOME_TYPES_HPP

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>
#include <cstdint>
#include <stdexcept>

namespace dome {

// ============================================================================
// Configuration Types
// ============================================================================

/**
 * Configuration options for initializing the Dome SDK.
 * 
 * @param api_key Authentication token for API requests
 * @param base_url Base URL for the API (defaults to https://api.domeapi.io/v1)
 * @param timeout Request timeout in seconds (defaults to 30)
 */
struct DomeSDKConfig {
    std::string api_key;
    std::string base_url = "https://api.domeapi.io/v1";
    float timeout = 30.0f;
};

/**
 * Configuration for individual requests.
 * 
 * @param timeout Request timeout in seconds
 * @param headers Additional headers to include
 */
struct RequestConfig {
    float timeout = 30.0f;
    std::map<std::string, std::string> headers;
};

// ============================================================================
// Market Price Types
// ============================================================================

/**
 * Response from the market price endpoint.
 * 
 * @param price Current market price
 * @param at_time Timestamp of the price data
 */
struct MarketPriceResponse {
    double price;
    int64_t at_time;
};

/**
 * Parameters for getting market price.
 * 
 * @param token_id Token ID for the market (required)
 * @param at_time Unix timestamp for historical price (optional)
 */
struct GetMarketPriceParams {
    std::string token_id;
    std::optional<int64_t> at_time;
};

// ============================================================================
// Candlestick Types
// ============================================================================

/**
 * Price data for a candlestick.
 * 
 * @param open Opening price
 * @param high Highest price
 * @param low Lowest price
 * @param close Closing price
 * @param open_dollars Opening price in dollars
 * @param high_dollars Highest price in dollars
 * @param low_dollars Lowest price in dollars
 * @param close_dollars Closing price in dollars
 * @param mean Mean price
 * @param mean_dollars Mean price in dollars
 * @param previous Previous price
 * @param previous_dollars Previous price in dollars
 */
struct CandlestickPrice {
    double open;
    double high;
    double low;
    double close;
    std::string open_dollars;
    std::string high_dollars;
    std::string low_dollars;
    std::string close_dollars;
    double mean;
    std::string mean_dollars;
    double previous;
    std::string previous_dollars;
};

/**
 * Ask/Bid data for a candlestick.
 * 
 * @param open Opening price
 * @param close Closing price
 * @param high Highest price
 * @param low Lowest price
 * @param open_dollars Opening price in dollars
 * @param close_dollars Closing price in dollars
 * @param high_dollars Highest price in dollars
 * @param low_dollars Lowest price in dollars
 */
struct CandlestickAskBid {
    double open;
    double close;
    double high;
    double low;
    std::string open_dollars;
    std::string close_dollars;
    std::string high_dollars;
    std::string low_dollars;
};

/**
 * Candlestick data point.
 * 
 * @param end_period_ts End period timestamp
 * @param open_interest Open interest
 * @param price Price data
 * @param volume Volume
 * @param yes_ask Yes ask data
 * @param yes_bid Yes bid data
 */
struct CandlestickData {
    int64_t end_period_ts;
    int64_t open_interest;
    CandlestickPrice price;
    int64_t volume;
    CandlestickAskBid yes_ask;
    CandlestickAskBid yes_bid;
};

/**
 * Token metadata.
 * 
 * @param token_id Token ID
 */
struct TokenMetadata {
    std::string token_id;
};

/**
 * Response from the candlesticks endpoint.
 * 
 * @param candlesticks List of candlestick data
 */
struct CandlesticksResponse {
    std::vector<CandlestickData> candlesticks;
};

/**
 * Parameters for getting candlestick data.
 * 
 * @param condition_id Condition ID for the market (required)
 * @param start_time Start time as Unix timestamp (required)
 * @param end_time End time as Unix timestamp (required)
 * @param interval Interval in minutes (1, 60, or 1440) (optional)
 */
struct GetCandlesticksParams {
    std::string condition_id;
    int64_t start_time;
    int64_t end_time;
    std::optional<int> interval;  // 1 = 1m, 60 = 1h, 1440 = 1d
};

// ============================================================================
// Wallet PnL Types
// ============================================================================

/**
 * PnL data point.
 * 
 * @param timestamp Timestamp
 * @param pnl_to_date PnL to date
 */
struct PnLDataPoint {
    int64_t timestamp;
    double pnl_to_date;
};

/**
 * Response from the wallet PnL endpoint.
 * 
 * @param granularity Data granularity
 * @param start_time Start time
 * @param end_time End time
 * @param wallet_address Wallet address
 * @param pnl_over_time PnL data over time
 */
struct WalletPnLResponse {
    std::string granularity;
    int64_t start_time;
    int64_t end_time;
    std::string wallet_address;
    std::vector<PnLDataPoint> pnl_over_time;
};

enum class Granularity {
    day,
    week,
    month,
    year,
    all
};

inline std::string granularity_to_string(Granularity g) {
    switch (g) {
        case Granularity::day: return "day";
        case Granularity::week: return "week";
        case Granularity::month: return "month";
        case Granularity::year: return "year";
        case Granularity::all: return "all";
    }
    return "day";
}

/**
 * Parameters for getting wallet PnL.
 * 
 * @param wallet_address Wallet address (required)
 * @param granularity Data granularity (required)
 * @param start_time Start time as Unix timestamp (optional)
 * @param end_time End time as Unix timestamp (optional)
 */
struct GetWalletPnLParams {
    std::string wallet_address;
    Granularity granularity = Granularity::day;
    std::optional<int64_t> start_time;
    std::optional<int64_t> end_time;
};

// ============================================================================
// Orders Types
// ============================================================================

/**
 * Pagination data.
 * 
 * @param limit Limit
 * @param offset Offset
 * @param total Total count
 * @param has_more Whether there are more results
 */
struct Pagination {
    int limit;
    int offset;
    int total;
    bool has_more;
};

/**
 * Order data.
 * 
 * @param token_id Token ID
 * @param token_label Human readable label for this outcome (yes/no etc)
 * @param side Order side (BUY or SELL)
 * @param market_slug Market slug
 * @param condition_id Condition ID
 * @param shares Raw number of shares purchased (from the blockchain)
 * @param shares_normalized Number of shares purchased normalized (raw divided by 1000000)
 * @param price Price
 * @param tx_hash Transaction hash
 * @param title Market title
 * @param timestamp Timestamp
 * @param order_hash Order hash
 * @param user User address (maker)
 * @param taker Taker address that was part of this trade (optional, may be CTF exchange)
 */
struct Order {
    std::string token_id;
    std::string token_label;
    std::string side;  // "BUY" or "SELL"
    std::string market_slug;
    std::string condition_id;
    int64_t shares;
    double shares_normalized;
    double price;
    std::string tx_hash;
    std::string title;
    int64_t timestamp;
    std::string order_hash;
    std::string user;
    std::optional<std::string> taker;
};

/**
 * Response from the orders endpoint.
 * 
 * @param orders List of orders
 * @param pagination Pagination information
 */
struct OrdersResponse {
    std::vector<Order> orders;
    Pagination pagination;
};

/**
 * Parameters for getting orders.
 * 
 * @param market_slug Market slug (optional). Can provide multiple values as array.
 * @param condition_id Condition ID (optional). Can provide multiple values as array.
 * @param token_id Token ID (optional). Can provide multiple values as array.
 * @param start_time Start time as Unix timestamp (optional)
 * @param end_time End time as Unix timestamp (optional)
 * @param limit Limit (optional)
 * @param offset Offset (optional)
 * @param user User address (optional)
 */
struct GetOrdersParams {
    std::optional<std::string> market_slug;
    std::optional<std::vector<std::string>> market_slugs;
    std::optional<std::string> condition_id;
    std::optional<std::string> token_id;
    std::optional<int64_t> start_time;
    std::optional<int64_t> end_time;
    std::optional<int> limit;
    std::optional<int> offset;
    std::optional<std::string> user;
};

// ============================================================================
// Polymarket Orderbooks Types
// ============================================================================

/**
 * Orderbook snapshot data.
 * 
 * @param asks Sell orders, ordered by price
 * @param bids Buy orders, ordered by price
 * @param hash Snapshot hash
 * @param minOrderSize Minimum order size
 * @param negRisk Negative risk flag
 * @param assetId Asset ID
 * @param timestamp Timestamp of the snapshot in milliseconds
 * @param tickSize Tick size
 * @param indexedAt When the snapshot was indexed in milliseconds
 * @param market Market identifier
 */
struct OrderbookSnapshot {
    std::vector<std::map<std::string, std::string>> asks;
    std::vector<std::map<std::string, std::string>> bids;
    std::string hash;
    std::string minOrderSize;
    bool negRisk;
    std::string assetId;
    int64_t timestamp;
    std::string tickSize;
    int64_t indexedAt;
    std::string market;
};

/**
 * Orderbook pagination data.
 * 
 * @param limit Limit
 * @param count Number of snapshots returned
 * @param pagination_key The pagination key to pass in to get the next chunk of data
 * @param has_more Whether there are more snapshots available
 */
struct OrderbookPagination {
    int limit;
    int count;
    std::optional<std::string> pagination_key;
    bool has_more;
};

/**
 * Response from the orderbooks endpoint.
 * 
 * @param snapshots Array of orderbook snapshots at different points in time
 * @param pagination Pagination information
 */
struct OrderbooksResponse {
    std::vector<OrderbookSnapshot> snapshots;
    OrderbookPagination pagination;
};

/**
 * Parameters for getting orderbooks.
 * 
 * @param token_id The token id (asset) for the Polymarket market (required)
 * @param start_time Start time in Unix timestamp (milliseconds) (required)
 * @param end_time End time in Unix timestamp (milliseconds) (required)
 * @param limit Maximum number of snapshots to return (optional, default: 100, max: 500)
 * @param pagination_key Pagination key to get the next chunk of data (optional)
 */
struct GetOrderbooksParams {
    std::string token_id;
    int64_t start_time;   // milliseconds
    int64_t end_time;     // milliseconds
    std::optional<int> limit;
    std::optional<std::string> pagination_key;
};

// ============================================================================
// Polymarket Markets Types
// ============================================================================

/**
 * Market side/outcome data.
 * 
 * @param id Token ID for the side
 * @param label Label for the side
 */
struct MarketSide {
    std::string id;
    std::string label;
};

/**
 * Market data.
 * 
 * @param market_slug Market slug
 * @param condition_id Condition ID
 * @param title Market title
 * @param start_time Unix timestamp in seconds when the market starts
 * @param end_time Unix timestamp in seconds when the market ends
 * @param completed_time Unix timestamp in seconds when the market was completed (nullable)
 * @param close_time Unix timestamp in seconds when the market was closed (nullable)
 * @param game_start_time Datetime string in UTC format for when the game/event starts (nullable)
 * @param tags List of tags
 * @param volume_1_week Trading volume in USD for the past week
 * @param volume_1_month Trading volume in USD for the past month
 * @param volume_1_year Trading volume in USD for the past year
 * @param volume_total Total trading volume in USD
 * @param resolution_source URL to the data source used for market resolution
 * @param image URL to the market image
 * @param side_a First side/outcome of the market
 * @param side_b Second side/outcome of the market
 * @param winning_side The winning side of the market (null if not yet resolved)
 * @param status Market status (open or closed)
 */
struct Market {
    std::string market_slug;
    std::string condition_id;
    std::string title;
    int64_t start_time;
    int64_t end_time;
    std::optional<int64_t> completed_time;
    std::optional<int64_t> close_time;
    std::optional<std::string> game_start_time;
    std::vector<std::string> tags;
    double volume_1_week;
    double volume_1_month;
    double volume_1_year;
    double volume_total;
    std::string resolution_source;
    std::string image;
    MarketSide side_a;
    MarketSide side_b;
    std::optional<MarketSide> winning_side;
    std::string status;  // "open" or "closed"
};

/**
 * Response from the markets endpoint.
 * 
 * @param markets List of markets
 * @param pagination Pagination information
 */
struct MarketsResponse {
    std::vector<Market> markets;
    Pagination pagination;
};

/**
 * Parameters for getting markets.
 * 
 * @param market_slug Filter markets by market slug(s). Can provide multiple values.
 * @param event_slug Filter markets by event slug(s). Can provide multiple values.
 * @param condition_id Filter markets by condition ID(s). Can provide multiple values.
 * @param tags Filter markets by tag(s). Can provide multiple values.
 * @param status Filter markets by status (whether they're open or closed)
 * @param min_volume Filter markets with total trading volume >= this amount (USD)
 * @param limit Number of markets to return (1-100). Default: 10
 * @param offset Number of markets to skip for pagination
 */
struct GetMarketsParams {
    std::optional<std::string> market_slug;
    std::optional<std::vector<std::string>> market_slugs;
    std::optional<std::string> event_slug;
    std::optional<std::string> condition_id;
    std::optional<std::vector<std::string>> tags;
    std::optional<std::string> status;  // "open" or "closed"
    std::optional<double> min_volume;
    std::optional<int> limit;
    std::optional<int> offset;
};

// ============================================================================
// Polymarket Activity Types
// ============================================================================

/**
 * Activity data.
 * 
 * @param token_id Token ID
 * @param side Activity side (MERGE, SPLIT, or REDEEM)
 * @param market_slug Market slug
 * @param condition_id Condition ID
 * @param shares Raw number of shares (from the blockchain)
 * @param shares_normalized Number of shares normalized (raw divided by 1000000)
 * @param price Price
 * @param tx_hash Transaction hash
 * @param title Market title
 * @param timestamp Unix timestamp in seconds when the activity occurred
 * @param order_hash Order hash
 * @param user User wallet address
 */
struct Activity {
    std::string token_id;
    std::string side;  // "MERGE", "SPLIT", "REDEEM"
    std::string market_slug;
    std::string condition_id;
    int64_t shares;
    double shares_normalized;
    double price;
    std::string tx_hash;
    std::string title;
    int64_t timestamp;
    std::string order_hash;
    std::string user;
};

/**
 * Activity pagination data.
 * 
 * @param limit Limit
 * @param offset Offset
 * @param count Total number of activities matching the filters
 * @param has_more Whether there are more activities available
 */
struct ActivityPagination {
    int limit;
    int offset;
    int count;
    bool has_more;
};

/**
 * Response from the activity endpoint.
 * 
 * @param activities List of activities
 * @param pagination Pagination information
 */
struct ActivityResponse {
    std::vector<Activity> activities;
    ActivityPagination pagination;
};

/**
 * Parameters for getting activity.
 * 
 * @param user User wallet address to fetch activity for (required)
 * @param start_time Filter activity from this Unix timestamp in seconds (inclusive) (optional)
 * @param end_time Filter activity until this Unix timestamp in seconds (inclusive) (optional)
 * @param market_slug Filter activity by market slug (optional)
 * @param condition_id Filter activity by condition ID (optional)
 * @param limit Number of activities to return (1-1000) (optional, default: 100)
 * @param offset Number of activities to skip for pagination (optional)
 */
struct GetActivityParams {
    std::string user;
    std::optional<int64_t> start_time;
    std::optional<int64_t> end_time;
    std::optional<std::string> market_slug;
    std::optional<std::string> condition_id;
    std::optional<int> limit;
    std::optional<int> offset;
};

// ============================================================================
// WebSocket Types
// ============================================================================

/**
 * Filters for WebSocket subscription.
 * 
 * @param users Array of wallet addresses to track
 * @param condition_ids Array of condition IDs to track
 * @param market_slugs Array of market slugs to track
 */
struct SubscribeFilters {
    std::optional<std::vector<std::string>> users;
    std::optional<std::vector<std::string>> condition_ids;
    std::optional<std::vector<std::string>> market_slugs;
};

/**
 * WebSocket order event.
 * 
 * @param type Always "event"
 * @param subscription_id The subscription ID that triggered this event
 * @param data Order information matching the format of the orders API
 */
struct WebSocketOrderEvent {
    std::string type;  // "event"
    std::string subscription_id;
    Order data;
};

/**
 * Active subscription information.
 * 
 * @param subscription_id The subscription ID assigned by the server
 * @param filters The subscription filters
 * @param active Whether the subscription is active
 */
struct ActiveSubscription {
    std::string subscription_id;
    SubscribeFilters filters;
    bool active;
};

using OrderEventCallback = std::function<void(const WebSocketOrderEvent&)>;

// ============================================================================
// Error Types
// ============================================================================

/**
 * API error response.
 * 
 * @param error Error code
 * @param message Error message
 */
class DomeAPIError : public std::runtime_error {
public:
    int status_code;
    std::string response_body;
    
    DomeAPIError(int code, const std::string& message, const std::string& body = "")
        : std::runtime_error(message), status_code(code), response_body(body) {}
};

// ============================================================================
// HTTP Client Types
// ============================================================================

enum class HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE_
};

}  // namespace dome

#endif  // DOME_TYPES_HPP