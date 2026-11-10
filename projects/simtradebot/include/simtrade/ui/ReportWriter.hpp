#pragma once
#include <string>
#include <vector>
#include "simtrade/core/Types.hpp"

namespace simtrade {

class ReportWriter {
public:
    void writeHtml(const std::string& path,
                   const std::vector<Tick>& ticks,
                   const std::vector<Trade>& trades,
                   const Metrics& m,
                   size_t maxPoints = 2000);
};

} // namespace simtrade
