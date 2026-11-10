#pragma once
#include <string>
#include "simtrade/data/MarketDataFeed.hpp"

namespace simtrade {

/**
 * @brief CSV-baserad marknadsdatafeed.
 *
 * Förväntar sig minst två kolumner per rad: ts,price (komma eller semikolon).
 * Header-rad (med bokstäver) ignoreras.
 */
class CsvMarketDataFeed final : public MarketDataFeed {
public:
    /**
     * @brief Skapar en feed som läser från given csv-fil.
     * @param csvPath Sökväg till csv med ts,price.
     */
    explicit CsvMarketDataFeed(std::string csvPath);

    ~CsvMarketDataFeed() override = default;

    /**
     * @brief Läser CSV och publicerar ticks via publish().
     */
    void run() override;

private:
    std::string path_; ///< Sökväg till CSV-fil.
};

} // namespace simtrade
