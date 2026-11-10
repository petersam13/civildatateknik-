#include "simtrade/trading/StrategyFactory.hpp"
#include "simtrade/trading/MeanReversionStrategy.hpp"
#include <stdexcept>

namespace simtrade {

std::unique_ptr<TradingStrategy> StrategyFactory::create(
    const std::string& name, std::size_t window, double threshold, int qty
) {
    if (name == "meanreversion") {
        return std::make_unique<MeanReversionStrategy>(window, threshold, qty);
    }
    throw std::invalid_argument("Unknown strategy: " + name);
}

} // namespace simtrade
