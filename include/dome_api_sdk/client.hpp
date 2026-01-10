#ifndef DOME_CLIENT_HPP
#define DOME_CLIENT_HPP

#include "types.hpp"
#include "dome_api_sdk/polymarket_client.hpp"

namespace dome {

class DomeClient {
public:
    // Constructor with configuration map (for compatibility with Python SDK style)
    explicit DomeClient(const std::map<std::string, std::string>& config_map = {}) 
        : DomeClient(parse_config_map(config_map)) {}
    
    // Constructor with typed config
    explicit DomeClient(const DomeSDKConfig& config)
        : config_(config), polymarket(config) {}
    
    // Public sub-client for Polymarket
    PolymarketClient polymarket;

private:
    DomeSDKConfig config_;
    
    // Parse config from map (for compatibility with Python SDK style)
    static DomeSDKConfig parse_config_map(const std::map<std::string, std::string>& config_map) {
        DomeSDKConfig config;
        
        auto it = config_map.find("api_key");
        if (it != config_map.end()) {
            config.api_key = it->second;
        } else {
            // Try environment variable
            const char* env_key = std::getenv("DOME_API_KEY");
            if (env_key != nullptr) {
                config.api_key = env_key;
            }
        }
        
        it = config_map.find("base_url");
        if (it != config_map.end()) {
            config.base_url = it->second;
        }
        
        it = config_map.find("timeout");
        if (it != config_map.end()) {
            config.timeout = std::stof(it->second);
        }
        
        return config;
    }
};

}  // namespace dome

// Convenience using for top-level access
using DomeClient = dome::DomeClient;

#endif  // DOME_CLIENT_HPP
