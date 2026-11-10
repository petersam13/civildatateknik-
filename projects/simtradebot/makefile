# ===== Compiler =====
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude -Iextern -Iextern/fmt/include -DFMT_HEADER_ONLY

# ===== App =====
APP = simtrade_app.exe

# ===== Sources (library part: NO main.cpp) =====
LIB_SRC = \
  src/CsvMarketDataFeed.cpp \
  src/StrategyFactory.cpp \
  src/RiskManager.cpp \
  src/ExecutionSimulator.cpp \
  src/PerformanceAnalyzer.cpp \
  src/BacktestEngine.cpp \
  src/Optimizer.cpp \
  src/ReportWriter.cpp \
  src/HtmlChartWriter.cpp \
  src/WalkForward.cpp



APP_SRC = src/main.cpp

# ===== Objects =====
LIB_OBJ = $(LIB_SRC:.cpp=.o)
APP_OBJ = $(APP_SRC:.cpp=.o)

# ===== Test executables (one per file; each test has its own main) =====
TEST_EXES = \
  tests/test_pnl.exe \
  tests/test_risk.exe \
  tests/test_optimizer_smoke.exe

all: $(APP)

$(APP): $(LIB_OBJ) $(APP_OBJ)
	$(CXX) -mconsole $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

tests/test_pnl.exe: $(LIB_OBJ) tests/test_pnl.o
	$(CXX) -mconsole $^ -o $@

tests/test_risk.exe: $(LIB_OBJ) tests/test_risk.o
	$(CXX) -mconsole $^ -o $@

tests/test_optimizer_smoke.exe: $(LIB_OBJ) tests/test_optimizer_smoke.o
	$(CXX) -mconsole $^ -o $@

tests: $(TEST_EXES)

run: $(APP)
	.\$(APP) tests\data\sample_prices.csv optimize

demo: $(APP)
	.\$(APP) tests\data\sample_prices.csv demo

check: tests
	.\tests\test_pnl.exe
	.\tests\test_risk.exe
	.\tests\test_optimizer_smoke.exe

clean:
	@cmd /C "del /Q /F $(subst /,\,$(LIB_OBJ) $(APP_OBJ) tests\test_pnl.o tests\test_risk.o tests\test_optimizer_smoke.o $(APP) $(TEST_EXES)) 2>nul & exit /b 0"
