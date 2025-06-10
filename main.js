/**
 * Ultra-Clean CV Application Controller
 * @description Hyper-optimized main application with modern ES6+ practices
 * @author Amelia Enora 🌈 Marceline Chavez Barroso
 * @version 2.0.0 - Ultra Performance Edition
 */

class CVApplication {
    // Static configuration
    static CONFIG = {
        TYPEWRITER_SPEED: 30,
        MATRIX_OPACITY: 0.03,
        MATRIX_INTERVAL: 120,
        ANIMATION_DURATION: 300,
        INTERSECTION_THRESHOLD: 0.3
    };

    constructor() {
        this.state = {
            isLoaded: false,
            currentSection: 'about',
            activeAnimations: new Set(),
            observers: new Map()
        };
        
        this.elements = {
            container: null,
            title: null,
            navLinks: null,
            sections: null
        };

        this.init();
    }

    // Main initialization with error handling
    async init() {
        try {
            console.log('🚀 Inicializando CV Application v2.0...');
            
            this.cacheElements();
            this.setupNavigationSystem();
            this.setupTerminalEffects();
            this.setupKeyboardShortcuts();
            this.setupPerformanceMonitoring();
            this.registerServiceWorker();
            
            await this.initializeModules();
            
            this.state.isLoaded = true;
            console.log('✅ CV Application listo!');
            
            this.displayWelcomeMessage();
        } catch (error) {
            console.error('❌ Error al inicializar aplicación:', error);
        }
    }

    // Register Service Worker for offline functionality
    async registerServiceWorker() {
        if ('serviceWorker' in navigator) {
            try {
                const registration = await navigator.serviceWorker.register('/service-worker.js');
                console.log('✅ Service Worker registrado:', registration.scope);
                
                // Check for updates
                registration.addEventListener('updatefound', () => {
                    const newWorker = registration.installing;
                    newWorker.addEventListener('statechange', () => {
                        if (newWorker.state === 'installed' && navigator.serviceWorker.controller) {
                            console.log('🔄 Nueva versión disponible, actualizando...');
                            newWorker.postMessage({ action: 'skipWaiting' });
                        }
                    });
                });
            } catch (error) {
                console.warn('⚠️ Error registrando Service Worker:', error);
            }
        }
    }

    // Cache DOM elements for performance
    cacheElements() {
        this.elements = {
            container: document.querySelector('.terminal-container'),
            title: document.querySelector('.terminal-title'),
            navLinks: document.querySelectorAll('.nav-link'),
            sections: document.querySelectorAll('.section')
        };
    }

    // Setup navigation with smooth scrolling and highlighting
    setupNavigationSystem() {
        this.setupSmoothScrolling();
        this.setupSectionObserver();
    }

    setupSmoothScrolling() {
        this.elements.navLinks.forEach(link => {
            link.addEventListener('click', this.handleNavClick.bind(this), { passive: false });
        });
    }

    handleNavClick(e) {
        e.preventDefault();
        
        const targetId = e.currentTarget.getAttribute('href').substring(1);
        const targetSection = document.getElementById(targetId);
        
        if (!targetSection) return;

        // Smooth scroll with modern API
        targetSection.scrollIntoView({
            behavior: 'smooth',
            block: 'start',
            inline: 'nearest'
        });

        this.updateActiveSection(targetId);
        this.addTerminalCommand(targetId);
    }

    setupSectionObserver() {
        const observerOptions = {
            rootMargin: '-10% 0px -10% 0px',
            threshold: CVApplication.CONFIG.INTERSECTION_THRESHOLD
        };

        const observer = new IntersectionObserver(
            this.handleSectionIntersection.bind(this),
            observerOptions
        );

        this.elements.sections.forEach(section => {
            observer.observe(section);
        });

        this.state.observers.set('sections', observer);
    }

    handleSectionIntersection(entries) {
        const visibleEntry = entries.find(entry => entry.isIntersecting);
        if (visibleEntry) {
            this.updateActiveSection(visibleEntry.target.id);
        }
    }

    updateActiveSection(sectionId) {
        // Update navigation highlighting
        this.elements.navLinks.forEach(link => {
            link.classList.toggle('active', 
                link.getAttribute('href') === `#${sectionId}`
            );
        });

        // Update terminal title
        if (this.elements.title) {
            this.elements.title.textContent = `amelia@systems:~/cv/${sectionId}$`;
        }

        this.state.currentSection = sectionId;
    }

    // Terminal visual effects
    setupTerminalEffects() {
        this.addMatrixBackground();
        this.addTerminalCursor();
        this.setupGlitchEffects();
    }

    addMatrixBackground() {
        const canvas = this.createCanvas();
        const ctx = canvas.getContext('2d');
        
        const matrixConfig = {
            chars: '01アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン'.split(''),
            fontSize: 10,
            drops: []
        };

        const resizeCanvas = () => {
            canvas.width = window.innerWidth;
            canvas.height = window.innerHeight;
            
            const columns = Math.floor(canvas.width / matrixConfig.fontSize);
            matrixConfig.drops = Array.from({ length: columns }, 
                () => Math.random() * canvas.height
            );
        };

        const drawMatrix = () => {
            ctx.fillStyle = 'rgba(0, 0, 0, 0.05)';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            
            ctx.font = `${matrixConfig.fontSize}px JetBrains Mono, monospace`;
            
            matrixConfig.drops.forEach((drop, i) => {
                const char = matrixConfig.chars[Math.floor(Math.random() * matrixConfig.chars.length)];
                const x = i * matrixConfig.fontSize;
                const y = drop * matrixConfig.fontSize;
                
                // Enhanced glow effect with multiple layers
                // Outer glow
                ctx.shadowBlur = 30;
                ctx.shadowColor = '#00ff00';
                ctx.fillStyle = 'rgba(0, 255, 0, 0.3)';
                ctx.fillText(char, x, y);
                
                // Middle glow
                ctx.shadowBlur = 15;
                ctx.fillStyle = 'rgba(0, 255, 0, 0.6)';
                ctx.fillText(char, x, y);
                
                // Inner bright core
                ctx.shadowBlur = 5;
                ctx.fillStyle = '#00ff00';
                ctx.fillText(char, x, y);
                
                // Reset shadow for performance
                ctx.shadowBlur = 0;
                
                if (drop * matrixConfig.fontSize > canvas.height && Math.random() > 0.975) {
                    matrixConfig.drops[i] = 0;
                } else {
                    matrixConfig.drops[i]++;
                }
            });
        };

        // Optimize with requestAnimationFrame
        let lastTime = 0;
        const animate = (currentTime) => {
            if (currentTime - lastTime >= CVApplication.CONFIG.MATRIX_INTERVAL) {
                drawMatrix();
                lastTime = currentTime;
            }
            requestAnimationFrame(animate);
        };

        resizeCanvas();
        window.addEventListener('resize', resizeCanvas, { passive: true });
        requestAnimationFrame(animate);
    }

    createCanvas() {
        const canvas = document.createElement('canvas');
        Object.assign(canvas.style, {
            position: 'fixed',
            top: '0',
            left: '0',
            width: '100%',
            height: '100%',
            zIndex: '-1',
            opacity: CVApplication.CONFIG.MATRIX_OPACITY,
            pointerEvents: 'none'
        });
        
        document.body.appendChild(canvas);
        return canvas;
    }

    addTerminalCursor() {
        if (document.getElementById('cursor-styles')) return;

        const style = document.createElement('style');
        style.id = 'cursor-styles';
        style.textContent = `
            .terminal-cursor {
                display: inline-block;
                background: var(--text-primary);
                width: 8px;
                height: 16px;
                margin-left: 2px;
                animation: cursorBlink 1s infinite;
            }
            
            @keyframes cursorBlink {
                0%, 50% { opacity: 1; }
                51%, 100% { opacity: 0; }
            }
            
            .glitch-effect:hover {
                animation: glitchEffect 0.3s ease-in-out;
            }
            
            @keyframes glitchEffect {
                0%, 100% { transform: translateX(0); }
                20% { transform: translateX(-2px); }
                40% { transform: translateX(2px); }
                60% { transform: translateX(-1px); }
                80% { transform: translateX(1px); }
            }
        `;
        
        document.head.appendChild(style);
    }

    setupGlitchEffects() {
        // Add glitch effect to special elements
        document.querySelectorAll('.nav-link, .cert-name, .skill-level').forEach(element => {
            element.classList.add('glitch-effect');
        });
    }

    // Keyboard shortcuts for navigation
    setupKeyboardShortcuts() {
        const shortcuts = {
            'KeyA': 'about',
            'KeyS': 'skills', 
            'KeyV': 'visualizations',
            'KeyP': 'projects',
            'KeyN': 'network',
            'KeyC': 'certifications'
        };

        document.addEventListener('keydown', (e) => {
            if (e.ctrlKey && shortcuts[e.code]) {
                e.preventDefault();
                const section = document.getElementById(shortcuts[e.code]);
                if (section) {
                    section.scrollIntoView({ behavior: 'smooth' });
                }
            }
        }, { passive: false });
    }

    // Performance monitoring
    setupPerformanceMonitoring() {
        if ('PerformanceObserver' in window) {
            const observer = new PerformanceObserver((list) => {
                list.getEntries().forEach(entry => {
                    if (entry.entryType === 'navigation') {
                        console.log(`⚡ Page Load: ${entry.loadEventEnd - entry.loadEventStart}ms`);
                    }
                });
            });
            
            observer.observe({ entryTypes: ['navigation'] });
        }
    }

    // Add terminal command simulation
    addTerminalCommand(section) {
        const command = `cd ~/cv/${section} && ls -la`;
        console.log(`%c$ ${command}`, 'color: #00ff00; font-family: JetBrains Mono;');
    }

    // Module initialization
    async initializeModules() {
        const modules = [
            () => window.algorithmRunner?.init(),
            () => window.mathVisualizer?.init(),
            () => window.networkTools?.init(),
            () => window.i18nManager?.init()
        ];

        const results = await Promise.allSettled(
            modules.map(module => {
                try {
                    return module?.() || Promise.resolve();
                } catch (error) {
                    console.warn('Module init error:', error);
                    return Promise.resolve();
                }
            })
        );

        const successful = results.filter(r => r.status === 'fulfilled').length;
        console.log(`📦 Módulos inicializados: ${successful}/${modules.length}`);
    }

    // Welcome message with style
    displayWelcomeMessage() {
        const styles = {
            title: 'color: #00ff00; font-size: 20px; font-weight: bold; text-shadow: 0 0 10px #00ff00;',
            info: 'color: #00aaff; font-size: 12px;',
            warning: 'color: #ffaa00; font-size: 11px;',
            success: 'color: #00ff00; font-size: 11px;'
        };

        console.clear();
        console.log('%c🌈 AMELIA ENORA CV SYSTEM v2.0', styles.title);
        console.log('%c━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━', styles.info);
        console.log('%c📧 enorastrokes@gmail.com', styles.info);
        console.log('%c🐙 github.com/AmeliaDi', styles.info);
        console.log('%c💼 linkedin.com/in/bogosort', styles.info);
        console.log('%c━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━', styles.info);
        console.log('%c⚡ Sistema optimizado y listo', styles.success);
        console.log('%c🎮 Usa Ctrl+A/S/V/P/N/C para navegación rápida', styles.warning);
        console.log('%c🔍 Panel de seguridad activo en el lado derecho', styles.warning);
    }

    // Cleanup method
    destroy() {
        this.state.observers.forEach(observer => observer.disconnect());
        this.state.activeAnimations.forEach(animation => animation.cancel?.());
        console.log('🧹 CV Application limpiado');
    }
}



/**
 * Performance Monitor
 */
class PerformanceMonitor {
    constructor() {
        this.metrics = {
            loadTime: 0,
            interactions: 0,
            errors: 0
        };
        this.startTime = performance.now();
    }

    recordInteraction() {
        this.metrics.interactions++;
    }

    recordError(error) {
        this.metrics.errors++;
        console.warn('📊 Error registrado:', error);
    }

    getReport() {
        const loadTime = performance.now() - this.startTime;
        return {
            ...this.metrics,
            loadTime: Math.round(loadTime),
            uptime: Math.round(performance.now())
        };
    }
}

// Global instances
window.cvApp = null;
window.performanceMonitor = null;

// Initialize when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    window.performanceMonitor = new PerformanceMonitor();
    
    // Wait a bit for other modules
    setTimeout(() => {
        window.cvApp = new CVApplication();
    }, 100);
});

// Cleanup on page unload
window.addEventListener('beforeunload', () => {
    window.cvApp?.destroy();
});

 