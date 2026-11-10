<?php
require 'includes/auth.php';
require 'config/db.php';

$user_id = $_SESSION["user_id"];
$type = $_GET["type"] ?? "";

$table = "";

if($type === "income") $table = "incomes";
if($type === "fixed") $table = "fixed_expenses";
if($type === "variable") $table = "variable_expenses";

if(!$table){
    header("Location: dashboard.php");
    exit;
}

if($_SERVER["REQUEST_METHOD"] === "POST"){

    $name = $_POST["name"];
    $amount = floatval($_POST["amount"]);

    if($type === "income"){
        $stmt = $pdo->prepare("INSERT INTO incomes (user_id, name, amount) VALUES (?,?,?)");
        $stmt->execute([$user_id, $name, $amount]);
    } else {
        $stmt = $pdo->prepare("INSERT INTO $table (user_id, name, amount) VALUES (?,?,?)");
        $stmt->execute([$user_id, $name, $amount]);
    }

    header("Location: dashboard.php");
    exit;
}

include 'includes/header.php';
?>

<h1>Lägg till</h1>

<form method="POST" class="section-block">

    <div class="input-line">
        <span>Namn</span>
        <input type="text" name="name" required>
    </div>

    <div class="input-line">
        <span>Belopp</span>
        <input type="number" step="0.01" name="amount" required>
    </div>

    <button class="save-btn">Spara</button>

</form>

<?php include 'includes/footer.php'; ?>