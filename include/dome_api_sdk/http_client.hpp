#ifndef DOME_HTTP_CLIENT_HPP
#define DOME_HTTP_CLIENT_HPP

#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "types.hpp"

namespace dome {

class HttpClient {
public:
    HttpClient(const std::string& base_url, const std::string& api_key, float timeout = 30.0f);
    ~HttpClient();

    // Perform a GET request
    nlohmann::json get(const std::string& endpoint, 
                       const std::map<std::string, std::string>& query_params = {});

    // Perform a POST request
    nlohmann::json post(const std::string& endpoint,
                        const nlohmann::json& body = {});

    // Set custom headers
    void set_header(const std::string& key, const std::string& value);

private:
    std::string base_url_;
    std::string api_key_;
    float timeout_;
    std::map<std::string, std::string> headers_;

    // Build full URL with query parameters
    std::string build_url(const std::string& endpoint,
                          const std::map<std::string, std::string>& query_params);

    // URL encode a string
    static std::string url_encode(const std::string& value);

    // Perform the actual HTTP request
    std::string perform_request(const std::string& url, 
                                HTTPMethod method,
                                const std::string& body = "");
};

}  // namespace dome

#endif  // DOME_HTTP_CLIENT_HPP
