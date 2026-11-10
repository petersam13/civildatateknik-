#pragma once
#include <memory>
#include <string>
#include "TradingStrategy.hpp"

namespace simtrade {

/** @brief Factory pattern: skapa strategi via namn + parametrar. */
class StrategyFactory {
public:
    static std::unique_ptr<TradingStrategy> create(
        const std::string& name,
        std::size_t window,
        double threshold,
        int qty
    );
};

} // namespace simtrade
