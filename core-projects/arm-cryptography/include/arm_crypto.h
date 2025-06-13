/**
 * ARM Cryptography Suite - Next-Generation Cryptographic Library
 * 
 * Copyright (c) 2024 Amelia Enora
 * 
 * This file is part of the ARM Cryptography Suite, a high-performance
 * cryptographic library optimized for ARM processors with post-quantum
 * algorithms, homomorphic encryption, and zero-knowledge proofs.
 */

#ifndef ARM_CRYPTO_H
#define ARM_CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ================================
// ARM Architecture Detection
// ================================

#if defined(__aarch64__) || defined(_M_ARM64)
    #define ARM_CRYPTO_AARCH64 1
    #define ARM_CRYPTO_NEON_AVAILABLE 1
    
    #ifdef __ARM_FEATURE_SVE
        #define ARM_CRYPTO_SVE_AVAILABLE 1
    #endif
    
    #ifdef __ARM_FEATURE_CRYPTO
        #define ARM_CRYPTO_EXTENSIONS_AVAILABLE 1
    #endif
#endif

// ================================
// Post-Quantum Cryptography
// ================================

// CRYSTALS-Kyber Parameters
#define KYBER_512_PUBLICKEY_BYTES    800
#define KYBER_512_SECRETKEY_BYTES    1632
#define KYBER_512_CIPHERTEXT_BYTES   768
#define KYBER_512_SHAREDSECRET_BYTES 32

#define KYBER_768_PUBLICKEY_BYTES    1184
#define KYBER_768_SECRETKEY_BYTES    2400
#define KYBER_768_CIPHERTEXT_BYTES   1088
#define KYBER_768_SHAREDSECRET_BYTES 32

#define KYBER_1024_PUBLICKEY_BYTES   1568
#define KYBER_1024_SECRETKEY_BYTES   3168
#define KYBER_1024_CIPHERTEXT_BYTES  1568
#define KYBER_1024_SHAREDSECRET_BYTES 32

typedef enum {
    KYBER_512  = 512,
    KYBER_768  = 768,
    KYBER_1024 = 1024
} kyber_variant_t;

typedef struct {
    kyber_variant_t variant;
    uint8_t *public_key;
    uint8_t *secret_key;
    size_t pk_size;
    size_t sk_size;
} kyber_ctx_t;

// Kyber Functions
int kyber_init(kyber_ctx_t *ctx, kyber_variant_t variant);
int kyber_keypair(kyber_ctx_t *ctx, uint8_t *pk, uint8_t *sk);
int kyber_encaps(kyber_ctx_t *ctx, uint8_t *ct, uint8_t *ss, const uint8_t *pk);
int kyber_decaps(kyber_ctx_t *ctx, uint8_t *ss, const uint8_t *ct, const uint8_t *sk);
void kyber_cleanup(kyber_ctx_t *ctx);

// CRYSTALS-Dilithium Parameters
#define DILITHIUM2_PUBLICKEY_BYTES  1312
#define DILITHIUM2_SECRETKEY_BYTES  2528
#define DILITHIUM2_SIGNATURE_BYTES  2420

#define DILITHIUM3_PUBLICKEY_BYTES  1952
#define DILITHIUM3_SECRETKEY_BYTES  4000
#define DILITHIUM3_SIGNATURE_BYTES  3293

#define DILITHIUM5_PUBLICKEY_BYTES  2592
#define DILITHIUM5_SECRETKEY_BYTES  4864
#define DILITHIUM5_SIGNATURE_BYTES  4595

typedef enum {
    DILITHIUM2 = 2,
    DILITHIUM3 = 3,
    DILITHIUM5 = 5
} dilithium_variant_t;

typedef struct {
    dilithium_variant_t variant;
    uint8_t *public_key;
    uint8_t *secret_key;
    size_t pk_size;
    size_t sk_size;
    size_t sig_size;
} dilithium_ctx_t;

// Dilithium Functions
int dilithium_init(dilithium_ctx_t *ctx, dilithium_variant_t variant);
int dilithium_keypair(dilithium_ctx_t *ctx, uint8_t *pk, uint8_t *sk);
int dilithium_sign(dilithium_ctx_t *ctx, uint8_t *sig, size_t *siglen, 
                   const uint8_t *msg, size_t msglen, const uint8_t *sk);
int dilithium_verify(dilithium_ctx_t *ctx, const uint8_t *sig, size_t siglen,
                     const uint8_t *msg, size_t msglen, const uint8_t *pk);
void dilithium_cleanup(dilithium_ctx_t *ctx);

// ================================
// Homomorphic Encryption (BFV)
// ================================

typedef struct {
    uint32_t poly_modulus_degree;
    uint64_t *coeff_modulus;
    size_t coeff_modulus_count;
    uint64_t plain_modulus;
} bfv_params_t;

typedef struct {
    bfv_params_t params;
    void *context;
    void *evaluator;
    void *encoder;
} bfv_ctx_t;

typedef struct {
    void *data;
    size_t size;
} bfv_secret_key_t;

typedef struct {
    void *data;
    size_t size;
} bfv_public_key_t;

typedef struct {
    void *data;
    size_t size;
} bfv_ciphertext_t;

// BFV Functions
int bfv_init(bfv_ctx_t *ctx, const bfv_params_t *params);
int bfv_keygen(bfv_ctx_t *ctx, bfv_secret_key_t *sk, bfv_public_key_t *pk);
int bfv_encrypt(bfv_ctx_t *ctx, bfv_ciphertext_t *ct, int64_t plaintext, 
                const bfv_public_key_t *pk);
int bfv_decrypt(bfv_ctx_t *ctx, int64_t *plaintext, const bfv_ciphertext_t *ct,
                const bfv_secret_key_t *sk);
int bfv_add(bfv_ctx_t *ctx, bfv_ciphertext_t *result, 
            const bfv_ciphertext_t *ct1, const bfv_ciphertext_t *ct2);
int bfv_multiply(bfv_ctx_t *ctx, bfv_ciphertext_t *result,
                 const bfv_ciphertext_t *ct1, const bfv_ciphertext_t *ct2);
void bfv_cleanup(bfv_ctx_t *ctx);

// ================================
// Zero-Knowledge Proofs (zk-SNARKs)
// ================================

typedef struct {
    void *circuit_data;
    size_t num_inputs;
    size_t num_outputs;
    size_t num_constraints;
} zksnark_circuit_t;

typedef struct {
    void *proving_key;
    void *verification_key;
    size_t pk_size;
    size_t vk_size;
} zksnark_setup_t;

typedef struct {
    int64_t *values;
    size_t count;
} zksnark_witness_t;

typedef struct {
    void *proof_data;
    size_t size;
} zksnark_proof_t;

// zk-SNARK Functions
int zksnark_init_circuit(zksnark_circuit_t *circuit, const char *r1cs_file);
int zksnark_trusted_setup(const zksnark_circuit_t *circuit, zksnark_setup_t *setup);
int zksnark_prove(const zksnark_circuit_t *circuit, const zksnark_setup_t *setup,
                  const zksnark_witness_t *witness, zksnark_proof_t *proof);
bool zksnark_verify(const zksnark_circuit_t *circuit, const zksnark_setup_t *setup,
                    const zksnark_proof_t *proof);
void zksnark_cleanup_circuit(zksnark_circuit_t *circuit);
void zksnark_cleanup_setup(zksnark_setup_t *setup);
void zksnark_cleanup_proof(zksnark_proof_t *proof);

// ================================
// Secure Multi-Party Computation
// ================================

typedef enum {
    MPC_SHAMIR_SECRET_SHARING,
    MPC_ADDITIVE_SECRET_SHARING,
    MPC_BGW_PROTOCOL,
    MPC_GMW_PROTOCOL
} mpc_protocol_t;

typedef struct {
    mpc_protocol_t protocol;
    uint32_t num_parties;
    uint32_t threshold;
    void *protocol_data;
} mpc_ctx_t;

// MPC Functions  
int mpc_init(mpc_ctx_t *ctx, uint32_t num_parties, mpc_protocol_t protocol);
int mpc_secure_addition(mpc_ctx_t *ctx, const uint32_t *inputs, uint32_t *result);
int mpc_secure_multiplication(mpc_ctx_t *ctx, const uint32_t *inputs, uint32_t *result);
void mpc_cleanup(mpc_ctx_t *ctx);

// ================================
// Threshold Cryptography
// ================================

typedef struct {
    uint32_t threshold;
    uint32_t num_parties;
    void *shared_secret;
    void *public_params;
} threshold_ecdsa_ctx_t;

typedef struct {
    void *signature_data;
    size_t size;
} partial_signature_t;

typedef struct {
    uint8_t r[32];
    uint8_t s[32];
} ecdsa_signature_t;

// Threshold ECDSA Functions
int threshold_ecdsa_init(threshold_ecdsa_ctx_t *ctx, uint32_t threshold, uint32_t num_parties);
int threshold_ecdsa_keygen(threshold_ecdsa_ctx_t *ctx);
int threshold_ecdsa_partial_sign(threshold_ecdsa_ctx_t *ctx, partial_signature_t *partial_sigs,
                                 const uint8_t *message, size_t msglen);
int threshold_ecdsa_combine(threshold_ecdsa_ctx_t *ctx, ecdsa_signature_t *signature,
                           const partial_signature_t *partial_sigs, uint32_t num_sigs);
void threshold_ecdsa_cleanup(threshold_ecdsa_ctx_t *ctx);

// ================================
// ARM TrustZone Integration
// ================================

typedef struct {
    void *secure_world_context;
    bool is_initialized;
} trustzone_ctx_t;

typedef enum {
    KEY_TYPE_AES128,
    KEY_TYPE_AES256,
    KEY_TYPE_CHACHA20,
    KEY_TYPE_ECDSA_P256,
    KEY_TYPE_ED25519
} key_type_t;

typedef struct {
    uint32_t handle_id;
    key_type_t type;
    bool is_secure;
} secure_key_handle_t;

// TrustZone Functions
int trustzone_init(trustzone_ctx_t *ctx);
int trustzone_store_key(trustzone_ctx_t *ctx, secure_key_handle_t *handle,
                        const uint8_t *key_data, key_type_t type);
int trustzone_encrypt(trustzone_ctx_t *ctx, secure_key_handle_t handle,
                      const uint8_t *plaintext, uint8_t *ciphertext);
int trustzone_decrypt(trustzone_ctx_t *ctx, secure_key_handle_t handle,
                      const uint8_t *ciphertext, uint8_t *plaintext);
void trustzone_cleanup(trustzone_ctx_t *ctx);

// ================================
// High-Performance Symmetric Crypto
// ================================

// ChaCha20-Poly1305 AEAD
int chacha20_poly1305_encrypt(uint8_t *ciphertext, size_t *ciphertext_len,
                              const uint8_t *plaintext, size_t plaintext_len,
                              const uint8_t *key, const uint8_t *nonce,
                              const uint8_t *aad, size_t aad_len);

int chacha20_poly1305_decrypt(uint8_t *plaintext, size_t *plaintext_len,
                              const uint8_t *ciphertext, size_t ciphertext_len,
                              const uint8_t *key, const uint8_t *nonce,
                              const uint8_t *aad, size_t aad_len);

// AES-GCM with ARM Crypto Extensions
int aes_gcm_encrypt_arm(uint8_t *ciphertext, size_t *ciphertext_len,
                        const uint8_t *plaintext, size_t plaintext_len,
                        const uint8_t *key, size_t key_len,
                        const uint8_t *iv, size_t iv_len,
                        const uint8_t *aad, size_t aad_len);

int aes_gcm_decrypt_arm(uint8_t *plaintext, size_t *plaintext_len,
                        const uint8_t *ciphertext, size_t ciphertext_len,
                        const uint8_t *key, size_t key_len,
                        const uint8_t *iv, size_t iv_len,
                        const uint8_t *aad, size_t aad_len);

// ================================
// Constant-Time Utilities
// ================================

// Constant-time memory comparison
int constant_time_memcmp(const void *a, const void *b, size_t len);

// Constant-time conditional select
void constant_time_select(uint8_t *dest, const uint8_t *src1, const uint8_t *src2,
                          size_t len, uint8_t condition);

// Secure memory operations
void secure_memzero(void *ptr, size_t len);
void *secure_malloc(size_t size);
void secure_free(void *ptr);

// ================================
// Performance Monitoring
// ================================

typedef struct {
    uint64_t cycles_start;
    uint64_t cycles_end;
    uint64_t instructions_start;
    uint64_t instructions_end;
    uint32_t cache_misses;
    uint32_t branch_misses;
} performance_counter_t;

void perf_counter_start(performance_counter_t *counter);
void perf_counter_stop(performance_counter_t *counter);
void perf_counter_print(const performance_counter_t *counter, const char *operation);

// ================================
// Error Codes
// ================================

#define ARM_CRYPTO_SUCCESS              0
#define ARM_CRYPTO_ERROR_INVALID_PARAM  -1
#define ARM_CRYPTO_ERROR_BUFFER_TOO_SMALL -2
#define ARM_CRYPTO_ERROR_MEMORY_ALLOCATION -3
#define ARM_CRYPTO_ERROR_VERIFICATION_FAILED -4
#define ARM_CRYPTO_ERROR_UNSUPPORTED_ALGORITHM -5
#define ARM_CRYPTO_ERROR_HARDWARE_NOT_SUPPORTED -6
#define ARM_CRYPTO_ERROR_TRUSTZONE_NOT_AVAILABLE -7

// ================================
// Version Information
// ================================

#define ARM_CRYPTO_VERSION_MAJOR 1
#define ARM_CRYPTO_VERSION_MINOR 0
#define ARM_CRYPTO_VERSION_PATCH 0

const char* arm_crypto_version_string(void);
bool arm_crypto_is_hardware_supported(void);
void arm_crypto_print_capabilities(void);

#ifdef __cplusplus
}
#endif

#endif // ARM_CRYPTO_H 