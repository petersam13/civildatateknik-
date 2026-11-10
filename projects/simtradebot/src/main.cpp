#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "simtrade/data/CsvMarketDataFeed.hpp"
#include "simtrade/trading/BacktestEngine.hpp"
#include "simtrade/trading/MeanReversionStrategy.hpp"
#include "simtrade/trading/RiskManager.hpp"
#include "simtrade/trading/WalkForward.hpp"
#include "simtrade/ui/HtmlChartWriter.hpp"

int main(int argc, char** argv) {
    using namespace simtrade;

    // --------- Input CSV ----------
    std::string csvPath = "out_prices.csv";
    if (argc >= 2 && argv[1]) {
        csvPath = argv[1];
    }

    // --------- Strategy params (baseline) ----------
    std::size_t window = 50;
    double threshold = 0.02;
    int baseQty = 1;

    // --------- Risk / regime / sizing knobs ----------
    const int maxTrades = 100000;
    const int maxPosition = 3;

    const std::size_t trendWindow = 200;
    const std::size_t volWindow = 100;
    const double highVolZ = 3.0;

    const double maxDrawdownStop = 1500.0;  // 0 = off
    const double riskPerTradePct = 0.005;   // 0.5% av equity per trade
    const int maxQty = 5;

    // --------- Build engine ----------
    auto strategy = std::make_unique<MeanReversionStrategy>(window, threshold, baseQty);
    RiskManager risk(maxTrades, maxPosition);
    ExecutionSimulator exec(10000.0);
    PerformanceAnalyzer perf;

    BacktestEngine engine(std::move(strategy), risk, exec, perf);

    // knobs for regime / sizing
    engine.setTrendWindow(trendWindow);
    engine.setVolWindow(volWindow);
    engine.setHighVolZ(highVolZ);
    engine.setMaxDrawdownStop(maxDrawdownStop);
    engine.setRiskPerTrade(riskPerTradePct);
    engine.setMaxQty(maxQty);

    // --------- Run feed ----------
    CsvMarketDataFeed feed(csvPath);
    feed.subscribe(&engine);
    feed.run();

    // --------- Metrics ----------
    Metrics m = engine.finalize();

    std::cout
        << "=== Single Run ===\n"
        << "NetProfit: " << m.netProfit
        << " | MaxDD: " << m.maxDrawdown
        << " | ClosedTrades: " << m.trades
        << " | WinRate: " << (m.winRate * 100.0) << "%\n"
        << "PF: " << m.profitFactor
        << " | Sharpe: " << m.sharpe
        << " | AvgTrade: " << m.avgTrade
        << " | AvgWin: " << m.avgWin
        << " | AvgLoss: " << m.avgLoss
        << "\n\n";

    // --------- HTML chart ----------
    HtmlChartWriter chart;
    chart.write("out_chart.html",
                engine.ticks(),
                engine.equity(),
                engine.trades(),
                engine.closedTrades(),
                m);

    std::cout << "Wrote: out_chart.html\n\n";

    // --------- Walk Forward ----------
    WalkForwardRunner wf;
    auto folds = wf.runMeanReversion(
        csvPath,
        maxTrades,
        {50, 100, 200},          // windows
        {0.01, 0.02, 0.03},      // thresholds
        {1, 2},                  // qtys
        0.6,                     // trainRatio
        3,                       // folds
        maxPosition,
        trendWindow,
        volWindow,
        highVolZ,
        maxDrawdownStop,
        riskPerTradePct,
        maxQty
    );

    std::cout << "=== Walk Forward ===\n";
    if (folds.empty()) {
        std::cout << "No folds produced (not enough data?)\n";
    } else {
        for (const auto& f : folds) {
            std::cout
                << "Fold " << f.fold
                << " best(w=" << f.window << ", th=" << f.threshold << ", q=" << f.qty << ")"
                << " | TrainPnL=" << f.trainMetrics.netProfit
                << " | TestPnL=" << f.testMetrics.netProfit
                << " | TestPF=" << f.testMetrics.profitFactor
                << " | TestSharpe=" << f.testMetrics.sharpe
                << " | TestWinRate=" << (f.testMetrics.winRate * 100.0) << "%\n";
        }
    }

    return 0;
}
