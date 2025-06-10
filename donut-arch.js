/**
 * ASCII Donut Animation - Neon Green Edition
 * Based on donut.c by a1k0n with neon green theme
 * @author Amelia Enora 🌈 Marceline Chavez Barroso
 */

class ArchDonut {
    constructor() {
        this.A = 0;
        this.B = 0;
        this.screenWidth = 24;  // Much smaller for better integration
        this.screenHeight = 12;  // Compact size
        this.R1 = 1;
        this.R2 = 1.8;
        this.K2 = 4;
        this.K1 = this.screenWidth * this.K2 * 3 / (8 * (this.R1 + this.R2));
        
        this.output = null;
        this.container = null;
        this.running = true;
        this.frameDelay = 80; // Smooth animation
        
        // Neon green themed characters
        this.chars = '.,-~:;=!*#$@';
        
        // Neon green color palette
        this.neonColors = [
            '#00ff00',    // Bright neon green
            '#00ff41',    // Light neon green
            '#39ff14',    // Electric green
            '#00ff88',    // Cyan-green mix
            '#00cc00'     // Darker neon green
        ];
    }

    init() {
        this.createDonutElement();
        this.animate();
    }

    createDonutElement() {
        // Create donut container in header area
        const headerControls = document.querySelector('.header-controls');
        if (headerControls) {
            this.container = document.createElement('div');
            this.container.className = 'arch-donut-container';
            this.container.innerHTML = `
                <div class="arch-donut-wrapper">
                    <pre id="arch-donut-output" class="arch-donut-ascii"></pre>
                    <div class="arch-donut-label">Terminal</div>
                </div>
            `;
            
            // Insert before language selector
            headerControls.insertBefore(this.container, headerControls.firstChild);
            this.output = document.getElementById('arch-donut-output');
        }
    }

    getGlowIntensity(luminance) {
        // Create different glow intensities based on luminance
        const intensity = Math.max(0.3, luminance);
        return intensity;
    }

    renderFrame() {
        const cosA = Math.cos(this.A);
        const sinA = Math.sin(this.A);
        const cosB = Math.cos(this.B);
        const sinB = Math.sin(this.B);

        const output = new Array(this.screenHeight).fill(null).map(() => 
            new Array(this.screenWidth).fill(' ')
        );
        const zbuffer = new Array(this.screenHeight).fill(null).map(() => 
            new Array(this.screenWidth).fill(0)
        );
        const luminanceBuffer = new Array(this.screenHeight).fill(null).map(() => 
            new Array(this.screenWidth).fill(0)
        );

        for (let theta = 0; theta < 2 * Math.PI; theta += 0.07) {
            const costheta = Math.cos(theta);
            const sintheta = Math.sin(theta);

            for (let phi = 0; phi < 2 * Math.PI; phi += 0.02) {
                const cosphi = Math.cos(phi);
                const sinphi = Math.sin(phi);

                const circlex = this.R2 + this.R1 * costheta;
                const circley = this.R1 * sintheta;

                const x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
                const y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
                const z = this.K2 + cosA * circlex * sinphi + circley * sinA;
                const ooz = 1 / z;

                const xp = Math.floor(this.screenWidth / 2 + this.K1 * ooz * x);
                const yp = Math.floor(this.screenHeight / 2 - this.K1 * ooz * y);

                const L = cosphi * costheta * sinB - cosA * costheta * sinphi - 
                         sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);

                if (L > 0 && xp >= 0 && xp < this.screenWidth && yp >= 0 && yp < this.screenHeight) {
                    if (ooz > zbuffer[yp][xp]) {
                        zbuffer[yp][xp] = ooz;
                        luminanceBuffer[yp][xp] = L;
                        const luminanceIndex = Math.floor(L * 8);
                        output[yp][xp] = this.chars[Math.max(0, Math.min(11, luminanceIndex))];
                    }
                }
            }
        }

        // Add neon green color styling with glow effects
        const frame = output.map((row, y) => 
            row.map((char, x) => {
                if (char !== ' ') {
                    const colorIndex = (x + y + Math.floor(this.A * 10)) % this.neonColors.length;
                    const glowIntensity = this.getGlowIntensity(luminanceBuffer[y][x]);
                    const shadowSize = Math.floor(glowIntensity * 15);
                    
                    return `<span style="
                        color: ${this.neonColors[colorIndex]};
                        text-shadow: 
                            0 0 ${shadowSize}px ${this.neonColors[colorIndex]},
                            0 0 ${shadowSize * 2}px ${this.neonColors[colorIndex]},
                            0 0 ${shadowSize * 3}px ${this.neonColors[0]};
                        font-weight: bold;
                    ">${char}</span>`;
                }
                return char;
            }).join('')
        ).join('\n');

        if (this.output) {
            this.output.innerHTML = frame;
        }
    }

    animate() {
        if (!this.running) return;
        
        this.renderFrame();
        this.A += 0.04;
        this.B += 0.02;
        
        setTimeout(() => this.animate(), this.frameDelay);
    }

    stop() {
        this.running = false;
    }
}

// Initialize when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    window.archDonut = new ArchDonut();
    window.archDonut.init();
}); 