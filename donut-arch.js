/**
 * ASCII Morphing Banner - Ultra Wide Responsive Edition
 * Optimized & Refactored for Perfect Mobile & Desktop Experience
 * @author Amelia Enora 🌈 Marceline Chavez Barroso
 */

class ASCIIMorphBanner {
    constructor() {
        this.config = {
            frameDelay: 120,
            morphSpeed: 25,
            holdTime: 80,
            colors: ['#ff00ff', '#ff1493', '#ff69b4', '#ff00aa', '#ff44cc', '#ff77dd']
        };
        
        this.state = {
            running: true,
            currentFrame: 0,
            frameCounter: 0,
            currentState: 0,
            targetState: 1
        };
        
        this.elements = {
            container: null,
            output: null
        };
        
        // Ultra wide ASCII states for full responsive experience
        this.artStates = [
            // State 0: Wide Terminal
            [
                "┌─────────────────────────────────────────────────────────────┐",
                "│ amelia@archlinux:~$ █                                       │",
                "│ > Initializing systems programming environment...           │",
                "│ > Loading low-level modules and assembly toolchain...      │",
                "└─────────────────────────────────────────────────────────────┘"
            ],
            // State 1: Code Matrix
            [
                "╔═══════════════════════════════════════════════════════════╗",
                "║  { function() }  <assembly>  { malloc() }  <kernel>      ║",
                "║    [memory]       [cpu]        [network]    [security]   ║",
                "║  { pointer++ }   <syscall>   { buffer() }  <exploit>     ║",
                "╚═══════════════════════════════════════════════════════════╝"
            ],
            // State 2: Network Grid
            [
                "    ◉═══════════◉═══════════◉═══════════◉═══════════◉    ",
                "   ╱│    SSH    │    HTTP   │    TCP    │    UDP    │╲   ",
                "  ◉ │ 192.168   │   :443    │   :22     │   :53     │ ◉  ",
                "   ╲│   .1.1    │  SECURE   │  SHELL    │   DNS     │╱   ",
                "    ◉═══════════◉═══════════◉═══════════◉═══════════◉    "
            ],
            // State 3: Binary Flow
            [
                " ░░▒▒▓▓██▓▓▒▒░░▓▓██▓▓░░▒▒▓▓██▓▓▒▒░░▓▓██▓▓░░▒▒▓▓██▓▓▒▒░░ ",
                " ▓ 1010 1100 0110 1001 1111 0000 1010 1100 0110 1001 ▓ ",
                " █ 0110 1001 1111 0000 1010 1100 0110 1001 1111 0000 █ ",
                " ▓ 1111 0000 1010 1100 0110 1001 1111 0000 1010 1100 ▓ ",
                " ░░▒▒▓▓██▓▓▒▒░░▓▓██▓▓░░▒▒▓▓██▓▓▒▒░░▓▓██▓▓░░▒▒▓▓██▓▓▒▒░░ "
            ],
            // State 4: CPU Architecture
            [
                "╔═══════════════════════════════════════════════════════════╗",
                "║ ▄▄▄▄▄▄▄▄▄▄▄ CORE-0 ▄▄▄▄▄▄▄▄▄▄▄ CORE-1 ▄▄▄▄▄▄▄▄▄▄▄ ║",
                "║ █ Intel i5-13420H │ 4.6GHz │ x86-64 │ 12-Threads █ ║",
                "║ ▀▀▀▀▀▀▀▀▀▀▀ CACHE ▀▀▀▀▀▀▀▀▀▀▀ L3-12M ▀▀▀▀▀▀▀▀▀▀▀ ║",
                "╚═══════════════════════════════════════════════════════════╝"
            ],
            // State 5: Git Workflow
            [
                "    ┌─○─┐     ┌─○─┐     ┌─○─┐     ┌─○─┐     ┌─○─┐      ",
                "  ┌─┴─○─┴─┐ ┌─┴─○─┴─┐ ┌─┴─○─┴─┐ ┌─┴─○─┴─┐ ┌─┴─○─┴─┐    ",
                "  │ main  │ │ dev   │ │ feat  │ │ test  │ │ prod  │    ",
                "  │ push  │ │ merge │ │ rebase│ │ ci/cd │ │ deploy│    ",
                "  └───○───┘ └───○───┘ └───○───┘ └───○───┘ └───○───┘    "
            ]
        ];
    }

    init() {
        this.createBannerDOM();
        this.injectOptimizedStyles();
        this.startAnimation();
    }

    createBannerDOM() {
        const body = document.body;
        
        this.elements.container = document.createElement('div');
        this.elements.container.className = 'ascii-banner-container';
        this.elements.container.innerHTML = `
            <div class="ascii-banner-wrapper">
                <pre id="ascii-output" class="ascii-art"></pre>
                <div class="ascii-label">Amelia Enora • Systems & Assembly Developer</div>
            </div>
        `;
        
        body.insertBefore(this.elements.container, body.firstChild);
        this.elements.output = document.getElementById('ascii-output');
    }

    injectOptimizedStyles() {
        const style = document.createElement('style');
        style.textContent = `
            .ascii-banner-container {
                width: 100%;
                min-height: 180px;
                display: flex;
                justify-content: center;
                align-items: center;
                padding: 2rem 1rem;
                background: linear-gradient(135deg, 
                    rgba(0, 0, 0, 0.98) 0%,
                    rgba(20, 0, 20, 0.95) 25%,
                    rgba(30, 0, 30, 0.9) 50%,
                    rgba(20, 0, 20, 0.95) 75%,
                    rgba(0, 0, 0, 0.98) 100%
                );
                border-bottom: 3px solid #ff00ff;
                position: relative;
                overflow: hidden;
                margin-bottom: 2rem;
            }
            
            .ascii-banner-container::before {
                content: '';
                position: absolute;
                top: 0;
                left: -100%;
                width: 100%;
                height: 100%;
                background: linear-gradient(90deg, 
                    transparent, 
                    rgba(255, 0, 255, 0.15), 
                    rgba(255, 20, 147, 0.1),
                    rgba(255, 0, 255, 0.15),
                    transparent
                );
                animation: ultraWideSweep 12s linear infinite;
            }
            
            @keyframes ultraWideSweep {
                0% { left: -100%; }
                100% { left: 100%; }
            }
            
            .ascii-banner-wrapper {
                position: relative;
                display: flex;
                flex-direction: column;
                align-items: center;
                padding: 1.5rem 2rem;
                background: linear-gradient(135deg, 
                    rgba(0, 0, 0, 0.9) 0%,
                    rgba(40, 0, 40, 0.7) 50%,
                    rgba(0, 0, 0, 0.9) 100%
                );
                border: 3px solid #ff00ff;
                border-radius: 15px;
                backdrop-filter: blur(15px);
                box-shadow: 
                    0 0 30px rgba(255, 0, 255, 0.5),
                    0 0 60px rgba(255, 0, 255, 0.3),
                    0 0 90px rgba(255, 0, 255, 0.1),
                    inset 0 3px 0 rgba(255, 0, 255, 0.3);
                width: 100%;
                max-width: 1200px;
                animation: bannerPulse 4s ease-in-out infinite alternate;
                transition: all 0.3s ease;
            }
            
            @keyframes bannerPulse {
                0% { 
                    transform: scale(1);
                    box-shadow: 
                        0 0 30px rgba(255, 0, 255, 0.5),
                        0 0 60px rgba(255, 0, 255, 0.3);
                }
                100% { 
                    transform: scale(1.01);
                    box-shadow: 
                        0 0 40px rgba(255, 0, 255, 0.7),
                        0 0 80px rgba(255, 0, 255, 0.4),
                        0 0 120px rgba(255, 0, 255, 0.2);
                }
            }
            
            .ascii-art {
                font-family: 'JetBrains Mono', 'Courier New', monospace;
                font-size: 0.75rem;
                line-height: 1.1;
                margin: 0;
                padding: 1rem;
                text-align: center;
                white-space: pre;
                overflow: hidden;
                min-height: 6rem;
                display: flex;
                align-items: center;
                justify-content: center;
                width: 100%;
                max-width: 100%;
                font-weight: 500;
            }
            
            .ascii-label {
                font-family: 'JetBrains Mono', monospace;
                font-size: 1rem;
                color: #ff00ff;
                text-transform: uppercase;
                letter-spacing: 2px;
                margin-top: 1rem;
                font-weight: 700;
                text-shadow: 
                    0 0 15px rgba(255, 0, 255, 0.9),
                    0 0 30px rgba(255, 0, 255, 0.5);
                animation: labelGlow 3s ease-in-out infinite alternate;
            }
            
            @keyframes labelGlow {
                0% { 
                    opacity: 0.9;
                    text-shadow: 
                        0 0 15px rgba(255, 0, 255, 0.9),
                        0 0 30px rgba(255, 0, 255, 0.5);
                }
                100% { 
                    opacity: 1;
                    text-shadow: 
                        0 0 25px rgba(255, 0, 255, 1),
                        0 0 50px rgba(255, 0, 255, 0.7);
                }
            }
            
            /* Ultra Responsive Design */
            @media (max-width: 1200px) {
                .ascii-banner-wrapper {
                    max-width: 95%;
                    padding: 1.2rem 1.5rem;
                }
                .ascii-art {
                    font-size: 0.7rem;
                }
            }
            
            @media (max-width: 768px) {
                .ascii-banner-container {
                    padding: 1.5rem 0.5rem;
                    min-height: 140px;
                }
                .ascii-banner-wrapper {
                    padding: 1rem;
                    border-radius: 12px;
                }
                .ascii-art {
                    font-size: 0.5rem;
                    line-height: 1;
                    min-height: 4rem;
                    padding: 0.8rem;
                }
                .ascii-label {
                    font-size: 0.8rem;
                    letter-spacing: 1px;
                    margin-top: 0.8rem;
                }
            }
            
            @media (max-width: 480px) {
                .ascii-banner-container {
                    padding: 1rem 0.25rem;
                    min-height: 120px;
                }
                .ascii-art {
                    font-size: 0.4rem;
                    min-height: 3rem;
                    padding: 0.5rem;
                }
                .ascii-label {
                    font-size: 0.7rem;
                    letter-spacing: 0.5px;
                }
            }
            
            @media (min-width: 1400px) {
                .ascii-banner-wrapper {
                    max-width: 1300px;
                }
                .ascii-art {
                    font-size: 0.85rem;
                    line-height: 1.2;
                }
                .ascii-label {
                    font-size: 1.1rem;
                    letter-spacing: 3px;
                }
            }
        `;
        document.head.appendChild(style);
    }

    interpolateChar(char1, char2, progress) {
        if (progress < 0.5) return char1;
        return char2;
    }

    morphBetween(state1, state2, progress) {
        const result = [];
        const maxLines = Math.max(state1.length, state2.length);
        
        for (let i = 0; i < maxLines; i++) {
            const line1 = state1[i] || '';
            const line2 = state2[i] || '';
            const maxChars = Math.max(line1.length, line2.length);
            let morphedLine = '';
            
            for (let j = 0; j < maxChars; j++) {
                const char1 = line1[j] || ' ';
                const char2 = line2[j] || ' ';
                morphedLine += this.interpolateChar(char1, char2, progress);
            }
            result.push(morphedLine);
        }
        return result;
    }

    getCurrentFrame() {
        const { currentState, targetState, frameCounter, morphSpeed, holdTime } = this.state;
        
        if (frameCounter < holdTime) {
            return this.artStates[currentState];
        }
        
        const morphProgress = (frameCounter - holdTime) / morphSpeed;
        
        if (morphProgress >= 1) {
            this.state.currentState = targetState;
            this.state.targetState = (targetState + 1) % this.artStates.length;
            this.state.frameCounter = 0;
            return this.artStates[targetState];
        }
        
        return this.morphBetween(
            this.artStates[currentState], 
            this.artStates[targetState], 
            morphProgress
        );
    }

    render() {
        if (!this.elements.output) return;
        
        const frame = this.getCurrentFrame();
        const colorIndex = this.state.currentState % this.config.colors.length;
        const color = this.config.colors[colorIndex];
        
        this.elements.output.textContent = frame.join('\n');
        this.elements.output.style.color = color;
        this.elements.output.style.textShadow = `
            0 0 10px ${color},
            0 0 20px ${color}80,
            0 0 30px ${color}40
        `;
    }

    animate() {
        if (!this.state.running) return;
        
        this.render();
        this.state.frameCounter++;
        
        setTimeout(() => {
            requestAnimationFrame(() => this.animate());
        }, this.config.frameDelay);
    }

    startAnimation() {
        this.animate();
    }

    stop() {
        this.state.running = false;
    }
}

// Auto-initialize when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', () => {
        new ASCIIMorphBanner().init();
    });
} else {
    new ASCIIMorphBanner().init();
} 