#include <iostream>
#include <vector>
#include <cmath>
#include "simtrade/trading/PerformanceAnalyzer.hpp"

static int fails = 0;
static void check(bool ok, const char* msg) {
    if (!ok) { std::cerr << "FAIL: " << msg << "\n"; ++fails; }
}

int main() {
    using namespace simtrade;

    PerformanceAnalyzer pa;
    std::vector<Trade> trades;

    // Equity: 100 -> 110 -> 95 -> 105  => net +5, max drawdown 15
    std::vector<double> equity{100.0, 110.0, 95.0, 105.0};

    std::vector<ClosedTrade> closed(3);
    closed[0].pnl = 10.0;
    closed[1].pnl = -15.0;
    closed[2].pnl = 10.0;

    auto m = pa.compute(100.0, equity, trades, closed);

    check(std::fabs(m.netProfit - 5.0) < 1e-9, "net profit = last equity - starting cash");
    check(std::fabs(m.maxDrawdown - 15.0) < 1e-9, "max drawdown peak-to-trough");
    check(m.trades == 3, "three closed trades");
    check(std::fabs(m.winRate - 2.0 / 3.0) < 1e-9, "win rate 2/3");
    check(std::fabs(m.profitFactor - 20.0 / 15.0) < 1e-9, "profit factor = gross win / gross loss");

    if (fails == 0) std::cout << "OK: test_pnl\n";
    return fails;
}
