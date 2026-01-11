/**
 * Dome SDK C++ - Polymarket Demo
 * 
 * This example demonstrates all the Polymarket API endpoints available
 * through the Dome SDK.
 * 
 * Usage:
 *   ./polymarket_demo
 */

#include <iostream>
#include <iomanip>
#include <dome_api_sdk/client.hpp>
#include <dome_api_sdk/utils.hpp>

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

void demo_orders(DomeClient& dome, const std::string& wallet_address) {
    print_separator("Orders");
    
    try {
        GetOrdersParams params;
        
        // Use wallet address if provided, otherwise query by market
        if (!wallet_address.empty()) {
            std::cout << "Fetching orders for user: " << wallet_address << "\n";
            params.user = wallet_address;
        } else {
            params.market_slug = "bitcoin-up-or-down-july-25-8pm-et";
        }
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

void demo_wallet_pnl(DomeClient& dome, const std::string& wallet_address) {
    print_separator("Wallet PnL");
    
    if (wallet_address.empty()) {
        std::cout << "Skipping Wallet PnL demo (no PROXY_WALLET configured)\n";
        return;
    }

    try {
        GetWalletPnLParams params;
        params.wallet_address = wallet_address;
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

void demo_activity(DomeClient& dome, const std::string& wallet_address) {
    print_separator("Activity");

    if (wallet_address.empty()) {
        std::cout << "Skipping Activity demo (no PROXY_WALLET configured)\n";
        return;
    }
    
    try {
        GetActivityParams params;
        params.user = wallet_address;
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

int main(int argc, char* argv[]) {
    std::cout << "Starting dome-sdk-cpp demo\n";
    
    // 1. Load API Key
    std::string api_key = load_config_value("DOME_API_KEY");
    
    // Check command line arg as fallback (backward compatibility)
    if (api_key.empty() && argc > 1) {
        api_key = argv[1];
    }
    
    if (api_key.empty()) {
        std::cerr << "\nError: No API key provided.\n";
        std::cerr << "Please set DOME_API_KEY in environment or .env file.\n";
        std::cerr << "Or usage: " << argv[0] << " <api_key>\n";
        return 1;
    }
    std::cout << "\nAPI Key: " << api_key.substr(0, 8) << "...\n";

    // 2. Load Proxy Wallet
    std::string proxy_wallet = load_config_value("PROXY_WALLET");
    if (!proxy_wallet.empty()) {
        std::cout << "Proxy Wallet: " << proxy_wallet << "\n";
    } else {
        std::cout << "Proxy Wallet: [Not Configured]\n";
    }
    
    // Create client
    DomeSDKConfig config;
    config.api_key = api_key;
    DomeClient dome(config);
    
    // Run demos
    demo_markets(dome);
    demo_market_price(dome);
    demo_candlesticks(dome);
    demo_orderbooks(dome);
    
    // Functions that use the wallet
    demo_orders(dome, proxy_wallet);
    demo_wallet_pnl(dome, proxy_wallet);
    demo_activity(dome, proxy_wallet);
    
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " Demo Complete!\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    return 0;
}
