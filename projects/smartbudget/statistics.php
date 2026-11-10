<?php
require 'includes/auth.php';
require 'config/db.php';

$user_id = $_SESSION["user_id"];

$stmt = $pdo->prepare("SELECT SUM(amount) FROM incomes WHERE user_id=?");
$stmt->execute([$user_id]);
$yourIncome = (float)($stmt->fetchColumn() ?? 0);

$stmt = $pdo->prepare("SELECT SUM(amount) FROM fixed_expenses WHERE user_id=?");
$stmt->execute([$user_id]);
$yourFixed = (float)($stmt->fetchColumn() ?? 0);

$stmt = $pdo->prepare("SELECT SUM(amount) FROM variable_expenses WHERE user_id=?");
$stmt->execute([$user_id]);
$yourVariable = (float)($stmt->fetchColumn() ?? 0);

$yourExpenses  = $yourFixed + $yourVariable;
$yourRemaining = $yourIncome - $yourExpenses;
$yourSavingPct = $yourIncome > 0 ? round(($yourRemaining / $yourIncome) * 100) : 0;

$stmt = $pdo->query("
    SELECT AVG(inc.t) AS avg_income, AVG(fix.t) AS avg_fixed, AVG(var.t) AS avg_variable
    FROM   (SELECT user_id, SUM(amount) AS t FROM incomes          GROUP BY user_id) inc
    LEFT JOIN (SELECT user_id, SUM(amount) AS t FROM fixed_expenses    GROUP BY user_id) fix USING(user_id)
    LEFT JOIN (SELECT user_id, SUM(amount) AS t FROM variable_expenses GROUP BY user_id) var USING(user_id)
");
$avg         = $stmt->fetch();
$avgIncome   = round((float)($avg['avg_income']   ?? 0));
$avgFixed    = round((float)($avg['avg_fixed']    ?? 0));
$avgVariable = round((float)($avg['avg_variable'] ?? 0));
$userCount   = (int)$pdo->query("SELECT COUNT(*) FROM users")->fetchColumn();

function riksbankGet(string $seriesId, string $from, string $to): ?array {
    $url = "https://api.riksbank.se/swea/v1/Observations/{$seriesId}/{$from}/{$to}";
    $ch  = curl_init($url);
    curl_setopt_array($ch, [
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_TIMEOUT        => 6,
        CURLOPT_HTTPHEADER     => ['Accept: application/json'],
        CURLOPT_SSL_VERIFYPEER => true,
    ]);
    $body = curl_exec($ch);
    $code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    curl_close($ch);
    if ($code !== 200 || !$body) return null;
    $json = json_decode($body, true);
    $obs  = $json['observations'] ?? $json;
    if (!is_array($obs)) return null;
    return array_values(array_filter($obs, fn($o) => isset($o['value']) && $o['value'] !== null && $o['value'] !== ''));
}

$rbFrom = date('Y-m-d', strtotime('-13 months'));
$rbTo   = date('Y-m-d');

$cpifObs = riksbankGet('SECPIFASTPCT12', $rbFrom, $rbTo) ?? [];
$repoObs = riksbankGet('SETBREPOEFF',    $rbFrom, $rbTo) ?? [];

$cpifLast = !empty($cpifObs) ? end($cpifObs) : null;
$repoLast = !empty($repoObs) ? end($repoObs) : null;

$rbOk        = $cpifLast !== null || $repoLast !== null;
$cpifVal     = $cpifLast ? round((float)$cpifLast['value'], 1) : null;
$cpifDate    = $cpifLast ? $cpifLast['date'] : null;
$repoVal     = $repoLast ? round((float)$repoLast['value'], 2) : null;
$repoDate    = $repoLast ? $repoLast['date'] : null;

$rbTip = '';
if ($cpifVal !== null && $repoVal !== null) {
    if ($cpifVal > 4)
        $rbTip = "Inflationen ar hog ({$cpifVal}%). Sparpengar tapper vardet — kolla hograntekonton eller indexfonder.";
    elseif ($cpifVal > 2)
        $rbTip = "Inflation {$cpifVal}% overstiger Riksbankens 2%-mal. Overvaeg att placera sparpengar i ranta- eller indexfonder.";
    else
        $rbTip = "Inflationen ar lag ({$cpifVal}%). Bra tillfalle att bygga nododfond och spara langsiktigt.";
    if ($repoVal >= 3.5)
        $rbTip .= " Hog styranta ({$repoVal}%) — jamfor sparkontoranta hos olika banker, det skiljer mycket.";
    elseif ($repoVal < 1.0)
        $rbTip .= " Lag styranta ({$repoVal}%) — sparkontoranta ar minimal, overvaeg fonder for langsiktigt sparande.";
}

$cpifLabels = array_map(fn($o) => substr($o['date'], 0, 7), array_slice($cpifObs, -12));
$cpifValues = array_map(fn($o) => round((float)$o['value'], 2),  array_slice($cpifObs, -12));

$needsPct = $yourIncome > 0 ? round(($yourFixed    / $yourIncome) * 100) : 0;
$wantsPct = $yourIncome > 0 ? round(($yourVariable / $yourIncome) * 100) : 0;

$tips = [];
if ($yourIncome === 0.0) {
    $tips[] = ['type'=>'warn', 'text'=>'Lagg till inkomster pa dashboarden for att fa rekommendationer.'];
} else {
    $tips[] = $needsPct > 50
        ? ['type'=>'warn',  'text'=>"Fasta kostnader ar {$needsPct}% av inkomsten (max 50%). Forsok pruta pa hyra, el eller abonnemang."]
        : ['type'=>'ok',    'text'=>"Bra! Fasta kostnader ar {$needsPct}% — under rekommenderade 50%."];
    $tips[] = $wantsPct > 30
        ? ['type'=>'warn',  'text'=>"Rorliga utgifter ar {$wantsPct}% (max 30%). Tanka over shopping och noje."]
        : ['type'=>'ok',    'text'=>"Rorliga utgifter ar {$wantsPct}% — under rekommenderade 30%."];
    $tips[] = $yourSavingPct < 10
        ? ['type'=>'warn',  'text'=>"Spargrad {$yourSavingPct}%. Mal: minst 20%. Tips: flytta 10% av lonen automatiskt pa lonedag."]
        : ($yourSavingPct < 20
            ? ['type'=>'ok',   'text'=>"Spargrad {$yourSavingPct}% — nastan pa malet 20%. Fortsatt sa!"]
            : ['type'=>'great','text'=>"Utmarkt spargrad pa {$yourSavingPct}%! Du overstiger 20%-malet."]);
}

include 'includes/header.php';
?>

<h1>Statistik</h1>

<!-- API status strip -->
<div class="api-strip">
    <div class="api-badge"><span class="api-dot" id="dot-wb"></span><span>World Bank API</span></div>
    <div class="api-badge"><span class="api-dot <?= $rbOk ? 'dot-ok' : 'dot-err' ?>"></span><span>Riksbanken API</span></div>
    <div class="api-badge"><span class="api-dot api-dot-int"></span><span>SmartBudget intern data</span></div>
</div>

<div class="stats-grid">

    <!-- 1. Du vs Sverige (World Bank) -->
    <div class="stats-card">
        <h2>Du vs Sverige</h2>
        <p class="api-src">Kalla: World Bank Open Data API</p>
        <div id="wb-load" class="stats-load">Hamtar data...</div>
        <div id="wb-res"  style="display:none">
            <div id="wb-text" class="stats-text"></div>
            <canvas id="chartSweden" height="200"></canvas>
        </div>
    </div>

    <!-- 2. Riksbanken: Inflation & Styrränta (server-side fetch) -->
    <div class="stats-card">
        <h2>Inflation &amp; Styrränta</h2>
        <p class="api-src">Kalla: Sveriges Riksbank Open API</p>
        <?php if (!$rbOk): ?>
        <p class="stats-load" style="color:#A83232">Riksbank-data ej tillganglig just nu.</p>
        <?php else: ?>
        <div class="rb-boxes">
            <div class="rb-box">
                <div class="rb-label">Inflation (CPIF)</div>
                <div class="rb-val"><?= $cpifVal !== null ? $cpifVal . ' %' : '—' ?></div>
                <div class="rb-date"><?= $cpifDate ?? '' ?></div>
            </div>
            <div class="rb-box">
                <div class="rb-label">Styrränta</div>
                <div class="rb-val"><?= $repoVal !== null ? $repoVal . ' %' : '—' ?></div>
                <div class="rb-date"><?= $repoDate ?? '' ?></div>
            </div>
        </div>
        <?php if ($rbTip): ?>
        <div class="rb-tip">
            <span class="tip-icon" style="color:#4a6a8a;flex-shrink:0">i</span>
            <?= htmlspecialchars($rbTip) ?>
        </div>
        <?php endif; ?>
        <?php if (!empty($cpifLabels)): ?>
        <canvas id="chartInflation" height="150" style="margin-top:1.5rem"></canvas>
        <?php endif; ?>
        <?php endif; ?>
    </div>

    <!-- 3. Du vs andra SmartBudget-anvandare -->
    <div class="stats-card">
        <h2>Du vs andra anvandare</h2>
        <p class="api-src">Baserat pa <?= $userCount ?> anvandare i SmartBudget</p>
        <canvas id="chartUsers" height="200"></canvas>
        <div class="stats-legend">
            <span class="leg-dot" style="background:#C9A84C"></span>Du &nbsp;
            <span class="leg-dot" style="background:#1a1a1a;border:1px solid #444"></span>Snitt
        </div>
    </div>

    <!-- 4. Din budgetfordelning -->
    <div class="stats-card">
        <h2>Din budgetfordelning</h2>
        <p class="api-src">50 / 30 / 20-regeln</p>
        <?php if($yourIncome > 0): ?>
        <div style="max-width:220px;margin:0 auto">
            <canvas id="chartBreak" height="220"></canvas>
        </div>
        <div class="stats-legend" style="margin-top:1rem">
            <span class="leg-dot" style="background:#C9A84C"></span>Fasta (<?= $needsPct ?>%)&nbsp;
            <span class="leg-dot" style="background:#7a5c1e"></span>Rorliga (<?= $wantsPct ?>%)&nbsp;
            <span class="leg-dot" style="background:#1a1a1a;border:1px solid #333"></span>Sparande (<?= max(0,$yourSavingPct) ?>%)
        </div>
        <?php else: ?>
        <p class="stats-empty">Lagg till inkomster pa dashboarden.</p>
        <?php endif; ?>
    </div>

    <!-- 5. Rekommendationer (full width) -->
    <div class="stats-card stats-wide">
        <h2>Smarta rekommendationer</h2>
        <div class="rule-bar">
            <div class="rule-seg" style="width:50%"><span>Fasta</span><strong>Max 50%</strong></div>
            <div class="rule-seg" style="width:30%;opacity:.75"><span>Rorliga</span><strong>Max 30%</strong></div>
            <div class="rule-seg" style="width:20%;opacity:.5"><span>Spara</span><strong>Min 20%</strong></div>
        </div>
        <div class="tips-list">
        <?php foreach($tips as $t): ?>
            <div class="tip tip-<?= $t['type'] ?>">
                <span class="tip-icon"><?= $t['type']==='ok'?'✓':($t['type']==='great'?'★':($t['type']==='warn'?'!':'i')) ?></span>
                <span class="tip-text"><?= htmlspecialchars($t['text']) ?></span>
            </div>
        <?php endforeach; ?>
        </div>
    </div>

</div>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script>
const GOLD='#C9A84C', DARK='#1a1a1a', MUTED='#6B6B5E', GRID='rgba(255,255,255,.04)';
const SC = { y:{ticks:{color:MUTED,font:{size:11}},grid:{color:GRID},border:{color:'transparent'}}, x:{ticks:{color:MUTED,font:{size:11}},grid:{display:false},border:{color:'transparent'}} };

/* ═══ API 1: World Bank ═══ */
(async()=>{
    const dot=document.getElementById('dot-wb');
    try{
        const r=await fetch('https://api.worldbank.org/v2/country/SE/indicator/NY.GNP.PCAP.PP.CD?format=json&per_page=5');
        const d=await r.json();
        const e=d[1]?.find(x=>x.value!==null);
        if(!e)throw new Error();
        const monthly=Math.round(e.value/12);
        const yours=<?= (int)$yourIncome ?>;
        const diff=yours>0?Math.round(((yours-monthly)/monthly)*100):null;
        let msg=`Sveriges genomsnittliga manadsinkomst <strong style="color:${GOLD}">(${e.date})</strong>: <strong>${monthly.toLocaleString('sv-SE')} kr</strong><br>`;
        if(diff===null) msg+='<span style="color:'+MUTED+'">Lagg till inkomster pa dashboarden for att jamfora.</span>';
        else if(diff>0) msg+=`Din inkomst ar <strong style="color:${GOLD}">+${diff}% over</strong> det svenska genomsnittet.`;
        else if(diff<0) msg+=`Din inkomst ar <strong style="color:#A83232">${diff}% under</strong> det svenska genomsnittet.`;
        else msg+='Din inkomst matchar exakt det svenska genomsnittet.';
        document.getElementById('wb-load').style.display='none';
        document.getElementById('wb-res').style.display='block';
        document.getElementById('wb-text').innerHTML=msg;
        dot.className='api-dot dot-ok';
        new Chart(document.getElementById('chartSweden'),{type:'bar',data:{labels:['Du','Sverige (snitt)'],datasets:[{data:[yours,monthly],backgroundColor:[GOLD,DARK],borderColor:[GOLD,'#444'],borderWidth:1,borderRadius:0}]},options:{plugins:{legend:{display:false}},scales:SC}});
    }catch(e){
        document.getElementById('wb-load').textContent='Kunde inte hamta World Bank-data.';
        dot.className='api-dot dot-err';
    }
})();

/* ═══ API 2: Riksbanken (data fran PHP/curl) ═══ */
<?php if (!empty($cpifLabels)): ?>
new Chart(document.getElementById('chartInflation'),{type:'line',data:{labels:<?= json_encode($cpifLabels) ?>,datasets:[{label:'CPIF (%)',data:<?= json_encode($cpifValues) ?>,borderColor:GOLD,backgroundColor:'rgba(201,168,76,.07)',borderWidth:1.5,pointRadius:2,pointBackgroundColor:GOLD,fill:true,tension:.35}]},options:{plugins:{legend:{display:false},tooltip:{callbacks:{label:c=>` ${c.parsed.y.toFixed(1)} %`}}},scales:SC}});
<?php endif; ?>

/* ═══ Du vs SmartBudget-anvandare ═══ */
new Chart(document.getElementById('chartUsers'),{type:'bar',data:{labels:['Inkomst','Fasta','Rorliga'],datasets:[{label:'Du',data:[<?= (int)$yourIncome ?>,<?= (int)$yourFixed ?>,<?= (int)$yourVariable ?>],backgroundColor:GOLD,borderColor:GOLD,borderWidth:1},{label:'Snitt',data:[<?= $avgIncome ?>,<?= $avgFixed ?>,<?= $avgVariable ?>],backgroundColor:DARK,borderColor:'#444',borderWidth:1}]},options:{plugins:{legend:{display:false}},scales:SC}});

/* ═══ Budgetfordelning ═══ */
<?php if($yourIncome>0): ?>
new Chart(document.getElementById('chartBreak'),{type:'doughnut',data:{labels:['Fasta','Rorliga','Sparande'],datasets:[{data:[<?= max(0,(int)$yourFixed) ?>,<?= max(0,(int)$yourVariable) ?>,<?= max(0,(int)$yourRemaining) ?>],backgroundColor:['#C9A84C','#7a5c1e',DARK],borderColor:['#C9A84C','#7a5c1e','#333'],borderWidth:1}]},options:{cutout:'68%',plugins:{legend:{display:false}}}});
<?php endif; ?>
</script>

<style>
.api-strip{display:flex;gap:1.5rem;margin-bottom:2rem;flex-wrap:wrap}
.api-badge{display:flex;align-items:center;gap:.5rem;font-size:.63rem;letter-spacing:.1em;color:var(--muted)}
.api-dot{width:7px;height:7px;border-radius:50%;background:var(--muted);flex-shrink:0;transition:background .5s}
.api-dot.dot-ok{background:#4a7c4e;box-shadow:0 0 6px #4a7c4e}
.api-dot.dot-err{background:var(--red)}
.api-dot-int{width:7px;height:7px;border-radius:50%;background:var(--gold);box-shadow:0 0 6px rgba(201,168,76,.5);flex-shrink:0}
.api-src{font-size:.55rem;letter-spacing:.2em;text-transform:uppercase;color:var(--muted);margin-bottom:1.2rem}
.stats-grid{display:grid;grid-template-columns:1fr 1fr;gap:1.5rem;max-width:1000px}
.stats-wide{grid-column:1/-1}
.stats-load{font-size:.8rem;color:var(--muted);letter-spacing:.06em;padding:1rem 0}
.stats-empty{font-size:.82rem;color:var(--muted);padding:1rem 0}
.stats-text{font-size:.85rem;line-height:2;color:var(--muted);margin-bottom:1.5rem}
.stats-legend{margin-top:1rem;font-size:.72rem;letter-spacing:.08em;color:var(--muted)}
.leg-dot{display:inline-block;width:8px;height:8px;border-radius:50%;margin-right:.3rem;vertical-align:middle}
.rb-boxes{display:grid;grid-template-columns:1fr 1fr;gap:1rem;margin-bottom:1.2rem}
.rb-box{background:var(--s3);border:1px solid var(--gbor);padding:1.2rem 1.4rem}
.rb-label{font-size:.55rem;letter-spacing:.22em;text-transform:uppercase;color:var(--muted);margin-bottom:.4rem}
.rb-val{font-family:var(--serif);font-size:2rem;font-weight:500;color:var(--gold);line-height:1}
.rb-date{font-size:.63rem;color:var(--muted);margin-top:.2rem}
.rb-tip{font-size:.81rem;line-height:1.8;color:var(--muted);padding:.9rem 1.1rem;background:rgba(201,168,76,.04);border-left:2px solid var(--gbor);display:flex;gap:.75rem;align-items:flex-start}
.rule-bar{display:flex;height:30px;border:1px solid var(--gbor);overflow:hidden;margin-bottom:2rem}
.rule-seg{display:flex;flex-direction:column;justify-content:center;align-items:center;background:rgba(201,168,76,.07);border-right:1px solid var(--gbor)}
.rule-seg:last-child{border:none}
.rule-seg span{font-size:.5rem;letter-spacing:.1em;text-transform:uppercase;color:var(--muted)}
.rule-seg strong{font-size:.63rem;color:var(--gold)}
.tips-list{display:flex;flex-direction:column;gap:.7rem}
.tip{display:flex;align-items:flex-start;gap:1rem;padding:.9rem 1.2rem;border:1px solid rgba(255,255,255,.04);border-left:2px solid transparent}
.tip-ok{border-left-color:#4a7c4e;background:rgba(74,124,78,.05)}
.tip-great{border-left-color:var(--gold);background:rgba(201,168,76,.06)}
.tip-warn{border-left-color:var(--red);background:rgba(168,50,50,.06)}
.tip-icon{width:20px;height:20px;flex-shrink:0;border-radius:50%;border:1px solid currentColor;display:flex;align-items:center;justify-content:center;font-size:.7rem;font-weight:700}
.tip-ok .tip-icon{color:#4a7c4e}.tip-great .tip-icon{color:var(--gold)}.tip-warn .tip-icon{color:var(--red)}
.tip-text{font-size:.83rem;line-height:1.7;color:rgba(245,243,238,.75);letter-spacing:.03em}
@media(max-width:760px){.stats-grid{grid-template-columns:1fr}.stats-wide{grid-column:1}.rb-boxes{grid-template-columns:1fr}}
</style>

<?php include 'includes/footer.php'; ?>
