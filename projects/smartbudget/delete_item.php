<?php
require 'includes/auth.php';
require 'config/db.php';

$user_id = $_SESSION["user_id"];
$type = $_GET["type"] ?? "";
$id = intval($_GET["id"]);

$table = "";

if($type === "income") $table = "incomes";
if($type === "fixed") $table = "fixed_expenses";
if($type === "variable") $table = "variable_expenses";

$isAjax = isset($_SERVER['HTTP_X_REQUESTED_WITH']) &&
          strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) === 'xmlhttprequest';

if($table && $id){
    $stmt = $pdo->prepare("DELETE FROM $table WHERE id=? AND user_id=?");
    $stmt->execute([$id, $user_id]);
}

if($isAjax){
    header('Content-Type: application/json');
    echo json_encode(['ok' => true]);
    exit;
}

header("Location: dashboard.php");
exit;