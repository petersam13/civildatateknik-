#include <iostream>
#include <vector>
#include "simtrade/trading/Optimizer.hpp"

static int fails = 0;
static void check(bool ok, const char* msg) {
    if (!ok) { std::cerr << "FAIL: " << msg << "\n"; ++fails; }
}

int main() {
    using namespace simtrade;

    Optimizer opt;
    auto best = opt.gridSearchMeanReversion(
        "tests/data/sample_prices.csv",
        50,
        {5, 10},
        {0.1, 0.2},
        {1}
    );

    check(best.window != 0, "optimizer produced result");
    if (fails == 0) std::cout << "OK: test_optimizer_smoke\n";
    return fails;
}
