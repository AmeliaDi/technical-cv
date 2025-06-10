// Network Tools Simulation
class NetworkTools {
    constructor() {
        this.packetDisplay = null;
        this.scanResults = null;
        this.isCapturing = false;
        this.isScanning = false;
        
        this.capturedPackets = [];
        this.packetTypes = ['TCP', 'UDP', 'ICMP', 'HTTP', 'HTTPS', 'DNS', 'ARP'];
        this.protocols = {
            'HTTP': 80,
            'HTTPS': 443,
            'SSH': 22,
            'FTP': 21,
            'SMTP': 25,
            'DNS': 53,
            'DHCP': 67,
            'SNMP': 161,
            'TELNET': 23,
            'POP3': 110
        };
    }

    init() {
        this.packetDisplay = document.getElementById('packet-display');
        this.scanResults = document.getElementById('scan-results');
        
        this.setupEventListeners();
        this.displayWelcomeMessage();
    }

    setupEventListeners() {
        document.getElementById('packet-capture').addEventListener('click', () => {
            this.togglePacketCapture();
        });
        
        document.getElementById('packet-stop').addEventListener('click', () => {
            this.stopPacketCapture();
        });
        
        document.getElementById('port-scan').addEventListener('click', () => {
            this.runPortScan();
        });
    }

    displayWelcomeMessage() {
        this.packetDisplay.innerHTML = `
            <div class="packet-line" style="color: #00ff00;">
                Network Analysis Tools v2.4.1 - Ready
            </div>
            <div class="packet-line" style="color: #888;">
                Click "Iniciar Captura" to begin packet analysis
            </div>
        `;
        
        this.scanResults.innerHTML = `
            <div class="scan-line" style="color: #00ff00;">
                Port Scanner v1.3.7 - Ready
            </div>
            <div class="scan-line" style="color: #888;">
                Enter target and port range, then click "Escanear"
            </div>
        `;
    }

    togglePacketCapture() {
        if (this.isCapturing) {
            this.stopPacketCapture();
        } else {
            this.startPacketCapture();
        }
    }

    startPacketCapture() {
        this.isCapturing = true;
        this.capturedPackets = [];
        
        document.getElementById('packet-capture').textContent = 'Capturando...';
        document.getElementById('packet-capture').style.background = '#ff4444';
        
        this.packetDisplay.innerHTML = `
            <div class="packet-line" style="color: #00ff00;">
                [${this.getCurrentTime()}] Starting packet capture on eth0...
            </div>
            <div class="packet-line" style="color: #00aaff;">
                [${this.getCurrentTime()}] Listening on interface: 192.168.1.100
            </div>
        `;
        
        // Simulate packet capture
        this.packetCaptureInterval = setInterval(() => {
            this.simulatePacketCapture();
        }, Math.random() * 1000 + 500);
    }

    stopPacketCapture() {
        this.isCapturing = false;
        
        if (this.packetCaptureInterval) {
            clearInterval(this.packetCaptureInterval);
        }
        
        document.getElementById('packet-capture').textContent = 'Iniciar Captura';
        document.getElementById('packet-capture').style.background = '';
        
        const captureTime = this.getCurrentTime();
        this.addPacketLine(`[${captureTime}] Packet capture stopped`, '#ffaa00');
        this.addPacketLine(`[${captureTime}] Total packets captured: ${this.capturedPackets.length}`, '#00ff00');
        
        // Display capture statistics
        this.displayCaptureStats();
    }

    simulatePacketCapture() {
        const packet = this.generateRandomPacket();
        this.capturedPackets.push(packet);
        
        const packetLine = this.formatPacketLine(packet);
        this.addPacketLine(packetLine, this.getPacketColor(packet.type));
        
        // Auto-scroll to bottom
        this.packetDisplay.scrollTop = this.packetDisplay.scrollHeight;
        
        // Limit displayed packets to last 50
        const lines = this.packetDisplay.querySelectorAll('.packet-line');
        if (lines.length > 50) {
            lines[0].remove();
        }
    }

    generateRandomPacket() {
        const type = this.packetTypes[Math.floor(Math.random() * this.packetTypes.length)];
        const srcIP = this.generateRandomIP();
        const dstIP = this.generateRandomIP();
        const srcPort = Math.floor(Math.random() * 65535) + 1;
        const dstPort = this.getPortForProtocol(type);
        const size = Math.floor(Math.random() * 1500) + 64;
        const flags = this.generateTCPFlags(type);
        
        return {
            timestamp: new Date(),
            type: type,
            srcIP: srcIP,
            dstIP: dstIP,
            srcPort: srcPort,
            dstPort: dstPort,
            size: size,
            flags: flags,
            ttl: Math.floor(Math.random() * 64) + 64
        };
    }

    generateRandomIP() {
        const ranges = [
            '192.168.1',
            '10.0.0',
            '172.16.1',
            '8.8.8',
            '1.1.1'
        ];
        const range = ranges[Math.floor(Math.random() * ranges.length)];
        const lastOctet = Math.floor(Math.random() * 254) + 1;
        return `${range}.${lastOctet}`;
    }

    getPortForProtocol(type) {
        const protocolPorts = {
            'HTTP': [80, 8080, 3000, 8000],
            'HTTPS': [443, 8443],
            'DNS': [53],
            'SSH': [22],
            'FTP': [21, 20],
            'TCP': [80, 443, 22, 25, 110, 143],
            'UDP': [53, 67, 68, 123],
            'ICMP': [0]
        };
        
        const ports = protocolPorts[type] || [Math.floor(Math.random() * 65535) + 1];
        return ports[Math.floor(Math.random() * ports.length)];
    }

    generateTCPFlags(type) {
        if (type !== 'TCP') return '';
        
        const flags = ['SYN', 'ACK', 'FIN', 'RST', 'PSH', 'URG'];
        const selectedFlags = [];
        
        // Common flag combinations
        const combinations = [
            ['SYN'],
            ['SYN', 'ACK'],
            ['ACK'],
            ['ACK', 'PSH'],
            ['FIN', 'ACK'],
            ['RST']
        ];
        
        return combinations[Math.floor(Math.random() * combinations.length)].join(', ');
    }

    formatPacketLine(packet) {
        const time = packet.timestamp.toLocaleTimeString('en-US', { 
            hour12: false, 
            millisecond: true 
        }).slice(0, -3);
        
        let line = `[${time}] ${packet.type} ${packet.srcIP}:${packet.srcPort} → ${packet.dstIP}:${packet.dstPort}`;
        line += ` (${packet.size}B, TTL:${packet.ttl})`;
        
        if (packet.flags) {
            line += ` [${packet.flags}]`;
        }
        
        return line;
    }

    getPacketColor(type) {
        const colors = {
            'TCP': '#00aaff',
            'UDP': '#00ff00',
            'HTTP': '#ffaa00',
            'HTTPS': '#aa00ff',
            'DNS': '#00ffaa',
            'ICMP': '#ff6666',
            'ARP': '#ffff00'
        };
        return colors[type] || '#ffffff';
    }

    addPacketLine(text, color = '#ffffff') {
        const line = document.createElement('div');
        line.className = 'packet-line';
        line.style.color = color;
        line.textContent = text;
        this.packetDisplay.appendChild(line);
    }

    displayCaptureStats() {
        const stats = this.analyzeCapturedPackets();
        
        this.addPacketLine('', '#ffffff');
        this.addPacketLine('=== CAPTURE STATISTICS ===', '#00ff00');
        this.addPacketLine(`Total Packets: ${stats.total}`, '#ffffff');
        this.addPacketLine(`TCP: ${stats.tcp} (${((stats.tcp/stats.total)*100).toFixed(1)}%)`, '#00aaff');
        this.addPacketLine(`UDP: ${stats.udp} (${((stats.udp/stats.total)*100).toFixed(1)}%)`, '#00ff00');
        this.addPacketLine(`HTTP: ${stats.http} (${((stats.http/stats.total)*100).toFixed(1)}%)`, '#ffaa00');
        this.addPacketLine(`HTTPS: ${stats.https} (${((stats.https/stats.total)*100).toFixed(1)}%)`, '#aa00ff');
        this.addPacketLine(`Average Size: ${stats.avgSize.toFixed(0)} bytes`, '#ffffff');
        this.addPacketLine(`Top Destination: ${stats.topDest}`, '#00ffaa');
    }

    analyzeCapturedPackets() {
        const stats = {
            total: this.capturedPackets.length,
            tcp: 0,
            udp: 0,
            http: 0,
            https: 0,
            avgSize: 0,
            topDest: 'N/A'
        };
        
        if (stats.total === 0) return stats;
        
        const destCounts = {};
        let totalSize = 0;
        
        this.capturedPackets.forEach(packet => {
            switch (packet.type) {
                case 'TCP': stats.tcp++; break;
                case 'UDP': stats.udp++; break;
                case 'HTTP': stats.http++; break;
                case 'HTTPS': stats.https++; break;
            }
            
            totalSize += packet.size;
            destCounts[packet.dstIP] = (destCounts[packet.dstIP] || 0) + 1;
        });
        
        stats.avgSize = totalSize / stats.total;
        
        // Find most frequent destination
        let maxCount = 0;
        for (const [ip, count] of Object.entries(destCounts)) {
            if (count > maxCount) {
                maxCount = count;
                stats.topDest = `${ip} (${count} packets)`;
            }
        }
        
        return stats;
    }

    async runPortScan() {
        if (this.isScanning) return;
        
        this.isScanning = true;
        const target = document.getElementById('scan-target').value || '127.0.0.1';
        const portRange = document.getElementById('port-range').value || '1-100';
        
        document.getElementById('port-scan').textContent = 'Escaneando...';
        document.getElementById('port-scan').style.background = '#ffaa00';
        
        this.scanResults.innerHTML = '';
        this.addScanLine(`Starting port scan on ${target}`, '#00ff00');
        this.addScanLine(`Port range: ${portRange}`, '#00aaff');
        this.addScanLine('', '#ffffff');
        
        const [startPort, endPort] = this.parsePortRange(portRange);
        const openPorts = [];
        
        for (let port = startPort; port <= endPort; port++) {
            const isOpen = this.simulatePortCheck(port);
            const service = this.getServiceName(port);
            
            if (isOpen) {
                openPorts.push(port);
                this.addScanLine(`${port}/tcp   open    ${service}`, '#00ff00');
            } else if (Math.random() < 0.1) { // Show some closed ports
                this.addScanLine(`${port}/tcp   closed  ${service}`, '#ff6666');
            }
            
            // Add small delay for realism
            if (port % 10 === 0) {
                await this.sleep(100);
            }
        }
        
        this.addScanLine('', '#ffffff');
        this.addScanLine(`Scan completed. ${openPorts.length} open ports found.`, '#00ff00');
        
        if (openPorts.length > 0) {
            this.addScanLine(`Open ports: ${openPorts.join(', ')}`, '#00ffaa');
        }
        
        this.addScanLine(`Scan time: ${((endPort - startPort + 1) * 0.05).toFixed(2)}s`, '#888888');
        
        document.getElementById('port-scan').textContent = 'Escanear';
        document.getElementById('port-scan').style.background = '';
        this.isScanning = false;
    }

    parsePortRange(range) {
        const parts = range.split('-');
        const start = parseInt(parts[0]) || 1;
        const end = parseInt(parts[1]) || start;
        return [Math.min(start, end), Math.max(start, end)];
    }

    simulatePortCheck(port) {
        // Simulate realistic port scanning results
        const commonOpenPorts = [22, 53, 80, 110, 143, 443, 993, 995];
        const isCommon = commonOpenPorts.includes(port);
        
        if (isCommon) {
            return Math.random() < 0.7; // 70% chance for common ports
        } else {
            return Math.random() < 0.05; // 5% chance for other ports
        }
    }

    getServiceName(port) {
        const services = {
            21: 'ftp',
            22: 'ssh',
            23: 'telnet',
            25: 'smtp',
            53: 'domain',
            67: 'dhcps',
            68: 'dhcpc',
            80: 'http',
            110: 'pop3',
            143: 'imap',
            443: 'https',
            993: 'imaps',
            995: 'pop3s',
            3389: 'ms-wbt-server',
            5432: 'postgresql',
            3306: 'mysql',
            6379: 'redis',
            27017: 'mongodb'
        };
        
        return services[port] || 'unknown';
    }

    addScanLine(text, color = '#ffffff') {
        const line = document.createElement('div');
        line.className = 'scan-line';
        line.style.color = color;
        line.textContent = text;
        this.scanResults.appendChild(line);
        
        // Auto-scroll to bottom
        this.scanResults.scrollTop = this.scanResults.scrollHeight;
    }

    getCurrentTime() {
        return new Date().toLocaleTimeString('en-US', { 
            hour12: false,
            millisecond: true 
        }).slice(0, -3);
    }

    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
}

// Advanced network utilities
class NetworkAnalyzer {
    static calculateSubnet(ip, cidr) {
        const ipParts = ip.split('.').map(Number);
        const ipInt = (ipParts[0] << 24) + (ipParts[1] << 16) + (ipParts[2] << 8) + ipParts[3];
        
        const mask = 0xFFFFFFFF << (32 - cidr);
        const network = ipInt & mask;
        const broadcast = network | (~mask >>> 0);
        
        return {
            network: this.intToIP(network),
            broadcast: this.intToIP(broadcast),
            hostMin: this.intToIP(network + 1),
            hostMax: this.intToIP(broadcast - 1),
            hostCount: broadcast - network - 1,
            subnetMask: this.intToIP(mask)
        };
    }
    
    static intToIP(int) {
        return [
            (int >>> 24) & 0xFF,
            (int >>> 16) & 0xFF,
            (int >>> 8) & 0xFF,
            int & 0xFF
        ].join('.');
    }
    
    static isPrivateIP(ip) {
        const parts = ip.split('.').map(Number);
        const ipInt = (parts[0] << 24) + (parts[1] << 16) + (parts[2] << 8) + parts[3];
        
        // 10.0.0.0/8
        if ((ipInt & 0xFF000000) === 0x0A000000) return true;
        // 172.16.0.0/12
        if ((ipInt & 0xFFF00000) === 0xAC100000) return true;
        // 192.168.0.0/16
        if ((ipInt & 0xFFFF0000) === 0xC0A80000) return true;
        
        return false;
    }
    
    static calculateLatency() {
        // Simulate network latency calculation
        const baseLatency = Math.random() * 50 + 10; // 10-60ms
        const jitter = Math.random() * 10 - 5; // ±5ms jitter
        return Math.max(0, baseLatency + jitter);
    }
    
    static generateMACAddress() {
        const hex = '0123456789ABCDEF';
        let mac = '';
        for (let i = 0; i < 6; i++) {
            if (i > 0) mac += ':';
            mac += hex[Math.floor(Math.random() * 16)];
            mac += hex[Math.floor(Math.random() * 16)];
        }
        return mac;
    }
}

// Initialize when page loads
document.addEventListener('DOMContentLoaded', () => {
    window.networkTools = new NetworkTools();
    window.networkTools.init();
    window.NetworkAnalyzer = NetworkAnalyzer;
    
    console.log('🌐 Network tools system ready!');
    console.log('🔧 NetworkAnalyzer available with: calculateSubnet, isPrivateIP, calculateLatency, generateMACAddress');
}); 