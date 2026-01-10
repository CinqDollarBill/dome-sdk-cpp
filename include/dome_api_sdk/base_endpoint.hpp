#ifndef DOME_BASE_ENDPOINT_HPP
#define DOME_BASE_ENDPOINT_HPP

#include <string>
#include <memory>
#include "http_client.hpp"
#include "types.hpp"

namespace dome {

class BaseEndpoint {
public:
    BaseEndpoint(const DomeSDKConfig& config);
    virtual ~BaseEndpoint() = default;

protected:
    std::shared_ptr<HttpClient> http_client_;
    DomeSDKConfig config_;

    // Helper to build query params from optional values
    template<typename T>
    void add_param_if_present(std::map<std::string, std::string>& params,
                              const std::string& key,
                              const std::optional<T>& value) {
        if (value.has_value()) {
            if constexpr (std::is_same_v<T, std::string>) {
                params[key] = value.value();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Join vector values with comma
                std::string joined;
                for (size_t i = 0; i < value.value().size(); ++i) {
                    if (i > 0) joined += ",";
                    joined += value.value()[i];
                }
                params[key] = joined;
            } else {
                params[key] = std::to_string(value.value());
            }
        }
    }
};

}  // namespace dome

#endif  // DOME_BASE_ENDPOINT_HPP
