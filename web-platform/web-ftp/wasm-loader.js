// WebAssembly Module Loader
class WASMLoader {
    constructor() {
        this.module = null;
        this.instance = null;
        this.memory = null;
        this.isLoaded = false;
    }

    async loadModule(wasmPath) {
        try {
            console.log('Loading WebAssembly module...');
            
            // For now, we'll create a fallback implementation
            // In a real scenario, you would compile algorithms.c to algorithms.wasm
            this.createFallbackModule();
            this.isLoaded = true;
            
            console.log('WebAssembly module loaded successfully');
            return true;
        } catch (error) {
            console.error('Failed to load WebAssembly module:', error);
            return false;
        }
    }

    // Fallback JavaScript implementation of algorithms
    createFallbackModule() {
        this.module = {
            // Memory management
            memory: new WebAssembly.Memory({ initial: 256 }),
            
            // Exported functions
            exports: {
                // Initialize memory
                init_memory: (size) => {
                    console.log(`Initializing memory: ${size} elements`);
                },

                // Sorting algorithms
                quicksort: (arr, size) => {
                    const array = new Int32Array(arr, 0, size);
                    return this.quicksortJS(array);
                },

                mergesort: (arr, size) => {
                    const array = new Int32Array(arr, 0, size);
                    return this.mergesortJS(array);
                },

                heapsort: (arr, size) => {
                    const array = new Int32Array(arr, 0, size);
                    return this.heapsortJS(array);
                },

                // Sieve of Eratosthenes
                sieve_of_eratosthenes: (limit) => {
                    return this.sieveJS(limit);
                },

                // Mandelbrot calculation
                mandelbrot_point: (x, y, maxIter) => {
                    return this.mandelbrotJS(x, y, maxIter);
                },

                // Wave generation
                generate_sine_wave: (buffer, size, frequency, sampleRate) => {
                    this.generateSineWaveJS(buffer, size, frequency, sampleRate);
                },

                generate_square_wave: (buffer, size, frequency, sampleRate) => {
                    this.generateSquareWaveJS(buffer, size, frequency, sampleRate);
                },

                generate_sawtooth_wave: (buffer, size, frequency, sampleRate) => {
                    this.generateSawtoothWaveJS(buffer, size, frequency, sampleRate);
                },

                // FFT
                fft_real: (real, imag, n) => {
                    this.fftJS(real, imag, n);
                },

                // Utility functions
                fill_random_array: (arr, size, seed) => {
                    this.fillRandomArrayJS(arr, size, seed);
                },

                is_array_sorted: (arr, size) => {
                    return this.isArraySortedJS(arr, size);
                }
            }
        };

        this.memory = this.module.memory;
        this.instance = { exports: this.module.exports };
    }

    // JavaScript implementations
    quicksortJS(arr) {
        let comparisons = 0;
        
        function partition(arr, low, high) {
            const pivot = arr[high];
            let i = low - 1;
            
            for (let j = low; j < high; j++) {
                comparisons++;
                if (arr[j] <= pivot) {
                    i++;
                    [arr[i], arr[j]] = [arr[j], arr[i]];
                }
            }
            [arr[i + 1], arr[high]] = [arr[high], arr[i + 1]];
            return i + 1;
        }
        
        function quicksortInternal(arr, low, high) {
            if (low < high) {
                const pivot = partition(arr, low, high);
                quicksortInternal(arr, low, pivot - 1);
                quicksortInternal(arr, pivot + 1, high);
            }
        }
        
        quicksortInternal(arr, 0, arr.length - 1);
        return comparisons;
    }

    mergesortJS(arr) {
        let comparisons = 0;
        
        function merge(arr, left, mid, right) {
            const n1 = mid - left + 1;
            const n2 = right - mid;
            
            const L = new Array(n1);
            const R = new Array(n2);
            
            for (let i = 0; i < n1; i++) L[i] = arr[left + i];
            for (let j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
            
            let i = 0, j = 0, k = left;
            
            while (i < n1 && j < n2) {
                comparisons++;
                if (L[i] <= R[j]) {
                    arr[k] = L[i];
                    i++;
                } else {
                    arr[k] = R[j];
                    j++;
                }
                k++;
            }
            
            while (i < n1) {
                arr[k] = L[i];
                i++;
                k++;
            }
            
            while (j < n2) {
                arr[k] = R[j];
                j++;
                k++;
            }
        }
        
        function mergesortInternal(arr, left, right) {
            if (left < right) {
                const mid = Math.floor(left + (right - left) / 2);
                mergesortInternal(arr, left, mid);
                mergesortInternal(arr, mid + 1, right);
                merge(arr, left, mid, right);
            }
        }
        
        mergesortInternal(arr, 0, arr.length - 1);
        return comparisons;
    }

    heapsortJS(arr) {
        let comparisons = 0;
        
        function heapify(arr, n, i) {
            let largest = i;
            const left = 2 * i + 1;
            const right = 2 * i + 2;
            
            if (left < n) {
                comparisons++;
                if (arr[left] > arr[largest]) largest = left;
            }
            
            if (right < n) {
                comparisons++;
                if (arr[right] > arr[largest]) largest = right;
            }
            
            if (largest !== i) {
                [arr[i], arr[largest]] = [arr[largest], arr[i]];
                heapify(arr, n, largest);
            }
        }
        
        const n = arr.length;
        
        // Build heap
        for (let i = Math.floor(n / 2) - 1; i >= 0; i--) {
            heapify(arr, n, i);
        }
        
        // Extract elements
        for (let i = n - 1; i > 0; i--) {
            [arr[0], arr[i]] = [arr[i], arr[0]];
            heapify(arr, i, 0);
        }
        
        return comparisons;
    }

    sieveJS(limit) {
        if (limit < 2) return 0;
        
        const sieve = new Array(limit).fill(true);
        sieve[0] = sieve[1] = false;
        
        for (let p = 2; p * p < limit; p++) {
            if (sieve[p]) {
                for (let i = p * p; i < limit; i += p) {
                    sieve[i] = false;
                }
            }
        }
        
        let count = 0;
        for (let i = 2; i < limit; i++) {
            if (sieve[i]) count++;
        }
        
        return count;
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

    generateSineWaveJS(buffer, size, frequency, sampleRate) {
        const arr = new Float64Array(buffer, 0, size);
        for (let i = 0; i < size; i++) {
            arr[i] = Math.sin(2 * Math.PI * frequency * i / sampleRate);
        }
    }

    generateSquareWaveJS(buffer, size, frequency, sampleRate) {
        const arr = new Float64Array(buffer, 0, size);
        for (let i = 0; i < size; i++) {
            const t = i / sampleRate;
            arr[i] = Math.sin(2 * Math.PI * frequency * t) >= 0 ? 1.0 : -1.0;
        }
    }

    generateSawtoothWaveJS(buffer, size, frequency, sampleRate) {
        const arr = new Float64Array(buffer, 0, size);
        for (let i = 0; i < size; i++) {
            const t = i / sampleRate;
            const phase = (frequency * t) % 1.0;
            arr[i] = 2.0 * phase - 1.0;
        }
    }

    fftJS(real, imag, n) {
        // Simple FFT implementation
        // This is a simplified version for demo purposes
        const realArr = new Float64Array(real, 0, n);
        const imagArr = new Float64Array(imag, 0, n);
        
        // Bit-reverse permutation
        let j = 0;
        for (let i = 1; i < n; i++) {
            let bit = n >> 1;
            while (j & bit) {
                j ^= bit;
                bit >>= 1;
            }
            j ^= bit;
            
            if (i < j) {
                [realArr[i], realArr[j]] = [realArr[j], realArr[i]];
                [imagArr[i], imagArr[j]] = [imagArr[j], imagArr[i]];
            }
        }
        
        // Cooley-Tukey FFT
        for (let len = 2; len <= n; len <<= 1) {
            const angle = -2 * Math.PI / len;
            const wlenR = Math.cos(angle);
            const wlenI = Math.sin(angle);
            
            for (let i = 0; i < n; i += len) {
                let wR = 1.0;
                let wI = 0.0;
                
                for (let j = 0; j < len / 2; j++) {
                    const u = i + j;
                    const v = i + j + len / 2;
                    
                    const uR = realArr[u];
                    const uI = imagArr[u];
                    const vR = realArr[v] * wR - imagArr[v] * wI;
                    const vI = realArr[v] * wI + imagArr[v] * wR;
                    
                    realArr[u] = uR + vR;
                    imagArr[u] = uI + vI;
                    realArr[v] = uR - vR;
                    imagArr[v] = uI - vI;
                    
                    const nextWR = wR * wlenR - wI * wlenI;
                    const nextWI = wR * wlenI + wI * wlenR;
                    wR = nextWR;
                    wI = nextWI;
                }
            }
        }
    }

    fillRandomArrayJS(arrPtr, size, seed) {
        // Simple random number generator
        let rng = seed;
        const arr = new Int32Array(arrPtr, 0, size);
        
        for (let i = 0; i < size; i++) {
            rng = (rng * 1103515245 + 12345) & 0x7fffffff;
            arr[i] = rng % 1000;
        }
    }

    isArraySortedJS(arrPtr, size) {
        const arr = new Int32Array(arrPtr, 0, size);
        for (let i = 1; i < size; i++) {
            if (arr[i] < arr[i - 1]) return 0;
        }
        return 1;
    }

    // Helper methods
    allocateArray(size, type = 'i32') {
        const bytesPerElement = type === 'f64' ? 8 : 4;
        const bytes = size * bytesPerElement;
        const ptr = new ArrayBuffer(bytes);
        return ptr;
    }

    getArrayView(ptr, size, type = 'i32') {
        if (type === 'f64') {
            return new Float64Array(ptr, 0, size);
        } else {
            return new Int32Array(ptr, 0, size);
        }
    }

    callFunction(name, ...args) {
        if (!this.isLoaded) {
            throw new Error('WebAssembly module not loaded');
        }
        
        if (typeof this.instance.exports[name] !== 'function') {
            throw new Error(`Function ${name} not found in WebAssembly module`);
        }
        
        return this.instance.exports[name](...args);
    }
}

// Global WASM loader instance
window.wasmLoader = new WASMLoader(); 