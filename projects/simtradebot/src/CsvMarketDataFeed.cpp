#include "simtrade/data/CsvMarketDataFeed.hpp"
#include "simtrade/core/Logger.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace simtrade {

CsvMarketDataFeed::CsvMarketDataFeed(std::string csvPath)
    : path_(std::move(csvPath)) {}

void CsvMarketDataFeed::run() {
    Logger::info("Loading CSV: " + path_);

    std::ifstream f(path_);
    if (!f.is_open()) {
        Logger::info("Failed to open CSV: " + path_);
        return;
    }

    std::string line;
    bool firstLine = true;

    while (std::getline(f, line)) {
        if (line.empty()) continue;

        if (firstLine) {
            firstLine = false;
            bool hasAlpha = false;
            for (char c : line) {
                if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) { hasAlpha = true; break; }
            }
            if (hasAlpha) continue; // skip header
        }

        char sep = (line.find(',') != std::string::npos) ? ',' : ';';
        std::stringstream ss(line);

        std::string tsS, priceS;
        if (!std::getline(ss, tsS, sep)) continue;
        if (!std::getline(ss, priceS)) continue;

        try {
            Tick t{};
            t.ts = std::stoll(tsS);
            t.price = std::stod(priceS);
            publish(t);
        } catch (...) {
            continue;
        }
    }

    Logger::info("CSV load complete.");
}

} // namespace simtrade
