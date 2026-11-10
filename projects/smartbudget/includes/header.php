<?php
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

/* BAS-URL till projektet på MIUN-servern */
$BASE_URL = "/~pesa2401/smartbudget/";
?>

<!DOCTYPE html>
<html lang="sv">

<head>

<meta charset="UTF-8">
<title>SmartBudget</title>

<meta name="viewport" content="width=device-width, initial-scale=1.0">

<!-- Google Fonts -->
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>

<link href="https://fonts.googleapis.com/css2?family=Cormorant+Garamond:wght@300;400;500;600&family=Jost:wght@300;400;500;600&display=swap" rel="stylesheet">

<!-- CSS -->
<link rel="stylesheet" href="<?= $BASE_URL ?>assets/css/style.css?v=4">

<!-- JavaScript -->
<script src="<?= $BASE_URL ?>assets/js/luxury.js?v=4" defer></script>

<!-- Favicon (valfri) -->
<link rel="icon" href="<?= $BASE_URL ?>assets/images/favicon.ico">

</head>

<body>

<div class="layout">

<!-- SIDEBAR -->
<aside class="sidebar">

<div class="sidebar-logo">
<div class="sidebar-logo-mark">SB</div>
<div class="sidebar-logo-text">SmartBudget</div>
</div>

<nav>

<div class="sidebar-label">Navigering</div>

<a href="<?= $BASE_URL ?>dashboard.php">Dashboard</a>

<a href="<?= $BASE_URL ?>statistics.php">Statistik</a>

<?php if(isset($_SESSION["role"]) && $_SESSION["role"] === "admin"): ?>
<a href="<?= $BASE_URL ?>admin_dashboard.php">Admin</a>
<?php endif; ?>

<a href="<?= $BASE_URL ?>logout.php">Logga ut</a>

</nav>

</aside>

<!-- MAIN CONTENT -->
<main class="main-content">