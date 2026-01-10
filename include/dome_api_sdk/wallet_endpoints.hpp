#ifndef DOME_WALLET_ENDPOINTS_HPP
#define DOME_WALLET_ENDPOINTS_HPP

#include "base_endpoint.hpp"
#include "types.hpp"

namespace dome {

class WalletEndpoints : public BaseEndpoint {
public:
    explicit WalletEndpoints(const DomeSDKConfig& config);

    // Get wallet PnL data
    // Endpoint: /polymarket/wallet/pnl/{wallet_address}
    WalletPnLResponse get_wallet_pnl(const GetWalletPnLParams& params);
};

}  // namespace dome

#endif  // DOME_WALLET_ENDPOINTS_HPP
