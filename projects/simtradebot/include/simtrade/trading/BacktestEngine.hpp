#pragma once
#include <memory>
#include <vector>
#include <deque>

#include "simtrade/core/Types.hpp"
#include "simtrade/data/MarketDataFeed.hpp"
#include "simtrade/trading/TradingStrategy.hpp"
#include "simtrade/trading/RiskManager.hpp"
#include "simtrade/trading/ExecutionSimulator.hpp"
#include "simtrade/trading/PerformanceAnalyzer.hpp"

namespace simtrade {

enum class Regime { Range, TrendUp, TrendDown, HighVol };

class BacktestEngine : public IMarketDataObserver {
public:
    BacktestEngine(std::unique_ptr<TradingStrategy> strategy,
                   RiskManager risk,
                   ExecutionSimulator exec = ExecutionSimulator{},
                   PerformanceAnalyzer perf = PerformanceAnalyzer{});

    void onTick(const Tick& t) override;
    void run(const std::vector<Tick>& series);
    Metrics finalize() const;

    const std::vector<Tick>& ticks() const { return ticks_; }
    const std::vector<double>& equity() const { return equity_; }
    const std::vector<Trade>& trades() const { return exec_.trades(); }
    const std::vector<ClosedTrade>& closedTrades() const { return exec_.closedTrades(); }

    // --- knobs ---
    void setTrendWindow(std::size_t w) { trendWindow_ = w; trendPrices_.clear(); }
    void setVolWindow(std::size_t w) { volWindow_ = w; returns_.clear(); }
    void setHighVolZ(double z) { highVolZ_ = z; }
    void setMaxDrawdownStop(double dd) { maxDrawdownStop_ = dd; } // 0=off

    // Adaptive sizing
    void setRiskPerTrade(double pct) { riskPerTradePct_ = pct; }   // e.g. 0.005 = 0.5%
    void setMaxQty(int q) { maxQty_ = q; }

    Regime lastRegime() const { return lastRegime_; }

private:
    std::unique_ptr<TradingStrategy> strategy_;
    RiskManager risk_;
    ExecutionSimulator exec_;
    PerformanceAnalyzer perf_;

    std::vector<Tick> ticks_;
    std::vector<double> equity_;

    // trend regime
    std::size_t trendWindow_{200};
    std::deque<double> trendPrices_;

    // vol regime
    std::size_t volWindow_{100};
    std::deque<double> returns_;
    double highVolZ_{3.0};

    // dd stop
    double peakEquity_{0.0};
    double maxDrawdownStop_{0.0};

    // adaptive sizing
    double riskPerTradePct_{0.005};
    int maxQty_{5};

    // cached
    Regime lastRegime_{Regime::Range};

    static Order makeOrder(const Tick& t, Signal s, int qty);
    double slowMA() const;

    double returnsMean() const;
    double returnsStd() const;
    Regime detectRegime(double price) const;

    int adaptiveQty(double equityNow, double priceNow) const;

    void forceFlatten(const Tick& t);
};

} // namespace simtrade
