#include "simtrade/trading/BacktestEngine.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace simtrade {

BacktestEngine::BacktestEngine(std::unique_ptr<TradingStrategy> strategy,
                               RiskManager risk,
                               ExecutionSimulator exec,
                               PerformanceAnalyzer perf)
    : strategy_(std::move(strategy)),
      risk_(std::move(risk)),
      exec_(std::move(exec)),
      perf_(std::move(perf)) {}

Order BacktestEngine::makeOrder(const Tick& t, Signal s, int qty) {
    Order o{};
    o.ts = t.ts;
    o.price = t.price;
    o.side = s;
    o.quantity = std::max(0, qty);
    return o;
}

double BacktestEngine::slowMA() const {
    if (trendPrices_.empty()) return 0.0;
    const double sum = std::accumulate(trendPrices_.begin(), trendPrices_.end(), 0.0);
    return sum / (double)trendPrices_.size();
}

double BacktestEngine::returnsMean() const {
    if (returns_.empty()) return 0.0;
    const double sum = std::accumulate(returns_.begin(), returns_.end(), 0.0);
    return sum / (double)returns_.size();
}

double BacktestEngine::returnsStd() const {
    if (returns_.size() < 2) return 0.0;
    const double mu = returnsMean();
    double acc = 0.0;
    for (double r : returns_) {
        const double d = r - mu;
        acc += d * d;
    }
    return std::sqrt(acc / (double)(returns_.size() - 1));
}

Regime BacktestEngine::detectRegime(double price) const {
    // trend requires enough samples
    if (trendPrices_.size() < trendWindow_) return Regime::Range;

    const double ma = slowMA();

    // high vol detection by z-score of latest return
    if (returns_.size() >= volWindow_) {
        const double mu = returnsMean();
        const double sd = returnsStd();
        if (sd > 1e-12) {
            const double last = returns_.back();
            const double z = std::abs((last - mu) / sd);
            if (z >= highVolZ_) return Regime::HighVol;
        }
    }

    // trend direction
    if (price >= ma) return Regime::TrendUp;
    return Regime::TrendDown;
}

int BacktestEngine::adaptiveQty(double equityNow, double priceNow) const {
    // Risk budget per trade in currency
    const double budget = std::max(0.0, riskPerTradePct_) * std::max(1.0, equityNow);

    // Vol proxy: stddev of returns, convert to “price move”
    const double sd = returnsStd();
    const double volMove = std::max(1e-9, sd * priceNow);

    // qty ~= budget / volMove
    int q = (int)std::floor(budget / volMove);
    q = std::max(1, q);
    q = std::min(maxQty_, q);
    return q;
}

void BacktestEngine::forceFlatten(const Tick& t) {
    const int pos = exec_.position();
    if (pos == 0) return;

    if (pos > 0) {
        Order o = makeOrder(t, Signal::Sell, pos);
        if (risk_.check(o, exec_.position()).allowed) {
            exec_.execute(o);
            risk_.onTradeExecuted();
        }
    } else {
        Order o = makeOrder(t, Signal::Buy, -pos);
        if (risk_.check(o, exec_.position()).allowed) {
            exec_.execute(o);
            risk_.onTradeExecuted();
        }
    }
}

void BacktestEngine::onTick(const Tick& t) {
    // store tick
    ticks_.push_back(t);

    // update trend buffer
    trendPrices_.push_back(t.price);
    if (trendPrices_.size() > trendWindow_) trendPrices_.pop_front();

    // update return buffer
    if (ticks_.size() >= 2) {
        const double p0 = ticks_[ticks_.size() - 2].price;
        const double r = (p0 != 0.0) ? (t.price - p0) / p0 : 0.0;
        returns_.push_back(r);
        if (returns_.size() > volWindow_) returns_.pop_front();
    }

    // equity now (before decision) for sizing
    const double eqNow = exec_.cash() + (double)exec_.position() * t.price;

    // detect regime
    lastRegime_ = detectRegime(t.price);

    // default: do nothing in high vol
    Signal sig = Signal::Hold;
    if (lastRegime_ != Regime::HighVol && strategy_) {
        sig = strategy_->generateSignal(t);
    }

    // Adaptive sizing
    int qty = 1;
    if (returns_.size() >= 10) { // need some vol history
        qty = adaptiveQty(eqNow, t.price);
    }

    // Regime rules:
    // - TrendUp: block opening/increasing shorts
    // - TrendDown: block opening/increasing longs
    // - Range: allow both
    if (sig != Signal::Hold) {
        const int pos = exec_.position();

        if (lastRegime_ == Regime::TrendUp && sig == Signal::Sell && pos <= 0)
            sig = Signal::Hold;

        if (lastRegime_ == Regime::TrendDown && sig == Signal::Buy && pos >= 0)
            sig = Signal::Hold;
    }

    // Execute
    if (sig != Signal::Hold) {
        Order o = makeOrder(t, sig, qty);
        const RiskDecision d = risk_.check(o, exec_.position());
        if (d.allowed) {
            exec_.execute(o);
            risk_.onTradeExecuted();
        }
    }

    // equity after
    const double eq = exec_.cash() + (double)exec_.position() * t.price;
    equity_.push_back(eq);

    // DD stop
    if (peakEquity_ <= 0.0) peakEquity_ = eq;
    peakEquity_ = std::max(peakEquity_, eq);

    if (maxDrawdownStop_ > 0.0) {
        const double dd = peakEquity_ - eq;
        if (dd >= maxDrawdownStop_) {
            forceFlatten(t);
            const double eq2 = exec_.cash() + (double)exec_.position() * t.price;
            equity_.back() = eq2;
            peakEquity_ = eq2;
        }
    }
}

void BacktestEngine::run(const std::vector<Tick>& series) {
    for (const auto& x : series) onTick(x);
}

Metrics BacktestEngine::finalize() const {
    if (ticks_.empty() || equity_.empty()) return Metrics{};
    return perf_.compute(exec_.startingCash(), equity_, exec_.trades(), exec_.closedTrades());
}

} // namespace simtrade
