#include <iostream>
#include <iomanip>
#include <dome_api_sdk/client.hpp>
#include "utils.hpp"

using namespace dome;

int main() {
    std::string api_key = load_config_value("DOME_API_KEY");
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

