cmake_minimum_required(VERSION 3.16)
project(SimTradeBot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(simtrade STATIC
    src/CsvMarketDataFeed.cpp
    src/StrategyFactory.cpp
    src/RiskManager.cpp
    src/ExecutionSimulator.cpp
    src/PerformanceAnalyzer.cpp
    src/BacktestEngine.cpp
    src/Optimizer.cpp
    src/ReportWriter.cpp
    src/HtmlChartWriter.cpp
    src/WalkForward.cpp
)
target_include_directories(simtrade PUBLIC include extern extern/fmt/include)
target_compile_definitions(simtrade PUBLIC FMT_HEADER_ONLY)
if(MSVC)
    target_compile_options(simtrade PRIVATE /W4)
else()
    target_compile_options(simtrade PRIVATE -Wall -Wextra)
endif()

add_executable(simtrade_app src/main.cpp)
target_link_libraries(simtrade_app PRIVATE simtrade)

enable_testing()
foreach(t test_pnl test_risk test_optimizer_smoke)
    add_executable(${t} tests/${t}.cpp)
    target_link_libraries(${t} PRIVATE simtrade)
    add_test(NAME ${t} COMMAND ${t} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
