// Main application controller and terminal effects
class CVApplication {
    constructor() {
        this.terminalContainer = null;
        this.currentSection = 'about';
        this.isLoaded = false;
        this.typewriterSpeed = 30;
    }

    async init() {
        console.log('🚀 Initializing CV Application...');
        
        this.terminalContainer = document.querySelector('.terminal-container');
        this.setupSmoothScrolling();
        this.setupNavigationHighlighting();
        this.setupTerminalEffects();
        this.setupKeyboardShortcuts();
        
        // Wait for all modules to load
        await this.waitForModulesLoad();
        
        // Add loading animation
        this.addTerminalStartupAnimation();
        
        this.isLoaded = true;
        console.log('✅ CV Application ready!');
        
        // Display welcome message in console
        this.displayWelcomeMessage();
    }

    setupSmoothScrolling() {
        // Smooth scrolling for navigation links
        document.querySelectorAll('.nav-link').forEach(link => {
            link.addEventListener('click', (e) => {
                e.preventDefault();
                const targetId = link.getAttribute('href').substring(1);
                const targetSection = document.getElementById(targetId);
                
                if (targetSection) {
                    targetSection.scrollIntoView({
                        behavior: 'smooth',
                        block: 'start'
                    });
                    
                    this.updateCurrentSection(targetId);
                    this.addTerminalCommand(targetId);
                }
            });
        });
    }

    setupNavigationHighlighting() {
        // Intersection Observer for section highlighting
        const options = {
            rootMargin: '-50px 0px -50px 0px',
            threshold: 0.3
        };

        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    this.updateCurrentSection(entry.target.id);
                }
            });
        }, options);

        // Observe all sections
        document.querySelectorAll('.section').forEach(section => {
            observer.observe(section);
        });
    }

    updateCurrentSection(sectionId) {
        // Update active navigation link
        document.querySelectorAll('.nav-link').forEach(link => {
            link.classList.remove('active');
        });
        
        const activeLink = document.querySelector(`a[href="#${sectionId}"]`);
        if (activeLink) {
            activeLink.classList.add('active');
        }
        
        this.currentSection = sectionId;
        
        // Update terminal title
        const terminalTitle = document.querySelector('.terminal-title');
        if (terminalTitle) {
            terminalTitle.textContent = `marcy@systems:~/cv/${sectionId}$`;
        }
    }

    setupTerminalEffects() {
        // Add cursor blink effect
        this.addCursorBlink();
        
        // Add matrix rain effect (subtle)
        this.addMatrixEffect();
        
        // Add glitch effect on hover for special elements
        this.setupGlitchEffects();
    }

    addCursorBlink() {
        const style = document.createElement('style');
        style.textContent = `
            .terminal-cursor {
                display: inline-block;
                background-color: var(--text-primary);
                animation: blink 1s infinite;
                width: 8px;
                height: 16px;
                margin-left: 2px;
            }
            
            @keyframes blink {
                0%, 50% { opacity: 1; }
                51%, 100% { opacity: 0; }
            }
        `;
        document.head.appendChild(style);
    }

    addMatrixEffect() {
        // Subtle matrix rain in the background
        const canvas = document.createElement('canvas');
        canvas.style.position = 'fixed';
        canvas.style.top = '0';
        canvas.style.left = '0';
        canvas.style.width = '100%';
        canvas.style.height = '100%';
        canvas.style.zIndex = '-1';
        canvas.style.opacity = '0.05';
        canvas.style.pointerEvents = 'none';
        
        document.body.appendChild(canvas);
        
        const ctx = canvas.getContext('2d');
        
        const resizeCanvas = () => {
            canvas.width = window.innerWidth;
            canvas.height = window.innerHeight;
        };
        
        resizeCanvas();
        window.addEventListener('resize', resizeCanvas);
        
        const chars = '01アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン';
        const charArray = chars.split('');
        
        const fontSize = 10;
        const columns = Math.floor(canvas.width / fontSize);
        const drops = [];
        
        for (let i = 0; i < columns; i++) {
            drops[i] = Math.random() * canvas.height;
        }
        
        const draw = () => {
            ctx.fillStyle = 'rgba(0, 0, 0, 0.05)';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            
            ctx.fillStyle = '#00ff00';
            ctx.font = `${fontSize}px monospace`;
            
            for (let i = 0; i < drops.length; i++) {
                const text = charArray[Math.floor(Math.random() * charArray.length)];
                ctx.fillText(text, i * fontSize, drops[i] * fontSize);
                
                if (drops[i] * fontSize > canvas.height && Math.random() > 0.975) {
                    drops[i] = 0;
                }
                drops[i]++;
            }
        };
        
        setInterval(draw, 100);
    }

    setupGlitchEffects() {
        const style = document.createElement('style');
        style.textContent = `
            .glitch {
                position: relative;
                overflow: hidden;
            }
            
            .glitch:hover::before,
            .glitch:hover::after {
                content: attr(data-text);
                position: absolute;
                top: 0;
                left: 0;
                width: 100%;
                height: 100%;
                background: var(--bg-terminal);
            }
            
            .glitch:hover::before {
                animation: glitch-1 0.5s infinite;
                color: #ff0000;
                z-index: -1;
            }
            
            .glitch:hover::after {
                animation: glitch-2 0.5s infinite;
                color: #00ffff;
                z-index: -2;
            }
            
            @keyframes glitch-1 {
                0%, 14%, 15%, 49%, 50%, 99%, 100% {
                    transform: translateX(0);
                }
                1%, 13% {
                    transform: translateX(-2px);
                }
                16%, 48% {
                    transform: translateX(2px);
                }
            }
            
            @keyframes glitch-2 {
                0%, 20%, 21%, 62%, 63%, 99%, 100% {
                    transform: translateY(0);
                }
                2%, 19% {
                    transform: translateY(-1px);
                }
                22%, 61% {
                    transform: translateY(1px);
                }
            }
        `;
        document.head.appendChild(style);
        
        // Add glitch effect to file names
        document.querySelectorAll('.file-name').forEach(element => {
            element.classList.add('glitch');
            element.setAttribute('data-text', element.textContent);
        });
    }

    setupKeyboardShortcuts() {
        document.addEventListener('keydown', (e) => {
            // Ctrl + number keys for quick navigation
            if (e.ctrlKey && e.key >= '1' && e.key <= '6') {
                e.preventDefault();
                const sections = ['about', 'skills', 'algorithms', 'visualizations', 'projects', 'network'];
                const sectionIndex = parseInt(e.key) - 1;
                
                if (sections[sectionIndex]) {
                    const section = document.getElementById(sections[sectionIndex]);
                    if (section) {
                        section.scrollIntoView({ behavior: 'smooth' });
                        this.addTerminalCommand(sections[sectionIndex]);
                    }
                }
            }
            
            // F12 for developer console
            if (e.key === 'F12') {
                e.preventDefault();
                this.showDeveloperConsole();
            }
            
            // Escape to clear terminal animations
            if (e.key === 'Escape') {
                this.clearTerminalAnimations();
            }
        });
    }

    addTerminalCommand(section) {
        // Simulate terminal command execution
        const commands = {
            'about': './about.sh',
            'skills': 'cat skills.asm',
            'algorithms': 'gcc -O3 algorithms.c -o algorithms.wasm',
            'visualizations': './math_viz.wasm --interactive',
            'projects': 'ls -la projects/',
            'network': 'python3 network_tools.py --demo'
        };
        
        console.log(`🔧 Executing: ${commands[section] || section}`);
    }

    async addTerminalStartupAnimation() {
        const startupMessages = [
            '🖥️  Booting CV Terminal...',
            '⚡ Loading WebAssembly modules...',
            '🔧 Initializing algorithm visualizers...',
            '🌐 Starting network tools...',
            '🎨 Rendering mathematical visualizations...',
            '✅ System ready. Welcome to Marcy\'s technical portfolio!'
        ];
        
        for (const message of startupMessages) {
            console.log(message);
            await this.sleep(300);
        }
    }

    async waitForModulesLoad() {
        // Wait for WebAssembly module
        let attempts = 0;
        while (!window.wasmLoader?.isLoaded && attempts < 50) {
            await this.sleep(100);
            attempts++;
        }
        
        // Wait for other modules
        const modules = ['algorithmVisualizer', 'mathVisualizer', 'networkTools'];
        for (const module of modules) {
            attempts = 0;
            while (!window[module] && attempts < 50) {
                await this.sleep(100);
                attempts++;
            }
        }
    }

    showDeveloperConsole() {
        const consoleContent = `
╔══════════════════════════════════════════════════════════════════════════════════════╗
║                                  DEVELOPER CONSOLE                                   ║
╠══════════════════════════════════════════════════════════════════════════════════════╣
║ Available Commands:                                                                  ║
║                                                                                      ║
║ 🔧 wasmLoader.callFunction(name, ...args)    - Call WebAssembly functions           ║
║ 📊 performanceBenchmark.benchmarkSortingAlgorithms() - Run performance tests        ║
║ 🔢 MathUtils.factorial(n)                    - Calculate factorial                   ║
║ 🌐 NetworkAnalyzer.calculateSubnet(ip, cidr) - Network subnet calculation           ║
║ 🎨 mathVisualizer.renderMandelbrot()         - Render Mandelbrot set               ║
║                                                                                      ║
║ Keyboard Shortcuts:                                                                  ║
║ • Ctrl+1-6: Quick navigation to sections                                            ║
║ • ESC: Clear terminal animations                                                    ║
║ • F12: Show this console                                                            ║
║                                                                                      ║
║ System Status:                                                                       ║
║ • WebAssembly: ${window.wasmLoader?.isLoaded ? '✅ Loaded' : '❌ Loading...'}                                              ║
║ • Algorithms: ${window.algorithmVisualizer ? '✅ Ready' : '❌ Loading...'}                                               ║
║ • Math Viz:   ${window.mathVisualizer ? '✅ Ready' : '❌ Loading...'}                                               ║
║ • Network:    ${window.networkTools ? '✅ Ready' : '❌ Loading...'}                                               ║
╚══════════════════════════════════════════════════════════════════════════════════════╝
        `;
        
        console.log(consoleContent);
    }

    clearTerminalAnimations() {
        // Stop any running animations
        document.querySelectorAll('.loading').forEach(el => {
            el.classList.remove('loading');
        });
        
        console.log('🧹 Terminal animations cleared');
    }

    displayWelcomeMessage() {
                 console.log(`
⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣤⣴⣶⣶⣿⣿⣿⣿⣿⣿⣿⣶⣶⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⢀⣤⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣤⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⠀⠀⠀⠀⠀
⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀
⠀⠀⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀
⠀⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⠀⠀
⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀
⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀
⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀
⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀
⠀⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠛⠛⠛⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀

╔══════════════════════════════════════════════════════════════════════════════════════╗
║                        WELCOME TO AMELIADI'S TECHNICAL CV                           ║
╠══════════════════════════════════════════════════════════════════════════════════════╣
║                                                                                      ║
║  🚀 Systems Developer & Low-Level Programming Specialist                            ║
║  💻 Expert in C, Assembly (x86/x64/ARM), WebAssembly                               ║
║  🌐 Network Analysis & Protocol Implementation                                       ║
║  🔧 Linux Systems Programming & Kernel Development                                   ║
║  🧮 Mathematical Algorithms & Performance Optimization                              ║
║                                                                                      ║
║  This interactive CV showcases my technical skills through:                          ║
║  • Live algorithm visualizations compiled to WebAssembly                            ║
║  • Real-time mathematical computations and fractals                                 ║
║  • Network analysis tools and packet capture simulation                             ║
║  • Assembly syntax highlighting and low-level code examples                         ║
║                                                                                      ║
║  Navigate using the terminal-style interface above or use keyboard shortcuts!       ║
║                                                                                      ║
╚══════════════════════════════════════════════════════════════════════════════════════╝

🎯 Ready to explore my technical portfolio!
💡 Press F12 for developer console and available commands.
        `);
    }

    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
}

// Performance monitoring
class PerformanceMonitor {
    constructor() {
        this.startTime = performance.now();
        this.metrics = {
            loadTime: 0,
            interactionCount: 0,
            errorCount: 0
        };
    }

    recordInteraction() {
        this.metrics.interactionCount++;
    }

    recordError(error) {
        this.metrics.errorCount++;
        console.error('🚨 Application Error:', error);
    }

    getMetrics() {
        this.metrics.loadTime = performance.now() - this.startTime;
        return this.metrics;
    }
}

// Global error handler
window.addEventListener('error', (event) => {
    if (window.performanceMonitor) {
        window.performanceMonitor.recordError(event.error);
    }
});

// Initialize application when DOM is loaded
document.addEventListener('DOMContentLoaded', async () => {
    try {
        window.performanceMonitor = new PerformanceMonitor();
        window.cvApp = new CVApplication();
        await window.cvApp.init();
        
        console.log('📊 Performance metrics:', window.performanceMonitor.getMetrics());
    } catch (error) {
        console.error('❌ Failed to initialize CV application:', error);
    }
});

// Add interaction tracking
document.addEventListener('click', () => {
    if (window.performanceMonitor) {
        window.performanceMonitor.recordInteraction();
    }
}); 