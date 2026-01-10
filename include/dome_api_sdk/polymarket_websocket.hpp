#ifndef DOME_POLYMARKET_WEBSOCKET_HPP
#define DOME_POLYMARKET_WEBSOCKET_HPP

#include <string>
#include <vector>
#include <map>

#include <atomic>
#include <mutex>
#include "types.hpp"

namespace dome {

/**
 * WebSocket client for real-time Polymarket order events.
 * 
 * Note: Full WebSocket implementation requires a WebSocket library
 * (e.g., websocketpp, Boost.Beast, or libwebsockets).
 * This is a placeholder that demonstrates the API structure.
 */
class PolymarketWebSocketClient {
public:
    explicit PolymarketWebSocketClient(const std::string& api_key);
    ~PolymarketWebSocketClient();

    // Connection management
    void connect();
    void disconnect();
    bool is_connected() const;

    // Subscribe to order events
    // Returns subscription_id for managing the subscription
    std::string subscribe(const SubscribeFilters& params, OrderEventCallback on_event);

    // Update an existing subscription's filters
    void update(const std::string& subscription_id, const SubscribeFilters& params);

    // Unsubscribe from a subscription
    void unsubscribe(const std::string& subscription_id);

    // Get all active subscriptions
    std::vector<ActiveSubscription> get_active_subscriptions() const;

private:
    std::string api_key_;
    std::string ws_url_;
    std::atomic<bool> connected_{false};
    
    // Subscription management
    std::map<std::string, ActiveSubscription> subscriptions_;
    std::map<std::string, OrderEventCallback> callbacks_;
    mutable std::mutex subscriptions_mutex_;
    
    // Subscription ID counter
    int next_subscription_id_{1};
    
    // Generate a unique subscription ID
    std::string generate_subscription_id();
};

}  // namespace dome

#endif  // DOME_POLYMARKET_WEBSOCKET_HPP
