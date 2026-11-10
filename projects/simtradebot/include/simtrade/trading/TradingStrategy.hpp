#pragma once
#include "../core/Types.hpp"
#include <string>

namespace simtrade {

/**
 * @brief Abstrakt bas-klass för handelsstrategier.
 *
 * Strategier tar emot Tick och returnerar en Signal.
 * Strategin ska vara “stateless” eller hantera sitt interna tillstånd själv.
 */
class TradingStrategy {
public:
    virtual ~TradingStrategy() = default;

    /**
     * @brief Genererar köp-/säljsignal baserat på aktuell tick.
     * @param t Aktuell marknadsdata.
     * @return Signal::Buy, Signal::Sell eller Signal::Hold.
     */
    
    /// Returnerar BUY/SELL/HOLD baserat på tick
    virtual Signal generateSignal(const Tick& t) = 0;

    /// Namn på strategin (för rapport/CLI)
    virtual std::string name() const = 0;

    /// Parametersträng (för rapport)
    virtual std::string parameters() const = 0;
};

} // namespace simtrade
