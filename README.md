# Dome SDK C++

A C++ SDK for [Dome API](https://www.domeapi.io/), providing access to prediction market data from Polymarket.

## Features

- **Market Data**: Get market prices, candlesticks, market listings, and orderbook snapshots
- **Orders**: Query order data with filtering by market, user, and time range
- **Wallet Analytics**: Get wallet PnL data with configurable granularity
- **Activity Tracking**: Track MERGE, SPLIT, and REDEEM activities
- **WebSocket**: Real-time order streaming with subscription management

## Requirements

- C++17 or later
- CMake 3.14+
- libcurl
- OpenSSL (for WebSocket TLS support)
- nlohmann/json (auto-fetched by CMake)
- ixwebsocket (auto-fetched by CMake)

## Configuration

Create a `.env` file in your project root:

```bash
DOME_API_KEY=your-api-key
PROXY_WALLET=0xYourWalletAddress
```

Or set environment variables:

```bash
export DOME_API_KEY="your-api-key"
export PROXY_WALLET="0xYourWalletAddress"
```

The SDK provides utilities to load these values:

```cpp
#include <dome_api_sdk/utils.hpp>

std::string api_key = dome::load_config_value("DOME_API_KEY");
std::string wallet = dome::load_config_value("PROXY_WALLET");
```

## Building

```bash
cd dome-sdk-cpp
mkdir build && cd build
cmake .. -DUSE_TLS=ON -DUSE_OPEN_SSL=ON
make

# Run the demo
./polymarket_demo

# Run the WebSocket example
./websocket_example
```

## Basic Usage

```cpp
#include <dome_api_sdk/client.hpp>
#include <dome_api_sdk/utils.hpp>

using namespace dome;

int main() {
    // Load API key from environment or .env file
    DomeSDKConfig config;
    config.api_key = load_config_value("DOME_API_KEY");
    DomeClient dome(config);
    
    // Get market price
    auto price = dome.polymarket.markets.get_market_price({
        .token_id = "98250445447699368679516529207365255018790721464590833209064266254238063117329"
    });
    std::cout << "Price: " << price.price << std::endl;
    
    // Get markets
    auto markets = dome.polymarket.markets.get_markets({
        .status = "open",
        .limit = 20,
        .min_volume = 100000.0
    });
    
    return 0;
}
```

## API Reference

### Markets

```cpp
// Get market price (current or historical)
auto price = dome.polymarket.markets.get_market_price({
    .token_id = "token-id",
    .at_time = 1740000000  // optional: Unix timestamp
});

// Get candlestick data
auto candles = dome.polymarket.markets.get_candlesticks({
    .condition_id = "0x...",
    .start_time = 1640995200,
    .end_time = 1672531200,
    .interval = 1440  // 1=1m, 60=1h, 1440=1d
});

// Get markets list
auto markets = dome.polymarket.markets.get_markets({
    .status = "open",
    .limit = 20,
    .min_volume = 100000.0
});

// Get orderbook snapshots
auto orderbooks = dome.polymarket.markets.get_orderbooks({
    .token_id = "token-id",
    .start_time = 1760470000000,  // milliseconds
    .end_time = 1760480000000,
    .limit = 100
});
```

### Orders

```cpp
auto orders = dome.polymarket.orders.get_orders({
    .market_slug = "bitcoin-up-or-down-july-25-8pm-et",
    .limit = 50
});

// Or by user
auto user_orders = dome.polymarket.orders.get_orders({
    .user = "0xYourWalletAddress",
    .limit = 100
});
```

### Wallet PnL

```cpp
auto pnl = dome.polymarket.wallet.get_wallet_pnl({
    .wallet_address = "0xYourWalletAddress",
    .granularity = dome::Granularity::day,
    .start_time = 1726857600,
    .end_time = 1758316829
});
```

### Activity

```cpp
auto activity = dome.polymarket.activity.get_activity({
    .user = "0xYourWalletAddress",
    .limit = 50
});
```

### WebSocket

Real-time order streaming from Polymarket:

```cpp
#include <dome_api_sdk/dome_websocket.hpp>

dome::DomeWebSocket ws(api_key);

// Set up callbacks
ws.set_order_event_callback([](const dome::WebSocketOrderEvent& event) {
    std::cout << "Order: " << event.data.side << " " 
              << event.data.shares_normalized << " shares @ " 
              << event.data.price << std::endl;
});

ws.set_ack_callback([](const std::string& sub_id) {
    std::cout << "Subscribed: " << sub_id << std::endl;
});

ws.set_connected_callback([&ws]() {
    // Subscribe to wallet addresses
    ws.subscribe({"0x123...", "0x456..."});
});

// Connect and run
ws.connect();

// Later: unsubscribe
ws.unsubscribe(subscription_id);
ws.disconnect();
```

Run the WebSocket example:

```bash
./websocket_example
```

## Error Handling

```cpp
try {
    auto result = dome.polymarket.markets.get_market_price({.token_id = "invalid"});
} catch (const dome::DomeAPIError& e) {
    std::cerr << "API Error: " << e.what() << std::endl;
    std::cerr << "HTTP Code: " << e.status_code << std::endl;
}
```

## License

MIT License
