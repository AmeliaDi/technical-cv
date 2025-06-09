/*
 * AES Crypto Library Header
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 */

#ifndef AES_CRYPTO_H
#define AES_CRYPTO_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

// Constants
#define AES_BLOCK_SIZE      16
#define AES_KEY_SIZE        16
#define AES_ROUNDS          10
#define AES_EXPANDED_KEYS   176  // 11 round keys * 16 bytes

// Error codes
#define AES_SUCCESS             0
#define AES_ERROR_NULL_POINTER  -1
#define AES_ERROR_INVALID_KEY   -2
#define AES_ERROR_INVALID_SIZE  -3
#define AES_ERROR_HARDWARE      -4

// AES modes
typedef enum {
    AES_MODE_ECB = 0,
    AES_MODE_CBC = 1,
    AES_MODE_CTR = 2,
    AES_MODE_GCM = 3
} aes_mode_t;

// Performance statistics
typedef struct {
    uint64_t encrypt_count;
    uint64_t decrypt_count;
    uint64_t total_bytes;
    double last_encrypt_time;
    double last_decrypt_time;
    time_t init_time;
    time_t uptime;
} aes_stats_t;

// AES context structure
typedef struct {
    uint8_t key[AES_KEY_SIZE];
    uint8_t expanded_keys[AES_EXPANDED_KEYS];
    aes_mode_t mode;
    int has_aes_ni;
    aes_stats_t stats;
} aes_context_t;

#ifdef __cplusplus
extern "C" {
#endif

// Core functions
int aes_init(aes_context_t *ctx, const uint8_t *key, aes_mode_t mode);
int aes_encrypt_block(aes_context_t *ctx, const uint8_t *plaintext, uint8_t *ciphertext);
int aes_decrypt_block(aes_context_t *ctx, const uint8_t *ciphertext, uint8_t *plaintext);

// Block cipher modes
int aes_encrypt_ecb(aes_context_t *ctx, const uint8_t *plaintext, 
                    size_t length, uint8_t *ciphertext);
int aes_decrypt_ecb(aes_context_t *ctx, const uint8_t *ciphertext,
                    size_t length, uint8_t *plaintext);
int aes_encrypt_cbc(aes_context_t *ctx, const uint8_t *plaintext,
                    size_t length, const uint8_t *iv, uint8_t *ciphertext);
int aes_decrypt_cbc(aes_context_t *ctx, const uint8_t *ciphertext,
                    size_t length, const uint8_t *iv, uint8_t *plaintext);

// Statistics and utilities
void aes_get_stats(const aes_context_t *ctx, aes_stats_t *stats);
void aes_reset_stats(aes_context_t *ctx);
void aes_print_hex(const char *label, const uint8_t *data, size_t length);
void aes_generate_random_key(uint8_t *key);
void aes_generate_random_iv(uint8_t *iv);
const char* aes_error_string(int error_code);

// Performance and testing
double aes_benchmark_encrypt(aes_context_t *ctx, size_t iterations);
int aes_set_key_from_string(aes_context_t *ctx, const char *key_string);

// Assembly function declarations (for reference)
extern void aes_encrypt(const uint8_t *plaintext, const uint8_t *expanded_keys, uint8_t *ciphertext);
extern void aes_decrypt(const uint8_t *ciphertext, const uint8_t *expanded_keys, uint8_t *plaintext);
extern void aes_key_expansion(const uint8_t *key, uint8_t *expanded_keys);
extern void aes_encrypt_ni(const uint8_t *plaintext, const uint8_t *expanded_keys, uint8_t *ciphertext);

#ifdef __cplusplus
}
#endif

#endif // AES_CRYPTO_H 