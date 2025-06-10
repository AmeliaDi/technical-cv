/*
 * AmeliaDebug - Advanced Debugger with ptrace and Disassembler
 * Author: Amelia Enora Marceline Chavez Barroso
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define DEBUGGER_VERSION "1.0.0"

typedef struct {
    pid_t target_pid;
    int attached;
    unsigned long breakpoints[100];
    int bp_count;
} debugger_t;

static debugger_t dbg = {0};

int attach_process(pid_t pid)
{
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        perror("ptrace attach failed");
        return -1;
    }
    
    dbg.target_pid = pid;
    dbg.attached = 1;
    
    printf("AmeliaDebug: Attached to process %d\n", pid);
    return 0;
}

int set_breakpoint(unsigned long addr)
{
    if (dbg.bp_count >= 100) return -1;
    
    // Save original instruction
    long orig = ptrace(PTRACE_PEEKTEXT, dbg.target_pid, addr, NULL);
    
    // Insert INT3 (0xCC)
    long trap = (orig & ~0xFF) | 0xCC;
    ptrace(PTRACE_POKETEXT, dbg.target_pid, addr, trap);
    
    dbg.breakpoints[dbg.bp_count++] = addr;
    printf("AmeliaDebug: Breakpoint set at 0x%lx\n", addr);
    
    return 0;
}

void disassemble_instruction(unsigned long addr)
{
    long instruction = ptrace(PTRACE_PEEKTEXT, dbg.target_pid, addr, NULL);
    printf("0x%lx: %02x %02x %02x %02x\n", addr,
           (int)(instruction & 0xFF),
           (int)((instruction >> 8) & 0xFF),
           (int)((instruction >> 16) & 0xFF),
           (int)((instruction >> 24) & 0xFF));
}

int main(int argc, char *argv[])
{
    printf("AmeliaDebug - Advanced Debugger v%s\n", DEBUGGER_VERSION);
    
    if (argc < 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        return 1;
    }
    
    pid_t target = atoi(argv[1]);
    
    if (attach_process(target) == -1) {
        return 1;
    }
    
    // Simple debugging loop
    char cmd[256];
    while (1) {
        printf("(ameliadebug) ");
        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        
        if (strncmp(cmd, "continue", 8) == 0) {
            ptrace(PTRACE_CONT, dbg.target_pid, NULL, NULL);
        } else if (strncmp(cmd, "step", 4) == 0) {
            ptrace(PTRACE_SINGLESTEP, dbg.target_pid, NULL, NULL);
        } else if (strncmp(cmd, "quit", 4) == 0) {
            break;
        }
    }
    
    ptrace(PTRACE_DETACH, dbg.target_pid, NULL, NULL);
    return 0;
} 