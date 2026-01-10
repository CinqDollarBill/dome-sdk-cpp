# Dome SDK C++

A C++ SDK for [Dome API](https://www.domeapi.io/), providing access to prediction market data from Polymarket.

## Features

- **Market Data**: Get market prices, candlesticks, market listings, and orderbook snapshots
- **Orders**: Query order data with filtering by market, user, and time range
- **Wallet Analytics**: Get wallet PnL data with configurable granularity
- **Activity Tracking**: Track MERGE, SPLIT, and REDEEM activities
- **WebSocket Support**: Real-time order event subscriptions (placeholder implementation)

## Requirements

- C++17 or later
- CMake 3.14+
- libcurl
- nlohmann/json (auto-fetched by CMake if not installed)

## Building

```bash
# Clone/navigate to the project
cd dome-sdk-cpp

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the demo (requires API key)
export DOME_API_KEY="your-api-key"
./polymarket_demo
```

## Quick Start

```cpp
#include <dome_api_sdk/client.hpp>
#include <iostream>

using namespace dome;

int main() {
    // Initialize with config map (like Python SDK)
    DomeClient dome({{"api_key", "your-dome-api-key-here"}});
    
    // Or use typed config
    dome::DomeSDKConfig config;
    config.api_key = "your-key";
    DomeClient dome2(config);
    
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
    std::cout << "Found " << markets.markets.size() << " markets" << std::endl;
    
    return 0;
}
```

To run this `quickstart` example:
```bash
# Build
cd build && make

# Run
./main
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
    .user = "0x7c3db723f1d4d8cb9c550095203b686cb11e5c6b",
    .limit = 100
});
```

### Wallet PnL

```cpp
auto pnl = dome.polymarket.wallet.get_wallet_pnl({
    .wallet_address = "0x7c3db723f1d4d8cb9c550095203b686cb11e5c6b",
    .granularity = dome::Granularity::day,
    .start_time = 1726857600,
    .end_time = 1758316829
});
```

### Activity

```cpp
auto activity = dome.polymarket.activity.get_activity({
    .user = "0x7c3db723f1d4d8cb9c550095203b686cb11e5c6b",
    .limit = 50
});
```

### WebSocket (Placeholder)

```cpp
auto& ws = dome.polymarket.websocket;
ws.connect();

auto sub_id = ws.subscribe(
    {.users = {"0x..."}},
    [](const dome::WebSocketOrderEvent& event) {
        std::cout << "Order: " << event.data.user << std::endl;
    }
);

// Later...
ws.unsubscribe(sub_id);
ws.disconnect();
```

> **Note**: Full WebSocket implementation requires a WebSocket library (e.g., websocketpp or Boost.Beast).

## Error Handling

```cpp
try {
    auto result = dome.polymarket.markets.get_market_price({.token_id = "invalid"});
} catch (const dome::DomeAPIError& e) {
    std::cerr << "API Error: " << e.what() << std::endl;
    std::cerr << "HTTP Code: " << e.status_code << std::endl;
    std::cerr << "Response: " << e.response_body << std::endl;
}
```

## Environment Variables

```bash
export DOME_API_KEY="your-api-key"
```

The SDK will automatically use `DOME_API_KEY` if no key is provided in the config.

## License

MIT License
