/*
 * AES Test Suite
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "aes_crypto.h"

// Test vectors from NIST
static const uint8_t test_key[AES_KEY_SIZE] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static const uint8_t test_plaintext[AES_BLOCK_SIZE] = {
    0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
    0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34
};

static const uint8_t expected_ciphertext[AES_BLOCK_SIZE] = {
    0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
    0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32
};

// Test counters
static int tests_passed = 0;
static int tests_failed = 0;

// Test helper macros
#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("✓ %s\n", message); \
            tests_passed++; \
        } else { \
            printf("✗ %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

#define TEST_EQUALS_BYTES(actual, expected, size, message) \
    do { \
        if (memcmp(actual, expected, size) == 0) { \
            printf("✓ %s\n", message); \
            tests_passed++; \
        } else { \
            printf("✗ %s\n", message); \
            printf("  Expected: "); \
            for (int i = 0; i < size; i++) printf("%02x ", expected[i]); \
            printf("\n  Actual:   "); \
            for (int i = 0; i < size; i++) printf("%02x ", actual[i]); \
            printf("\n"); \
            tests_failed++; \
        } \
    } while(0)

// Test basic encryption/decryption
void test_basic_encryption(void) {
    printf("\n=== Testing Basic Encryption/Decryption ===\n");
    
    aes_context_t ctx;
    uint8_t ciphertext[AES_BLOCK_SIZE];
    uint8_t decrypted[AES_BLOCK_SIZE];
    
    // Initialize context
    int result = aes_init(&ctx, test_key, AES_MODE_ECB);
    TEST_ASSERT(result == AES_SUCCESS, "AES context initialization");
    
    // Test encryption
    result = aes_encrypt_block(&ctx, test_plaintext, ciphertext);
    TEST_ASSERT(result == AES_SUCCESS, "Single block encryption");
    TEST_EQUALS_BYTES(ciphertext, expected_ciphertext, AES_BLOCK_SIZE, 
                      "Encryption produces expected ciphertext");
    
    // Test decryption
    result = aes_decrypt_block(&ctx, ciphertext, decrypted);
    TEST_ASSERT(result == AES_SUCCESS, "Single block decryption");
    TEST_EQUALS_BYTES(decrypted, test_plaintext, AES_BLOCK_SIZE,
                      "Decryption produces original plaintext");
}

// Test ECB mode with multiple blocks
void test_ecb_mode(void) {
    printf("\n=== Testing ECB Mode ===\n");
    
    aes_context_t ctx;
    const size_t data_size = AES_BLOCK_SIZE * 4; // 4 blocks
    uint8_t plaintext[data_size];
    uint8_t ciphertext[data_size];
    uint8_t decrypted[data_size];
    
    // Fill plaintext with pattern
    for (size_t i = 0; i < data_size; i++) {
        plaintext[i] = (uint8_t)(i & 0xFF);
    }
    
    int result = aes_init(&ctx, test_key, AES_MODE_ECB);
    TEST_ASSERT(result == AES_SUCCESS, "ECB mode initialization");
    
    // Encrypt multiple blocks
    result = aes_encrypt_ecb(&ctx, plaintext, data_size, ciphertext);
    TEST_ASSERT(result == AES_SUCCESS, "ECB encryption of multiple blocks");
    
    // Decrypt multiple blocks
    result = aes_decrypt_ecb(&ctx, ciphertext, data_size, decrypted);
    TEST_ASSERT(result == AES_SUCCESS, "ECB decryption of multiple blocks");
    TEST_EQUALS_BYTES(decrypted, plaintext, data_size,
                      "ECB round-trip preserves data");
}

// Test CBC mode
void test_cbc_mode(void) {
    printf("\n=== Testing CBC Mode ===\n");
    
    aes_context_t ctx;
    const size_t data_size = AES_BLOCK_SIZE * 3;
    uint8_t plaintext[data_size];
    uint8_t ciphertext[data_size];
    uint8_t decrypted[data_size];
    uint8_t iv[AES_BLOCK_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };
    
    // Fill plaintext with repeating pattern
    for (size_t i = 0; i < data_size; i++) {
        plaintext[i] = (uint8_t)(0xAA);
    }
    
    int result = aes_init(&ctx, test_key, AES_MODE_CBC);
    TEST_ASSERT(result == AES_SUCCESS, "CBC mode initialization");
    
    // Encrypt in CBC mode
    result = aes_encrypt_cbc(&ctx, plaintext, data_size, iv, ciphertext);
    TEST_ASSERT(result == AES_SUCCESS, "CBC encryption");
    
    // Decrypt in CBC mode
    result = aes_decrypt_cbc(&ctx, ciphertext, data_size, iv, decrypted);
    TEST_ASSERT(result == AES_SUCCESS, "CBC decryption");
    TEST_EQUALS_BYTES(decrypted, plaintext, data_size,
                      "CBC round-trip preserves data");
    
    // Test that CBC produces different ciphertext for identical plaintext blocks
    int blocks_identical = 1;
    for (size_t i = AES_BLOCK_SIZE; i < data_size; i += AES_BLOCK_SIZE) {
        if (memcmp(ciphertext, ciphertext + i, AES_BLOCK_SIZE) != 0) {
            blocks_identical = 0;
            break;
        }
    }
    TEST_ASSERT(!blocks_identical, "CBC produces different ciphertext for identical blocks");
}

// Test error handling
void test_error_handling(void) {
    printf("\n=== Testing Error Handling ===\n");
    
    aes_context_t ctx;
    uint8_t buffer[AES_BLOCK_SIZE];
    
    // Test null pointer handling
    int result = aes_init(NULL, test_key, AES_MODE_ECB);
    TEST_ASSERT(result == AES_ERROR_NULL_POINTER, "Null context pointer detection");
    
    result = aes_init(&ctx, NULL, AES_MODE_ECB);
    TEST_ASSERT(result == AES_ERROR_NULL_POINTER, "Null key pointer detection");
    
    // Initialize valid context for further tests
    aes_init(&ctx, test_key, AES_MODE_ECB);
    
    result = aes_encrypt_block(&ctx, NULL, buffer);
    TEST_ASSERT(result == AES_ERROR_NULL_POINTER, "Null plaintext pointer detection");
    
    result = aes_encrypt_block(&ctx, test_plaintext, NULL);
    TEST_ASSERT(result == AES_ERROR_NULL_POINTER, "Null ciphertext pointer detection");
    
    // Test invalid size handling
    result = aes_encrypt_ecb(&ctx, test_plaintext, 15, buffer); // Not multiple of block size
    TEST_ASSERT(result == AES_ERROR_INVALID_SIZE, "Invalid data size detection");
}

// Test statistics functionality
void test_statistics(void) {
    printf("\n=== Testing Statistics ===\n");
    
    aes_context_t ctx;
    aes_stats_t stats;
    uint8_t buffer[AES_BLOCK_SIZE];
    
    int result = aes_init(&ctx, test_key, AES_MODE_ECB);
    TEST_ASSERT(result == AES_SUCCESS, "Context initialization for stats test");
    
    // Check initial statistics
    aes_get_stats(&ctx, &stats);
    TEST_ASSERT(stats.encrypt_count == 0, "Initial encrypt count is zero");
    TEST_ASSERT(stats.decrypt_count == 0, "Initial decrypt count is zero");
    TEST_ASSERT(stats.total_bytes == 0, "Initial total bytes is zero");
    
    // Perform some operations
    aes_encrypt_block(&ctx, test_plaintext, buffer);
    aes_encrypt_block(&ctx, test_plaintext, buffer);
    aes_decrypt_block(&ctx, buffer, buffer);
    
    // Check updated statistics
    aes_get_stats(&ctx, &stats);
    TEST_ASSERT(stats.encrypt_count == 2, "Encrypt count updated correctly");
    TEST_ASSERT(stats.decrypt_count == 1, "Decrypt count updated correctly");
    TEST_ASSERT(stats.total_bytes == 3 * AES_BLOCK_SIZE, "Total bytes calculated correctly");
    
    // Test reset
    aes_reset_stats(&ctx);
    aes_get_stats(&ctx, &stats);
    TEST_ASSERT(stats.encrypt_count == 0, "Stats reset correctly");
}

// Test utility functions
void test_utilities(void) {
    printf("\n=== Testing Utility Functions ===\n");
    
    aes_context_t ctx;
    uint8_t key[AES_KEY_SIZE];
    uint8_t iv[AES_BLOCK_SIZE];
    
    // Test random key generation
    aes_generate_random_key(key);
    int all_zero = 1;
    for (int i = 0; i < AES_KEY_SIZE; i++) {
        if (key[i] != 0) {
            all_zero = 0;
            break;
        }
    }
    TEST_ASSERT(!all_zero, "Random key generation produces non-zero key");
    
    // Test random IV generation
    aes_generate_random_iv(iv);
    all_zero = 1;
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        if (iv[i] != 0) {
            all_zero = 0;
            break;
        }
    }
    TEST_ASSERT(!all_zero, "Random IV generation produces non-zero IV");
    
    // Test string key setting
    int result = aes_set_key_from_string(&ctx, "testkey123456789");
    TEST_ASSERT(result == AES_SUCCESS, "String key setting works");
    
    // Test error string function
    const char *error_str = aes_error_string(AES_SUCCESS);
    TEST_ASSERT(strcmp(error_str, "Success") == 0, "Error string for success");
    
    error_str = aes_error_string(AES_ERROR_NULL_POINTER);
    TEST_ASSERT(strcmp(error_str, "Null pointer error") == 0, "Error string for null pointer");
}

// Performance test
void test_performance(void) {
    printf("\n=== Testing Performance ===\n");
    
    aes_context_t ctx;
    const size_t iterations = 10000;
    
    int result = aes_init(&ctx, test_key, AES_MODE_ECB);
    TEST_ASSERT(result == AES_SUCCESS, "Context initialization for performance test");
    
    double elapsed = aes_benchmark_encrypt(&ctx, iterations);
    TEST_ASSERT(elapsed > 0, "Performance benchmark runs");
    
    printf("  Performance: %.2f encryptions/second\n", iterations / elapsed);
    printf("  Time per encryption: %.6f seconds\n", elapsed / iterations);
    
    // Check if AES-NI is being used
    if (ctx.has_aes_ni) {
        printf("  Using hardware AES-NI acceleration\n");
    } else {
        printf("  Using software implementation\n");
    }
}

// Main test runner
int main(void) {
    printf("🔐 AES Crypto Library Test Suite\n");
    printf("================================\n");
    
    test_basic_encryption();
    test_ecb_mode();
    test_cbc_mode();
    test_error_handling();
    test_statistics();
    test_utilities();
    test_performance();
    
    printf("\n=== Test Summary ===\n");
    printf("✓ Tests passed: %d\n", tests_passed);
    printf("✗ Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed!\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed!\n");
        return 1;
    }
} 