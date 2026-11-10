#include "simtrade/trading/WalkForward.hpp"
#include "simtrade/trading/BacktestEngine.hpp"
#include "simtrade/trading/StrategyFactory.hpp"
#include "simtrade/core/Logger.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

namespace simtrade {

std::vector<Tick> WalkForwardRunner::loadCsvTicks(const std::string& csvPath) {
    std::vector<Tick> out;
    std::ifstream f(csvPath);
    if (!f.is_open()) {
        Logger::info("WalkForward: failed to open CSV: " + csvPath);
        return out;
    }

    std::string line;
    bool first = true;
    while (std::getline(f, line)) {
        if (line.empty()) continue;

        if (first) {
            first = false;
            bool hasAlpha = false;
            for (char c : line) {
                if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) { hasAlpha = true; break; }
            }
            if (hasAlpha) continue;
        }

        char sep = (line.find(',') != std::string::npos) ? ',' : ';';
        std::stringstream ss(line);
        std::string tsS, priceS;
        if (!std::getline(ss, tsS, sep)) continue;
        if (!std::getline(ss, priceS)) continue;

        try {
            Tick t{};
            t.ts = std::stoll(tsS);
            t.price = std::stod(priceS);
            out.push_back(t);
        } catch (...) {}
    }
    return out;
}

static std::vector<Tick> sliceTicks(const std::vector<Tick>& all, size_t a, size_t b) {
    if (a >= all.size() || a >= b) return {};
    b = std::min(b, all.size());
    return std::vector<Tick>(all.begin() + (long)a, all.begin() + (long)b);
}

std::vector<WalkForwardFold> WalkForwardRunner::runMeanReversion(
    const std::string& csvPath,
    int maxTrades,
    const std::vector<std::size_t>& windows,
    const std::vector<double>& thresholds,
    const std::vector<int>& qtys,
    double trainRatio,
    int folds,
    int maxPosition,
    std::size_t trendWindow,
    std::size_t volWindow,
    double highVolZ,
    double maxDrawdownStop,
    double riskPerTradePct,
    int maxQty)
{
    std::vector<WalkForwardFold> results;
    const auto all = loadCsvTicks(csvPath);
    if (all.size() < 200) return results;

    const size_t n = all.size();
    const size_t trainEnd0 = (size_t)std::max(10.0, trainRatio * (double)n);
    const size_t remaining = n - trainEnd0;
    const size_t foldSize = (folds > 0) ? (remaining / (size_t)folds) : remaining;

    Optimizer opt;

    for (int k = 0; k < folds; ++k) {
        const size_t trainEnd = trainEnd0 + (size_t)k * foldSize;
        const size_t testEnd  = (k == folds - 1) ? n : (trainEnd + foldSize);

        auto trainTicks = sliceTicks(all, 0, trainEnd);
        auto testTicks  = sliceTicks(all, trainEnd, testEnd);
        if (trainTicks.size() < 200 || testTicks.size() < 50) break;

        // --- Train optimize: write temp CSV? We avoid that:
        // Reuse your Optimizer that works with csvPath by just optimizing on full file isn't correct.
        // So here: we do in-memory evaluation by running engines directly.

        // brute grid on trainTicks:
        Optimizer::BestResult best;
        best.metrics.netProfit = -1e300;

        for (auto w : windows) for (auto th : thresholds) for (auto q : qtys) {
            auto strat = StrategyFactory::create("meanreversion", w, th, q);
            BacktestEngine eng(std::move(strat), RiskManager(maxTrades, maxPosition));
            eng.setTrendWindow(trendWindow);
            eng.setVolWindow(volWindow);
            eng.setHighVolZ(highVolZ);
            eng.setMaxDrawdownStop(maxDrawdownStop);
            eng.setRiskPerTrade(riskPerTradePct);
            eng.setMaxQty(maxQty);
            eng.run(trainTicks);
            auto m = eng.finalize();
            if (m.netProfit > best.metrics.netProfit) {
                best.window = w; best.threshold = th; best.qty = q; best.metrics = m;
            }
        }

        // --- Test with best params
        auto strat2 = StrategyFactory::create("meanreversion", best.window, best.threshold, best.qty);
        BacktestEngine testEng(std::move(strat2), RiskManager(maxTrades, maxPosition));
        testEng.setTrendWindow(trendWindow);
        testEng.setVolWindow(volWindow);
        testEng.setHighVolZ(highVolZ);
        testEng.setMaxDrawdownStop(maxDrawdownStop);
        testEng.setRiskPerTrade(riskPerTradePct);
        testEng.setMaxQty(maxQty);
        testEng.run(testTicks);
        auto testM = testEng.finalize();

        WalkForwardFold fold{};
        fold.fold = k;
        fold.window = best.window;
        fold.threshold = best.threshold;
        fold.qty = best.qty;
        fold.trainMetrics = best.metrics;
        fold.testMetrics = testM;
        results.push_back(fold);
    }

    return results;
}

} // namespace simtrade
