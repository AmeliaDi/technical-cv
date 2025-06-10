# Advanced Web Vulnerability Scanner

A comprehensive web application security testing tool written in C. Designed to identify common web vulnerabilities including SQL injection, XSS, CSRF, directory traversal, and various other security flaws.

## Features

- **Comprehensive Vulnerability Detection**: SQL injection, XSS (reflected, stored, DOM), CSRF, directory traversal, file inclusion, command injection, XXE, SSRF, and more
- **Multi-threaded Architecture**: Configurable thread pool for high-speed scanning
- **Advanced Web Crawling**: Intelligent link extraction and form discovery
- **Multiple Output Formats**: Human-readable text, JSON, and XML reports
- **Security Header Analysis**: Detection of missing or misconfigured security headers
- **Authentication Testing**: Weak password detection, session fixation, and session hijacking tests
- **Information Disclosure Detection**: Sensitive data exposure and debug information leakage
- **Configurable Scanning**: Adjustable depth, threading, timeouts, and aggressive mode
- **Real-time Statistics**: Comprehensive scan metrics and progress tracking

## Building

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install build-essential libcurl4-openssl-dev libjson-c-dev libxml2-dev libpcre3-dev

# Arch Linux
sudo pacman -S base-devel curl json-c libxml2 pcre

# CentOS/RHEL
sudo yum install gcc curl-devel json-c-devel libxml2-devel pcre-devel
```

### Compilation

```bash
# Build the scanner
make

# Build with debug symbols
make debug

# Build optimized version
make performance
```

### Installation

```bash
# Install system-wide (requires sudo)
sudo make install
```

## Usage

### Basic Scanning

```bash
# Scan single website
./webvulnscan -u http://example.com

# Scan with custom depth and threads
./webvulnscan -u http://example.com -d 3 -t 10

# Aggressive scanning mode
./webvulnscan -u http://example.com -a -v

# Skip SSL verification for testing
./webvulnscan -u https://example.com -s
```

### Advanced Scanning

```bash
# Comprehensive scan with all options
./webvulnscan -u http://target.com -d 5 -t 15 -a -v -i -f json -o results.json

# Quick scan with minimal depth
./webvulnscan -u http://target.com -d 1 -t 5

# Scan with custom timeout
./webvulnscan -u http://target.com -T 60 -v
```

### Output Formats

```bash
# JSON output
./webvulnscan -u http://example.com -f json -o vulnerability_report.json

# XML output
./webvulnscan -u http://example.com -f xml -o vulnerability_report.xml

# Verbose text output
./webvulnscan -u http://example.com -v -i
```

## Command Line Options

| Option | Description | Example |
|--------|-------------|---------|
| `-u <url>` | Target URL (required) | `-u http://example.com` |
| `-d <depth>` | Maximum crawl depth | `-d 3` |
| `-t <threads>` | Number of threads | `-t 10` |
| `-T <timeout>` | Request timeout in seconds | `-T 30` |
| `-o <file>` | Output file | `-o results.json` |
| `-f <format>` | Output format (text/json/xml) | `-f json` |
| `-a` | Enable aggressive scanning | `-a` |
| `-v` | Verbose output | `-v` |
| `-i` | Include informational findings | `-i` |
| `-s` | Skip SSL certificate verification | `-s` |
| `-k` | Don't follow redirects | `-k` |
| `-h` | Show help message | `-h` |

## Vulnerability Categories

### Injection Vulnerabilities
- **SQL Injection**: Database query manipulation
- **Command Injection**: Operating system command execution
- **LDAP Injection**: LDAP query manipulation
- **XPath Injection**: XML database query manipulation

### Cross-Site Scripting (XSS)
- **Reflected XSS**: Non-persistent script injection
- **Stored XSS**: Persistent script injection
- **DOM XSS**: Client-side DOM manipulation

### Access Control
- **Insecure Direct Object References**: Unauthorized resource access
- **Missing Function Level Access Control**: Privilege escalation
- **Cross-Site Request Forgery (CSRF)**: Unauthorized action execution

### Security Misconfiguration
- **Missing Security Headers**: X-Frame-Options, CSP, HSTS, etc.
- **Information Disclosure**: Sensitive data exposure
- **Debug Information**: Development artifacts in production

### File System Attacks
- **Directory Traversal**: Path traversal vulnerabilities
- **File Inclusion**: Local and remote file inclusion
- **Unrestricted File Upload**: Dangerous file upload functionality

### Advanced Attacks
- **XML External Entity (XXE)**: XML parser exploitation
- **Server-Side Request Forgery (SSRF)**: Internal system access
- **Session Management Flaws**: Session fixation, hijacking

## Testing Payloads

The scanner includes comprehensive payload databases for:

### SQL Injection
```sql
' OR '1'='1
'; DROP TABLE users; --
' UNION SELECT null,null,version()--
```

### XSS Payloads
```javascript
<script>alert('XSS')</script>
"><script>alert(document.cookie)</script>
javascript:alert('XSS')
```

### Directory Traversal
```
../../../etc/passwd
..\..\..\..\windows\system32\drivers\etc\hosts
```

### Command Injection
```bash
; ls -la
| whoami
`cat /etc/passwd`
```

## Configuration

### Environment Variables

```bash
export WEBVULNSCAN_THREADS=10
export WEBVULNSCAN_TIMEOUT=30
export WEBVULNSCAN_USER_AGENT="Custom Scanner 1.0"
```

### Custom Payloads

Create custom payload files in the `payloads/` directory:

```
payloads/
├── sql_injection.txt
├── xss_payloads.txt
├── directory_traversal.txt
└── command_injection.txt
```

## Output Examples

### Text Report
```
Web Vulnerability Scan Report
=============================

Target URL: http://example.com
Scan Time: Mon Jan 15 10:30:00 2025
Total Vulnerabilities Found: 3

Severity Summary:
- Critical: 1
- High:     1
- Medium:   1
- Low:      0

Detailed Findings:
==================

[1] SQL Injection in Login Form
Severity: Critical
Type: SQL Injection
URL: http://example.com/login.php
Parameter: username
Description: The application is vulnerable to SQL injection attacks
Evidence: MySQL error: "You have an error in your SQL syntax"
Recommendation: Use parameterized queries and input validation
```

### JSON Report
```json
{
  "scan_info": {
    "target_url": "http://example.com",
    "scan_time": 1705314600,
    "duration": 45.32,
    "total_vulnerabilities": 3
  },
  "vulnerabilities": [
    {
      "id": 1,
      "title": "SQL Injection in Login Form",
      "severity": "Critical",
      "type": "SQL Injection",
      "url": "http://example.com/login.php",
      "parameter": "username",
      "description": "The application is vulnerable to SQL injection attacks",
      "evidence": "MySQL error: \"You have an error in your SQL syntax\"",
      "recommendation": "Use parameterized queries and input validation"
    }
  ]
}
```

## Performance

### Optimization Features

- **Multi-threading**: Parallel vulnerability testing
- **Connection pooling**: Efficient HTTP connection reuse
- **Smart crawling**: Duplicate URL detection and filtering
- **Rate limiting**: Configurable request throttling
- **Memory management**: Efficient payload and response handling

### Benchmarks

Typical performance on modern hardware:

| Scenario | Speed | Notes |
|----------|-------|-------|
| Small website (< 100 pages) | 2-5 minutes | Standard scan |
| Medium website (100-500 pages) | 10-15 minutes | Depth 3, 10 threads |
| Large website (500+ pages) | 30+ minutes | Aggressive mode |

## Security Features

### Safe Scanning Practices

- **Request rate limiting**: Prevents server overload
- **User-Agent rotation**: Reduces detection probability
- **Timeout handling**: Prevents hanging requests
- **SSL verification**: Optional certificate validation
- **Redirect following**: Configurable redirect behavior

### Ethical Considerations

- Only scan systems you own or have explicit permission to test
- Use responsible disclosure for any vulnerabilities found
- Respect rate limits and avoid causing service disruption
- Follow local laws and regulations regarding security testing

## Development

### Building Tests

```bash
# Build and run unit tests
make test

# Run integration tests
make integration-test

# Memory leak detection
make memcheck

# Static code analysis
make static-analysis
```

### Code Quality

```bash
# Format code
make format

# Security scan of binary
make security-scan

# Performance benchmark
make benchmark
```

### Docker Support

```bash
# Build Docker image
make docker-build

# Run in container
make docker-run
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

## Configuration Files

### Scanner Configuration

Create `~/.webvulnscan.conf`:

```ini
[default]
threads = 10
timeout = 30
max_depth = 3
output_format = json
user_agent = WebVulnScanner/1.0

[payloads]
sql_injection_file = payloads/sql_injection.txt
xss_payloads_file = payloads/xss_payloads.txt
command_injection_file = payloads/command_injection.txt
```

## Troubleshooting

### Common Issues

1. **SSL certificate errors**
   ```bash
   ./webvulnscan -u https://example.com -s
   ```

2. **Slow scanning**
   ```bash
   # Increase threads and reduce timeout
   ./webvulnscan -u http://example.com -t 20 -T 10
   ```

3. **Memory usage**
   ```bash
   # Reduce thread count and depth
   ./webvulnscan -u http://example.com -t 5 -d 2
   ```

4. **Connection timeouts**
   ```bash
   # Increase timeout
   ./webvulnscan -u http://example.com -T 60
   ```

### Debugging

```bash
# Enable verbose output
./webvulnscan -u http://example.com -v

# Enable debug build
make debug
./webvulnscan -u http://example.com -v
```

## Legal Notice

This tool is intended for authorized security testing and vulnerability assessment only. Users are responsible for ensuring they have proper authorization before scanning any systems. Unauthorized scanning may be illegal in your jurisdiction.

## Dependencies

- **libcurl**: HTTP client functionality
- **json-c**: JSON report generation
- **libxml2**: HTML parsing and XML reports
- **libpcre**: Regular expression matching
- **pthread**: Multi-threading support

## License

GPL v2

## Author

AmeliaDi <enorastrokes@gmail.com>

## Changelog

### v1.0.0
- Initial release
- SQL injection detection
- XSS vulnerability testing
- CSRF detection
- Directory traversal testing
- Security header analysis
- Multi-threaded scanning
- Multiple output formats
- Comprehensive vulnerability database

## Roadmap

### Upcoming Features
- **OWASP Top 10 2021 Coverage**: Complete implementation
- **API Security Testing**: REST/GraphQL endpoint testing
- **Mobile Application Testing**: Android/iOS app analysis
- **Cloud Security Scanning**: AWS/Azure/GCP configuration testing
- **AI-Powered Detection**: Machine learning vulnerability identification
- **Plugin Architecture**: Custom vulnerability test modules

# AmeliaHV - Type-1 Bare Metal Hypervisor

🚀 **Advanced Type-1 Bare Metal Hypervisor with Intel VT-x Support**

*Created by Amelia Enora Marceline Chavez Barroso*

## Features

- Intel VT-x (VMX) hardware virtualization
- Multi-CPU support with per-CPU VMCS management
- Direct hardware access for minimal overhead
- Professional kernel module architecture

## Build & Install

```bash
make all
sudo make install
```

## Requirements

- Intel CPU with VT-x support
- Linux kernel headers
- Root privileges

## License

MIT License - See LICENSE file 