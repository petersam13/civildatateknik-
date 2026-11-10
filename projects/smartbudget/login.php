<?php
require 'config/db.php';
session_start();

$error = "";

if(isset($_SESSION["user_id"])){
    if($_SESSION["role"] === "admin"){
        header("Location: admin_dashboard.php");
    } else {
        header("Location: dashboard.php");
    }
    exit;
}

if($_SERVER["REQUEST_METHOD"] === "POST"){
    $username = trim($_POST["username"]);
    $password = $_POST["password"];

    $stmt = $pdo->prepare("SELECT * FROM users WHERE username=?");
    $stmt->execute([$username]);
    $user = $stmt->fetch();

    if($user && password_verify($password, $user["password"])){
        if(!empty($user["suspended"])){
            $error = "Kontot ar suspenderat. Kontakta support.";
        } else {
            $_SESSION["user_id"]  = $user["id"];
            $_SESSION["username"] = $user["username"];
            $_SESSION["name"]     = !empty($user["name"]) ? $user["name"] : $user["username"];
            $_SESSION["role"]     = $user["role"];

            $ip = $_SERVER['HTTP_X_FORWARDED_FOR'] ?? $_SERVER['REMOTE_ADDR'] ?? '';
            $ip = trim(explode(',', $ip)[0]);

            $pdo->prepare("UPDATE users SET last_login = NOW(), last_ip = ? WHERE id=?")
                ->execute([$ip, $user["id"]]);

            if($user["role"] === "admin"){
                header("Location: admin_dashboard.php");
            } else {
                header("Location: dashboard.php");
            }
            exit;
        }
    } else {
        $error = "Fel anvandarnamn eller losenord";
    }
}
?>
<!DOCTYPE html>
<html lang="sv">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Logga in — SmartBudget</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Cormorant+Garamond:wght@300;400;500;600&family=Jost:wght@300;400;500;600&display=swap" rel="stylesheet">
<link rel="stylesheet" href="assets/css/style.css?v=4">
</head>
<body class="auth-body">

<!-- Left brand panel -->
<div class="auth-brand">
    <div class="auth-brand-ring"></div>
    <div class="auth-brand-ring auth-brand-ring-2"></div>
    <div class="auth-brand-name">SmartBudget</div>
    <div class="auth-brand-tagline">Privatekonomi · Precision</div>
</div>

<!-- Right form panel -->
<div class="auth-panel">
    <div class="auth-card">

        <div class="auth-logo">Logga in</div>
        <p class="auth-sub">Ange dina uppgifter</p>

        <?php if($error): ?>
            <div class="auth-error"><?= htmlspecialchars($error) ?></div>
        <?php endif; ?>

        <form method="POST">
            <div class="auth-field">
                <label>Anvandarnamn</label>
                <input type="text" name="username" placeholder="Anvandarnamn" required>
            </div>
            <div class="auth-field">
                <label>Losenord</label>
                <input type="password" name="password" placeholder="Losenord" required>
            </div>
            <button type="submit"><span>Logga in</span></button>
        </form>

        <p class="auth-link">
            Inget konto? <a href="register.php">Skapa konto</a>
        </p>

    </div>
</div>

<script src="assets/js/luxury.js?v=4"></script>
</body>
</html>
