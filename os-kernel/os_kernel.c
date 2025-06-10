/*
 * AmeliaOS - Microkernel with IPC and CFS Scheduling
 * Author: Amelia Enora Marceline Chavez Barroso
 */

#include <stdint.h>
#include <stddef.h>

#define KERNEL_VERSION "1.0.0"
#define MAX_PROCESSES 256
#define QUANTUM_MS 10

// Process states
typedef enum {
    PROC_READY,
    PROC_RUNNING,
    PROC_BLOCKED,
    PROC_TERMINATED
} proc_state_t;

// Process structure
typedef struct process {
    uint32_t pid;
    proc_state_t state;
    uint64_t vruntime;
    uint32_t priority;
    void *stack_ptr;
    struct process *next;
} process_t;

// CFS scheduler
typedef struct {
    process_t *runqueue;
    uint64_t min_vruntime;
    uint32_t nr_running;
} cfs_scheduler_t;

static cfs_scheduler_t scheduler = {0};
static process_t processes[MAX_PROCESSES];
static uint32_t next_pid = 1;

// Kernel entry point
void kernel_main(void)
{
    // Initialize kernel
    init_scheduler();
    init_ipc();
    
    // Start first process
    create_process(init_process);
    
    // Main scheduler loop
    while (1) {
        schedule();
    }
}

void init_scheduler(void)
{
    scheduler.runqueue = NULL;
    scheduler.min_vruntime = 0;
    scheduler.nr_running = 0;
}

process_t *create_process(void (*entry)(void))
{
    if (next_pid >= MAX_PROCESSES) return NULL;
    
    process_t *proc = &processes[next_pid];
    proc->pid = next_pid++;
    proc->state = PROC_READY;
    proc->vruntime = scheduler.min_vruntime;
    proc->priority = 120; // Default priority
    
    // Add to runqueue
    enqueue_process(proc);
    
    return proc;
}

void enqueue_process(process_t *proc)
{
    proc->next = scheduler.runqueue;
    scheduler.runqueue = proc;
    scheduler.nr_running++;
}

void schedule(void)
{
    process_t *next = pick_next_task();
    if (next) {
        context_switch(next);
    }
}

process_t *pick_next_task(void)
{
    process_t *leftmost = NULL;
    process_t *current = scheduler.runqueue;
    
    while (current) {
        if (!leftmost || current->vruntime < leftmost->vruntime) {
            leftmost = current;
        }
        current = current->next;
    }
    
    return leftmost;
}

void context_switch(process_t *proc)
{
    proc->state = PROC_RUNNING;
    // Switch to process context
    // Update vruntime
    proc->vruntime += QUANTUM_MS;
}

// IPC system
typedef struct message {
    uint32_t sender;
    uint32_t receiver;
    uint32_t type;
    uint8_t data[256];
} message_t;

void init_ipc(void) {}

int send_message(uint32_t dest_pid, message_t *msg)
{
    // IPC implementation
    return 0;
}

int receive_message(message_t *msg)
{
    // IPC implementation
    return 0;
}

void init_process(void)
{
    // First user process
    while (1) {
        // Do work
    }
} 