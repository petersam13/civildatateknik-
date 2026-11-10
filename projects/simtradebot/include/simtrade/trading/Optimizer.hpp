#pragma once
#include "StrategyFactory.hpp"
#include "BacktestEngine.hpp"
#include "../data/CsvMarketDataFeed.hpp"
#include <limits>
#include <vector>
#include <string>

namespace simtrade {

/** @brief Grid-search optimering: maximerar netProfit. */
class Optimizer {
public:
    struct BestResult {
        std::size_t window{0};
        double threshold{0.0};
        int qty{1};
        Metrics metrics{};
    };

    BestResult gridSearchMeanReversion(
        const std::string& csvPath,
        int maxTrades,
        const std::vector<std::size_t>& windows,
        const std::vector<double>& thresholds,
        const std::vector<int>& qtys,
        int maxPosition = 3,
        std::size_t trendWindow = 200,
        double maxDrawdownStop = 0.0 // 0 = off
    ) {
        BestResult best;
        best.metrics.netProfit = -std::numeric_limits<double>::infinity();

        for (auto w : windows) {
            for (auto th : thresholds) {
                for (auto q : qtys) {
                    auto strat = StrategyFactory::create("meanreversion", w, th, q);

                    BacktestEngine engine(std::move(strat), RiskManager(maxTrades, maxPosition));
                    engine.setTrendWindow(trendWindow);
                    engine.setMaxDrawdownStop(maxDrawdownStop);

                    CsvMarketDataFeed feed(csvPath);
                    feed.subscribe(&engine);
                    feed.run();

                    const auto m = engine.finalize();

                    if (m.netProfit > best.metrics.netProfit) {
                        best.window = w;
                        best.threshold = th;
                        best.qty = q;
                        best.metrics = m;
                    }
                }
            }
        }
        return best;
    }
};

} // namespace simtrade
