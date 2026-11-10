#pragma once
#include <cstddef>
#include <deque>
#include <numeric>
#include <string>

#include "simtrade/trading/TradingStrategy.hpp"
#include "simtrade/core/Types.hpp"

namespace simtrade {

/**
 * @brief Mean reversion-strategi baserad på glidande medelvärde.
 *
 * - Beräknar mean över ett fönster (window)
 * - Om priset är över mean*(1+threshold) -> Sell
 * - Om priset är under mean*(1-threshold) -> Buy
 * - Annars Hold
 */
class MeanReversionStrategy final : public TradingStrategy {
public:
    MeanReversionStrategy(std::size_t window, double threshold, int qty)
        : window_(window), threshold_(threshold), qty_(qty) {}

    Signal generateSignal(const Tick& t) override {
        prices_.push_back(t.price);
        if (prices_.size() > window_) prices_.pop_front();

        if (prices_.size() < window_ || window_ == 0) return Signal::Hold;

        double sum = 0.0;
        for (double p : prices_) sum += p;
        const double mean = sum / static_cast<double>(prices_.size());

        if (mean <= 0.0) return Signal::Hold;

        const double upper = mean * (1.0 + threshold_);
        const double lower = mean * (1.0 - threshold_);

        if (t.price > upper) return Signal::Sell;
        if (t.price < lower) return Signal::Buy;
        return Signal::Hold;
    }

    // --- Required by TradingStrategy ---
    std::string name() const override { return "MeanReversion"; }

    std::string parameters() const override {
        return "window=" + std::to_string(window_) +
               ",threshold=" + std::to_string(threshold_) +
               ",qty=" + std::to_string(qty_);
    }

    // Optional getters
    std::size_t window() const { return window_; }
    double threshold() const { return threshold_; }
    int quantity() const { return qty_; }

private:
    std::size_t window_{0};
    double threshold_{0.0};
    int qty_{1};
    std::deque<double> prices_;
};

} // namespace simtrade
