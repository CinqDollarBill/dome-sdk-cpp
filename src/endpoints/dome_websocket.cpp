#include "dome_api_sdk/dome_websocket.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

namespace dome {

using json = nlohmann::json;

// Dome WebSocket endpoint
static const std::string DOME_WS_BASE_URL = "wss://ws.domeapi.io/";

DomeWebSocket::DomeWebSocket(const std::string& api_key)
    : api_key_(api_key)
{
    std::string url = DOME_WS_BASE_URL + api_key_;
    client_ = std::make_unique<WebSocketClient>(url);
    
    // Set up internal message handler
    client_->set_message_callback([this](const std::string& msg) {
        on_message(msg);
    });
    
    client_->set_error_callback([this](const std::string& error) {
        ErrorCallback cb;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            cb = error_callback_;
        }
        if (cb) cb(error);
    });
    
    client_->set_connected_callback([this]() {
        ConnectedCallback cb;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            cb = connected_callback_;
        }
        if (cb) cb();
    });
    
    client_->set_disconnected_callback([this]() {
        DisconnectedCallback cb;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            cb = disconnected_callback_;
        }
        if (cb) cb();
    });
}

DomeWebSocket::~DomeWebSocket() {
    disconnect();
}

void DomeWebSocket::connect() {
    client_->connect();
}

void DomeWebSocket::disconnect() {
    client_->disconnect();
}

bool DomeWebSocket::is_connected() const {
    return client_->is_connected();
}

bool DomeWebSocket::subscribe(const std::vector<std::string>& user_addresses) {
    SubscribeFilters filters;
    filters.users = user_addresses;
    return subscribe(filters);
}

bool DomeWebSocket::subscribe(const SubscribeFilters& filters) {
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_filters_ = filters;
    }
    
    std::string message = build_subscribe_message(filters);
    client_->send(message);
    return true;
}

bool DomeWebSocket::unsubscribe(const std::string& subscription_id) {
    if (!is_connected()) {
        std::cerr << "DomeWebSocket: Cannot unsubscribe, not connected" << std::endl;
        return false;
    }
    
    std::string message = build_unsubscribe_message(subscription_id);
    client_->send(message);
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscriptions_.erase(subscription_id);
    }
    
    return true;
}

void DomeWebSocket::unsubscribe_all() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& [id, sub] : subscriptions_) {
        if (is_connected()) {
            std::string message = build_unsubscribe_message(id);
            client_->send(message);
        }
    }
    subscriptions_.clear();
}

void DomeWebSocket::on_message(const std::string& message) {
    try {
        json j = json::parse(message);
        std::string type = j.value("type", "");
        
        if (type == "ack") {
            std::string subscription_id = j.value("subscription_id", "");
            handle_ack_message(subscription_id);
        } else if (type == "event") {
            std::string subscription_id = j.value("subscription_id", "");
            std::string data_json = j.contains("data") ? j["data"].dump() : "{}";
            handle_event_message(subscription_id, data_json);
        }
    } catch (const json::exception& e) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (error_callback_) {
            error_callback_(std::string("JSON parse error: ") + e.what());
        }
    }
}

void DomeWebSocket::handle_ack_message(const std::string& subscription_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Store the subscription
    ActiveSubscription sub;
    sub.subscription_id = subscription_id;
    sub.filters = pending_filters_;
    sub.active = true;
    subscriptions_[subscription_id] = sub;
    
    if (ack_callback_) {
        ack_callback_(subscription_id);
    }
}

void DomeWebSocket::handle_event_message(const std::string& subscription_id, const std::string& data_json) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!order_event_callback_) {
        return;
    }
    
    try {
        json data = json::parse(data_json);
        
        WebSocketOrderEvent event;
        event.type = "event";
        event.subscription_id = subscription_id;
        
        // Parse order data
        event.data.token_id = data.value("token_id", "");
        event.data.token_label = data.value("token_label", "");
        event.data.side = data.value("side", "");
        event.data.market_slug = data.value("market_slug", "");
        event.data.condition_id = data.value("condition_id", "");
        event.data.shares = data.value("shares", 0.0f);
        event.data.shares_normalized = data.value("shares_normalized", 0.0);
        event.data.price = data.value("price", 0.0);
        event.data.tx_hash = data.value("tx_hash", "");
        event.data.title = data.value("title", "");
        event.data.timestamp = data.value("timestamp", 0LL);
        event.data.order_hash = data.value("order_hash", "");
        event.data.user = data.value("user", "");
        if (data.contains("taker") && !data["taker"].is_null()) {
            event.data.taker = data.value("taker", "");
        }
        
        order_event_callback_(event);
    } catch (const json::exception& e) {
        if (error_callback_) {
            error_callback_(std::string("Event parse error: ") + e.what());
        }
    }
}

std::string DomeWebSocket::build_subscribe_message(const SubscribeFilters& filters) {
    json msg;
    msg["action"] = "subscribe";
    msg["platform"] = "polymarket";
    msg["version"] = 1;
    msg["type"] = "orders";
    
    json filter_obj = json::object();
    
    if (filters.users.has_value() && !filters.users->empty()) {
        filter_obj["users"] = *filters.users;
    }
    if (filters.condition_ids.has_value() && !filters.condition_ids->empty()) {
        filter_obj["condition_ids"] = *filters.condition_ids;
    }
    if (filters.market_slugs.has_value() && !filters.market_slugs->empty()) {
        filter_obj["market_slugs"] = *filters.market_slugs;
    }
    
    msg["filters"] = filter_obj;
    
    return msg.dump();
}

std::string DomeWebSocket::build_unsubscribe_message(const std::string& subscription_id) {
    json msg;
    msg["action"] = "unsubscribe";
    msg["version"] = 1;
    msg["subscription_id"] = subscription_id;
    return msg.dump();
}

void DomeWebSocket::set_order_event_callback(OrderEventCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    order_event_callback_ = std::move(callback);
}

void DomeWebSocket::set_ack_callback(AckCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    ack_callback_ = std::move(callback);
}

void DomeWebSocket::set_error_callback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    error_callback_ = std::move(callback);
}

void DomeWebSocket::set_connected_callback(ConnectedCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    connected_callback_ = std::move(callback);
}

void DomeWebSocket::set_disconnected_callback(DisconnectedCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    disconnected_callback_ = std::move(callback);
}

const std::map<std::string, ActiveSubscription>& DomeWebSocket::get_subscriptions() const {
    return subscriptions_;
}

}  // namespace dome
