#include "dome_api_sdk/polymarket_websocket.hpp"
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

namespace dome {

PolymarketWebSocketClient::PolymarketWebSocketClient(const std::string& api_key)
    : api_key_(api_key), ws_url_("wss://ws.domeapi.io/" + api_key) {}

PolymarketWebSocketClient::~PolymarketWebSocketClient() {
    if (connected_) {
        disconnect();
    }
}

void PolymarketWebSocketClient::connect() {
    // TODO: Implement actual WebSocket connection using websocketpp or Boost.Beast.
    // Logic should match Python client:
    // 1. Connect to ws_url_
    // 2. Add header "x-dome-sdk: cpp/1.0.0"
    // 3. Start receive loop
    // 4. Resubscribe to existing subscriptions on reconnect
    
    std::cout << "[WebSocket] Connecting to " << ws_url_ << std::endl;
    std::cout << "[WebSocket] Headers: x-dome-sdk: cpp/1.0.0" << std::endl;
    connected_ = true;
    std::cout << "[WebSocket] Connected (placeholder - no actual connection)" << std::endl;
}

void PolymarketWebSocketClient::disconnect() {
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    
    subscriptions_.clear();
    callbacks_.clear();
    connected_ = false;
    
    std::cout << "[WebSocket] Disconnected" << std::endl;
}

bool PolymarketWebSocketClient::is_connected() const {
    return connected_;
}

std::string PolymarketWebSocketClient::generate_subscription_id() {
    std::stringstream ss;
    ss << "sub_" << next_subscription_id_++;
    return ss.str();
}

std::string PolymarketWebSocketClient::subscribe(const SubscribeFilters& params, 
                                                   OrderEventCallback on_event) {
    if (!connected_) {
        throw DomeAPIError(-1, "WebSocket not connected. Call connect() first.");
    }
    
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    
    // In actual implementation, we'd generate a temporary request ID, 
    // send the message, and wait for an "ack" message containing the real subscription_id.
    // For this stub, we generate one immediately.
    std::string sub_id = generate_subscription_id();
    
    ActiveSubscription sub;
    sub.subscription_id = sub_id;
    sub.filters = params;
    sub.active = true;
    
    subscriptions_[sub_id] = sub;
    callbacks_[sub_id] = on_event;
    
    // Construct the actual message payload matching Python SDK
    nlohmann::json msg;
    msg["action"] = "subscribe";
    msg["platform"] = "polymarket";
    msg["version"] = 1;
    msg["type"] = "orders";
    
    nlohmann::json filters;
    if (params.users.has_value()) filters["users"] = params.users.value();
    if (params.condition_ids.has_value()) filters["condition_ids"] = params.condition_ids.value();
    if (params.market_slugs.has_value()) filters["market_slugs"] = params.market_slugs.value();
    msg["filters"] = filters;
    
    std::cout << "[WebSocket] Sending subscribe message: " << msg.dump() << std::endl;
    std::cout << "[WebSocket] Subscription confirmed: " << sub_id << std::endl;
    
    return sub_id;
}

void PolymarketWebSocketClient::update(const std::string& subscription_id,
                                         const SubscribeFilters& params) {
    if (!connected_) {
        throw DomeAPIError(-1, "WebSocket not connected. Call connect() first.");
    }
    
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    
    auto it = subscriptions_.find(subscription_id);
    if (it == subscriptions_.end()) {
        throw DomeAPIError(-1, "Subscription not found: " + subscription_id);
    }
    
    it->second.filters = params;
    
    // Match Python structure for update message
    nlohmann::json msg;
    msg["action"] = "update";
    msg["subscription_id"] = subscription_id;
    msg["platform"] = "polymarket";
    msg["version"] = 1;
    msg["type"] = "orders";
    
    nlohmann::json filters;
    if (params.users.has_value()) filters["users"] = params.users.value();
    if (params.condition_ids.has_value()) filters["condition_ids"] = params.condition_ids.value();
    if (params.market_slugs.has_value()) filters["market_slugs"] = params.market_slugs.value();
    msg["filters"] = filters;
    
    std::cout << "[WebSocket] Sending update message: " << msg.dump() << std::endl;
}

void PolymarketWebSocketClient::unsubscribe(const std::string& subscription_id) {
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    
    auto it = subscriptions_.find(subscription_id);
    if (it != subscriptions_.end()) {
        subscriptions_.erase(it);
    }
    
    auto cb_it = callbacks_.find(subscription_id);
    if (cb_it != callbacks_.end()) {
        callbacks_.erase(cb_it);
    }
    
    // Match Python structure for unsubscribe message
    nlohmann::json msg;
    msg["action"] = "unsubscribe";
    msg["version"] = 1;
    msg["subscription_id"] = subscription_id;
    
    std::cout << "[WebSocket] Sending unsubscribe message: " << msg.dump() << std::endl;
}

std::vector<ActiveSubscription> PolymarketWebSocketClient::get_active_subscriptions() const {
    std::lock_guard<std::mutex> lock(subscriptions_mutex_);
    
    std::vector<ActiveSubscription> result;
    for (const auto& [id, sub] : subscriptions_) {
        if (sub.active) {
            result.push_back(sub);
        }
    }
    
    return result;
}

}  // namespace dome
