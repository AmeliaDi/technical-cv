<?php
/*
 * Database Configuration for CV Website
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * Hosting: InfinityFree
 */

// Database configuration
define('DB_HOST', 'sql207.infinityfree.com');
define('DB_NAME', '***REMOVED***_cv_portfolio');
define('DB_USER', '***REMOVED***');
define('DB_PASS', '3JCqt7TUk4ED');
define('DB_PORT', 3306);

// Application configuration
define('APP_NAME', 'AmeliaDi - Technical CV');
define('APP_VERSION', '2.0.0');
define('DEBUG_MODE', false);

// Security settings
define('SESSION_TIMEOUT', 3600); // 1 hour
define('MAX_LOGIN_ATTEMPTS', 5);
define('RATE_LIMIT_REQUESTS', 100);
define('RATE_LIMIT_WINDOW', 3600); // 1 hour

// Email configuration (for contact form)
define('SMTP_HOST', 'smtp.gmail.com');
define('SMTP_PORT', 587);
define('SMTP_USERNAME', 'enorastrokes@gmail.com');
define('SMTP_PASSWORD', ''); // Set this in production
define('CONTACT_EMAIL', 'enorastrokes@gmail.com');

// File paths
define('UPLOAD_PATH', '/uploads/');
define('LOG_PATH', '/logs/');
define('CACHE_PATH', '/cache/');

// Site settings
define('SITE_URL', 'https://ameliadi.infinityfreeapp.com');
define('ADMIN_EMAIL', 'enorastrokes@gmail.com');
define('TIMEZONE', 'America/Mexico_City');

// Set timezone
date_default_timezone_set(TIMEZONE);

// Error reporting
if (DEBUG_MODE) {
    error_reporting(E_ALL);
    ini_set('display_errors', 1);
} else {
    error_reporting(0);
    ini_set('display_errors', 0);
}

// Database connection class
class Database {
    private static $instance = null;
    private $connection;
    
    private function __construct() {
        try {
            $this->connection = new PDO(
                "mysql:host=" . DB_HOST . ";port=" . DB_PORT . ";dbname=" . DB_NAME . ";charset=utf8mb4",
                DB_USER,
                DB_PASS,
                [
                    PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
                    PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
                    PDO::ATTR_EMULATE_PREPARES => false,
                    PDO::ATTR_PERSISTENT => true
                ]
            );
        } catch (PDOException $e) {
            error_log("Database connection failed: " . $e->getMessage());
            die("Database connection failed. Please try again later.");
        }
    }
    
    public static function getInstance() {
        if (self::$instance === null) {
            self::$instance = new self();
        }
        return self::$instance;
    }
    
    public function getConnection() {
        return $this->connection;
    }
    
    public function query($sql, $params = []) {
        try {
            $stmt = $this->connection->prepare($sql);
            $stmt->execute($params);
            return $stmt;
        } catch (PDOException $e) {
            error_log("Query failed: " . $e->getMessage() . " SQL: " . $sql);
            throw new Exception("Database query failed");
        }
    }
    
    public function lastInsertId() {
        return $this->connection->lastInsertId();
    }
    
    public function beginTransaction() {
        return $this->connection->beginTransaction();
    }
    
    public function commit() {
        return $this->connection->commit();
    }
    
    public function rollback() {
        return $this->connection->rollback();
    }
}

// Utility functions
function sanitizeInput($input) {
    return htmlspecialchars(trim($input), ENT_QUOTES, 'UTF-8');
}

function generateCSRFToken() {
    if (!isset($_SESSION['csrf_token'])) {
        $_SESSION['csrf_token'] = bin2hex(random_bytes(32));
    }
    return $_SESSION['csrf_token'];
}

function verifyCSRFToken($token) {
    return isset($_SESSION['csrf_token']) && hash_equals($_SESSION['csrf_token'], $token);
}

function logActivity($action, $details = '') {
    $db = Database::getInstance();
    $ip = $_SERVER['REMOTE_ADDR'] ?? 'unknown';
    $userAgent = $_SERVER['HTTP_USER_AGENT'] ?? 'unknown';
    
    try {
        $db->query(
            "INSERT INTO activity_log (action, details, ip_address, user_agent, created_at) VALUES (?, ?, ?, ?, NOW())",
            [$action, $details, $ip, $userAgent]
        );
    } catch (Exception $e) {
        error_log("Failed to log activity: " . $e->getMessage());
    }
}

function rateLimitCheck($identifier = null) {
    if ($identifier === null) {
        $identifier = $_SERVER['REMOTE_ADDR'] ?? 'unknown';
    }
    
    $db = Database::getInstance();
    
    try {
        // Clean old entries
        $db->query(
            "DELETE FROM rate_limits WHERE created_at < DATE_SUB(NOW(), INTERVAL ? SECOND)",
            [RATE_LIMIT_WINDOW]
        );
        
        // Check current rate
        $stmt = $db->query(
            "SELECT COUNT(*) as count FROM rate_limits WHERE identifier = ? AND created_at > DATE_SUB(NOW(), INTERVAL ? SECOND)",
            [$identifier, RATE_LIMIT_WINDOW]
        );
        
        $count = $stmt->fetch()['count'];
        
        if ($count >= RATE_LIMIT_REQUESTS) {
            return false;
        }
        
        // Record this request
        $db->query(
            "INSERT INTO rate_limits (identifier, created_at) VALUES (?, NOW())",
            [$identifier]
        );
        
        return true;
    } catch (Exception $e) {
        error_log("Rate limit check failed: " . $e->getMessage());
        return true; // Allow on error
    }
}

// Initialize session
session_start();

// Check rate limiting
if (!rateLimitCheck()) {
    http_response_code(429);
    die(json_encode(['error' => 'Rate limit exceeded. Please try again later.']));
}

// Set security headers
header('X-Content-Type-Options: nosniff');
header('X-Frame-Options: DENY');
header('X-XSS-Protection: 1; mode=block');
header('Referrer-Policy: strict-origin-when-cross-origin');

if (!DEBUG_MODE) {
    header('Strict-Transport-Security: max-age=31536000; includeSubDomains');
}
?> 