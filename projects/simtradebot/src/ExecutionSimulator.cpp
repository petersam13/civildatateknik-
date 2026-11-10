#include "simtrade/trading/ExecutionSimulator.hpp"

#include <algorithm>

namespace simtrade {

ExecutionSimulator::ExecutionSimulator(double startingCash, double slippageBps, double feePerTrade)
    : startingCash_(startingCash),
      cash_(startingCash),
      position_(0),
      slippageBps_(slippageBps),
      feePerTrade_(feePerTrade) {}

double ExecutionSimulator::applySlippage(double price, Signal side) const {
    const double bps = slippageBps_ / 10000.0; // 1 bps = 0.01%
    if (side == Signal::Buy)  return price * (1.0 + bps);
    if (side == Signal::Sell) return price * (1.0 - bps);
    return price;
}

void ExecutionSimulator::closeLongLots(std::int64_t exitTs, int& qtyToClose, double exitPrice) {
    // SELL closes existing long lots first (FIFO)
    while (qtyToClose > 0 && !longLots_.empty()) {
        Lot& lot = longLots_.front();
        const int matched = std::min(qtyToClose, lot.qty);

        ClosedTrade ct{};
        ct.entryTs = lot.ts;
        ct.exitTs = exitTs;
        ct.entrySide = Signal::Buy; // long
        ct.quantity = matched;
        ct.entryPrice = lot.price;
        ct.exitPrice = exitPrice;
        ct.pnl = (exitPrice - lot.price) * static_cast<double>(matched);
        closedTrades_.push_back(ct);

        lot.qty -= matched;
        qtyToClose -= matched;
        if (lot.qty == 0) longLots_.pop_front();
    }
}

void ExecutionSimulator::closeShortLots(std::int64_t exitTs, int& qtyToClose, double exitPrice) {
    // BUY closes existing short lots first (FIFO)
    while (qtyToClose > 0 && !shortLots_.empty()) {
        Lot& lot = shortLots_.front();
        const int matched = std::min(qtyToClose, lot.qty);

        ClosedTrade ct{};
        ct.entryTs = lot.ts;
        ct.exitTs = exitTs;
        ct.entrySide = Signal::Sell; // short
        ct.quantity = matched;
        ct.entryPrice = lot.price;
        ct.exitPrice = exitPrice;
        ct.pnl = (lot.price - exitPrice) * static_cast<double>(matched);
        closedTrades_.push_back(ct);

        lot.qty -= matched;
        qtyToClose -= matched;
        if (lot.qty == 0) shortLots_.pop_front();
    }
}

void ExecutionSimulator::execute(const Order& o) {
    const int qty0 = std::max(0, o.quantity);
    if (qty0 == 0) return;

    const Signal side = o.side;
    if (side != Signal::Buy && side != Signal::Sell) return;

    const double fillPrice = applySlippage(o.price, side);

    // 1) Build closed trades via FIFO lots
    int qty = qty0;
    if (side == Signal::Buy) {
        // If we're short, BUY closes shorts first
        closeShortLots(o.ts, qty, fillPrice);
        // Remaining qty opens long lots
        if (qty > 0) longLots_.push_back(Lot{ o.ts, qty, fillPrice });
    } else { // Sell
        // If we're long, SELL closes longs first
        closeLongLots(o.ts, qty, fillPrice);
        // Remaining qty opens short lots
        if (qty > 0) shortLots_.push_back(Lot{ o.ts, qty, fillPrice });
    }

    // 2) Update cash/position (fills are always qty0)
    if (side == Signal::Buy) {
        cash_ -= fillPrice * static_cast<double>(qty0);
        cash_ -= feePerTrade_;
        position_ += qty0;
    } else {
        cash_ += fillPrice * static_cast<double>(qty0);
        cash_ -= feePerTrade_;
        position_ -= qty0;
    }

    // 3) Save fill for chart markers
    Trade tr{};
    tr.ts = o.ts;
    tr.side = side;
    tr.quantity = qty0;
    tr.price = fillPrice;
    trades_.push_back(tr);
}

} // namespace simtrade
