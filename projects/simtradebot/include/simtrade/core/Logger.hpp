#pragma once
#include <iostream>
#include <string>

namespace simtrade {

/**
 * @brief Minimal logger för kursprojekt (ingen nätverk/logserver).
 */
class Logger {
public:
    static void info(const std::string& msg)  { std::cout << "[INFO] "  << msg << "\n"; }
    static void warn(const std::string& msg)  { std::cout << "[WARN] "  << msg << "\n"; }
    static void debug(const std::string& msg) { std::cout << "[DEBUG] " << msg << "\n"; }
};

} // namespace simtrade
