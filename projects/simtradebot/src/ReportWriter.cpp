#include "simtrade/ui/ReportWriter.hpp"
#include <fstream>
#include <algorithm>

namespace simtrade {

void ReportWriter::writeHtml(const std::string& path,
                             const std::vector<Tick>& ticks,
                             const std::vector<Trade>& trades,
                             const Metrics& m,
                             size_t maxPoints)
{
    if (ticks.empty()) return;

    const size_t N = ticks.size();
    const size_t start = (N > maxPoints) ? (N - maxPoints) : 0;

    std::ofstream f(path);
    f << "<!DOCTYPE html><html><head><meta charset='utf-8'>\n";
    f << "<title>SimTradeBot – Offline Backtest Chart</title>\n";
    f << "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>\n";
    f << "<style>body{background:#0b1220;color:#e5e7eb;font-family:sans-serif}</style>\n";
    f << "</head><body>\n";

    f << "<h2>SimTradeBot – Offline Backtest Chart</h2>\n";
    f << "<p><b>NetProfit:</b> " << m.netProfit
      << " | <b>MaxDD:</b> " << m.maxDrawdown
      << " | <b>Trades:</b> " << trades.size()
      << " (showing last ≤ " << maxPoints << ")</p>\n";

    f << "<canvas id='c' height='120'></canvas>\n";

    // ---- Price series ----
    f << "<script>\nconst labels=[";
    for (size_t i = start; i < N; ++i) {
        f << "'" << ticks[i].ts << "'" << (i + 1 < N ? "," : "");
    }
    f << "];\n";

    f << "const price=[";
    for (size_t i = start; i < N; ++i) {
        f << ticks[i].price << (i + 1 < N ? "," : "");
    }
    f << "];\n";

    // ---- BUY / SELL points (INDEX-BASERAT) ----
    f << "const buyX=[], buyY=[], sellX=[], sellY=[];\n";

    for (const auto& tr : trades) {
        // hitta tick-index som matchar trade.ts
        auto it = std::lower_bound(
            ticks.begin(), ticks.end(), tr.ts,
            [](const Tick& t, int64_t ts){ return t.ts < ts; });

        if (it == ticks.end()) continue;

        size_t idx = std::distance(ticks.begin(), it);
        if (idx < start) continue;   // ligger utanför zoom-fönstret

        size_t plotIdx = idx - start;

        if (tr.side == Signal::Buy) {
            f << "buyX.push(labels[" << plotIdx << "]);";
            f << "buyY.push(price[" << plotIdx << "]);\n";
        } else if (tr.side == Signal::Sell) {
            f << "sellX.push(labels[" << plotIdx << "]);";
            f << "sellY.push(price[" << plotIdx << "]);\n";
        }
    }

    // ---- Chart.js ----
    f << R"(
new Chart(document.getElementById('c'),{
 type:'line',
 data:{
  labels:labels,
  datasets:[
   {label:'Price',data:price,borderColor:'#7aa2ff',pointRadius:0},
   {label:'BUY',data:buyY,type:'scatter',
    backgroundColor:'#22c55e',pointRadius:5},
   {label:'SELL',data:sellY,type:'scatter',
    backgroundColor:'#ef4444',pointRadius:5}
  ]
 },
 options:{
  plugins:{legend:{labels:{color:'#e5e7eb'}}},
  scales:{
    x:{ticks:{color:'#9ca3af'}},
    y:{ticks:{color:'#9ca3af'}}
  }
 }
});
</script>
</body></html>
)";
}

} // namespace simtrade
