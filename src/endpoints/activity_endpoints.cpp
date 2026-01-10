#include "dome_api_sdk/activity_endpoints.hpp"

namespace dome {

ActivityEndpoints::ActivityEndpoints(const DomeSDKConfig& config)
    : BaseEndpoint(config) {}

ActivityResponse ActivityEndpoints::get_activity(const GetActivityParams& params) {
    std::string endpoint = "/polymarket/activity";
    
    std::map<std::string, std::string> query_params;
    query_params["user"] = params.user;
    add_param_if_present(query_params, "start_time", params.start_time);
    add_param_if_present(query_params, "end_time", params.end_time);
    add_param_if_present(query_params, "market_slug", params.market_slug);
    add_param_if_present(query_params, "condition_id", params.condition_id);
    add_param_if_present(query_params, "limit", params.limit);
    add_param_if_present(query_params, "offset", params.offset);
    
    auto json = http_client_->get(endpoint, query_params);
    
    ActivityResponse response;
    
    // Parse pagination
    if (json.contains("pagination")) {
        const auto& p = json["pagination"];
        response.pagination.limit = p.value("limit", 0);
        response.pagination.offset = p.value("offset", 0);
        response.pagination.count = p.value("count", 0);
        response.pagination.has_more = p.value("has_more", false);
    }
    
    if (json.contains("activities") && json["activities"].is_array()) {
        for (const auto& item : json["activities"]) {
            Activity activity;
            activity.token_id = item.value("token_id", "");
            activity.side = item.value("side", "");
            activity.market_slug = item.value("market_slug", "");
            activity.condition_id = item.value("condition_id", "");
            activity.shares = item.value("shares", 0LL);
            activity.shares_normalized = item.value("shares_normalized", 0.0);
            activity.price = item.value("price", 0.0);
            activity.tx_hash = item.value("tx_hash", "");
            activity.title = item.value("title", "");
            activity.timestamp = item.value("timestamp", 0LL);
            activity.order_hash = item.value("order_hash", "");
            activity.user = item.value("user", "");
            response.activities.push_back(activity);
        }
    }
    
    return response;
}

}  // namespace dome
