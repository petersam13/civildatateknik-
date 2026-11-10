#pragma once
#include <cstdint>
#include <string>

namespace simtrade {

/**
 * @brief Marknadsdata vid en tidpunkt (tick).
 *
 * Tick används som primär input i backtestmotorn.
 * ts bör vara i samma tidsenhet genom hela systemet (t.ex. epoch ms eller sekunder).
 */
struct Tick {
    std::int64_t ts{};   ///< Timestamp (epoch ms/sek eller annan konsekvent enhet).
    double price{};      ///< Senaste pris.
};

/**
 * @brief Handelssignal från en strategi.
 */
enum class Signal { Buy, Sell, Hold };

/**
 * @brief Order som skickas till exekveringssimulatorn.
 *
 * I detta projekt är ordern marknadsnära: pris sätts till tick-priset
 * och slippage/avgifter hanteras i ExecutionSimulator.
 */
struct Order {
    Signal side{Signal::Hold};   ///< Buy/Sell/Hold
    int quantity{0};             ///< Antal enheter
    double price{0.0};           ///< Pris vid ordertillfället (oftast tick.price)
    std::int64_t ts{0};           ///< Timestamp (matchar Tick.ts)
};

/**
 * @brief En fill/exekverad order (för visualisering och logg).
 *
 * Trade representerar en faktisk exekvering och används för BUY/SELL-markörer i grafen.
 */
struct Trade {
    std::int64_t ts{0};          ///< Timestamp för fill
    Signal side{Signal::Hold};   ///< Buy/Sell
    int quantity{0};             ///< Fylld kvantitet
    double price{0.0};           ///< Fill-pris (inkl slippage om aktiverat)
};

/**
 * @brief Ett ihop-parat avslutat trade (entry + exit) med realiserad PnL.
 *
 * Systemet skapar ClosedTrade via FIFO-lots när positioner stängs delvis eller helt.
 */
struct ClosedTrade {
    std::int64_t entryTs{0};     ///< Timestamp för entry
    std::int64_t exitTs{0};      ///< Timestamp för exit
    Signal entrySide{Signal::Hold}; ///< Buy=long entry, Sell=short entry
    int quantity{0};             ///< Matchad kvantitet
    double entryPrice{0.0};      ///< Entry-pris
    double exitPrice{0.0};       ///< Exit-pris
    double pnl{0.0};             ///< Realiserad vinst/förlust för trade-paret
};

/**
 * @brief Samlade resultatmått för en backtestkörning.
 *
 * Beräknas i PerformanceAnalyzer och används i rapport/HTML-graf.
 */
struct Metrics {
    double netProfit{0.0};       ///< Slutlig PnL = equity.back() - startingCash
    double maxDrawdown{0.0};     ///< Max drawdown (absolut) från equity-kurva
    int trades{0};               ///< Antal stängda trades (ClosedTrade)

    double winRate{0.0};         ///< Andel trades med pnl >= 0 (0..1)
    double profitFactor{0.0};    ///< grossWin / grossLoss
    double avgTrade{0.0};        ///< Medel-PnL per closed trade
    double avgWin{0.0};          ///< Medelvinst per vinnartrade
    double avgLoss{0.0};         ///< Medelförlust per förlorartrade (negativ)
    double sharpe{0.0};          ///< Enkel Sharpe approx (ej annualiserad)
};

} // namespace simtrade
