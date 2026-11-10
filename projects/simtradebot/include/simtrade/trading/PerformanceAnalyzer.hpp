#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include "simtrade/core/Types.hpp"

namespace simtrade {

class PerformanceAnalyzer {
public:
    Metrics compute(double startingCash,
                    const std::vector<double>& equity,
                    const std::vector<Trade>&,
                    const std::vector<ClosedTrade>& closedTrades) const
    {
        Metrics m{};
        if (equity.empty()) return m;

        // Net profit
        m.netProfit = equity.back() - startingCash;

        // Max drawdown (absolute)
        double peak = equity.front();
        double maxDD = 0.0;
        for (double v : equity) {
            peak = std::max(peak, v);
            maxDD = std::max(maxDD, peak - v);
        }
        m.maxDrawdown = maxDD;

        // Closed-trade stats
        m.trades = static_cast<int>(closedTrades.size());
        if (!closedTrades.empty()) {
            double grossWin = 0.0, grossLoss = 0.0;
            int wins = 0, losses = 0;
            double sum = 0.0, sumWin = 0.0, sumLoss = 0.0;

            for (const auto& ct : closedTrades) {
                sum += ct.pnl;
                if (ct.pnl >= 0.0) { wins++; sumWin += ct.pnl; grossWin += ct.pnl; }
                else { losses++; sumLoss += ct.pnl; grossLoss += -ct.pnl; }
            }

            m.winRate = (wins + losses) ? (double)wins / (double)(wins + losses) : 0.0;
            m.avgTrade = sum / (double)closedTrades.size();
            m.avgWin = wins ? (sumWin / (double)wins) : 0.0;
            m.avgLoss = losses ? (sumLoss / (double)losses) : 0.0; // negative
            m.profitFactor = (grossLoss > 1e-12) ? (grossWin / grossLoss) : 0.0;
        }

        // Sharpe approximation from equity returns (per-tick returns)
        // (Not annualized; good for relative comparison.)
        if (equity.size() >= 3) {
            double mean = 0.0, m2 = 0.0;
            int n = 0;
            for (size_t i = 1; i < equity.size(); ++i) {
                const double prev = equity[i - 1];
                const double r = (prev != 0.0) ? (equity[i] - prev) / prev : 0.0;
                n++;
                const double delta = r - mean;
                mean += delta / n;
                const double delta2 = r - mean;
                m2 += delta * delta2;
            }
            const double var = (n > 1) ? (m2 / (n - 1)) : 0.0;
            const double sd = std::sqrt(std::max(0.0, var));
            m.sharpe = (sd > 1e-12) ? (mean / sd) : 0.0;
        }

        return m;
    }
};

} // namespace simtrade
