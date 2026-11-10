#include "simtrade/ui/HtmlChartWriter.hpp"
#include <fstream>
#include <iomanip>

namespace simtrade {

void HtmlChartWriter::write(
    const std::string& path,
    const std::vector<Tick>& ticks,
    const std::vector<double>& equity,
    const std::vector<Trade>& trades,
    const std::vector<ClosedTrade>& closedTrades,
    const Metrics& m)
{
    std::ofstream out(path);
    if (!out.is_open()) return;

    out << "<!doctype html><html><head><meta charset='utf-8'/>"
           "<title>SimTradeBot – Backtest</title>"
           "<style>"
           "body{margin:0;font-family:system-ui;background:#0b0f17;color:#e6edf3}"
           ".wrap{max-width:1200px;margin:20px auto;padding:0 14px}"
           ".card{background:#121826;border-radius:14px;padding:14px}"
           "canvas{width:100%;background:#0f1420;border-radius:12px}"
           "#c{height:560px} #h{height:160px;margin-top:10px}"
           ".controls{display:flex;gap:14px;align-items:center;margin:8px 0;font-size:13px;flex-wrap:wrap}"
           ".dot{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:6px}"
           ".price{background:#7aa2ff}.buy{background:#2bd576}.sell{background:#ff4d4d}.eq{background:#f5c542}"
           ".tooltip{position:fixed;pointer-events:none;background:#121826;border:1px solid #333;padding:8px;border-radius:8px;font-size:12px;display:none}"
           "</style></head><body>";

           
    out << "<div class='wrap'><div class='card'>";
    out << "<h3>SimTradeBot – Backtest</h3>";
    out << "<div>NetProfit: <b>" << std::fixed << std::setprecision(2) << m.netProfit
        << "</b> | MaxDD: <b>" << m.maxDrawdown
        << "</b> | Closed trades: <b>" << m.trades << "</b></div>";

    out << "<canvas id='c' width='1200' height='560'></canvas>";
    out << "<canvas id='h' width='1200' height='160'></canvas>";

    out << "<div class='controls'>"
           "<label><input type='checkbox' id='showEq' checked> Equity</label>"
           "<label><input type='checkbox' id='showCT' checked> Closed trades</label>"
           "<label>Zoom <input type='range' id='zoom' min='50' max='3000' step='10' value='600'></label>"
           "</div>";

    out << "<div class='controls'>"
           "<span><span class='dot price'></span>Price</span>"
           "<span><span class='dot buy'></span>BUY</span>"
           "<span><span class='dot sell'></span>SELL</span>"
           "<span><span class='dot eq'></span>Equity</span>"
           "<span>Histogram: Closed-trade PnL</span>"
           "</div>";

    out << "</div></div><div id='tt' class='tooltip'></div>";

    // --- Data arrays ---
    out << "<script>\n";
    out << "const TS=[";
    for (size_t i=0;i<ticks.size();++i){ if(i) out<<","; out<<ticks[i].ts; }
    out << "];\n";

    out << "const P=[";
    for (size_t i=0;i<ticks.size();++i){ if(i) out<<","; out<<ticks[i].price; }
    out << "];\n";

    out << "const EQ=[";
    for (size_t i=0;i<equity.size();++i){ if(i) out<<","; out<<equity[i]; }
    out << "];\n";

    out << "const TR=[";
    for (size_t i=0;i<trades.size();++i){
        if(i) out<<",";
        out << "{ts:" << trades[i].ts
            << ",side:'" << (trades[i].side==Signal::Buy?"BUY":"SELL") << "'"
            << ",price:" << trades[i].price
            << ",qty:" << trades[i].quantity << "}";
    }
    out << "];\n";

    out << "const CT=[";
    for (size_t i=0;i<closedTrades.size();++i){
        if(i) out<<",";
        out << "{eTs:" << closedTrades[i].entryTs
            << ",xTs:" << closedTrades[i].exitTs
            << ",side:'" << (closedTrades[i].entrySide==Signal::Buy?"LONG":"SHORT") << "'"
            << ",ep:" << closedTrades[i].entryPrice
            << ",xp:" << closedTrades[i].exitPrice
            << ",qty:" << closedTrades[i].quantity
            << ",pnl:" << closedTrades[i].pnl << "}";
    }
    out << "];\n";

    // --- JS drawing ---
    out << R"JS(
const c=document.getElementById('c'), ctx=c.getContext('2d');
const h=document.getElementById('h'), hctx=h.getContext('2d');
const tt=document.getElementById('tt');
const zoom=document.getElementById('zoom');
const showEq=document.getElementById('showEq');
const showCT=document.getElementById('showCT');

function minmax(a,i0,i1){
  let mn=a[i0],mx=a[i0];
  for(let i=i0;i<=i1;i++){ mn=Math.min(mn,a[i]); mx=Math.max(mx,a[i]); }
  if(mx-mn<1e-9) mx=mn+1;
  return [mn,mx];
}
function idxByTs(ts,a,b){
  let bi=a,bd=Math.abs(TS[a]-ts);
  for(let i=a+1;i<=b;i++){
    const d=Math.abs(TS[i]-ts);
    if(d<bd){bd=d;bi=i;}
  }
  return bi;
}

function drawMain(){
  const n=P.length;
  const cnt=Math.min(n,Math.max(50,parseInt(zoom.value)));
  const b=n-1,a=Math.max(0,n-cnt);

  const pad={l:70,r:50,t:20,b:40};
  const W=c.width,H=c.height;

  const [pMin,pMax]=minmax(P,a,b);
  const [eMin,eMax]=EQ.length?minmax(EQ,a,b):[0,1];

  const x=i=>pad.l+(i-a)/(b-a)*(W-pad.l-pad.r);
  const yP=p=>pad.t+(1-(p-pMin)/(pMax-pMin))*(H-pad.t-pad.b);
  const yE=e=>pad.t+(1-(e-eMin)/(eMax-eMin))*(H-pad.t-pad.b);

  ctx.clearRect(0,0,W,H);

  ctx.strokeStyle='rgba(255,255,255,0.08)';
  for(let i=0;i<6;i++){
    const y=pad.t+i/5*(H-pad.t-pad.b);
    ctx.beginPath(); ctx.moveTo(pad.l,y); ctx.lineTo(W-pad.r,y); ctx.stroke();
  }

  // price
  ctx.strokeStyle='#7aa2ff'; ctx.lineWidth=2;
  ctx.beginPath(); ctx.moveTo(x(a),yP(P[a]));
  for(let i=a+1;i<=b;i++) ctx.lineTo(x(i),yP(P[i]));
  ctx.stroke();

  // equity
  if(showEq.checked && EQ.length){
    ctx.strokeStyle='#f5c542'; ctx.lineWidth=2;
    ctx.beginPath(); ctx.moveTo(x(a),yE(EQ[a]));
    for(let i=a+1;i<=b;i++) ctx.lineTo(x(i),yE(EQ[i]));
    ctx.stroke();
  }

  // trade markers
  for(const tr of TR){
    if(tr.ts<TS[a]||tr.ts>TS[b]) continue;
    const i=idxByTs(tr.ts,a,b);
    const px=x(i), py=yP(P[i])+(tr.side==='BUY'?-10:10);
    ctx.fillStyle=tr.side==='BUY'?'#2bd576':'#ff4d4d';
    ctx.beginPath(); ctx.arc(px,py,5,0,Math.PI*2); ctx.fill();
  }

  // closed trade lines
  if(showCT.checked){
    for(const ct of CT){
      if(ct.xTs<TS[a]||ct.eTs>TS[b]) continue;
      const i0=idxByTs(ct.eTs,a,b);
      const i1=idxByTs(ct.xTs,a,b);
      ctx.strokeStyle=(ct.pnl>=0)?'#2bd576':'#ff4d4d';
      ctx.lineWidth=2;
      ctx.beginPath();
      ctx.moveTo(x(i0),yP(P[i0]));
      ctx.lineTo(x(i1),yP(P[i1]));
      ctx.stroke();
    }
  }
}

function drawHistogram(){
  const W=h.width,H=h.height;
  hctx.clearRect(0,0,W,H);

  if(!CT.length){
    hctx.fillStyle='rgba(255,255,255,0.6)';
    hctx.fillText('No closed trades', 20, 40);
    return;
  }

  // build pnl array
  const pnls=CT.map(x=>x.pnl);
  let mn=pnls[0], mx=pnls[0];
  for(const v of pnls){ mn=Math.min(mn,v); mx=Math.max(mx,v); }
  if(mx-mn<1e-9){ mx=mn+1; }

  // bins
  const bins=40;
  const counts=new Array(bins).fill(0);
  for(const v of pnls){
    const t=(v-mn)/(mx-mn);
    let bi=Math.floor(t*(bins-1));
    bi=Math.max(0,Math.min(bins-1,bi));
    counts[bi]++;
  }
  let cmax=1;
  for(const c of counts) cmax=Math.max(cmax,c);

  const pad={l:70,r:20,t:15,b:25};
  const bw=(W-pad.l-pad.r)/bins;
  const zeroY=pad.t+(H-pad.t-pad.b)*0.5;

  // axis
  hctx.strokeStyle='rgba(255,255,255,0.15)';
  hctx.beginPath();
  hctx.moveTo(pad.l, zeroY);
  hctx.lineTo(W-pad.r, zeroY);
  hctx.stroke();

  // bars
  for(let i=0;i<bins;i++){
    const x=pad.l+i*bw;
    const hgt=(counts[i]/cmax)*(H-pad.t-pad.b);
    const y=(H-pad.b)-hgt;

    // color by pnl sign of bin center
    const center = mn + (i+0.5)/bins*(mx-mn);
    hctx.fillStyle = (center>=0)?'rgba(43,213,118,0.75)':'rgba(255,77,77,0.75)';
    hctx.fillRect(x+1, y, Math.max(1,bw-2), hgt);
  }

  // labels
  hctx.fillStyle='rgba(255,255,255,0.7)';
  hctx.fillText('PnL Histogram', 10, 14);
  hctx.fillText('min: '+mn.toFixed(2), 10, H-8);
  hctx.fillText('max: '+mx.toFixed(2), 160, H-8);
}

function drawAll(){ drawMain(); drawHistogram(); }

zoom.oninput=drawAll;
showEq.onchange=drawAll;
showCT.onchange=drawAll;
drawAll();
)JS";

    out << "</script></body></html>";
}

} // namespace simtrade
