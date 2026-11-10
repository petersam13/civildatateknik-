<?php
require 'includes/auth.php';
require 'config/db.php';

if($_SESSION["role"] !== "admin"){
    header("Location: dashboard.php");
    exit;
}

$users = $pdo->query("
    SELECT
        u.*,
        (SELECT COUNT(*) FROM incomes          WHERE user_id=u.id) +
        (SELECT COUNT(*) FROM fixed_expenses   WHERE user_id=u.id) +
        (SELECT COUNT(*) FROM variable_expenses WHERE user_id=u.id) AS activity_count,
        (SELECT COALESCE(SUM(amount),0) FROM incomes WHERE user_id=u.id) AS total_income
    FROM users u
    ORDER BY u.created_at DESC
")->fetchAll();

$totalUsers     = count($users);
$activeUsers    = count(array_filter($users, fn($u) => !$u['suspended']));
$suspendedUsers = $totalUsers - $activeUsers;

include 'includes/header.php';
?>

<h1>Admin Panel</h1>

<!-- Summary strip -->
<div class="adm-summary">
    <div class="adm-stat">
        <div class="adm-stat-val"><?= $totalUsers ?></div>
        <div class="adm-stat-label">Totalt anvandare</div>
    </div>
    <div class="adm-stat">
        <div class="adm-stat-val" style="color:var(--gold)"><?= $activeUsers ?></div>
        <div class="adm-stat-label">Aktiva</div>
    </div>
    <div class="adm-stat">
        <div class="adm-stat-val" style="color:var(--red)"><?= $suspendedUsers ?></div>
        <div class="adm-stat-label">Suspenderade</div>
    </div>
</div>

<!-- User table -->
<div class="adm-table-wrap section-block">
    <table class="adm-table">
        <thead>
            <tr>
                <th>Anvandare</th>
                <th>Roll</th>
                <th>IP-adress</th>
                <th>Aktivitet</th>
                <th>Senast inloggad</th>
                <th>Status</th>
                <th>Atgarder</th>
            </tr>
        </thead>
        <tbody>
        <?php foreach($users as $u): ?>
        <tr class="adm-row <?= $u['suspended'] ? 'adm-row-suspended' : '' ?>" id="row-<?= $u['id'] ?>">
            <td>
                <div class="adm-user-name"><?= htmlspecialchars($u['name'] ?? $u['username']) ?></div>
                <div class="adm-user-sub"><?= htmlspecialchars($u['username']) ?></div>
                <?php if($u['email']): ?>
                <div class="adm-user-sub"><?= htmlspecialchars($u['email']) ?></div>
                <?php endif; ?>
            </td>
            <td>
                <span class="adm-badge <?= $u['role']==='admin' ? 'adm-badge-admin' : '' ?>">
                    <?= htmlspecialchars($u['role']) ?>
                </span>
            </td>
            <td class="adm-ip"><?= $u['last_ip'] ? htmlspecialchars($u['last_ip']) : '<span class="adm-none">—</span>' ?></td>
            <td class="adm-activity"><?= $u['activity_count'] ?> poster</td>
            <td class="adm-date"><?= $u['last_login'] ? date('Y-m-d H:i', strtotime($u['last_login'])) : '<span class="adm-none">Aldrig</span>' ?></td>
            <td>
                <span class="adm-status <?= $u['suspended'] ? 'adm-status-off' : 'adm-status-on' ?>">
                    <?= $u['suspended'] ? 'Suspenderad' : 'Aktiv' ?>
                </span>
            </td>
            <td class="adm-actions">
                <?php if($u['id'] != $_SESSION['user_id']): ?>
                <?php if($u['suspended']): ?>
                <button class="adm-btn adm-btn-unsuspend" data-id="<?= $u['id'] ?>" data-action="unsuspend">Atersta</button>
                <?php else: ?>
                <button class="adm-btn adm-btn-suspend" data-id="<?= $u['id'] ?>" data-action="suspend">Suspendera</button>
                <?php endif; ?>
                <button class="adm-btn adm-btn-delete" data-id="<?= $u['id'] ?>" data-name="<?= htmlspecialchars($u['username']) ?>" data-action="delete">Radera</button>
                <?php else: ?>
                <span class="adm-none">Du</span>
                <?php endif; ?>
            </td>
        </tr>
        <?php endforeach; ?>
        </tbody>
    </table>
</div>

<!-- Confirmation modal -->
<div id="adm-modal" class="adm-modal" style="display:none">
    <div class="adm-modal-box">
        <div class="adm-modal-title">Bekrafta borttagning</div>
        <p class="adm-modal-text">Ar du saker pa att du vill radera <strong id="adm-modal-name"></strong>?<br>All data raderas permanent.</p>
        <div class="adm-modal-actions">
            <button class="adm-btn adm-btn-cancel" id="adm-modal-cancel">Avbryt</button>
            <button class="adm-btn adm-btn-delete" id="adm-modal-confirm">Radera</button>
        </div>
    </div>
</div>

<script>
let pendingDeleteId = null;

document.querySelectorAll('[data-action]').forEach(btn => {
    btn.addEventListener('click', async function() {
        const id     = this.dataset.id;
        const action = this.dataset.action;

        if (action === 'delete') {
            pendingDeleteId = id;
            document.getElementById('adm-modal-name').textContent = this.dataset.name;
            document.getElementById('adm-modal').style.display = 'flex';
            return;
        }

        await doAction(action, id, this);
    });
});

document.getElementById('adm-modal-cancel').addEventListener('click', () => {
    document.getElementById('adm-modal').style.display = 'none';
    pendingDeleteId = null;
});

document.getElementById('adm-modal-confirm').addEventListener('click', async () => {
    document.getElementById('adm-modal').style.display = 'none';
    if (!pendingDeleteId) return;
    const row = document.getElementById('row-' + pendingDeleteId);
    row.style.transition = 'opacity .4s, transform .4s';
    row.style.opacity = '0';
    row.style.transform = 'translateX(40px)';
    setTimeout(() => row.remove(), 420);
    await fetch('admin_action.php', {
        method: 'POST',
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: 'action=delete&id=' + pendingDeleteId
    });
    pendingDeleteId = null;
});

async function doAction(action, id, btn) {
    const row = document.getElementById('row-' + id);
    btn.disabled = true;

    const res  = await fetch('admin_action.php', {
        method: 'POST',
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: `action=${action}&id=${id}`
    });
    const data = await res.json();
    if (!data.ok) { btn.disabled = false; return; }

    if (action === 'suspend') {
        row.classList.add('adm-row-suspended');
        row.querySelector('.adm-status').textContent = 'Suspenderad';
        row.querySelector('.adm-status').className   = 'adm-status adm-status-off';
        btn.textContent    = 'Atersta';
        btn.dataset.action = 'unsuspend';
        btn.className      = 'adm-btn adm-btn-unsuspend';
        btn.disabled       = false;
    } else if (action === 'unsuspend') {
        row.classList.remove('adm-row-suspended');
        row.querySelector('.adm-status').textContent = 'Aktiv';
        row.querySelector('.adm-status').className   = 'adm-status adm-status-on';
        btn.textContent    = 'Suspendera';
        btn.dataset.action = 'suspend';
        btn.className      = 'adm-btn adm-btn-suspend';
        btn.disabled       = false;
    }
}
</script>

<style>
.adm-summary{display:flex;gap:2rem;margin-bottom:2.5rem;flex-wrap:wrap}
.adm-stat{background:var(--s2);border:1px solid var(--gbor);padding:1.4rem 2rem;flex:1;min-width:140px}
.adm-stat-val{font-family:var(--serif);font-size:2.4rem;font-weight:500;color:var(--white);line-height:1}
.adm-stat-label{font-size:.58rem;letter-spacing:.22em;text-transform:uppercase;color:var(--muted);margin-top:.4rem}

.adm-table-wrap{overflow-x:auto;padding:0}
.adm-table{width:100%;border-collapse:collapse;font-size:.82rem}
.adm-table thead th{font-size:.55rem;letter-spacing:.22em;text-transform:uppercase;color:var(--muted);padding:.9rem 1.2rem;text-align:left;border-bottom:1px solid var(--gbor);font-weight:400}
.adm-row td{padding:1.1rem 1.2rem;border-bottom:1px solid rgba(255,255,255,.04);vertical-align:middle;transition:background .2s}
.adm-row:hover td{background:rgba(201,168,76,.03)}
.adm-row-suspended td{opacity:.55}

.adm-user-name{font-weight:500;color:var(--white);letter-spacing:.04em}
.adm-user-sub{font-size:.72rem;color:var(--muted);margin-top:.15rem}

.adm-badge{display:inline-block;font-size:.55rem;letter-spacing:.18em;text-transform:uppercase;padding:.2rem .6rem;border:1px solid var(--gbor);color:var(--muted)}
.adm-badge-admin{border-color:var(--gold);color:var(--gold)}

.adm-ip{font-family:monospace;font-size:.8rem;color:var(--muted);letter-spacing:.05em}
.adm-activity{color:var(--muted);font-size:.8rem}
.adm-date{color:var(--muted);font-size:.78rem}
.adm-none{color:rgba(107,107,94,.4)}

.adm-status{font-size:.6rem;letter-spacing:.15em;text-transform:uppercase;padding:.25rem .7rem;border-radius:0;border:1px solid transparent}
.adm-status-on{border-color:#4a7c4e;color:#4a7c4e}
.adm-status-off{border-color:var(--red);color:var(--red)}

.adm-actions{display:flex;gap:.5rem;align-items:center;flex-wrap:wrap}
.adm-btn{font-size:.62rem;letter-spacing:.14em;text-transform:uppercase;padding:.4rem .9rem;border:1px solid var(--gbor);background:transparent;color:var(--muted);cursor:pointer;transition:all .2s;font-family:var(--sans)}
.adm-btn:hover{border-color:var(--gold);color:var(--gold)}
.adm-btn-suspend:hover{border-color:#A83232;color:#A83232}
.adm-btn-unsuspend:hover{border-color:#4a7c4e;color:#4a7c4e}
.adm-btn-delete:hover{border-color:#A83232;color:#A83232}
.adm-btn:disabled{opacity:.4;cursor:not-allowed}

/* Modal */
.adm-modal{position:fixed;inset:0;background:rgba(0,0,0,.7);backdrop-filter:blur(6px);z-index:9999;display:flex;align-items:center;justify-content:center}
.adm-modal-box{background:var(--s2);border:1px solid var(--gbor);padding:2.5rem 3rem;max-width:420px;width:90%;position:relative}
.adm-modal-title{font-family:var(--serif);font-size:1.5rem;color:var(--white);margin-bottom:1rem;letter-spacing:.04em}
.adm-modal-text{font-size:.85rem;line-height:1.8;color:var(--muted);margin-bottom:2rem}
.adm-modal-actions{display:flex;gap:1rem;justify-content:flex-end}
.adm-btn-cancel{border-color:var(--gbor);color:var(--muted)}
.adm-btn-cancel:hover{border-color:var(--gold);color:var(--gold)}

@media(max-width:760px){
    .adm-table thead th:nth-child(4),.adm-table td:nth-child(4),
    .adm-table thead th:nth-child(5),.adm-table td:nth-child(5){display:none}
}
</style>

<?php include 'includes/footer.php'; ?>
