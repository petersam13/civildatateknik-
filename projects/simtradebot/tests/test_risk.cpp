#include <iostream>
#include "simtrade/trading/RiskManager.hpp"

static int fails = 0;
static void check(bool ok, const char* msg) {
    if (!ok) { std::cerr << "FAIL: " << msg << "\n"; ++fails; }
}

int main() {
    using namespace simtrade;

    // Max trades
    RiskManager rm(2);
    Order o{Signal::Buy, 1, 100.0, 1};

    check(rm.check(o, 0).allowed, "first allowed");
    rm.onTradeExecuted();

    check(rm.check(o, 1).allowed, "second allowed");
    rm.onTradeExecuted();

    check(!rm.check(o, 2).allowed, "third denied (max trades)");

    // Max position
    RiskManager pos(100, 3);
    check(pos.check(o, 2).allowed, "position 3 allowed");
    check(!pos.check(o, 3).allowed, "position 4 denied (max position)");
    Order sell{Signal::Sell, 1, 100.0, 2};
    check(pos.check(sell, 3).allowed, "reducing position allowed");
    if (fails == 0) std::cout << "OK: test_risk\n";
    return fails;
}
