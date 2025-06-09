/*
 * AES Performance Benchmark Suite
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "aes_crypto.h"

// Benchmark configuration
#define BENCHMARK_ITERATIONS    100000
#define LARGE_DATA_SIZE        (1024 * 1024)  // 1MB
#define NUM_THREADS            4
#define WARMUP_ITERATIONS      1000

// Benchmark results structure
typedef struct {
    double encryptions_per_second;
    double decryptions_per_second;
    double throughput_mbps;
    double avg_encrypt_time;
    double avg_decrypt_time;
    int hardware_accelerated;
} benchmark_results_t;

// Thread data for multithreaded benchmarks
typedef struct {
    aes_context_t *ctx;
    uint8_t *data;
    size_t data_size;
    size_t iterations;
    double elapsed_time;
    int thread_id;
} thread_data_t;

// High-resolution timer
static double get_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Generate random data
static void generate_random_data(uint8_t *data, size_t size) {
    srand(time(NULL));
    for (size_t i = 0; i < size; i++) {
        data[i] = rand() & 0xFF;
    }
}

// Basic encryption benchmark
static void benchmark_basic_encryption(benchmark_results_t *results) {
    printf("Running basic encryption benchmark...\n");
    
    aes_context_t ctx;
    uint8_t key[AES_KEY_SIZE];
    uint8_t plaintext[AES_BLOCK_SIZE];
    uint8_t ciphertext[AES_BLOCK_SIZE];
    
    generate_random_data(key, AES_KEY_SIZE);
    generate_random_data(plaintext, AES_BLOCK_SIZE);
    
    aes_init(&ctx, key, AES_MODE_ECB);
    results->hardware_accelerated = ctx.has_aes_ni;
    
    // Warmup
    for (int i = 0; i < WARMUP_ITERATIONS; i++) {
        aes_encrypt_block(&ctx, plaintext, ciphertext);
    }
    
    // Encryption benchmark
    double start_time = get_time();
    for (size_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        aes_encrypt_block(&ctx, plaintext, ciphertext);
    }
    double encrypt_time = get_time() - start_time;
    
    // Decryption benchmark
    start_time = get_time();
    for (size_t i = 0; i < BENCHMARK_ITERATIONS; i++) {
        aes_decrypt_block(&ctx, ciphertext, plaintext);
    }
    double decrypt_time = get_time() - start_time;
    
    results->encryptions_per_second = BENCHMARK_ITERATIONS / encrypt_time;
    results->decryptions_per_second = BENCHMARK_ITERATIONS / decrypt_time;
    results->avg_encrypt_time = encrypt_time / BENCHMARK_ITERATIONS;
    results->avg_decrypt_time = decrypt_time / BENCHMARK_ITERATIONS;
}

// Large data benchmark
static void benchmark_large_data(benchmark_results_t *results) {
    printf("Running large data throughput benchmark...\n");
    
    aes_context_t ctx;
    uint8_t key[AES_KEY_SIZE];
    uint8_t *plaintext = malloc(LARGE_DATA_SIZE);
    uint8_t *ciphertext = malloc(LARGE_DATA_SIZE);
    
    if (!plaintext || !ciphertext) {
        printf("Error: Failed to allocate memory for large data benchmark\n");
        return;
    }
    
    generate_random_data(key, AES_KEY_SIZE);
    generate_random_data(plaintext, LARGE_DATA_SIZE);
    
    aes_init(&ctx, key, AES_MODE_ECB);
    
    // Throughput benchmark
    double start_time = get_time();
    aes_encrypt_ecb(&ctx, plaintext, LARGE_DATA_SIZE, ciphertext);
    double elapsed = get_time() - start_time;
    
    results->throughput_mbps = (LARGE_DATA_SIZE / (1024.0 * 1024.0)) / elapsed;
    
    free(plaintext);
    free(ciphertext);
}

// Thread function for multithreaded benchmark
static void* thread_benchmark(void *arg) {
    thread_data_t *data = (thread_data_t*)arg;
    uint8_t ciphertext[AES_BLOCK_SIZE];
    
    double start_time = get_time();
    
    for (size_t i = 0; i < data->iterations; i++) {
        aes_encrypt_block(data->ctx, data->data, ciphertext);
    }
    
    data->elapsed_time = get_time() - start_time;
    
    return NULL;
}

// Multithreaded benchmark
static void benchmark_multithreaded(void) {
    printf("Running multithreaded benchmark with %d threads...\n", NUM_THREADS);
    
    aes_context_t ctx[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    uint8_t key[AES_KEY_SIZE];
    uint8_t plaintext[AES_BLOCK_SIZE];
    
    generate_random_data(key, AES_KEY_SIZE);
    generate_random_data(plaintext, AES_BLOCK_SIZE);
    
    // Initialize contexts and thread data
    for (int i = 0; i < NUM_THREADS; i++) {
        aes_init(&ctx[i], key, AES_MODE_ECB);
        thread_data[i].ctx = &ctx[i];
        thread_data[i].data = plaintext;
        thread_data[i].iterations = BENCHMARK_ITERATIONS / NUM_THREADS;
        thread_data[i].thread_id = i;
    }
    
    // Run benchmark
    double start_time = get_time();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_benchmark, &thread_data[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double total_time = get_time() - start_time;
    double total_ops = BENCHMARK_ITERATIONS;
    
    printf("  Multithreaded performance: %.2f encryptions/second\n", total_ops / total_time);
    printf("  Speedup: %.2fx\n", (total_ops / total_time) / (BENCHMARK_ITERATIONS / thread_data[0].elapsed_time));
}

// Mode comparison benchmark
static void benchmark_modes(void) {
    printf("Running cipher mode comparison...\n");
    
    aes_context_t ctx;
    uint8_t key[AES_KEY_SIZE];
    uint8_t iv[AES_BLOCK_SIZE];
    const size_t data_size = AES_BLOCK_SIZE * 1000;
    uint8_t *plaintext = malloc(data_size);
    uint8_t *ciphertext = malloc(data_size);
    
    if (!plaintext || !ciphertext) {
        printf("Error: Failed to allocate memory for mode benchmark\n");
        return;
    }
    
    generate_random_data(key, AES_KEY_SIZE);
    generate_random_data(iv, AES_BLOCK_SIZE);
    generate_random_data(plaintext, data_size);
    
    aes_init(&ctx, key, AES_MODE_ECB);
    
    // ECB benchmark
    double start_time = get_time();
    aes_encrypt_ecb(&ctx, plaintext, data_size, ciphertext);
    double ecb_time = get_time() - start_time;
    
    // CBC benchmark
    start_time = get_time();
    aes_encrypt_cbc(&ctx, plaintext, data_size, iv, ciphertext);
    double cbc_time = get_time() - start_time;
    
    printf("  ECB mode: %.2f MB/s\n", (data_size / (1024.0 * 1024.0)) / ecb_time);
    printf("  CBC mode: %.2f MB/s\n", (data_size / (1024.0 * 1024.0)) / cbc_time);
    printf("  CBC overhead: %.1f%%\n", ((cbc_time - ecb_time) / ecb_time) * 100);
    
    free(plaintext);
    free(ciphertext);
}

// CPU feature detection and reporting
static void report_cpu_features(void) {
    printf("CPU Features:\n");
    
    // Check for AES-NI
    aes_context_t ctx;
    uint8_t dummy_key[AES_KEY_SIZE] = {0};
    aes_init(&ctx, dummy_key, AES_MODE_ECB);
    
    printf("  AES-NI: %s\n", ctx.has_aes_ni ? "Supported" : "Not supported");
    
    // Additional CPU info
    printf("  CPU cores: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
    
    // Cache line size (common values)
    long cache_line_size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    if (cache_line_size > 0) {
        printf("  L1 cache line: %ld bytes\n", cache_line_size);
    }
}

// Memory usage analysis
static void analyze_memory_usage(void) {
    printf("Memory Usage Analysis:\n");
    printf("  AES context size: %zu bytes\n", sizeof(aes_context_t));
    printf("  Key size: %d bytes\n", AES_KEY_SIZE);
    printf("  Block size: %d bytes\n", AES_BLOCK_SIZE);
    printf("  Expanded keys: %d bytes\n", AES_EXPANDED_KEYS);
    
    // Memory alignment check
    aes_context_t ctx;
    printf("  Context alignment: %zu bytes\n", (size_t)&ctx % sizeof(void*));
}

// Constant-time verification (basic)
static void verify_constant_time(void) {
    printf("Constant-time verification (basic):\n");
    
    aes_context_t ctx;
    uint8_t key[AES_KEY_SIZE];
    uint8_t plaintext1[AES_BLOCK_SIZE] = {0};
    uint8_t plaintext2[AES_BLOCK_SIZE] = {0xFF};
    uint8_t ciphertext1[AES_BLOCK_SIZE];
    uint8_t ciphertext2[AES_BLOCK_SIZE];
    
    generate_random_data(key, AES_KEY_SIZE);
    aes_init(&ctx, key, AES_MODE_ECB);
    
    const int test_iterations = 10000;
    
    // Time encryption of all-zero block
    double start_time = get_time();
    for (int i = 0; i < test_iterations; i++) {
        aes_encrypt_block(&ctx, plaintext1, ciphertext1);
    }
    double time1 = get_time() - start_time;
    
    // Time encryption of all-FF block
    start_time = get_time();
    for (int i = 0; i < test_iterations; i++) {
        aes_encrypt_block(&ctx, plaintext2, ciphertext2);
    }
    double time2 = get_time() - start_time;
    
    double diff_percent = fabs(time1 - time2) / ((time1 + time2) / 2) * 100;
    printf("  Timing difference: %.4f%% ", diff_percent);
    
    if (diff_percent < 5.0) {
        printf("(Good - likely constant time)\n");
    } else {
        printf("(Warning - may not be constant time)\n");
    }
}

// Print detailed results
static void print_results(const benchmark_results_t *results) {
    printf("\n=== Benchmark Results ===\n");
    printf("Hardware acceleration: %s\n", results->hardware_accelerated ? "Yes (AES-NI)" : "No");
    printf("Encryption rate: %.2f ops/sec\n", results->encryptions_per_second);
    printf("Decryption rate: %.2f ops/sec\n", results->decryptions_per_second);
    printf("Average encrypt time: %.6f ms\n", results->avg_encrypt_time * 1000);
    printf("Average decrypt time: %.6f ms\n", results->avg_decrypt_time * 1000);
    printf("Throughput: %.2f MB/s\n", results->throughput_mbps);
    
    // Performance rating
    if (results->encryptions_per_second > 1000000) {
        printf("Performance rating: Excellent\n");
    } else if (results->encryptions_per_second > 500000) {
        printf("Performance rating: Good\n");
    } else if (results->encryptions_per_second > 100000) {
        printf("Performance rating: Average\n");
    } else {
        printf("Performance rating: Needs improvement\n");
    }
}

// Main benchmark runner
int main(int argc, char *argv[]) {
    printf("🚀 AES Crypto Performance Benchmark Suite\n");
    printf("==========================================\n\n");
    
    // Check for command line arguments
    int run_extended = 0;
    if (argc > 1 && strcmp(argv[1], "--extended") == 0) {
        run_extended = 1;
        printf("Running extended benchmark suite...\n\n");
    }
    
    // System information
    report_cpu_features();
    printf("\n");
    
    analyze_memory_usage();
    printf("\n");
    
    // Core benchmarks
    benchmark_results_t results = {0};
    benchmark_basic_encryption(&results);
    benchmark_large_data(&results);
    
    // Extended benchmarks
    if (run_extended) {
        benchmark_multithreaded();
        printf("\n");
        
        benchmark_modes();
        printf("\n");
        
        verify_constant_time();
        printf("\n");
    }
    
    // Print results
    print_results(&results);
    
    // Save results to file
    FILE *result_file = fopen("benchmark_results.txt", "w");
    if (result_file) {
        fprintf(result_file, "AES Benchmark Results\n");
        fprintf(result_file, "====================\n");
        fprintf(result_file, "Encryption rate: %.2f ops/sec\n", results.encryptions_per_second);
        fprintf(result_file, "Decryption rate: %.2f ops/sec\n", results.decryptions_per_second);
        fprintf(result_file, "Throughput: %.2f MB/s\n", results.throughput_mbps);
        fprintf(result_file, "Hardware acceleration: %s\n", results.hardware_accelerated ? "Yes" : "No");
        fclose(result_file);
        printf("\nResults saved to benchmark_results.txt\n");
    }
    
    return 0;
} 