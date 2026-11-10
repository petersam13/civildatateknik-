#pragma once
#include <string>
#include <vector>
#include "simtrade/core/Types.hpp"
#include "simtrade/trading/Optimizer.hpp"

namespace simtrade {

struct WalkForwardFold {
    int fold{0};
    Metrics trainMetrics{};
    Metrics testMetrics{};
    std::size_t window{0};
    double threshold{0.0};
    int qty{1};
};

class WalkForwardRunner {
public:
    // split: e.g. trainRatio=0.6, folds=3 => 3 test windows after first 60%
    std::vector<WalkForwardFold> runMeanReversion(
        const std::string& csvPath,
        int maxTrades,
        const std::vector<std::size_t>& windows,
        const std::vector<double>& thresholds,
        const std::vector<int>& qtys,
        double trainRatio = 0.6,
        int folds = 3,
        int maxPosition = 3,
        std::size_t trendWindow = 200,
        std::size_t volWindow = 100,
        double highVolZ = 3.0,
        double maxDrawdownStop = 0.0,
        double riskPerTradePct = 0.005,
        int maxQty = 5
    );

private:
    static std::vector<Tick> loadCsvTicks(const std::string& csvPath);
};

} // namespace simtrade
