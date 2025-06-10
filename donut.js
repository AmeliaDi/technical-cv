/**
 * ASCII Donut Animation
 * Based on donut.c by a1k0n
 * https://www.a1k0n.net/2011/07/20/donut-math.html
 */

class DonutAnimation {
    constructor() {
        this.A = 0;
        this.B = 0;
        this.screenWidth = 80;
        this.screenHeight = 24;
        this.R1 = 1;
        this.R2 = 2;
        this.K2 = 5;
        this.K1 = this.screenWidth * this.K2 * 3 / (8 * (this.R1 + this.R2));
        
        this.output = document.getElementById('donut-output');
        this.running = true;
        this.frameDelay = 50; // milliseconds
    }

    renderFrame() {
        const cosA = Math.cos(this.A);
        const sinA = Math.sin(this.A);
        const cosB = Math.cos(this.B);
        const sinB = Math.sin(this.B);

        // Initialize buffers
        const output = new Array(this.screenHeight).fill(null).map(() => 
            new Array(this.screenWidth).fill(' ')
        );
        const zbuffer = new Array(this.screenHeight).fill(null).map(() => 
            new Array(this.screenWidth).fill(0)
        );

        // theta goes around the cross-sectional circle of a torus
        for (let theta = 0; theta < 2 * Math.PI; theta += 0.07) {
            const costheta = Math.cos(theta);
            const sintheta = Math.sin(theta);

            // phi goes around the center of revolution of a torus
            for (let phi = 0; phi < 2 * Math.PI; phi += 0.02) {
                const cosphi = Math.cos(phi);
                const sinphi = Math.sin(phi);

                // the x,y coordinate of the circle, before revolving
                const circlex = this.R2 + this.R1 * costheta;
                const circley = this.R1 * sintheta;

                // final 3D (x,y,z) coordinate after rotations
                const x = circlex * (cosB * cosphi + sinA * sinB * sinphi) - circley * cosA * sinB;
                const y = circlex * (sinB * cosphi - sinA * cosB * sinphi) + circley * cosA * cosB;
                const z = this.K2 + cosA * circlex * sinphi + circley * sinA;
                const ooz = 1 / z;

                // x and y projection
                const xp = Math.floor(this.screenWidth / 2 + this.K1 * ooz * x);
                const yp = Math.floor(this.screenHeight / 2 - this.K1 * ooz * y);

                // calculate luminance
                const L = cosphi * costheta * sinB - cosA * costheta * sinphi - 
                         sinA * sintheta + cosB * (cosA * sintheta - costheta * sinA * sinphi);

                // plot if visible
                if (L > 0 && xp >= 0 && xp < this.screenWidth && yp >= 0 && yp < this.screenHeight) {
                    if (ooz > zbuffer[yp][xp]) {
                        zbuffer[yp][xp] = ooz;
                        const luminanceIndex = Math.floor(L * 8);
                        output[yp][xp] = '.,-~:;=!*#$@'[Math.max(0, Math.min(11, luminanceIndex))];
                    }
                }
            }
        }

        // Convert to string and display
        const frame = output.map(row => row.join('')).join('\n');
        this.output.textContent = frame;
    }

    animate() {
        if (!this.running) return;
        
        this.renderFrame();
        this.A += 0.04;
        this.B += 0.02;
        
        setTimeout(() => this.animate(), this.frameDelay);
    }

    start() {
        this.running = true;
        this.animate();
    }

    stop() {
        this.running = false;
    }
}

// Initialize donut animation when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    window.donutAnimation = new DonutAnimation();
    
    // Add a small delay to ensure smooth start
    setTimeout(() => {
        window.donutAnimation.start();
    }, 500);
});

// Stop animation on page unload
window.addEventListener('beforeunload', () => {
    window.donutAnimation?.stop();
}); 