<?php
require 'config/db.php';
session_start();

$error = "";

if($_SERVER["REQUEST_METHOD"] === "POST"){
    $name     = trim($_POST["name"]);
    $username = trim($_POST["username"]);
    $password = password_hash($_POST["password"], PASSWORD_DEFAULT);

    try{
        $stmt = $pdo->prepare("INSERT INTO users (name, username, password) VALUES (?,?,?)");
        $stmt->execute([$name, $username, $password]);
        header("Location: login.php");
        exit;
    } catch(PDOException $e){
        $error = "Anvandarnamn redan taget";
    }
}
?>
<!DOCTYPE html>
<html lang="sv">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Skapa konto — SmartBudget</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Cormorant+Garamond:wght@300;400;500;600&family=Jost:wght@300;400;500;600&display=swap" rel="stylesheet">
<link rel="stylesheet" href="assets/css/style.css?v=4">
</head>
<body class="auth-body">

<div class="auth-brand">
    <div class="auth-brand-ring"></div>
    <div class="auth-brand-ring auth-brand-ring-2"></div>
    <div class="auth-brand-name">SmartBudget</div>
    <div class="auth-brand-tagline">Privatekonomi · Precision</div>
</div>

<div class="auth-panel">
    <div class="auth-card">

        <div class="auth-logo">Skapa konto</div>
        <p class="auth-sub">Borja planera idag</p>

        <?php if($error): ?>
            <div class="auth-error"><?= htmlspecialchars($error) ?></div>
        <?php endif; ?>

        <form method="POST">
            <div class="auth-field">
                <label>Ditt namn</label>
                <input type="text" name="name" placeholder="Ditt namn" required>
            </div>
            <div class="auth-field">
                <label>Anvandarnamn</label>
                <input type="text" name="username" placeholder="Anvandarnamn" required>
            </div>
            <div class="auth-field">
                <label>Losenord</label>
                <input type="password" name="password" placeholder="Losenord" required>
            </div>
            <button type="submit"><span>Skapa konto</span></button>
        </form>

        <p class="auth-link">
            Har du konto? <a href="login.php">Logga in</a>
        </p>

    </div>
</div>

<script src="assets/js/luxury.js?v=4"></script>
</body>
</html>
