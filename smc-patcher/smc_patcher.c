/*
 * AmeliaSMC - Self-Modifying Code Engine with Anti-Debug
 * Author: Amelia Enora Marceline Chavez Barroso
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

#define SMC_VERSION "1.0.0"

// Anti-debug techniques
int detect_debugger(void)
{
    // Check for ptrace
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
        return 1; // Debugger detected
    }
    
    // Check for breakpoints
    uint8_t *code = (uint8_t *)detect_debugger;
    for (int i = 0; i < 100; i++) {
        if (code[i] == 0xCC) { // INT3 breakpoint
            return 1;
        }
    }
    
    return 0;
}

// Self-modifying code
void modify_code(void *addr, uint8_t *new_code, size_t len)
{
    // Make memory writable
    mprotect((void *)((uintptr_t)addr & ~0xFFF), 4096, 
             PROT_READ | PROT_WRITE | PROT_EXEC);
    
    // Modify the code
    memcpy(addr, new_code, len);
    
    // Restore protection
    mprotect((void *)((uintptr_t)addr & ~0xFFF), 4096, 
             PROT_READ | PROT_EXEC);
}

// Polymorphic function
void polymorphic_function(void)
{
    printf("Original function\n");
    
    // Self-modify to change behavior
    uint8_t new_code[] = {
        0x48, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rdi, string
        0xE8, 0x00, 0x00, 0x00, 0x00, // call puts
        0xC3 // ret
    };
    
    static int modified = 0;
    if (!modified) {
        modify_code(polymorphic_function, new_code, sizeof(new_code));
        modified = 1;
    }
}

int main(void)
{
    printf("AmeliaSMC - Self-Modifying Code Engine v%s\n", SMC_VERSION);
    
    if (detect_debugger()) {
        printf("Debugger detected! Exiting...\n");
        return 1;
    }
    
    printf("Calling polymorphic function:\n");
    polymorphic_function();
    
    printf("Calling again (should be modified):\n");
    polymorphic_function();
    
    return 0;
} 