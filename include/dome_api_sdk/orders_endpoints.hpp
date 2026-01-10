#ifndef DOME_ORDERS_ENDPOINTS_HPP
#define DOME_ORDERS_ENDPOINTS_HPP

#include "base_endpoint.hpp"
#include "types.hpp"

namespace dome {

class OrdersEndpoints : public BaseEndpoint {
public:
    explicit OrdersEndpoints(const DomeSDKConfig& config);

    // Get orders with filtering
    // Endpoint: /polymarket/orders
    OrdersResponse get_orders(const GetOrdersParams& params = {});
};

}  // namespace dome

#endif  // DOME_ORDERS_ENDPOINTS_HPP
