#pragma once
/**
 * @file HtmlChartWriter.hpp
 * @brief Skriver en self-contained HTML-chart för backtest.
 */

#include <string>
#include <vector>

#include "simtrade/core/Types.hpp"

namespace simtrade {

/**
 * @class HtmlChartWriter
 * @brief Exporterar tick- och trade-data till en HTML-fil som kan öppnas i webbläsare.
 *
 * @details
 *  HTML-filen innehåller all data inbäddad (TS/P/TR-arrayer) och stödjer:
 *   - Pris som linje eller candlesticks
 *   - Periodval (start/end)
 *   - BUY/SELL-markers med tooltip
 *   - Realized PnL per exekvering och positionsinfo
 *   - Equity curve (mark-to-market eller realized)
 */
class HtmlChartWriter {
public:
    /**
     * @brief Skriver en HTML-fil som innehåller chart + trades.
     *
     * @param outPath Sökväg till output HTML.
     * @param ticks        Tick-serie (ts, price) i tidsordning.
     * @param equity       Equity-kurva, ett värde per tick.
     * @param trades       Exekverade trades (ts, side, qty, price).
     * @param closedTrades Stängda trades med realiserad PnL.
     * @param m            För-beräknade metrics (används som initial text; JS räknar också).
     */
    void write(const std::string& outPath,
               const std::vector<Tick>& ticks,
               const std::vector<double>& equity,
               const std::vector<Trade>& trades,
               const std::vector<ClosedTrade>& closedTrades,
               const Metrics& m);
};

} // namespace simtrade
