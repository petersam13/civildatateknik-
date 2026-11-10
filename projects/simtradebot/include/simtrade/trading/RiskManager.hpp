#pragma once
#include <cstdlib>
#include "simtrade/core/Types.hpp"

namespace simtrade {

/**
 * @brief Beslut från RiskManager.
 */
struct RiskDecision {
    bool allowed;        ///< True om order får läggas.
    const char* reason;  ///< Kort förklaring (för logg/debug).
};

/**
 * @brief Enkel riskhantering för backtest.
 *
 * Funktioner:
 * - maxTrades: stoppar efter N exekverade orders
 * - maxPosition: begränsar absolut position (t.ex. 3 => [-3,+3])
 */
class RiskManager {
public:
    /**
     * @brief Skapar riskmanager.
     * @param maxTrades Max antal exekverade trades (orders) i hela körningen.
     * @param maxPosition Max absolut position.
     */
    explicit RiskManager(int maxTrades, int maxPosition = 3)
        : maxTrades_(maxTrades), maxPosition_(maxPosition) {}

    /**
     * @brief Validerar om en order får genomföras.
     * @param o Order som planeras.
     * @param currentPos Nuvarande position innan order.
     * @return RiskDecision med allowed+reason.
     */
    RiskDecision check(const Order& o, int currentPos) const {
        if (tradesSoFar_ >= maxTrades_) return {false, "Max trades reached"};

        int nextPos = currentPos;
        if (o.side == Signal::Buy)  nextPos += o.quantity;
        if (o.side == Signal::Sell) nextPos -= o.quantity;

        if (std::abs(nextPos) > maxPosition_) return {false, "Max position exceeded"};
        return {true, "OK"};
    }

    /**
     * @brief Informerar riskmanager att en trade/order exekverats.
     */
    void onTradeExecuted() { ++tradesSoFar_; }

    /// @return antal trades hittills.
    int tradesSoFar() const { return tradesSoFar_; }

    /// @return maxPosition.
    int maxPosition() const { return maxPosition_; }

private:
    int maxTrades_;
    int maxPosition_;
    int tradesSoFar_{0};
};

} // namespace simtrade
