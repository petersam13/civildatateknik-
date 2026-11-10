<?php
require 'includes/auth.php';
require 'config/db.php';

header('Content-Type: application/json');

if ($_SESSION['role'] !== 'admin') {
    echo json_encode(['ok' => false, 'error' => 'unauthorized']);
    exit;
}

$action = $_POST['action'] ?? '';
$id     = intval($_POST['id'] ?? 0);

if (!$id) {
    echo json_encode(['ok' => false, 'error' => 'invalid id']);
    exit;
}

/* Prevent admin from acting on themselves */
if ($id === intval($_SESSION['user_id'])) {
    echo json_encode(['ok' => false, 'error' => 'cannot act on self']);
    exit;
}

if ($action === 'suspend') {
    $pdo->prepare("UPDATE users SET suspended=1 WHERE id=?")->execute([$id]);
    echo json_encode(['ok' => true]);

} elseif ($action === 'unsuspend') {
    $pdo->prepare("UPDATE users SET suspended=0 WHERE id=?")->execute([$id]);
    echo json_encode(['ok' => true]);

} elseif ($action === 'delete') {
    $pdo->prepare("DELETE FROM users WHERE id=?")->execute([$id]);
    echo json_encode(['ok' => true]);

} else {
    echo json_encode(['ok' => false, 'error' => 'unknown action']);
}
