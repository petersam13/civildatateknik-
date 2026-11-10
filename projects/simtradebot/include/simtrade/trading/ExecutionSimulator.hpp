#pragma once
#include <cstdint>
#include <deque>
#include <vector>
#include "simtrade/core/Types.hpp"

namespace simtrade {

/**
 * @brief Simulerad exekvering. Håller cash/position + fill-logg + CLOSED trades (realiserad PnL).
 *
 * Viktigt:
 * - För att BUY/SELL ska synas i HTML måste varje Trade få en timestamp (ts)
 *   som matchar Tick.ts-skalan (samma som CsvMarketDataFeed producerar).
 */
class ExecutionSimulator {
public:
    explicit ExecutionSimulator(double startingCash = 10000.0,
                                double slippageBps = 0.0,
                                double feePerTrade = 0.0);

    void execute(const Order& o);

    double startingCash() const { return startingCash_; }
    double cash() const { return cash_; }
    int position() const { return position_; }

    // Fills (BUY/SELL markers)
    const std::vector<Trade>& trades() const { return trades_; }
    // Closed trades (entry->exit, realized PnL)
    const std::vector<ClosedTrade>& closedTrades() const { return closedTrades_; }

private:
    struct Lot {
        std::int64_t ts{0};
        int qty{0};
        double price{0.0};
    };

    double startingCash_;
    double cash_;
    int position_;
    double slippageBps_;
    double feePerTrade_;

    std::deque<Lot> longLots_;  // BUY entries (qty>0)
    std::deque<Lot> shortLots_; // SELL entries (qty>0)

    std::vector<Trade> trades_;
    std::vector<ClosedTrade> closedTrades_;

    double applySlippage(double price, Signal side) const;

    // closes longs on SELL, closes shorts on BUY
    void closeLongLots(std::int64_t exitTs, int& qtyToClose, double exitPrice);
    void closeShortLots(std::int64_t exitTs, int& qtyToClose, double exitPrice);
};

} // namespace simtrade
