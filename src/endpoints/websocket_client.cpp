#include "dome_api_sdk/websocket_client.hpp"
#include <iostream>

namespace dome {

// Constructor
WebSocketClient::WebSocketClient(const std::string& url)
    : url_(url)
    , ws_(std::make_unique<ix::WebSocket>())
{
    ws_->setUrl(url_);
    setup_callbacks();
}

// Destructor
WebSocketClient::~WebSocketClient() {
    disconnect();
}

// Setup internal ixwebsocket callbacks
void WebSocketClient::setup_callbacks() {
    ws_->setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        on_message(msg);
    });
}

// Handle incoming WebSocket messages
void WebSocketClient::on_message(const ix::WebSocketMessagePtr& msg) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    
    switch (msg->type) {
        case ix::WebSocketMessageType::Open:
            connected_ = true;
            if (connected_callback_) {
                connected_callback_();
            }
            break;
            
        case ix::WebSocketMessageType::Close:
            connected_ = false;
            if (disconnected_callback_) {
                disconnected_callback_();
            }
            break;
            
        case ix::WebSocketMessageType::Message:
            if (message_callback_) {
                message_callback_(msg->str);
            }
            break;
            
        case ix::WebSocketMessageType::Error:
            if (error_callback_) {
                error_callback_(msg->errorInfo.reason);
            }
            break;
            
        case ix::WebSocketMessageType::Ping:
            // ixwebsocket handles pong automatically
            break;
            
        case ix::WebSocketMessageType::Pong:
            // Pong received - connection is alive
            break;
            
        case ix::WebSocketMessageType::Fragment:
            // Handle message fragments if needed
            break;
    }
}

// Connect to WebSocket server
void WebSocketClient::connect() {
    // TODO: Add any pre-connection setup here (e.g., authentication headers)
    // ws_->setExtraHeaders({{"Authorization", "Bearer YOUR_API_KEY"}});
    
    ws_->start();
}

// Disconnect from WebSocket server
void WebSocketClient::disconnect() {
    ws_->stop();
    connected_ = false;
}

// Check if connected
bool WebSocketClient::is_connected() const {
    return connected_;
}

// Send a message
void WebSocketClient::send(const std::string& message) {
    if (connected_) {
        ws_->send(message);
    } else {
        // TODO: Handle send when not connected - queue message or throw?
        std::cerr << "WebSocketClient: Cannot send, not connected" << std::endl;
    }
}

// Set callback for incoming messages
void WebSocketClient::set_message_callback(MessageCallback callback) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    message_callback_ = std::move(callback);
}

// Set callback for errors
void WebSocketClient::set_error_callback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    error_callback_ = std::move(callback);
}

// Set callback for connection established
void WebSocketClient::set_connected_callback(ConnectedCallback callback) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    connected_callback_ = std::move(callback);
}

// Set callback for disconnection
void WebSocketClient::set_disconnected_callback(DisconnectedCallback callback) {
    std::lock_guard<std::mutex> lock(callback_mutex_);
    disconnected_callback_ = std::move(callback);
}

} // namespace dome