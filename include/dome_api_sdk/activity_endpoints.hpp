#ifndef DOME_ACTIVITY_ENDPOINTS_HPP
#define DOME_ACTIVITY_ENDPOINTS_HPP

#include "base_endpoint.hpp"
#include "types.hpp"

namespace dome {

class ActivityEndpoints : public BaseEndpoint {
public:
    explicit ActivityEndpoints(const DomeSDKConfig& config);

    // Get trading activity (MERGE, SPLIT, REDEEM)
    // Endpoint: /polymarket/activity
    ActivityResponse get_activity(const GetActivityParams& params);
};

}  // namespace dome

#endif  // DOME_ACTIVITY_ENDPOINTS_HPP
