#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// Export functions for WebAssembly
#define WASM_EXPORT __attribute__((visibility("default")))

// Memory management for WebAssembly
static int32_t* shared_memory = NULL;
static int32_t memory_size = 0;

// Initialize shared memory
WASM_EXPORT
void init_memory(int32_t size) {
    if (shared_memory) free(shared_memory);
    shared_memory = (int32_t*)malloc(size * sizeof(int32_t));
    memory_size = size;
}

// Get memory pointer for JavaScript
WASM_EXPORT
int32_t* get_memory_ptr() {
    return shared_memory;
}

// QuickSort implementation
static void quicksort_internal(int32_t* arr, int32_t low, int32_t high, int32_t* comparisons) {
    if (low < high) {
        int32_t pivot = partition(arr, low, high, comparisons);
        quicksort_internal(arr, low, pivot - 1, comparisons);
        quicksort_internal(arr, pivot + 1, high, comparisons);
    }
}

static int32_t partition(int32_t* arr, int32_t low, int32_t high, int32_t* comparisons) {
    int32_t pivot = arr[high];
    int32_t i = low - 1;
    
    for (int32_t j = low; j < high; j++) {
        (*comparisons)++;
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

static void swap(int32_t* a, int32_t* b) {
    int32_t temp = *a;
    *a = *b;
    *b = temp;
}

WASM_EXPORT
int32_t quicksort(int32_t* arr, int32_t size) {
    int32_t comparisons = 0;
    quicksort_internal(arr, 0, size - 1, &comparisons);
    return comparisons;
}

// MergeSort implementation
static void merge(int32_t* arr, int32_t left, int32_t mid, int32_t right, int32_t* comparisons) {
    int32_t n1 = mid - left + 1;
    int32_t n2 = right - mid;
    
    int32_t* L = (int32_t*)malloc(n1 * sizeof(int32_t));
    int32_t* R = (int32_t*)malloc(n2 * sizeof(int32_t));
    
    for (int32_t i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int32_t j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];
    
    int32_t i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        (*comparisons)++;
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
    
    free(L);
    free(R);
}

static void mergesort_internal(int32_t* arr, int32_t left, int32_t right, int32_t* comparisons) {
    if (left < right) {
        int32_t mid = left + (right - left) / 2;
        mergesort_internal(arr, left, mid, comparisons);
        mergesort_internal(arr, mid + 1, right, comparisons);
        merge(arr, left, mid, right, comparisons);
    }
}

WASM_EXPORT
int32_t mergesort(int32_t* arr, int32_t size) {
    int32_t comparisons = 0;
    mergesort_internal(arr, 0, size - 1, &comparisons);
    return comparisons;
}

// HeapSort implementation
static void heapify(int32_t* arr, int32_t n, int32_t i, int32_t* comparisons) {
    int32_t largest = i;
    int32_t left = 2 * i + 1;
    int32_t right = 2 * i + 2;
    
    if (left < n) {
        (*comparisons)++;
        if (arr[left] > arr[largest])
            largest = left;
    }
    
    if (right < n) {
        (*comparisons)++;
        if (arr[right] > arr[largest])
            largest = right;
    }
    
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest, comparisons);
    }
}

WASM_EXPORT
int32_t heapsort(int32_t* arr, int32_t size) {
    int32_t comparisons = 0;
    
    // Build heap
    for (int32_t i = size / 2 - 1; i >= 0; i--)
        heapify(arr, size, i, &comparisons);
    
    // Extract elements from heap
    for (int32_t i = size - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0, &comparisons);
    }
    
    return comparisons;
}

// Sieve of Eratosthenes
WASM_EXPORT
int32_t sieve_of_eratosthenes(int32_t limit) {
    if (limit < 2) return 0;
    
    // Use shared memory for sieve
    if (memory_size < limit) {
        init_memory(limit);
    }
    
    // Initialize array (using as boolean array)
    for (int32_t i = 0; i < limit; i++) {
        shared_memory[i] = 1; // True
    }
    shared_memory[0] = shared_memory[1] = 0; // 0 and 1 are not prime
    
    for (int32_t p = 2; p * p < limit; p++) {
        if (shared_memory[p]) {
            for (int32_t i = p * p; i < limit; i += p) {
                shared_memory[i] = 0; // Mark as not prime
            }
        }
    }
    
    // Count primes
    int32_t count = 0;
    for (int32_t i = 2; i < limit; i++) {
        if (shared_memory[i]) count++;
    }
    
    return count;
}

// Mandelbrot set calculation
WASM_EXPORT
int32_t mandelbrot_point(double x, double y, int32_t max_iter) {
    double zx = 0.0, zy = 0.0;
    int32_t iter = 0;
    
    while (zx * zx + zy * zy < 4.0 && iter < max_iter) {
        double temp = zx * zx - zy * zy + x;
        zy = 2.0 * zx * zy + y;
        zx = temp;
        iter++;
    }
    
    return iter;
}

// Fast Fourier Transform (simplified version)
WASM_EXPORT
void fft_real(double* real, double* imag, int32_t n) {
    // Bit-reverse permutation
    int32_t j = 0;
    for (int32_t i = 1; i < n; i++) {
        int32_t bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        
        if (i < j) {
            double temp = real[i];
            real[i] = real[j];
            real[j] = temp;
            
            temp = imag[i];
            imag[i] = imag[j];
            imag[j] = temp;
        }
    }
    
    // Cooley-Tukey FFT
    for (int32_t len = 2; len <= n; len <<= 1) {
        double angle = -2.0 * M_PI / len;
        double wlen_r = cos(angle);
        double wlen_i = sin(angle);
        
        for (int32_t i = 0; i < n; i += len) {
            double w_r = 1.0;
            double w_i = 0.0;
            
            for (int32_t j = 0; j < len / 2; j++) {
                int32_t u = i + j;
                int32_t v = i + j + len / 2;
                
                double u_r = real[u];
                double u_i = imag[u];
                double v_r = real[v] * w_r - imag[v] * w_i;
                double v_i = real[v] * w_i + imag[v] * w_r;
                
                real[u] = u_r + v_r;
                imag[u] = u_i + v_i;
                real[v] = u_r - v_r;
                imag[v] = u_i - v_i;
                
                double next_w_r = w_r * wlen_r - w_i * wlen_i;
                double next_w_i = w_r * wlen_i + w_i * wlen_r;
                w_r = next_w_r;
                w_i = next_w_i;
            }
        }
    }
}

// Generate test waveforms
WASM_EXPORT
void generate_sine_wave(double* buffer, int32_t size, double frequency, double sample_rate) {
    for (int32_t i = 0; i < size; i++) {
        buffer[i] = sin(2.0 * M_PI * frequency * i / sample_rate);
    }
}

WASM_EXPORT
void generate_square_wave(double* buffer, int32_t size, double frequency, double sample_rate) {
    for (int32_t i = 0; i < size; i++) {
        double t = i / sample_rate;
        buffer[i] = (sin(2.0 * M_PI * frequency * t) >= 0) ? 1.0 : -1.0;
    }
}

WASM_EXPORT
void generate_sawtooth_wave(double* buffer, int32_t size, double frequency, double sample_rate) {
    for (int32_t i = 0; i < size; i++) {
        double t = i / sample_rate;
        double phase = fmod(frequency * t, 1.0);
        buffer[i] = 2.0 * phase - 1.0;
    }
}

// Performance counter
static uint64_t performance_counter = 0;

WASM_EXPORT
uint64_t get_performance_counter() {
    return performance_counter;
}

WASM_EXPORT
void reset_performance_counter() {
    performance_counter = 0;
}

// Utility functions for benchmarking
WASM_EXPORT
void fill_random_array(int32_t* arr, int32_t size, int32_t seed) {
    srand(seed);
    for (int32_t i = 0; i < size; i++) {
        arr[i] = rand() % 1000;
    }
}

WASM_EXPORT
int32_t is_array_sorted(int32_t* arr, int32_t size) {
    for (int32_t i = 1; i < size; i++) {
        if (arr[i] < arr[i-1]) return 0;
    }
    return 1;
} 