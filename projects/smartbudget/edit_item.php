<?php
require 'includes/auth.php';
require 'config/db.php';

$user_id = $_SESSION["user_id"];
$type = $_GET["type"];
$id = intval($_GET["id"]);

$table = "";

if($type === "income") $table = "incomes";
if($type === "fixed") $table = "fixed_expenses";
if($type === "variable") $table = "variable_expenses";

$stmt = $pdo->prepare("SELECT * FROM $table WHERE id=? AND user_id=?");
$stmt->execute([$id, $user_id]);
$item = $stmt->fetch();

if(!$item) {
    header("Location: dashboard.php");
    exit;
}

if($_SERVER["REQUEST_METHOD"] === "POST") {

    $amount = floatval($_POST["amount"]);

    $stmt = $pdo->prepare("UPDATE $table SET amount=? WHERE id=? AND user_id=?");
    $stmt->execute([$amount, $id, $user_id]);

    header("Location: dashboard.php");
    exit;
}

include 'includes/header.php';
?>

<h1>Ändra <?= htmlspecialchars($item['name'] ?? 'Post') ?></h1>

<form method="POST" class="section-block">

    <div class="input-line">
        <span>Belopp</span>
        <input type="number" name="amount" value="<?= $item['amount'] ?>">
    </div>

    <button class="save-btn">Spara</button>

</form>

<?php include 'includes/footer.php'; ?>