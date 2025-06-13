OPENQASM 3;
include "stdgates.inc";

/*
 * BB84 Quantum Key Distribution Protocol
 * 
 * This implementation demonstrates the famous BB84 protocol for quantum key distribution,
 * providing information-theoretic security for cryptographic key exchange.
 * 
 * Protocol Flow:
 * 1. Alice generates random bits and bases
 * 2. Alice prepares quantum states based on bits and bases
 * 3. Quantum transmission to Bob
 * 4. Bob randomly chooses measurement bases
 * 5. Bob measures received qubits
 * 6. Classical communication for basis reconciliation
 * 7. Key sifting and error detection
 * 
 * Security: Based on no-cloning theorem and uncertainty principle
 */

// Protocol parameters
const key_length = 64;           // Desired key length
const total_bits = key_length * 4; // Account for basis mismatch and errors

// Quantum registers
qubit[total_bits] alice_qubits;  // Alice's preparation qubits
qubit[total_bits] channel;       // Quantum channel (transmission)
bit[total_bits] alice_bits;      // Alice's random bits
bit[total_bits] alice_bases;     // Alice's random bases (0=Z, 1=X)
bit[total_bits] bob_bases;       // Bob's random bases
bit[total_bits] bob_results;     // Bob's measurement results

// === ALICE'S PREPARATION PHASE ===

// Generate truly random bits using quantum superposition
for i in [0:total_bits-1] {
    // Generate random bit (0 or 1)
    h alice_qubits[i];
    alice_bits[i] = measure alice_qubits[i];
    
    // Generate random basis choice (Z=0 or X=1)
    h alice_qubits[i];
    alice_bases[i] = measure alice_qubits[i];
}

// Prepare qubits based on bits and bases
for i in [0:total_bits-1] {
    // Reset qubit to |0⟩
    reset alice_qubits[i];
    
    // Encode bit: |0⟩ for bit=0, |1⟩ for bit=1
    if (alice_bits[i] == 1) {
        x alice_qubits[i];
    }
    
    // Apply basis rotation: Z basis = computational, X basis = Hadamard
    if (alice_bases[i] == 1) {
        h alice_qubits[i];
    }
    
    // Transfer to quantum channel (in real implementation, this is physical transmission)
    channel[i] = alice_qubits[i];
}

// === BOB'S MEASUREMENT PHASE ===

// Bob randomly chooses measurement bases
for i in [0:total_bits-1] {
    h alice_qubits[i];  // Reuse alice_qubits for randomness
    bob_bases[i] = measure alice_qubits[i];
}

// Bob measures received qubits in chosen bases
for i in [0:total_bits-1] {
    // Apply basis rotation if measuring in X basis
    if (bob_bases[i] == 1) {
        h channel[i];
    }
    
    // Measure qubit
    bob_results[i] = measure channel[i];
}

// === CLASSICAL POST-PROCESSING ===
// In a real implementation, this would be done classically:
// 1. Alice and Bob announce their bases publicly
// 2. Keep only bits where bases match
// 3. Use subset for error detection
// 4. Apply privacy amplification
// 5. Final key is information-theoretically secure

/*
 * Expected Results:
 * - When bases match: bob_results[i] == alice_bits[i] (perfect correlation)
 * - When bases mismatch: bob_results[i] is random (50% correlation)
 * - Security: Eavesdropping introduces detectable errors
 * 
 * Key Rate: Approximately 25% of transmitted bits become key
 * Security: Information-theoretic (unconditional)
 */ 