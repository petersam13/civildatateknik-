#pragma once
#include <vector>
#include <algorithm>
#include "Types.hpp"

namespace simtrade {

/** @brief Observer-interface för marknadsdata. */
class IMarketDataObserver {
public:
    virtual ~IMarketDataObserver() = default;
    virtual void onTick(const Tick& t) = 0;
};

/** @brief Observable (Observer-mönstret) för ticks. */
class TickObservable {
public:
    void subscribe(IMarketDataObserver* obs) { observers_.push_back(obs); }

    void unsubscribe(IMarketDataObserver* obs) {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), obs), observers_.end());
    }

protected:
    void notify(const Tick& t) {
        for (auto* obs : observers_) obs->onTick(t);
    }

private:
    std::vector<IMarketDataObserver*> observers_;
};

} // namespace simtrade
