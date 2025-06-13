/*
 * AES Wrapper Functions for Assembly Implementation
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <cpuid.h>
#include "aes_crypto.h"

// External assembly functions
extern void aes_encrypt(const uint8_t *plaintext, const uint8_t *expanded_keys, uint8_t *ciphertext);
extern void aes_decrypt(const uint8_t *ciphertext, const uint8_t *expanded_keys, uint8_t *plaintext);
extern void aes_key_expansion(const uint8_t *key, uint8_t *expanded_keys);
extern void aes_encrypt_ni(const uint8_t *plaintext, const uint8_t *expanded_keys, uint8_t *ciphertext);

// Check for AES-NI support
static int check_aes_ni_support(void) {
    unsigned int eax, ebx, ecx, edx;
    
    // Check if CPUID is available
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        // Check for AES-NI support (bit 25 of ECX)
        return (ecx & (1 << 25)) != 0;
    }
    
    return 0;
}

// Initialize AES context
int aes_init(aes_context_t *ctx, const uint8_t *key, aes_mode_t mode) {
    if (!ctx || !key) {
        return AES_ERROR_NULL_POINTER;
    }
    
    // Copy key
    memcpy(ctx->key, key, AES_KEY_SIZE);
    ctx->mode = mode;
    
    // Expand key
    aes_key_expansion(key, ctx->expanded_keys);
    
    // Check for hardware acceleration
    ctx->has_aes_ni = check_aes_ni_support();
    
    // Initialize statistics
    ctx->stats.encrypt_count = 0;
    ctx->stats.decrypt_count = 0;
    ctx->stats.total_bytes = 0;
    ctx->stats.init_time = time(NULL);
    
    return AES_SUCCESS;
}

// Encrypt single block
int aes_encrypt_block(aes_context_t *ctx, const uint8_t *plaintext, uint8_t *ciphertext) {
    if (!ctx || !plaintext || !ciphertext) {
        return AES_ERROR_NULL_POINTER;
    }
    
    clock_t start = clock();
    
    // Use hardware acceleration if available
    if (ctx->has_aes_ni) {
        aes_encrypt_ni(plaintext, ctx->expanded_keys, ciphertext);
    } else {
        aes_encrypt(plaintext, ctx->expanded_keys, ciphertext);
    }
    
    // Update statistics
    ctx->stats.encrypt_count++;
    ctx->stats.total_bytes += AES_BLOCK_SIZE;
    ctx->stats.last_encrypt_time = ((double)(clock() - start)) / CLOCKS_PER_SEC;
    
    return AES_SUCCESS;
}

// Decrypt single block
int aes_decrypt_block(aes_context_t *ctx, const uint8_t *ciphertext, uint8_t *plaintext) {
    if (!ctx || !ciphertext || !plaintext) {
        return AES_ERROR_NULL_POINTER;
    }
    
    clock_t start = clock();
    
    // Note: Assembly implementation includes decrypt function
    aes_decrypt(ciphertext, ctx->expanded_keys, plaintext);
    
    // Update statistics
    ctx->stats.decrypt_count++;
    ctx->stats.total_bytes += AES_BLOCK_SIZE;
    ctx->stats.last_decrypt_time = ((double)(clock() - start)) / CLOCKS_PER_SEC;
    
    return AES_SUCCESS;
}

// Encrypt multiple blocks (ECB mode)
int aes_encrypt_ecb(aes_context_t *ctx, const uint8_t *plaintext, 
                    size_t length, uint8_t *ciphertext) {
    if (!ctx || !plaintext || !ciphertext) {
        return AES_ERROR_NULL_POINTER;
    }
    
    if (length % AES_BLOCK_SIZE != 0) {
        return AES_ERROR_INVALID_SIZE;
    }
    
    size_t blocks = length / AES_BLOCK_SIZE;
    
    for (size_t i = 0; i < blocks; i++) {
        int result = aes_encrypt_block(ctx, 
                                     plaintext + i * AES_BLOCK_SIZE,
                                     ciphertext + i * AES_BLOCK_SIZE);
        if (result != AES_SUCCESS) {
            return result;
        }
    }
    
    return AES_SUCCESS;
}

// Decrypt multiple blocks (ECB mode)
int aes_decrypt_ecb(aes_context_t *ctx, const uint8_t *ciphertext,
                    size_t length, uint8_t *plaintext) {
    if (!ctx || !ciphertext || !plaintext) {
        return AES_ERROR_NULL_POINTER;
    }
    
    if (length % AES_BLOCK_SIZE != 0) {
        return AES_ERROR_INVALID_SIZE;
    }
    
    size_t blocks = length / AES_BLOCK_SIZE;
    
    for (size_t i = 0; i < blocks; i++) {
        int result = aes_decrypt_block(ctx,
                                     ciphertext + i * AES_BLOCK_SIZE,
                                     plaintext + i * AES_BLOCK_SIZE);
        if (result != AES_SUCCESS) {
            return result;
        }
    }
    
    return AES_SUCCESS;
}

// CBC mode encryption
int aes_encrypt_cbc(aes_context_t *ctx, const uint8_t *plaintext,
                    size_t length, const uint8_t *iv, uint8_t *ciphertext) {
    if (!ctx || !plaintext || !ciphertext || !iv) {
        return AES_ERROR_NULL_POINTER;
    }
    
    if (length % AES_BLOCK_SIZE != 0) {
        return AES_ERROR_INVALID_SIZE;
    }
    
    uint8_t previous_block[AES_BLOCK_SIZE];
    uint8_t xor_block[AES_BLOCK_SIZE];
    
    memcpy(previous_block, iv, AES_BLOCK_SIZE);
    
    size_t blocks = length / AES_BLOCK_SIZE;
    
    for (size_t i = 0; i < blocks; i++) {
        // XOR with previous ciphertext block (or IV for first block)
        for (int j = 0; j < AES_BLOCK_SIZE; j++) {
            xor_block[j] = plaintext[i * AES_BLOCK_SIZE + j] ^ previous_block[j];
        }
        
        // Encrypt XORed block
        int result = aes_encrypt_block(ctx, xor_block, 
                                     ciphertext + i * AES_BLOCK_SIZE);
        if (result != AES_SUCCESS) {
            return result;
        }
        
        // Update previous block
        memcpy(previous_block, ciphertext + i * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    }
    
    return AES_SUCCESS;
}

// CBC mode decryption
int aes_decrypt_cbc(aes_context_t *ctx, const uint8_t *ciphertext,
                    size_t length, const uint8_t *iv, uint8_t *plaintext) {
    if (!ctx || !ciphertext || !plaintext || !iv) {
        return AES_ERROR_NULL_POINTER;
    }
    
    if (length % AES_BLOCK_SIZE != 0) {
        return AES_ERROR_INVALID_SIZE;
    }
    
    uint8_t previous_block[AES_BLOCK_SIZE];
    uint8_t decrypted_block[AES_BLOCK_SIZE];
    
    memcpy(previous_block, iv, AES_BLOCK_SIZE);
    
    size_t blocks = length / AES_BLOCK_SIZE;
    
    for (size_t i = 0; i < blocks; i++) {
        // Decrypt block
        int result = aes_decrypt_block(ctx, 
                                     ciphertext + i * AES_BLOCK_SIZE,
                                     decrypted_block);
        if (result != AES_SUCCESS) {
            return result;
        }
        
        // XOR with previous ciphertext block (or IV for first block)
        for (int j = 0; j < AES_BLOCK_SIZE; j++) {
            plaintext[i * AES_BLOCK_SIZE + j] = decrypted_block[j] ^ previous_block[j];
        }
        
        // Update previous block
        memcpy(previous_block, ciphertext + i * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
    }
    
    return AES_SUCCESS;
}

// Get performance statistics
void aes_get_stats(const aes_context_t *ctx, aes_stats_t *stats) {
    if (ctx && stats) {
        *stats = ctx->stats;
        stats->uptime = time(NULL) - stats->init_time;
    }
}

// Reset statistics
void aes_reset_stats(aes_context_t *ctx) {
    if (ctx) {
        memset(&ctx->stats, 0, sizeof(aes_stats_t));
        ctx->stats.init_time = time(NULL);
    }
}

// Utility: Print hex data
void aes_print_hex(const char *label, const uint8_t *data, size_t length) {
    printf("%s: ", label);
    for (size_t i = 0; i < length; i++) {
        printf("%02x", data[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n%*s", (int)strlen(label) + 2, "");
        } else if ((i + 1) % 8 == 0) {
            printf("  ");
        } else {
            printf(" ");
        }
    }
    printf("\n");
}

// Utility: Generate random key
void aes_generate_random_key(uint8_t *key) {
    if (key) {
        srand(time(NULL));
        for (int i = 0; i < AES_KEY_SIZE; i++) {
            key[i] = rand() & 0xFF;
        }
    }
}

// Utility: Generate random IV
void aes_generate_random_iv(uint8_t *iv) {
    if (iv) {
        for (int i = 0; i < AES_BLOCK_SIZE; i++) {
            iv[i] = rand() & 0xFF;
        }
    }
}

// Error code to string conversion
const char* aes_error_string(int error_code) {
    switch (error_code) {
        case AES_SUCCESS: return "Success";
        case AES_ERROR_NULL_POINTER: return "Null pointer error";
        case AES_ERROR_INVALID_KEY: return "Invalid key";
        case AES_ERROR_INVALID_SIZE: return "Invalid data size";
        case AES_ERROR_HARDWARE: return "Hardware error";
        default: return "Unknown error";
    }
}

// Performance benchmark
double aes_benchmark_encrypt(aes_context_t *ctx, size_t iterations) {
    uint8_t plaintext[AES_BLOCK_SIZE] = {0};
    uint8_t ciphertext[AES_BLOCK_SIZE];
    
    clock_t start = clock();
    
    for (size_t i = 0; i < iterations; i++) {
        aes_encrypt_block(ctx, plaintext, ciphertext);
    }
    
    clock_t end = clock();
    double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    return elapsed;
}

// Memory-safe string copy for keys
int aes_set_key_from_string(aes_context_t *ctx, const char *key_string) {
    if (!ctx || !key_string) {
        return AES_ERROR_NULL_POINTER;
    }
    
    size_t len = strlen(key_string);
    uint8_t key[AES_KEY_SIZE] = {0};
    
    // If string is shorter than key size, pad with zeros
    // If longer, truncate
    size_t copy_len = (len < AES_KEY_SIZE) ? len : AES_KEY_SIZE;
    memcpy(key, key_string, copy_len);
    
    return aes_init(ctx, key, AES_MODE_ECB);
} 