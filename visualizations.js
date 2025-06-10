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
        
        console.log('🔢 MathVisualizer constructor called');
    }

    init() {
        console.log('🔢 MathVisualizer init() called');
        
        // Initialize Mandelbrot visualization
        this.mandelCanvas = document.getElementById('mandelbrot-canvas');
        if (this.mandelCanvas) {
            this.mandelCtx = this.mandelCanvas.getContext('2d');
            this.mandelStats = document.getElementById('mandelbrot-stats');
            console.log('✅ Mandelbrot canvas initialized:', this.mandelCanvas.width + 'x' + this.mandelCanvas.height);
        } else {
            console.warn('⚠️ Mandelbrot canvas not found');
        }
        
        // Initialize FFT visualization
        this.fftCanvas = document.getElementById('fft-canvas');
        if (this.fftCanvas) {
            this.fftCtx = this.fftCanvas.getContext('2d');
            this.fftStats = document.getElementById('fft-stats');
            console.log('✅ FFT canvas initialized:', this.fftCanvas.width + 'x' + this.fftCanvas.height);
        } else {
            console.warn('⚠️ FFT canvas not found');
        }
        
        // Setup event listeners
        this.setupEventListeners();
        
        // Initial render only if canvases exist
        if (this.mandelCanvas && this.mandelCtx) {
            console.log('🎨 Starting initial Mandelbrot render...');
            setTimeout(() => this.renderMandelbrot(), 500);
        }
        if (this.fftCanvas && this.fftCtx) {
            console.log('🎵 Starting initial FFT render...');
            setTimeout(() => this.renderFFT(), 1000);
        }
    }

    setupEventListeners() {
        console.log('🔗 Setting up event listeners...');
        
        // Mandelbrot controls
        const mandelZoom = document.getElementById('mandel-zoom');
        if (mandelZoom) {
            mandelZoom.addEventListener('input', (e) => {
                this.mandelZoom = parseFloat(e.target.value);
                console.log('🔍 Zoom changed to:', this.mandelZoom);
                this.renderMandelbrot();
            });
            console.log('✅ Mandelbrot zoom control connected');
        }
        
        const mandelIter = document.getElementById('mandel-iter');
        if (mandelIter) {
            mandelIter.addEventListener('input', (e) => {
                this.mandelIterations = parseInt(e.target.value);
                console.log('🔢 Iterations changed to:', this.mandelIterations);
                this.renderMandelbrot();
            });
            console.log('✅ Mandelbrot iterations control connected');
        }
        
        // FFT waveform controls
        const sineWave = document.getElementById('sine-wave');
        if (sineWave) {
            sineWave.addEventListener('click', () => {
                this.currentWaveform = 'sine';
                console.log('🎵 Waveform changed to: sine');
                this.updateActiveButton('sine-wave');
                this.renderFFT();
            });
            console.log('✅ Sine wave button connected');
        }
        
        const squareWave = document.getElementById('square-wave');
        if (squareWave) {
            squareWave.addEventListener('click', () => {
                this.currentWaveform = 'square';
                console.log('🎵 Waveform changed to: square');
                this.updateActiveButton('square-wave');
                this.renderFFT();
            });
            console.log('✅ Square wave button connected');
        }
        
        const sawtoothWave = document.getElementById('sawtooth-wave');
        if (sawtoothWave) {
            sawtoothWave.addEventListener('click', () => {
                this.currentWaveform = 'sawtooth';
                console.log('🎵 Waveform changed to: sawtooth');
                this.updateActiveButton('sawtooth-wave');
                this.renderFFT();
            });
            console.log('✅ Sawtooth wave button connected');
        }
        
        // Canvas click for Mandelbrot zoom
        if (this.mandelCanvas) {
            this.mandelCanvas.addEventListener('click', (e) => {
                this.handleMandelClick(e);
            });
            console.log('✅ Mandelbrot canvas click handler connected');
        }
    }

    updateActiveButton(activeId) {
        // Remove active class from all wave buttons
        ['sine-wave', 'square-wave', 'sawtooth-wave'].forEach(id => {
            const btn = document.getElementById(id);
            if (btn) {
                btn.classList.remove('active');
            }
        });
        
        // Add active class to clicked button
        const activeBtn = document.getElementById(activeId);
        if (activeBtn) {
            activeBtn.classList.add('active');
        }
    }

    async renderMandelbrot() {
        if (this.isRendering) {
            console.log('⚠️ Mandelbrot render already in progress');
            return;
        }
        
        if (!this.mandelCanvas || !this.mandelCtx) {
            console.error('❌ Mandelbrot canvas not available');
            return;
        }
        
        console.log('🎨 Starting Mandelbrot render, zoom:', this.mandelZoom, 'iterations:', this.mandelIterations);
        
        this.isRendering = true;
        const startTime = performance.now();
        
        const ctx = this.mandelCtx;
        const canvas = this.mandelCanvas;
        const width = canvas.width;
        const height = canvas.height;
        
        // Clear canvas first
        ctx.fillStyle = '#000000';
        ctx.fillRect(0, 0, width, height);
        
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
                    // Try WebAssembly first if available
                    if (window.wasmLoader && window.wasmLoader.callFunction) {
                        iterations = window.wasmLoader.callFunction('mandelbrot_point', x, y, this.mandelIterations);
                    } else {
                        iterations = this.mandelbrotJS(x, y, this.mandelIterations);
                    }
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
            
            // Update progress every 20 rows for smoother rendering
            if (py % 20 === 0) {
                ctx.putImageData(imageData, 0, 0);
                await this.sleep(1);
            }
        }
        
        ctx.putImageData(imageData, 0, 0);
        
        const endTime = performance.now();
        const renderTime = endTime - startTime;
        
        console.log('✅ Mandelbrot render completed in', renderTime.toFixed(2), 'ms');
        
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
        
        // Enhanced color mapping
        const t = iterations / maxIter;
        const hue = (t * 360) % 360;
        const saturation = 0.8 + (t * 0.2);
        const value = t < 1 ? 0.6 + (t * 0.4) : 0.0;
        
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
        
        console.log('🖱️ Mandelbrot canvas clicked at:', x, y);
        
        // Convert click coordinates to complex plane
        const width = this.mandelCanvas.width;
        const height = this.mandelCanvas.height;
        const scale = 3 / this.mandelZoom;
        
        const complexX = -0.5 + (x - width / 2) * scale / width * 2;
        const complexY = 0 + (y - height / 2) * scale / height * 2;
        
        console.log('🔍 Zooming into complex point:', complexX.toFixed(4), complexY.toFixed(4));
        
        // Increase zoom
        this.mandelZoom *= 2;
        const zoomSlider = document.getElementById('mandel-zoom');
        if (zoomSlider && this.mandelZoom <= 100) {
            zoomSlider.value = this.mandelZoom;
        }
        
        this.renderMandelbrot();
    }

    updateMandelStats(renderTime, totalIterations, pixelsCalculated) {
        if (!this.mandelStats) return;
        
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
        if (!this.fftCanvas || !this.fftCtx) {
            console.error('❌ FFT canvas not available');
            return;
        }
        
        console.log('🎵 Starting FFT render for waveform:', this.currentWaveform);
        
        const startTime = performance.now();
        
        try {
            // Generate time domain signal
            const timeData = new Float32Array(this.bufferSize);
            
            // Generate waveform in JavaScript
            for (let i = 0; i < this.bufferSize; i++) {
                const t = i / this.sampleRate;
                switch (this.currentWaveform) {
                    case 'sine':
                        timeData[i] = Math.sin(2 * Math.PI * this.frequency * t);
                        break;
                    case 'square':
                        timeData[i] = Math.sign(Math.sin(2 * Math.PI * this.frequency * t));
                        break;
                    case 'sawtooth':
                        timeData[i] = 2 * (this.frequency * t - Math.floor(this.frequency * t + 0.5));
                        break;
                    default:
                        timeData[i] = Math.sin(2 * Math.PI * this.frequency * t);
                }
            }
            
            // Compute FFT using JavaScript
            const fftResult = this.computeFFTJS(timeData);
            
            // Visualize results
            this.drawFFT(timeData, fftResult.real, fftResult.imag);
            
            console.log('✅ FFT render completed');
            
        } catch (error) {
            console.error('❌ Error in FFT calculation:', error);
            this.drawFFTFallback();
        }
        
        const endTime = performance.now();
        const processingTime = endTime - startTime;
        
        this.updateFFTStats(processingTime);
    }

    computeFFTJS(data) {
        const N = data.length;
        
        // Ensure N is a power of 2
        const logN = Math.log2(N);
        if (Math.floor(logN) !== logN) {
            console.warn('⚠️ Buffer size is not a power of 2, padding with zeros');
            // Pad to next power of 2
            const newSize = Math.pow(2, Math.ceil(logN));
            const paddedData = new Float32Array(newSize);
            paddedData.set(data);
            return this.computeFFTJS(paddedData);
        }
        
        const real = new Float32Array(N);
        const imag = new Float32Array(N);
        
        // Copy input data
        for (let i = 0; i < N; i++) {
            real[i] = data[i];
            imag[i] = 0;
        }
        
        // Bit reversal
        for (let i = 0; i < N; i++) {
            const j = this.reverseBits(i, logN);
            if (j > i) {
                [real[i], real[j]] = [real[j], real[i]];
                [imag[i], imag[j]] = [imag[j], imag[i]];
            }
        }
        
        // FFT computation
        for (let len = 2; len <= N; len *= 2) {
            const angle = -2 * Math.PI / len;
            const wlenReal = Math.cos(angle);
            const wlenImag = Math.sin(angle);
            
            for (let i = 0; i < N; i += len) {
                let wReal = 1;
                let wImag = 0;
                
                for (let j = 0; j < len / 2; j++) {
                    const u = i + j;
                    const v = i + j + len / 2;
                    
                    const tReal = real[v] * wReal - imag[v] * wImag;
                    const tImag = real[v] * wImag + imag[v] * wReal;
                    
                    real[v] = real[u] - tReal;
                    imag[v] = imag[u] - tImag;
                    real[u] += tReal;
                    imag[u] += tImag;
                    
                    const nextWReal = wReal * wlenReal - wImag * wlenImag;
                    const nextWImag = wReal * wlenImag + wImag * wlenReal;
                    wReal = nextWReal;
                    wImag = nextWImag;
                }
            }
        }
        
        return { real, imag };
    }

    reverseBits(n, bits) {
        let result = 0;
        for (let i = 0; i < bits; i++) {
            result = (result << 1) | (n & 1);
            n >>= 1;
        }
        return result;
    }

    drawFFT(timeData, freqReal, freqImag) {
        const ctx = this.fftCtx;
        const canvas = this.fftCanvas;
        const width = canvas.width;
        const height = canvas.height;
        
        // Clear canvas with dark background
        ctx.fillStyle = '#0a0a0a';
        ctx.fillRect(0, 0, width, height);
        
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
        ctx.lineWidth = 2;
        ctx.beginPath();
        
        const freqHeight = height * 0.4;
        const freqStart = height * 0.6;
        const freqScale = width / (this.bufferSize / 2);
        
        for (let i = 0; i < this.bufferSize / 2; i++) {
            const magnitude = Math.sqrt(freqReal[i] * freqReal[i] + freqImag[i] * freqImag[i]);
            const x = i * freqScale;
            const y = freqStart + freqHeight - (magnitude * freqHeight / this.bufferSize * 4);
            
            if (i === 0) {
                ctx.moveTo(x, y);
            } else {
                ctx.lineTo(x, y);
            }
        }
        ctx.stroke();
        
        // Draw labels
        ctx.fillStyle = '#ffffff';
        ctx.font = '14px "Courier New", monospace';
        ctx.fillText('Time Domain - ' + this.currentWaveform.toUpperCase(), 10, 25);
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
        
        ctx.fillStyle = '#1a1a1a';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        
        ctx.fillStyle = '#ff4444';
        ctx.font = '16px "Courier New", monospace';
        ctx.textAlign = 'center';
        ctx.fillText('FFT Calculation Error', canvas.width / 2, canvas.height / 2);
        ctx.fillText('Check console for details', canvas.width / 2, canvas.height / 2 + 25);
    }

    updateFFTStats(processingTime) {
        if (!this.fftStats) return;
        
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
    // Add a small delay to ensure all DOM elements are fully loaded
    setTimeout(() => {
        console.log('🔢 Initializing mathematical visualization system...');
        window.mathVisualizer = new MathVisualizer();
        window.mathVisualizer.init();
        window.MathUtils = MathUtils;
        
        // Set initial active button
        const sineBtn = document.getElementById('sine-wave');
        if (sineBtn) {
            sineBtn.classList.add('active');
        }
        
        console.log('🎉 Mathematical visualization system ready!');
        console.log('🧮 MathUtils available with: fibonacci, isPerfectNumber, gcd, lcm, factorial, combinations');
    }, 100);
}); 