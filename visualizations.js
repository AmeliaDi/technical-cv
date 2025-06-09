// Mathematical Visualizations
class MathVisualizer {
    constructor() {
        this.mandelCanvas = null;
        this.mandelCtx = null;
        this.mandelStats = null;
        
        this.fftCanvas = null;
        this.fftCtx = null;
        this.fftStats = null;
        
        this.isRendering = false;
        this.mandelZoom = 1;
        this.mandelIterations = 100;
        
        this.currentWaveform = 'sine';
        this.sampleRate = 44100;
        this.frequency = 440;
        this.bufferSize = 1024;
    }

    init() {
        // Initialize Mandelbrot visualization
        this.mandelCanvas = document.getElementById('mandel-canvas');
        this.mandelCtx = this.mandelCanvas.getContext('2d');
        this.mandelStats = document.getElementById('mandel-stats');
        
        // Initialize FFT visualization
        this.fftCanvas = document.getElementById('fft-canvas');
        this.fftCtx = this.fftCanvas.getContext('2d');
        this.fftStats = document.getElementById('fft-stats');
        
        // Setup event listeners
        this.setupEventListeners();
        
        // Initial render
        this.renderMandelbrot();
        this.renderFFT();
    }

    setupEventListeners() {
        // Mandelbrot controls
        document.getElementById('mandel-zoom').addEventListener('input', (e) => {
            this.mandelZoom = parseFloat(e.target.value);
            this.renderMandelbrot();
        });
        
        document.getElementById('mandel-iter').addEventListener('input', (e) => {
            this.mandelIterations = parseInt(e.target.value);
            this.renderMandelbrot();
        });
        
        // FFT waveform controls
        document.getElementById('sine-wave').addEventListener('click', () => {
            this.currentWaveform = 'sine';
            this.renderFFT();
        });
        
        document.getElementById('square-wave').addEventListener('click', () => {
            this.currentWaveform = 'square';
            this.renderFFT();
        });
        
        document.getElementById('sawtooth-wave').addEventListener('click', () => {
            this.currentWaveform = 'sawtooth';
            this.renderFFT();
        });
        
        // Canvas click for Mandelbrot zoom
        this.mandelCanvas.addEventListener('click', (e) => {
            this.handleMandelClick(e);
        });
    }

    async renderMandelbrot() {
        if (this.isRendering) return;
        
        this.isRendering = true;
        const startTime = performance.now();
        
        const ctx = this.mandelCtx;
        const canvas = this.mandelCanvas;
        const width = canvas.width;
        const height = canvas.height;
        
        const imageData = ctx.createImageData(width, height);
        const data = imageData.data;
        
        // Mandelbrot parameters
        const centerX = -0.5;
        const centerY = 0;
        const scale = 3 / this.mandelZoom;
        
        let totalIterations = 0;
        let pixelsCalculated = 0;
        
        // Calculate Mandelbrot set
        for (let py = 0; py < height; py++) {
            for (let px = 0; px < width; px++) {
                const x = centerX + (px - width / 2) * scale / width * 2;
                const y = centerY + (py - height / 2) * scale / height * 2;
                
                let iterations;
                try {
                    iterations = wasmLoader.callFunction('mandelbrot_point', x, y, this.mandelIterations);
                } catch (error) {
                    // Fallback JavaScript implementation
                    iterations = this.mandelbrotJS(x, y, this.mandelIterations);
                }
                
                totalIterations += iterations;
                pixelsCalculated++;
                
                // Color mapping
                const color = this.mapIterationsToColor(iterations, this.mandelIterations);
                const index = (py * width + px) * 4;
                
                data[index] = color.r;     // Red
                data[index + 1] = color.g; // Green
                data[index + 2] = color.b; // Blue
                data[index + 3] = 255;     // Alpha
            }
            
            // Update progress every few rows
            if (py % 10 === 0) {
                ctx.putImageData(imageData, 0, 0);
                await this.sleep(1);
            }
        }
        
        ctx.putImageData(imageData, 0, 0);
        
        const endTime = performance.now();
        const renderTime = endTime - startTime;
        
        // Update statistics
        this.updateMandelStats(renderTime, totalIterations, pixelsCalculated);
        
        this.isRendering = false;
    }

    mandelbrotJS(x, y, maxIter) {
        let zx = 0, zy = 0;
        let iter = 0;
        
        while (zx * zx + zy * zy < 4 && iter < maxIter) {
            const temp = zx * zx - zy * zy + x;
            zy = 2 * zx * zy + y;
            zx = temp;
            iter++;
        }
        
        return iter;
    }

    mapIterationsToColor(iterations, maxIter) {
        if (iterations === maxIter) {
            return { r: 0, g: 0, b: 0 }; // Black for points in the set
        }
        
        // HSV to RGB color mapping
        const hue = (iterations / maxIter) * 360;
        const saturation = 1.0;
        const value = iterations < maxIter ? 1.0 : 0.0;
        
        return this.hsvToRgb(hue, saturation, value);
    }

    hsvToRgb(h, s, v) {
        const c = v * s;
        const x = c * (1 - Math.abs((h / 60) % 2 - 1));
        const m = v - c;
        
        let r, g, b;
        
        if (h >= 0 && h < 60) {
            r = c; g = x; b = 0;
        } else if (h >= 60 && h < 120) {
            r = x; g = c; b = 0;
        } else if (h >= 120 && h < 180) {
            r = 0; g = c; b = x;
        } else if (h >= 180 && h < 240) {
            r = 0; g = x; b = c;
        } else if (h >= 240 && h < 300) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }
        
        return {
            r: Math.round((r + m) * 255),
            g: Math.round((g + m) * 255),
            b: Math.round((b + m) * 255)
        };
    }

    handleMandelClick(event) {
        const rect = this.mandelCanvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        
        // Convert to complex coordinates and zoom in
        const centerX = -0.5;
        const centerY = 0;
        const scale = 3 / this.mandelZoom;
        
        const newCenterX = centerX + (x - this.mandelCanvas.width / 2) * scale / this.mandelCanvas.width * 2;
        const newCenterY = centerY + (y - this.mandelCanvas.height / 2) * scale / this.mandelCanvas.height * 2;
        
        this.mandelZoom *= 2;
        document.getElementById('mandel-zoom').value = this.mandelZoom;
        
        this.renderMandelbrot();
    }

    updateMandelStats(renderTime, totalIterations, pixelsCalculated) {
        const avgIterations = totalIterations / pixelsCalculated;
        this.mandelStats.innerHTML = `
            <div><span class="variable">ZOOM</span>: ${this.mandelZoom.toFixed(1)}x</div>
            <div><span class="variable">ITERATIONS</span>: ${this.mandelIterations}</div>
            <div><span class="variable">RENDER_TIME</span>: ${renderTime.toFixed(2)}ms</div>
            <div><span class="variable">AVG_ITER</span>: ${avgIterations.toFixed(2)}</div>
            <div><span class="variable">PIXELS</span>: ${pixelsCalculated}</div>
        `;
    }

    async renderFFT() {
        const startTime = performance.now();
        
        // Generate waveform data
        const buffer = wasmLoader.allocateArray(this.bufferSize, 'f64');
        const realBuffer = wasmLoader.allocateArray(this.bufferSize, 'f64');
        const imagBuffer = wasmLoader.allocateArray(this.bufferSize, 'f64');
        
        try {
            // Generate waveform
            switch (this.currentWaveform) {
                case 'sine':
                    wasmLoader.callFunction('generate_sine_wave', buffer, this.bufferSize, this.frequency, this.sampleRate);
                    break;
                case 'square':
                    wasmLoader.callFunction('generate_square_wave', buffer, this.bufferSize, this.frequency, this.sampleRate);
                    break;
                case 'sawtooth':
                    wasmLoader.callFunction('generate_sawtooth_wave', buffer, this.bufferSize, this.frequency, this.sampleRate);
                    break;
            }
            
            // Copy to real buffer, zero imaginary
            const bufferView = wasmLoader.getArrayView(buffer, this.bufferSize, 'f64');
            const realView = wasmLoader.getArrayView(realBuffer, this.bufferSize, 'f64');
            const imagView = wasmLoader.getArrayView(imagBuffer, this.bufferSize, 'f64');
            
            for (let i = 0; i < this.bufferSize; i++) {
                realView[i] = bufferView[i];
                imagView[i] = 0;
            }
            
            // Perform FFT
            wasmLoader.callFunction('fft_real', realBuffer, imagBuffer, this.bufferSize);
            
            // Visualize results
            this.drawFFT(bufferView, realView, imagView);
            
        } catch (error) {
            console.error('Error in FFT calculation:', error);
            this.drawFFTFallback();
        }
        
        const endTime = performance.now();
        const processingTime = endTime - startTime;
        
        this.updateFFTStats(processingTime);
    }

    drawFFT(timeData, freqReal, freqImag) {
        const ctx = this.fftCtx;
        const canvas = this.fftCanvas;
        const width = canvas.width;
        const height = canvas.height;
        
        ctx.clearRect(0, 0, width, height);
        
        // Draw time domain (top half)
        ctx.strokeStyle = '#00aaff';
        ctx.lineWidth = 2;
        ctx.beginPath();
        
        const timeHeight = height * 0.4;
        const timeScale = width / this.bufferSize;
        
        for (let i = 0; i < this.bufferSize; i++) {
            const x = i * timeScale;
            const y = timeHeight / 2 + (timeData[i] * timeHeight / 4);
            
            if (i === 0) {
                ctx.moveTo(x, y);
            } else {
                ctx.lineTo(x, y);
            }
        }
        ctx.stroke();
        
        // Draw frequency domain (bottom half)
        ctx.strokeStyle = '#00ff00';
        ctx.lineWidth = 1;
        ctx.beginPath();
        
        const freqHeight = height * 0.4;
        const freqStart = height * 0.6;
        const freqScale = width / (this.bufferSize / 2);
        
        for (let i = 0; i < this.bufferSize / 2; i++) {
            const magnitude = Math.sqrt(freqReal[i] * freqReal[i] + freqImag[i] * freqImag[i]);
            const x = i * freqScale;
            const y = freqStart + freqHeight - (magnitude * freqHeight / this.bufferSize);
            
            if (i === 0) {
                ctx.moveTo(x, y);
            } else {
                ctx.lineTo(x, y);
            }
        }
        ctx.stroke();
        
        // Draw labels
        ctx.fillStyle = '#ffffff';
        ctx.font = '12px JetBrains Mono';
        ctx.fillText('Time Domain', 10, 20);
        ctx.fillText('Frequency Domain', 10, freqStart - 10);
        
        // Draw grid lines
        ctx.strokeStyle = '#333333';
        ctx.lineWidth = 1;
        
        // Horizontal grid
        for (let i = 1; i < 4; i++) {
            const y = i * height / 4;
            ctx.beginPath();
            ctx.moveTo(0, y);
            ctx.lineTo(width, y);
            ctx.stroke();
        }
        
        // Vertical grid
        for (let i = 1; i < 8; i++) {
            const x = i * width / 8;
            ctx.beginPath();
            ctx.moveTo(x, 0);
            ctx.lineTo(x, height);
            ctx.stroke();
        }
    }

    drawFFTFallback() {
        const ctx = this.fftCtx;
        const canvas = this.fftCanvas;
        
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        ctx.fillStyle = '#ff4444';
        ctx.font = '16px JetBrains Mono';
        ctx.textAlign = 'center';
        ctx.fillText('FFT Calculation Error', canvas.width / 2, canvas.height / 2);
    }

    updateFFTStats(processingTime) {
        const nyquist = this.sampleRate / 2;
        const resolution = nyquist / (this.bufferSize / 2);
        
        this.fftStats.innerHTML = `
            <div><span class="variable">WAVEFORM</span>: ${this.currentWaveform.toUpperCase()}</div>
            <div><span class="variable">FREQUENCY</span>: ${this.frequency}Hz</div>
            <div><span class="variable">SAMPLE_RATE</span>: ${this.sampleRate}Hz</div>
            <div><span class="variable">BUFFER_SIZE</span>: ${this.bufferSize}</div>
            <div><span class="variable">RESOLUTION</span>: ${resolution.toFixed(2)}Hz</div>
            <div><span class="variable">PROCESS_TIME</span>: ${processingTime.toFixed(2)}ms</div>
        `;
    }

    sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
}

// Additional mathematical utilities
class MathUtils {
    static generateFibonacci(n) {
        const fib = [0, 1];
        for (let i = 2; i < n; i++) {
            fib[i] = fib[i-1] + fib[i-2];
        }
        return fib;
    }
    
    static isPerfectNumber(n) {
        let sum = 1;
        for (let i = 2; i <= Math.sqrt(n); i++) {
            if (n % i === 0) {
                sum += i;
                if (i !== n / i) {
                    sum += n / i;
                }
            }
        }
        return sum === n && n > 1;
    }
    
    static gcd(a, b) {
        while (b !== 0) {
            const temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
    
    static lcm(a, b) {
        return Math.abs(a * b) / this.gcd(a, b);
    }
    
    static factorial(n) {
        if (n <= 1) return 1;
        return n * this.factorial(n - 1);
    }
    
    static combinations(n, r) {
        if (r > n) return 0;
        return this.factorial(n) / (this.factorial(r) * this.factorial(n - r));
    }
}

// Initialize when page loads
document.addEventListener('DOMContentLoaded', () => {
    window.mathVisualizer = new MathVisualizer();
    window.mathVisualizer.init();
    window.MathUtils = MathUtils;
    
    console.log('🔢 Mathematical visualization system ready!');
    console.log('🧮 MathUtils available with: fibonacci, isPerfectNumber, gcd, lcm, factorial, combinations');
}); 