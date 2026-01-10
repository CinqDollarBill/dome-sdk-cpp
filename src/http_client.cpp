#include "dome_api_sdk/http_client.hpp"
#include <curl/curl.h>


namespace dome {

// Callback function for libcurl to write response data
static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t total_size = size * nmemb;
    userp->append(static_cast<char*>(contents), total_size);
    return total_size;
}

HttpClient::HttpClient(const std::string& base_url, const std::string& api_key, float timeout)
    : base_url_(base_url), api_key_(api_key), timeout_(timeout) {
    // Initialize libcurl globally (should be done once per application)
    static bool curl_initialized = false;
    if (!curl_initialized) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_initialized = true;
    }

    // Set default headers
    headers_["Content-Type"] = "application/json";
    headers_["Accept"] = "application/json";
    headers_["x-dome-sdk"] = "cpp/1.0.0";
    if (!api_key_.empty()) {
        headers_["Authorization"] = "Bearer " + api_key_;
    }
}

HttpClient::~HttpClient() {
    // Note: curl_global_cleanup() should be called at application exit
}

void HttpClient::set_header(const std::string& key, const std::string& value) {
    headers_[key] = value;
}

std::string HttpClient::url_encode(const std::string& value) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return value;
    }

    char* encoded = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.length()));
    std::string result(encoded);
    curl_free(encoded);
    curl_easy_cleanup(curl);
    return result;
}

std::string HttpClient::build_url(const std::string& endpoint,
                                   const std::map<std::string, std::string>& query_params) {
    std::string url = base_url_;
    
    // Ensure base_url doesn't end with '/' and endpoint starts with '/'
    if (!base_url_.empty() && base_url_.back() == '/') {
        url = base_url_.substr(0, base_url_.length() - 1);
    }
    if (!endpoint.empty() && endpoint.front() != '/') {
        url += "/";
    }
    url += endpoint;

    // Add query parameters
    if (!query_params.empty()) {
        url += "?";
        bool first = true;
        for (const auto& [key, value] : query_params) {
            if (!first) {
                url += "&";
            }
            url += url_encode(key) + "=" + url_encode(value);
            first = false;
        }
    }

    return url;
}

std::string HttpClient::perform_request(const std::string& url,
                                         HTTPMethod method,
                                         const std::string& body) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw DomeAPIError(-1, "Failed to initialize CURL");
    }

    std::string response_body;
    long http_code = 0;

    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, static_cast<long>(timeout_));

    // Set write callback
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

    // Set headers
    struct curl_slist* headers = nullptr;
    for (const auto& [key, value] : headers_) {
        std::string header = key + ": " + value;
        headers = curl_slist_append(headers, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set method and body
    switch (method) {
        case HTTPMethod::GET:
            // GET is default
            break;
        case HTTPMethod::POST:
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            if (!body.empty()) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            }
            break;
        case HTTPMethod::PUT:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            if (!body.empty()) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            }
            break;
        case HTTPMethod::DELETE_:
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
    }

    // Perform request
    CURLcode res = curl_easy_perform(curl);

    // Get HTTP response code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    // Check for CURL errors
    if (res != CURLE_OK) {
        throw DomeAPIError(-1, std::string("CURL error: ") + curl_easy_strerror(res));
    }

    // Check for HTTP errors
    if (http_code >= 400) {
        std::string error_message = "HTTP Error " + std::to_string(http_code);
        
        // Try to parse JSON error response
        try {
            auto json_error = nlohmann::json::parse(response_body);
            if (json_error.contains("error")) {
                std::string api_error = json_error.value("error", "");
                std::string api_message = json_error.value("message", "Unknown error");
                error_message = "API Error: " + api_error + " - " + api_message;
            }
        } catch (...) {
            // If parsing fails, use default message
        }
        
        throw DomeAPIError(static_cast<int>(http_code), error_message, response_body);
    }

    return response_body;
}

nlohmann::json HttpClient::get(const std::string& endpoint,
                                const std::map<std::string, std::string>& query_params) {
    std::string url = build_url(endpoint, query_params);
    std::string response = perform_request(url, HTTPMethod::GET);
    
    try {
        return nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error& e) {
        throw DomeAPIError(-1, std::string("JSON parse error: ") + e.what(), response);
    }
}

nlohmann::json HttpClient::post(const std::string& endpoint, const nlohmann::json& body) {
    std::string url = build_url(endpoint, {});
    std::string body_str = body.empty() ? "" : body.dump();
    std::string response = perform_request(url, HTTPMethod::POST, body_str);
    
    try {
        return nlohmann::json::parse(response);
    } catch (const nlohmann::json::parse_error& e) {
        throw DomeAPIError(-1, std::string("JSON parse error: ") + e.what(), response);
    }
}

}  // namespace dome
