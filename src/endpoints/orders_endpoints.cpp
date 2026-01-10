#include "dome_api_sdk/orders_endpoints.hpp"

namespace dome {

OrdersEndpoints::OrdersEndpoints(const DomeSDKConfig& config)
    : BaseEndpoint(config) {}

OrdersResponse OrdersEndpoints::get_orders(const GetOrdersParams& params) {
    std::string endpoint = "/polymarket/orders";
    
    std::map<std::string, std::string> query_params;
    add_param_if_present(query_params, "market_slug", params.market_slug);
    add_param_if_present(query_params, "market_slugs", params.market_slugs);
    add_param_if_present(query_params, "condition_id", params.condition_id);
    add_param_if_present(query_params, "token_id", params.token_id);
    add_param_if_present(query_params, "user", params.user);
    add_param_if_present(query_params, "limit", params.limit);
    add_param_if_present(query_params, "offset", params.offset);
    add_param_if_present(query_params, "start_time", params.start_time);
    add_param_if_present(query_params, "end_time", params.end_time);
    
    auto json = http_client_->get(endpoint, query_params);
    
    OrdersResponse response;
    
    // Parse pagination
    if (json.contains("pagination")) {
        const auto& p = json["pagination"];
        response.pagination.total = p.value("total", 0);
        response.pagination.limit = p.value("limit", 0);
        response.pagination.offset = p.value("offset", 0);
        response.pagination.has_more = p.value("has_more", false);
    }
    
    if (json.contains("orders") && json["orders"].is_array()) {
        for (const auto& item : json["orders"]) {
            Order order;
            order.token_id = item.value("token_id", "");
            order.token_label = item.value("token_label", "");
            order.side = item.value("side", "");
            order.market_slug = item.value("market_slug", "");
            order.condition_id = item.value("condition_id", "");
            order.shares = item.value("shares", 0LL);
            order.shares_normalized = item.value("shares_normalized", 0.0);
            order.price = item.value("price", 0.0);
            order.tx_hash = item.value("tx_hash", "");
            order.title = item.value("title", "");
            order.timestamp = item.value("timestamp", 0LL);
            order.order_hash = item.value("order_hash", "");
            order.user = item.value("user", "");
            if (item.contains("taker") && !item["taker"].is_null()) {
                order.taker = item["taker"].get<std::string>();
            }
            response.orders.push_back(order);
        }
    }
    
    return response;
}

}  // namespace dome
