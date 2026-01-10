#include <dome_api_sdk/base_endpoint.hpp>

namespace dome {

BaseEndpoint::BaseEndpoint(const DomeSDKConfig& config) : config_(config) {
    // Resolve API key from config or environment
    std::string api_key = config.api_key;
    if (api_key.empty()) {
        const char* env_key = std::getenv("DOME_API_KEY");
        if (env_key != nullptr) {
            api_key = env_key;
        }
    }

    // Create HTTP client
    http_client_ = std::make_shared<HttpClient>(
        config.base_url,
        api_key,
        config.timeout
    );
}

}  // namespace dome
