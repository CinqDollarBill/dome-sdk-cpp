#include "dome_api_sdk/wallet_endpoints.hpp"

namespace dome {

WalletEndpoints::WalletEndpoints(const DomeSDKConfig& config)
    : BaseEndpoint(config) {}

WalletPnLResponse WalletEndpoints::get_wallet_pnl(const GetWalletPnLParams& params) {
    std::string endpoint = "/polymarket/wallet/pnl/" + params.wallet_address;
    
    std::map<std::string, std::string> query_params;
    query_params["granularity"] = granularity_to_string(params.granularity);
    add_param_if_present(query_params, "start_time", params.start_time);
    add_param_if_present(query_params, "end_time", params.end_time);
    
    auto json = http_client_->get(endpoint, query_params);
    
    WalletPnLResponse response;
    response.granularity = json.value("granularity", "");
    response.start_time = json.value("start_time", 0LL);
    response.end_time = json.value("end_time", 0LL);
    response.wallet_address = json.value("wallet_address", "");
    
    if (json.contains("pnl_over_time") && json["pnl_over_time"].is_array()) {
        for (const auto& item : json["pnl_over_time"]) {
            PnLDataPoint point;
            point.timestamp = item.value("timestamp", 0LL);
            point.pnl_to_date = item.value("pnl_to_date", 0.0);
            response.pnl_over_time.push_back(point);
        }
    }
    
    return response;
}

}  // namespace dome
