/**
 * Dome SDK C++ - Polymarket Demo
 * 
 * This example demonstrates all the Polymarket API endpoints available
 * through the Dome SDK.
 * 
 * Usage:
 *   export DOME_API_KEY="your-api-key"
 *   ./polymarket_demo
 * 
 * Or:
 *   ./polymarket_demo your-api-key
 */

#include <iostream>
#include <iomanip>
#include <dome_api_sdk/client.hpp>

using namespace dome;

void print_separator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void demo_market_price(DomeClient& dome) {
    print_separator("Market Price");
    
    try {
        GetMarketPriceParams params;
        params.token_id = "59188569797573800265708961156960073059414997981580912261349270395396668774823";
        
        auto result = dome.polymarket.markets.get_market_price(params);
        
        std::cout << "Token ID: " << params.token_id.substr(0, 20) << "...\n";
        std::cout << "Price: " << std::fixed << std::setprecision(4) << result.price << "\n";
        std::cout << "At Time: " << result.at_time << "\n";
    } catch (const DomeAPIError& e) {
        std::cerr << "API Error: " << e.what() << " (HTTP " << e.status_code << ")\n";
    }
}

void demo_markets(DomeClient& dome) {
    print_separator("Markets List");
    
    try {
        GetMarketsParams params;
        params.status = "open";
        params.limit = 5;
        params.min_volume = 100000.0;
        
        auto result = dome.polymarket.markets.get_markets(params);
        
        std::cout << "Total markets: " << result.pagination.total << "\n";
        std::cout << "Showing: " << result.markets.size() << "\n\n";
        
        for (const auto& market : result.markets) {
            std::cout << "  Slug: " << market.market_slug << "\n";
            std::cout << "  Title: " << market.title.substr(0, 60);
            if (market.title.length() > 60) std::cout << "...";
            std::cout << "\n";
            std::cout << "  Volume: $" << std::fixed << std::setprecision(2) << market.volume_total << "\n";
            std::cout << "  Status: " << market.status << "\n";
            std::cout << "  Side A: " << market.side_a.label << " (" << market.side_a.id.substr(0, 10) << "...)\n";
            std::cout << "  Side B: " << market.side_b.label << " (" << market.side_b.id.substr(0, 10) << "...)\n";
            std::cout << "\n";
        }
    } catch (const DomeAPIError& e) {
        std::cerr << "API Error: " << e.what() << " (HTTP " << e.status_code << ")\n";
    }
}

void demo_candlesticks(DomeClient& dome) {
    print_separator("Candlestick Data");
    
    try {
        GetCandlesticksParams params;
        params.condition_id = "0x4567b275e6b667a6217f5cb4f06a797d3a1eaf1d0281fb5bc8c75e2046ae7e57";
        params.start_time = 1640995200;
        params.end_time = 1672531200;
        params.interval = 1440;  // Daily
        
        auto result = dome.polymarket.markets.get_candlesticks(params);
        
        std::cout << "Candlesticks: " << result.candlesticks.size() << "\n\n";
        
        int count = 0;
        for (const auto& candle : result.candlesticks) {
            if (count++ >= 5) break;  // Show first 5
            std::cout << "  End Period: " << candle.end_period_ts << "\n";
            std::cout << "  OHLC: " << candle.price.open << " / " << candle.price.high 
                      << " / " << candle.price.low << " / " << candle.price.close << "\n\n";
        }
    } catch (const DomeAPIError& e) {
        std::cerr << "API Error: " << e.what() << " (HTTP " << e.status_code << ")\n";
    }
}

void demo_orderbooks(DomeClient& dome) {
    print_separator("Orderbook Snapshots");
    
    try {
        GetOrderbooksParams params;
        params.token_id = "59188569797573800265708961156960073059414997981580912261349270395396668774823";
        params.start_time = 1760470000000;  // milliseconds
        params.end_time = 1760480000000;
        params.limit = 10;
        
        auto result = dome.polymarket.markets.get_orderbooks(params);
        
        std::cout << "Total snapshots: " << result.pagination.count << "\n";
        std::cout << "Showing: " << result.snapshots.size() << "\n\n";
        
        for (const auto& snapshot : result.snapshots) {
            std::cout << "  Timestamp: " << snapshot.timestamp << "\n";
            std::cout << "  Bids: " << snapshot.bids.size() << " levels\n";
            std::cout << "  Asks: " << snapshot.asks.size() << " levels\n\n";
        }
    } catch (const DomeAPIError& e) {
        std::cerr << "API Error: " << e.what() << " (HTTP " << e.status_code << ")\n";
    }
}

void demo_orders(DomeClient& dome) {
    print_separator("Orders");
    
    try {
        GetOrdersParams params;
        params.market_slug = "bitcoin-up-or-down-july-25-8pm-et";
        params.limit = 10;
        
        auto result = dome.polymarket.orders.get_orders(params);
        
        std::cout << "Total orders: " << result.pagination.total << "\n";
        std::cout << "Showing: " << result.orders.size() << "\n\n";
        
        for (const auto& order : result.orders) {
            std::cout << "  Order Hash: " << order.order_hash.substr(0, 16) << "...\n";
            std::cout << "  User: " << order.user.substr(0, 10) << "...\n";
            std::cout << "  Side: " << order.side << " " << order.token_label << "\n";
            std::cout << "  Price: " << std::fixed << std::setprecision(4) << order.price << "\n";
            std::cout << "  Shares: " << order.shares_normalized << "\n\n";
        }
    } catch (const DomeAPIError& e) {
        std::cerr << "API Error: " << e.what() << " (HTTP " << e.status_code << ")\n";
    }
}

void demo_wallet_pnl(DomeClient& dome) {
    print_separator("Wallet PnL");
    
    try {
        GetWalletPnLParams params;
        params.wallet_address = "0x7c3db723f1d4d8cb9c550095203b686cb11e5c6b";
        params.granularity = Granularity::day;
        params.start_time = 1726857600;
        params.end_time = 1758316829;
        
        auto result = dome.polymarket.wallet.get_wallet_pnl(params);
        
        std::cout << "Wallet: " << result.wallet_address << "\n";
        std::cout << "Granularity: " << result.granularity << "\n";
        std::cout << "Data points: " << result.pnl_over_time.size() << "\n\n";
        
        int count = 0;
        for (const auto& point : result.pnl_over_time) {
            if (count++ >= 5) break;  // Show first 5
            std::cout << "  " << point.timestamp << ": PnL to date: $" 
                      << std::fixed << std::setprecision(2) << point.pnl_to_date << "\n";
        }
    } catch (const DomeAPIError& e) {
        std::cerr << "API Error: " << e.what() << " (HTTP " << e.status_code << ")\n";
    }
}

void demo_activity(DomeClient& dome) {
    print_separator("Activity");
    
    try {
        GetActivityParams params;
        params.user = "0x7c3db723f1d4d8cb9c550095203b686cb11e5c6b";
        params.limit = 10;
        
        auto result = dome.polymarket.activity.get_activity(params);
        
        std::cout << "Total activities: " << result.pagination.count << "\n";
        std::cout << "Showing: " << result.activities.size() << "\n\n";
        
        for (const auto& activity : result.activities) {
            std::cout << "  Type: " << activity.side << "\n";
            std::cout << "  Market: " << activity.market_slug << "\n";
            std::cout << "  Shares: " << std::fixed << std::setprecision(4) 
                      << activity.shares_normalized << "\n";
            std::cout << "  Timestamp: " << activity.timestamp << "\n\n";
        }
    } catch (const DomeAPIError& e) {
        std::cerr << "API Error: " << e.what() << " (HTTP " << e.status_code << ")\n";
    }
}

void demo_websocket(DomeClient& dome) {
    print_separator("WebSocket (Placeholder Demo)");
    
    try {
        auto& ws = dome.polymarket.websocket;
        
        // Connect
        ws.connect();
        std::cout << "Connected: " << (ws.is_connected() ? "yes" : "no") << "\n";
        
        // Subscribe (placeholder - no actual messages received)
        SubscribeFilters params;
        params.users = std::vector<std::string>{"0x6031b6eed1c97e853c6e0f03ad3ce3529351f96d"};
        
        auto sub_id = ws.subscribe(params, [](const WebSocketOrderEvent& event) {
            std::cout << "Order event received!\n";
        });
        std::cout << "Subscription ID: " << sub_id << "\n";
        
        // Get active subscriptions
        auto subs = ws.get_active_subscriptions();
        std::cout << "Active subscriptions: " << subs.size() << "\n";
        
        // Cleanup
        ws.unsubscribe(sub_id);
        ws.disconnect();
        std::cout << "\nWebSocket demo complete.\n";
        std::cout << "\nNOTE: Full WebSocket functionality requires a WebSocket library.\n";
        std::cout << "This is a placeholder demonstrating the API structure.\n";
    } catch (const DomeAPIError& e) {
        std::cerr << "WebSocket Error: " << e.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Starting dome-sdk-cpp demo\n";
    
    // Get API key from argument or environment
    std::string api_key;
    if (argc > 1) {
        api_key = argv[1];
    } else {
        const char* env_key = std::getenv("DOME_API_KEY");
        if (env_key) {
            api_key = env_key;
        }
    }
    
    if (api_key.empty()) {
        std::cerr << "\nError: No API key provided.\n";
        std::cerr << "Usage: " << argv[0] << " <api_key>\n";
        std::cerr << "   Or: export DOME_API_KEY=your-key && " << argv[0] << "\n";
        return 1;
    }
    
    std::cout << "\nAPI Key: " << api_key.substr(0, 8) << "...\n";
    
    // Create client
    DomeSDKConfig config;
    config.api_key = api_key;
    DomeClient dome(config);
    
    // Run demos
    demo_markets(dome);
    demo_market_price(dome);
    demo_candlesticks(dome);
    demo_orderbooks(dome);
    demo_orders(dome);
    demo_wallet_pnl(dome);
    demo_activity(dome);
    demo_websocket(dome);
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " Demo Complete!\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    return 0;
}
