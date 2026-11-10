<?php session_start(); ?>
<!DOCTYPE html>
<html lang="sv">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>SmartBudget</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Cormorant+Garamond:wght@300;400;500;600&family=Jost:wght@300;400;500;600&display=swap" rel="stylesheet">
<link rel="stylesheet" href="assets/css/style.css?v=4">
<style>
/* Landing 3D extras */
#luxury-canvas {
    position:fixed; inset:0; z-index:1; pointer-events:none;
}
.landing-body { background:#050505; }
.landing-container { position:relative; z-index:5; }

.landing-title {
    text-shadow: 0 0 80px rgba(201,168,76,.15);
    transform-style: preserve-3d;
}
.landing-title-gold {
    text-shadow:
        0 0 40px rgba(201,168,76,.4),
        0 2px 0 rgba(0,0,0,.8),
        0 4px 20px rgba(201,168,76,.2);
}
</style>
</head>
<body class="landing-body">

<canvas id="luxury-canvas"></canvas>

<div class="corner tl"></div>
<div class="corner tr"></div>
<div class="corner bl"></div>
<div class="corner br"></div>
<div class="landing-scan"></div>

<div class="landing-container">

    <div class="landing-eyebrow">Privatekonomi</div>

    <h1 class="landing-title">
        Smart<span class="landing-title-gold">Budget</span>
    </h1>

    <div class="landing-rule"></div>

    <p class="landing-sub">
        Full kontroll över din ekonomi. Registrera inkomster,
        fasta och rorliga kostnader — se exakt vad du kan spara.
    </p>

    <div class="landing-buttons">
        <a href="register.php" class="primary-btn">Skapa konto</a>
        <a href="login.php" class="secondary-btn">Logga in</a>
    </div>

</div>

<script src="assets/js/luxury.js?v=4"></script>
</body>
</html>
