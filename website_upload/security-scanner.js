/**
 * Security Scanner & Browser Analysis - Inline Results
 * @description Clean security analyzer with inline expanding results
 * @author Amelia Enora 🌈 Marceline Chavez Barroso
 * @version 3.0.0 - Inline Edition
 */

class SecurityScanner {
    constructor() {
        this.state = {
            vulnerabilities: [],
            browserData: {},
            locationData: {},
            riskLevel: 'LOW',
            isScanning: false,
            isInitialized: false
        };
        this.init();
    }

    async init() {
        if (this.state.isInitialized) return;
        console.log('🔍 Inicializando Security Scanner v3.0...');
        this.createScannerInterface();
        this.state.isInitialized = true;
    }

    // Create the main scanner interface
    createScannerInterface() {
        const container = document.createElement('div');
        container.id = 'security-scanner';
        container.className = 'security-scanner-container';
        container.innerHTML = `
            <div class="scanner-header">
                <h3>🛡️ Análisis de Seguridad del Navegador</h3>
                <button id="scan-button" class="scan-button">
                    <span class="scan-icon">🔍</span>
                    <span class="scan-text">Iniciar Análisis</span>
                </button>
            </div>
            <div id="scanner-results" class="scanner-results hidden">
                <div class="results-loading">
                    <div class="loading-spinner"></div>
                    <span>Analizando seguridad del navegador...</span>
                </div>
            </div>
        `;

        // Insert after the security section in the main page
        const securitySection = document.querySelector('.security-section');
        if (securitySection) {
            securitySection.appendChild(container);
        } else {
            document.body.appendChild(container);
        }

        // Add event listener
        document.getElementById('scan-button').addEventListener('click', () => {
            this.startScan();
        });
    }

    // Start the security scan
    async startScan() {
        if (this.state.isScanning) return;
        
        this.state.isScanning = true;
        const button = document.getElementById('scan-button');
        const results = document.getElementById('scanner-results');
        
        // Update button state
        button.disabled = true;
        button.innerHTML = `
            <div class="loading-spinner small"></div>
            <span>Escaneando...</span>
        `;
        
        // Show results container
        results.classList.remove('hidden');
        results.scrollIntoView({ behavior: 'smooth', block: 'nearest' });

        try {
            // Perform the scan
            await this.performScan();
            
            // Show results
            this.displayResults();
            
        } catch (error) {
            console.error('Error durante el escaneo:', error);
            this.showError();
        } finally {
            // Reset button
            this.state.isScanning = false;
            button.disabled = false;
            button.innerHTML = `
                <span class="scan-icon">🔄</span>
                <span class="scan-text">Reanalizar</span>
            `;
        }
    }

    // Perform the actual security scan
    async performScan() {
        // Simulate scanning delay for better UX
        await new Promise(resolve => setTimeout(resolve, 1500));
        
        await Promise.all([
            this.scanBrowser(),
            this.getLocationData()
        ]);
        
        this.analyzeVulnerabilities();
    }

    // Browser scanning
    async scanBrowser() {
        const nav = navigator;
        const scr = screen;
        const win = window;

        this.state.browserData = {
            browser: this.getBrowserInfo(nav.userAgent),
            platform: nav.platform,
            language: nav.language,
            
            features: {
                cookies: nav.cookieEnabled,
                online: nav.onLine,
                geolocation: !!nav.geolocation,
                webrtc: this.hasWebRTC(),
                doNotTrack: nav.doNotTrack === '1'
            },
            
            hardware: {
                cores: nav.hardwareConcurrency || 'Unknown',
                memory: nav.deviceMemory || 'Unknown',
                maxTouch: nav.maxTouchPoints || 0
            },
            
            display: {
                screen: `${scr.width}x${scr.height}`,
                viewport: `${win.innerWidth}x${win.innerHeight}`,
                colorDepth: scr.colorDepth
            }
        };

        this.checkVulnerabilities();
    }

    // Get browser info
    getBrowserInfo(userAgent) {
        const ua = userAgent.toLowerCase();
        const browsers = [
            { name: 'Chrome', regex: /chrome\/(\d+)/, key: 'chrome' },
            { name: 'Firefox', regex: /firefox\/(\d+)/, key: 'firefox' },
            { name: 'Safari', regex: /safari\/(\d+)/, key: 'safari' },
            { name: 'Edge', regex: /edg\/(\d+)/, key: 'edge' }
        ];

        for (const browser of browsers) {
            if (ua.includes(browser.key)) {
                const match = ua.match(browser.regex);
                return {
                    name: browser.name,
                    version: match ? parseInt(match[1]) : 'Unknown'
                };
            }
        }
        return { name: 'Unknown', version: 'Unknown' };
    }

    // Check WebRTC availability
    hasWebRTC() {
        return !!(window.RTCPeerConnection || window.webkitRTCPeerConnection || window.mozRTCPeerConnection);
    }

    // Check for vulnerabilities
    checkVulnerabilities() {
        const vulns = [];
        const browser = this.state.browserData.browser;
        const features = this.state.browserData.features;

        // Browser version checks
        if (browser.name === 'Chrome' && browser.version < 120) {
            vulns.push({
                type: 'Navegador Desactualizado',
                severity: 'HIGH',
                description: `${browser.name} v${browser.version} tiene vulnerabilidades conocidas`,
                recommendation: 'Actualizar navegador inmediatamente',
                icon: '🚨'
            });
        }

        if (browser.name === 'Firefox' && browser.version < 110) {
            vulns.push({
                type: 'Navegador Desactualizado',
                severity: 'HIGH',
                description: `${browser.name} v${browser.version} necesita actualización`,
                recommendation: 'Actualizar navegador inmediatamente',
                icon: '🚨'
            });
        }

        // Privacy checks
        if (features.webrtc) {
            vulns.push({
                type: 'Filtración de IP',
                severity: 'HIGH',
                description: 'WebRTC puede revelar tu IP real incluso con VPN',
                recommendation: 'Deshabilitar WebRTC en configuración avanzada',
                icon: '🔴'
            });
        }

        if (features.geolocation) {
            vulns.push({
                type: 'Geolocalización Expuesta',
                severity: 'MEDIUM',
                description: 'API de ubicación está disponible para sitios web',
                recommendation: 'Denegar permisos de ubicación por defecto',
                icon: '🟡'
            });
        }

        if (!features.doNotTrack) {
            vulns.push({
                type: 'Rastreo Habilitado',
                severity: 'MEDIUM',
                description: 'Do Not Track no está activado',
                recommendation: 'Habilitar Do Not Track en configuración',
                icon: '🟡'
            });
        }

        // Plugin check
        if (navigator.plugins?.length > 8) {
            vulns.push({
                type: 'Exceso de Plugins',
                severity: 'LOW',
                description: `${navigator.plugins.length} plugins detectados`,
                recommendation: 'Deshabilitar plugins innecesarios',
                icon: '🟢'
            });
        }

        // JavaScript warning
        vulns.push({
            type: 'JavaScript Habilitado',
            severity: 'LOW',
            description: 'JavaScript completamente funcional',
            recommendation: 'Usar NoScript para sitios no confiables',
            icon: '🟢'
        });

        this.state.vulnerabilities = vulns;
    }

    // Get location data
    async getLocationData() {
        try {
            const response = await fetch('https://ipapi.co/json/', { timeout: 5000 });
            if (response.ok) {
                const data = await response.json();
                this.state.locationData = {
                    publicIP: data.ip || 'Unknown',
                    country: data.country_name || 'Unknown',
                    city: data.city || 'Unknown',
                    isp: data.org || 'Unknown'
                };
            }
        } catch (error) {
            this.state.locationData = {
                publicIP: 'No disponible',
                country: 'No disponible',
                city: 'No disponible',
                isp: 'No disponible'
            };
        }
    }

    // Analyze risk level
    analyzeVulnerabilities() {
        const counts = { HIGH: 0, MEDIUM: 0, LOW: 0 };
        
        this.state.vulnerabilities.forEach(vuln => {
            counts[vuln.severity]++;
        });
        
        if (counts.HIGH >= 2) {
            this.state.riskLevel = 'CRITICAL';
        } else if (counts.HIGH >= 1 || counts.MEDIUM >= 3) {
            this.state.riskLevel = 'HIGH';
        } else if (counts.MEDIUM >= 1 || counts.LOW >= 3) {
            this.state.riskLevel = 'MEDIUM';
        } else {
            this.state.riskLevel = 'LOW';
        }
    }

    // Display results inline
    displayResults() {
        const resultsContainer = document.getElementById('scanner-results');
        const riskColors = {
            CRITICAL: '#ff0000',
            HIGH: '#ff6600',
            MEDIUM: '#ffaa00',
            LOW: '#00aa00'
        };

        const riskIcons = {
            CRITICAL: '🚨',
            HIGH: '🔴',
            MEDIUM: '🟡',
            LOW: '🟢'
        };

        resultsContainer.innerHTML = `
            <div class="results-summary">
                <div class="risk-indicator ${this.state.riskLevel.toLowerCase()}" 
                     style="border-left: 4px solid ${riskColors[this.state.riskLevel]}">
                    <h4>
                        ${riskIcons[this.state.riskLevel]} 
                        Nivel de Riesgo: ${this.state.riskLevel}
                    </h4>
                    <p>Se encontraron ${this.state.vulnerabilities.length} elementos de seguridad</p>
                </div>
            </div>

            <div class="results-tabs">
                <button class="tab-btn active" onclick="showTab('vulnerabilities')">
                    🛡️ Vulnerabilidades (${this.state.vulnerabilities.length})
                </button>
                <button class="tab-btn" onclick="showTab('browser')">
                    🌐 Navegador
                </button>
                <button class="tab-btn" onclick="showTab('location')">
                    📍 Ubicación
                </button>
            </div>

            <div class="results-content">
                ${this.renderVulnerabilitiesTab()}
                ${this.renderBrowserTab()}
                ${this.renderLocationTab()}
            </div>
        `;

        // Animate in
        resultsContainer.style.maxHeight = resultsContainer.scrollHeight + 'px';
        resultsContainer.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
    }

    // Render vulnerabilities
    renderVulnerabilitiesTab() {
        const vulnItems = this.state.vulnerabilities.map(vuln => `
            <div class="vuln-item ${vuln.severity.toLowerCase()}">
                <div class="vuln-header">
                    <span class="vuln-icon">${vuln.icon}</span>
                    <span class="vuln-type">${vuln.type}</span>
                    <span class="vuln-severity ${vuln.severity.toLowerCase()}">${vuln.severity}</span>
                </div>
                <div class="vuln-description">${vuln.description}</div>
                <div class="vuln-recommendation">
                    <strong>💡 Recomendación:</strong> ${vuln.recommendation}
                </div>
            </div>
        `).join('');

        return `
            <div id="vulnerabilities-tab" class="tab-content active">
                <h4>🛡️ Análisis de Vulnerabilidades</h4>
                ${vulnItems || '<p class="no-data">No se encontraron vulnerabilidades críticas</p>'}
            </div>
        `;
    }

    // Render browser info
    renderBrowserTab() {
        const { browser, hardware, display, features } = this.state.browserData;
        
        return `
            <div id="browser-tab" class="tab-content">
                <h4>🌐 Información del Navegador</h4>
                <div class="info-grid">
                    <div class="info-section">
                        <h5>Navegador</h5>
                        <p><strong>Nombre:</strong> ${browser.name}</p>
                        <p><strong>Versión:</strong> ${browser.version}</p>
                        <p><strong>Plataforma:</strong> ${this.state.browserData.platform}</p>
                        <p><strong>Idioma:</strong> ${this.state.browserData.language}</p>
                    </div>
                    <div class="info-section">
                        <h5>Hardware</h5>
                        <p><strong>CPU Cores:</strong> ${hardware.cores}</p>
                        <p><strong>Memoria:</strong> ${hardware.memory} GB</p>
                        <p><strong>Pantalla:</strong> ${display.screen}</p>
                        <p><strong>Viewport:</strong> ${display.viewport}</p>
                    </div>
                    <div class="info-section">
                        <h5>Características</h5>
                        <p><strong>Cookies:</strong> ${features.cookies ? '✅ Habilitadas' : '❌ Deshabilitadas'}</p>
                        <p><strong>Online:</strong> ${features.online ? '✅ Conectado' : '❌ Desconectado'}</p>
                        <p><strong>Geolocalización:</strong> ${features.geolocation ? '⚠️ Disponible' : '✅ No disponible'}</p>
                        <p><strong>WebRTC:</strong> ${features.webrtc ? '⚠️ Activo' : '✅ Inactivo'}</p>
                        <p><strong>Do Not Track:</strong> ${features.doNotTrack ? '✅ Activado' : '⚠️ Desactivado'}</p>
                    </div>
                </div>
            </div>
        `;
    }

    // Render location info
    renderLocationTab() {
        const loc = this.state.locationData;
        
        return `
            <div id="location-tab" class="tab-content">
                <h4>📍 Información de Ubicación</h4>
                <div class="info-grid">
                    <div class="info-section">
                        <h5>IP Pública</h5>
                        <p><strong>Dirección IP:</strong> ${loc.publicIP}</p>
                        <p><strong>Proveedor:</strong> ${loc.isp}</p>
                    </div>
                    <div class="info-section">
                        <h5>Ubicación Aproximada</h5>
                        <p><strong>País:</strong> ${loc.country}</p>
                        <p><strong>Ciudad:</strong> ${loc.city}</p>
                        <p><em>⚠️ Esta información es visible para todos los sitios web</em></p>
                    </div>
                </div>
            </div>
        `;
    }

    // Show error message
    showError() {
        const resultsContainer = document.getElementById('scanner-results');
        resultsContainer.innerHTML = `
            <div class="error-message">
                <h4>❌ Error en el Análisis</h4>
                <p>Ocurrió un error durante el escaneo de seguridad. Inténtalo de nuevo.</p>
                <button onclick="location.reload()">Recargar Página</button>
            </div>
        `;
    }
}

// Global function for tab switching
window.showTab = (tabName) => {
    // Hide all tabs
    document.querySelectorAll('.tab-content').forEach(tab => {
        tab.classList.remove('active');
    });
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.classList.remove('active');
    });
    
    // Show selected tab
    document.getElementById(tabName + '-tab').classList.add('active');
    event.target.classList.add('active');
};

// Initialize when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    setTimeout(() => {
        window.securityScanner = new SecurityScanner();
    }, 1000);
}); 