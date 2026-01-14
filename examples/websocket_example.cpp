/**
 * WebSocket Example - Dome API SDK
 * 
 * This example demonstrates how to connect to the Dome API WebSocket
 * and subscribe to real-time order events from Polymarket.
 * 
 * Usage:
 *   1. Set DOME_API_KEY in .env file or environment variable
 *   2. Optionally set PROXY_WALLET to subscribe to your orders
 *   3. Run: ./websocket_example
 */

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

#include "dome_api_sdk/dome_websocket.hpp"
#include "dome_api_sdk/utils.hpp"

// Global flag for graceful shutdown
static std::atomic<bool> g_running{true};

void signal_handler(int signal) {
    std::cout << "\n[INFO] Shutting down..." << std::endl;
    g_running = false;
}

void print_order_event(const dome::WebSocketOrderEvent& event) {
    std::cout << "Market: " << event.data.title << std::endl;
    std::cout << "Slug:   " << event.data.market_slug << std::endl;
    std::cout << "Side:   " << event.data.side << " Token: " << event.data.token_label << std::endl;
    std::cout << "Price:  " << std::fixed << std::setprecision(2) << event.data.price 
              << " Shares: " << event.data.shares_normalized << std::endl;
    std::cout << "User:   " << event.data.user << std::endl;
    if (!event.data.taker.value_or("").empty()) {
        std::cout << "Taker:  " << event.data.taker.value_or("") << std::endl;
    }
    std::cout << "TxHash: " << event.data.tx_hash << std::endl;
    std::cout << std::endl;
}

int main() {
    // Register signal handler for graceful shutdown
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    std::cout << "Dome WebSocket Client Example" << std::endl;
    
    // 1. Load API Key
    std::string api_key = dome::load_config_value("DOME_API_KEY");
    if (api_key.empty()) {
        std::cerr << "[ERROR] No API key provided." << std::endl;
        std::cerr << "        Set DOME_API_KEY in .env file or environment variable." << std::endl;
        return 1;
    }
    std::cout << "[INFO] API Key: " << api_key.substr(0, 8) << "..." << std::endl;
    
    // 2. Load wallet addresses to track
    std::string proxy_wallet = dome::load_config_value("PROXY_WALLET");
    std::vector<std::string> wallets_to_track;
    
    if (!proxy_wallet.empty()) {
        std::cout << "[INFO] Tracking wallet: " << proxy_wallet << std::endl;
        wallets_to_track.push_back(proxy_wallet);
    } else {
        // Example wallet addresses to track (replace with real addresses)
        wallets_to_track = {
            "0x0ea574f3204c5c9c0cdead90392ea0990f4d17e4",
        };
        std::cout << "[INFO] No PROXY_WALLET set, using example addresses:" << std::endl;
        for (const auto& wallet : wallets_to_track) {
            std::cout << "       - " << wallet << std::endl;
        }
    }
    
    // 3. Create DomeWebSocket client
    dome::DomeWebSocket ws(api_key);
    
    // 4. Set up callbacks
    ws.set_connected_callback([&ws, &wallets_to_track]() {
        std::cout << "[CONNECTED] WebSocket connection established!" << std::endl;
        std::cout << "[INFO] Subscribing to order events..." << std::endl;
        
        // Subscribe to order events for the specified wallets
        ws.subscribe(wallets_to_track);
    });
    
    ws.set_disconnected_callback([]() {
        std::cout << "[DISCONNECTED] WebSocket connection closed." << std::endl;
    });
    
    ws.set_ack_callback([](const std::string& subscription_id) {
        std::cout << "[ACK] Subscription confirmed: " << subscription_id << std::endl;
        std::cout << "[INFO] Listening for order events..." << std::endl;
    });
    
    ws.set_order_event_callback([](const dome::WebSocketOrderEvent& event) {
        print_order_event(event);
    });
    
    ws.set_error_callback([](const std::string& error) {
        std::cerr << "[ERROR] " << error << std::endl;
    });
    
    // Debug: Print raw messages by adding a wrapper
    // Note: We'll use the internal client's raw message callback for debugging
    
    // 5. Connect to WebSocket
    std::cout << "[INFO] Connecting to Dome WebSocket..." << std::endl;
    ws.connect();
    
    // 6. Run event loop
    std::cout << "[INFO] Press Ctrl+C to exit" << std::endl;
    std::cout << std::endl;
    
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // 7. Clean shutdown - use quick_exit since WebSocket stop() may block
    std::cout << "[INFO] Exiting..." << std::endl;
    std::quick_exit(0);
}
