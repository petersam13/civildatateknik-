<?php
require 'includes/auth.php';
require 'config/db.php';

$user_id      = $_SESSION["user_id"];
$username     = $_SESSION["username"];
$displayName  = $_SESSION["name"] ?? $username;

$incomes = $pdo->prepare("SELECT * FROM incomes WHERE user_id=?");
$incomes->execute([$user_id]);
$incomes = $incomes->fetchAll();

$fixed = $pdo->prepare("SELECT * FROM fixed_expenses WHERE user_id=?");
$fixed->execute([$user_id]);
$fixed = $fixed->fetchAll();

$variable = $pdo->prepare("SELECT * FROM variable_expenses WHERE user_id=?");
$variable->execute([$user_id]);
$variable = $variable->fetchAll();

$totalIncome    = array_sum(array_column($incomes, "amount"));
$totalFixed     = array_sum(array_column($fixed, "amount"));
$totalVariable  = array_sum(array_column($variable, "amount"));
$totalExpenses  = $totalFixed + $totalVariable;
$remaining      = $totalIncome - $totalExpenses;
$savingPercent  = $totalIncome > 0 ? round(($remaining / $totalIncome) * 100) : 0;
$statusColor    = $remaining >= 0 ? "#C9A84C" : "#A83232";

include 'includes/header.php';
?>

<h1>Hej, <?= htmlspecialchars($displayName) ?></h1>

<div class="economy-summary"
     data-income="<?= $totalIncome ?>"
     data-expenses="<?= $totalExpenses ?>"
     data-color-pos="#C9A84C"
     data-color-neg="#A83232">
    <div class="economy-summary-label">Totalt kvar denna manad</div>
    <div class="big-number" id="js-remaining" style="color:<?= $statusColor ?>">
        <?= number_format($remaining, 2) ?> kr
    </div>
    <div class="economy-meta">
        <div class="economy-meta-item">
            <span class="meta-label">Inkomst</span>
            <span class="meta-val" id="js-income"><?= number_format($totalIncome, 2) ?> kr</span>
        </div>
        <div class="economy-meta-item">
            <span class="meta-label">Utgifter</span>
            <span class="meta-val" id="js-expenses"><?= number_format($totalExpenses, 2) ?> kr</span>
        </div>
        <div class="economy-meta-item">
            <span class="meta-label">Spargrad</span>
            <span class="meta-val" id="js-saving"><?= $savingPercent ?>%</span>
        </div>
    </div>
</div>

<div class="section-block">
    <h2>Inkomster</h2>
    <?php foreach($incomes as $item): ?>
    <div class="budget-row" data-amount="<?= $item['amount'] ?>" data-type="income">
        <span><?= htmlspecialchars($item['name'] ?? 'Inkomst') ?></span>
        <strong><?= number_format($item['amount'], 2) ?> kr</strong>
        <div class="row-actions">
            <a href="edit_item.php?type=income&id=<?= $item['id'] ?>">Redigera</a>
            <a href="#" class="delete-btn"
               data-href="delete_item.php?type=income&id=<?= $item['id'] ?>">Ta bort</a>
        </div>
    </div>
    <?php endforeach; ?>
    <a href="add_item.php?type=income" class="add-btn">Lagg till inkomst</a>
</div>

<div class="section-block">
    <h2>Fasta kostnader</h2>
    <?php foreach($fixed as $item): ?>
    <div class="budget-row" data-amount="<?= $item['amount'] ?>" data-type="fixed">
        <span><?= htmlspecialchars($item['name']) ?></span>
        <strong><?= number_format($item['amount'], 2) ?> kr</strong>
        <div class="row-actions">
            <a href="edit_item.php?type=fixed&id=<?= $item['id'] ?>">Redigera</a>
            <a href="#" class="delete-btn"
               data-href="delete_item.php?type=fixed&id=<?= $item['id'] ?>">Ta bort</a>
        </div>
    </div>
    <?php endforeach; ?>
    <a href="add_item.php?type=fixed" class="add-btn">Lagg till fast kostnad</a>
</div>

<div class="section-block">
    <h2>Rorliga kostnader</h2>
    <?php foreach($variable as $item): ?>
    <div class="budget-row" data-amount="<?= $item['amount'] ?>" data-type="variable">
        <span><?= htmlspecialchars($item['name']) ?></span>
        <strong><?= number_format($item['amount'], 2) ?> kr</strong>
        <div class="row-actions">
            <a href="edit_item.php?type=variable&id=<?= $item['id'] ?>">Redigera</a>
            <a href="#" class="delete-btn"
               data-href="delete_item.php?type=variable&id=<?= $item['id'] ?>">Ta bort</a>
        </div>
    </div>
    <?php endforeach; ?>
    <a href="add_item.php?type=variable" class="add-btn">Lagg till rorlig kostnad</a>
</div>

<script>
(function() {
    let totals = {
        income:   <?= $totalIncome ?>,
        expenses: <?= $totalExpenses ?>
    };

    function fmt(n) {
        return n.toLocaleString('sv-SE', { minimumFractionDigits: 2, maximumFractionDigits: 2 }) + ' kr';
    }

    function updateSummary() {
        const remaining = totals.income - totals.expenses;
        const savePct   = totals.income > 0 ? Math.round((remaining / totals.income) * 100) : 0;
        const color     = remaining >= 0 ? '#C9A84C' : '#A83232';

        const remEl = document.getElementById('js-remaining');
        remEl.style.color    = color;
        remEl.style.transform = 'scale(1.05)';
        remEl.textContent    = fmt(remaining);
        setTimeout(() => remEl.style.transform = 'scale(1)', 300);

        document.getElementById('js-income').textContent   = fmt(totals.income);
        document.getElementById('js-expenses').textContent = fmt(totals.expenses);
        document.getElementById('js-saving').textContent   = savePct + '%';
    }

    document.querySelectorAll('.delete-btn').forEach(btn => {
        btn.addEventListener('click', async function(e) {
            e.preventDefault();

            const row    = this.closest('.budget-row');
            const url    = this.dataset.href;
            const type   = row.dataset.type;
            const amount = parseFloat(row.dataset.amount);

            row.style.transition  = 'opacity .35s ease, transform .35s ease, max-height .4s ease, margin .4s ease, padding .4s ease';
            row.style.overflow    = 'hidden';
            row.style.opacity     = '0';
            row.style.transform   = 'translateX(30px)';
            row.style.maxHeight   = row.offsetHeight + 'px';

            setTimeout(() => {
                row.style.maxHeight = '0';
                row.style.marginBottom = '0';
                row.style.paddingTop   = '0';
                row.style.paddingBottom = '0';
            }, 300);

            try {
                await fetch(url, {
                    headers: { 'X-Requested-With': 'XMLHttpRequest' }
                });
            } catch(_) {}

            if (type === 'income') {
                totals.income -= amount;
            } else {
                totals.expenses -= amount;
            }
            updateSummary();
            setTimeout(() => row.remove(), 700);
        });
    });
})();
</script>

<?php include 'includes/footer.php'; ?>
