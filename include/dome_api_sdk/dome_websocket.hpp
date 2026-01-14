#ifndef DOME_WEBSOCKET_HPP
#define DOME_WEBSOCKET_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>

#include "websocket_client.hpp"
#include "types.hpp"

namespace dome {

/**
 * DomeWebSocket - High-level WebSocket client for Dome API
 * 
 * Provides real-time order streaming from Polymarket via Dome's WebSocket server.
 * Handles subscription management, message parsing, and reconnection logic.
 * 
 * Example usage:
 *   DomeWebSocket ws(api_key);
 *   ws.set_order_event_callback([](const WebSocketOrderEvent& event) {
 *       std::cout << "Order: " << event.data.side << " " << event.data.shares_normalized << std::endl;
 *   });
 *   ws.connect();
 *   ws.subscribe({"0x123...", "0x456..."});
 */
class DomeWebSocket {
public:
    using AckCallback = std::function<void(const std::string& subscription_id)>;
    using OrderEventCallback = std::function<void(const WebSocketOrderEvent&)>;
    using ErrorCallback = std::function<void(const std::string&)>;
    using ConnectedCallback = std::function<void()>;
    using DisconnectedCallback = std::function<void()>;

    /**
     * Create a DomeWebSocket client.
     * @param api_key Your Dome API key for authentication
     */
    explicit DomeWebSocket(const std::string& api_key);
    ~DomeWebSocket();

    // Connection management
    void connect();
    void disconnect();
    bool is_connected() const;

    /**
     * Subscribe to order events for specific wallet addresses.
     * @param user_addresses Vector of wallet addresses to track
     * @return true if subscription message was sent
     */
    bool subscribe(const std::vector<std::string>& user_addresses);

    /**
     * Subscribe with custom filters.
     * @param filters Subscription filters (users, condition_ids, market_slugs)
     * @return true if subscription message was sent
     */
    bool subscribe(const SubscribeFilters& filters);

    /**
     * Unsubscribe from a subscription by ID.
     * @param subscription_id The ID returned in the ack message
     * @return true if unsubscribe message was sent
     */
    bool unsubscribe(const std::string& subscription_id);

    /**
     * Unsubscribe from all active subscriptions.
     */
    void unsubscribe_all();

    // Callback setters
    void set_order_event_callback(OrderEventCallback callback);
    void set_ack_callback(AckCallback callback);
    void set_error_callback(ErrorCallback callback);
    void set_connected_callback(ConnectedCallback callback);
    void set_disconnected_callback(DisconnectedCallback callback);

    // Get current active subscriptions
    const std::map<std::string, ActiveSubscription>& get_subscriptions() const;

private:
    void on_message(const std::string& message);
    void handle_ack_message(const std::string& subscription_id);
    void handle_event_message(const std::string& subscription_id, const std::string& data_json);
    std::string build_subscribe_message(const SubscribeFilters& filters);
    std::string build_unsubscribe_message(const std::string& subscription_id);

    std::string api_key_;
    std::unique_ptr<WebSocketClient> client_;
    
    std::map<std::string, ActiveSubscription> subscriptions_;
    SubscribeFilters pending_filters_;  // Filters for pending subscription (before ack)
    
    OrderEventCallback order_event_callback_;
    AckCallback ack_callback_;
    ErrorCallback error_callback_;
    ConnectedCallback connected_callback_;
    DisconnectedCallback disconnected_callback_;
    
    mutable std::mutex mutex_;
};

}  // namespace dome

#endif  // DOME_WEBSOCKET_HPP
