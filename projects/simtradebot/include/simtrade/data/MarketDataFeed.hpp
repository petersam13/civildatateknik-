#pragma once
#include <vector>
#include "simtrade/core/Types.hpp"

namespace simtrade {

/**
 * @brief Observer-interface för marknadsdata.
 *
 * BacktestEngine implementerar denna och tar emot ticks från en feed.
 */
struct IMarketDataObserver {
    virtual ~IMarketDataObserver() = default;

    /**
     * @brief Kallas av MarketDataFeed när ett nytt Tick publiceras.
     * @param t Tick med timestamp och pris.
     */
    virtual void onTick(const Tick& t) = 0;
};

/**
 * @brief Bas-klass för marknadsdataflöden.
 *
 * En feed ansvarar för att läsa in data (t.ex. CSV) och publicera Tick till observers.
 */
class MarketDataFeed {
public:
    virtual ~MarketDataFeed() = default;

    /**
     * @brief Prenumerera på ticks från feed.
     * @param obs Pekare till observer (t.ex. BacktestEngine). Måste leva längre än feed.
     */
    void subscribe(IMarketDataObserver* obs) { observers_.push_back(obs); }

    /**
     * @brief Startar feed och publicerar ticks i ordning.
     *
     * Subklasser (t.ex. CsvMarketDataFeed) implementerar run().
     */
    virtual void run() = 0;

protected:
    /**
     * @brief Skicka ett Tick till alla observers.
     * @param t Tick att publicera.
     */
    void publish(const Tick& t) {
        for (auto* o : observers_) {
            if (o) o->onTick(t);
        }
    }

private:
    std::vector<IMarketDataObserver*> observers_;
};

} // namespace simtrade
