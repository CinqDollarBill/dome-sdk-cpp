#ifndef WEBSOCKET_CLIENT_HPP
#define WEBSOCKET_CLIENT_HPP

#include <string>
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>

#include <ixwebsocket/IXWebSocket.h>

namespace dome {
class WebSocketClient {
public:
    using MessageCallback = std::function<void(const std::string&)>;
    using ErrorCallback = std::function<void(const std::string&)>;
    using ConnectedCallback = std::function<void()>;
    using DisconnectedCallback = std::function<void()>;

    explicit WebSocketClient(const std::string& url);
    ~WebSocketClient();

    // Connecition management
    void connect();
    void disconnect();
    bool is_connected() const;

    // Message handling
    void send(const std::string& message);
    void set_message_callback(MessageCallback callback);
    void set_error_callback(ErrorCallback callback);
    void set_connected_callback(ConnectedCallback callback);
    void set_disconnected_callback(DisconnectedCallback callback);
    
private:
    void setup_callbacks();
    void on_message(const ix::WebSocketMessagePtr& msg);
    
    std::string url_;
    std::unique_ptr<ix::WebSocket> ws_;
    
    std::atomic<bool> connected_{false};
    
    MessageCallback message_callback_;
    ErrorCallback error_callback_;
    ConnectedCallback connected_callback_;
    DisconnectedCallback disconnected_callback_;
    
    std::mutex callback_mutex_;
};
} // namespace dome

#endif // WEBSOCKET_CLIENT_HPP