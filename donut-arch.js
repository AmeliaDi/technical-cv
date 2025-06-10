/**
 * ASCII Donut Animation - Arch Linux Edition
 * Based on donut.c by a1k0n with Arch Linux theme
 * @author Amelia Enora 🌈 Marceline Chavez Barroso
 */

class ArchDonut {
    constructor() {
        this.A = 0;
        this.B = 0;
        this.screenWidth = 40;  // Smaller for better integration
        this.screenHeight = 16;  // Compact size
        this.R1 = 1;
        this.R2 = 2;
        this.K2 = 5;
        this.K1 = this.screenWidth * this.K2 * 3 / (8 * (this.R1 + this.R2));
        
        this.output = null;
        this.container = null;
        this.running = true;
        this.frameDelay = 60; // Smooth animation
        
        // Arch Linux themed characters
        this.chars = '.,-~:;=!*#$@';
        this.archColors = ['#1793d1', '#333333', '#00aaff', '#0088cc'];
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
                    <div class="arch-donut-label">Arch Linux</div>
                </div>
            `;
            
            // Insert before language selector
            headerControls.insertBefore(this.container, headerControls.firstChild);
            this.output = document.getElementById('arch-donut-output');
        }
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
                        const luminanceIndex = Math.floor(L * 8);
                        output[yp][xp] = this.chars[Math.max(0, Math.min(11, luminanceIndex))];
                    }
                }
            }
        }

        // Add Arch Linux color styling
        const frame = output.map((row, y) => 
            row.map((char, x) => {
                if (char !== ' ') {
                    const colorIndex = (x + y) % this.archColors.length;
                    return `<span style="color: ${this.archColors[colorIndex]}">${char}</span>`;
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