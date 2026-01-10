#include <iostream>
#include <iomanip>
#include <dome_api_sdk/client.hpp>

using namespace dome;

#include <fstream>
#include <string>

std::string get_env_var(const std::string& key) {
    const char* val = std::getenv(key.c_str());
    return val ? std::string(val) : "";
}

std::string parse_env_file(const std::string& path, const std::string& key) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, key.length()) == key && line[key.length()] == '=') {
            return line.substr(key.length() + 1);
        }
    }
    return "";
}

std::string load_api_key() {
    // 1. Check environment variable
    std::string key = get_env_var("DOME_API_KEY");
    if (!key.empty()) return key;
    
    // 2. Check .env in current directory
    key = parse_env_file(".env", "DOME_API_KEY");
    if (!key.empty()) return key;
    
    // 3. Check .env in parent directory
    key = parse_env_file("../.env", "DOME_API_KEY");
    if (!key.empty()) return key;
    
    return "";
}

int main() {
    std::string api_key = load_api_key();
    if (api_key.empty()) {
        std::cerr << "Error: DOME_API_KEY not found in environment or .env file.\n";
        return 1;
    }
    DomeSDKConfig config;
    config.api_key = api_key;
    DomeClient dome(config);

    try {
        GetMarketPriceParams params;
        params.token_id = "53246717819011119677552303714112847791810741644457544333564494709938039872568";
        params.at_time = 1768027660;
        
        auto result = dome.polymarket.markets.get_market_price(params);
        
        std::cout << "Token ID: " << params.token_id.substr(0, 20) << "...\n";
        std::cout << "Price: " << std::fixed << std::setprecision(4) << result.price << "\n";
        std::cout << "At Time: " << result.at_time << "\n";
    } catch (const DomeAPIError& e) {
        std::cerr << "API Error: " << e.what() << " (HTTP " << e.status_code << ")\n";
    }
}

