/*
 * kernel_module.c - Custom Linux Kernel Module
 * Author: Amelia Enora Marceline Chavez Barroso
 * Description: A simple but functional kernel module demonstrating:
 *              - Module initialization and cleanup
 *              - proc filesystem interface
 *              - Parameter passing
 *              - Basic I/O operations
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PROCFS_NAME    "amelia_module"
#define PROCFS_MAX_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amelia Enora Marceline Chavez Barroso");
MODULE_DESCRIPTION("Custom kernel module for system monitoring and control");
MODULE_VERSION("1.0");

/* Module parameters */
static int debug_level = 1;
static char *greeting = "Hello from Amelia's kernel module!";

module_param(debug_level, int, S_IRUGO);
MODULE_PARM_DESC(debug_level, "Debug level (0=quiet, 1=normal, 2=verbose)");

module_param(greeting, charp, S_IRUGO);
MODULE_PARM_DESC(greeting, "Greeting message");

/* Global variables */
static struct proc_dir_entry *proc_entry;
static char proc_buffer[PROCFS_MAX_SIZE];
static unsigned long proc_buffer_size = 0;

/* System information structure */
struct system_info {
    unsigned long uptime;
    unsigned long free_mem;
    unsigned long total_mem;
    int cpu_count;
};

/* Function to gather system information */
static void get_system_info(struct system_info *info)
{
    struct sysinfo si;
    
    si_meminfo(&si);
    info->total_mem = si.totalram * si.mem_unit / 1024; // KB
    info->free_mem = si.freeram * si.mem_unit / 1024;   // KB
    info->cpu_count = num_online_cpus();
    info->uptime = get_seconds();
}

/* Proc file read operation */
static ssize_t procfile_read(struct file *file, char __user *buffer, 
                           size_t count, loff_t *pos)
{
    struct system_info sysinfo;
    int len;
    char temp_buffer[512];
    
    if (*pos > 0) {
        return 0;
    }
    
    get_system_info(&sysinfo);
    
    len = snprintf(temp_buffer, sizeof(temp_buffer),
        "=== Amelia's Kernel Module Status ===\n"
        "Module: %s v%s\n"
        "Author: %s\n"
        "Greeting: %s\n"
        "Debug Level: %d\n"
        "\n=== System Information ===\n"
        "Uptime: %lu seconds\n"
        "Total Memory: %lu KB\n"
        "Free Memory: %lu KB\n"
        "CPU Count: %d\n"
        "Kernel Version: %s\n",
        PROCFS_NAME, "1.0",
        "Amelia Enora Marceline Chavez Barroso",
        greeting, debug_level,
        sysinfo.uptime, sysinfo.total_mem, 
        sysinfo.free_mem, sysinfo.cpu_count,
        UTS_RELEASE);
    
    if (len > count) {
        len = count;
    }
    
    if (copy_to_user(buffer, temp_buffer, len)) {
        return -EFAULT;
    }
    
    *pos += len;
    
    if (debug_level >= 1) {
        printk(KERN_INFO "amelia_module: proc read, %d bytes\n", len);
    }
    
    return len;
}

/* Proc file write operation */
static ssize_t procfile_write(struct file *file, const char __user *buffer,
                            size_t count, loff_t *pos)
{
    if (count > PROCFS_MAX_SIZE - 1) {
        count = PROCFS_MAX_SIZE - 1;
    }
    
    if (copy_from_user(proc_buffer, buffer, count)) {
        return -EFAULT;
    }
    
    proc_buffer[count] = '\0';
    proc_buffer_size = count;
    
    if (debug_level >= 1) {
        printk(KERN_INFO "amelia_module: received %lu bytes: %s\n", 
               count, proc_buffer);
    }
    
    /* Parse simple commands */
    if (strncmp(proc_buffer, "debug=", 6) == 0) {
        int new_level;
        if (kstrtoint(proc_buffer + 6, 10, &new_level) == 0) {
            debug_level = new_level;
            printk(KERN_INFO "amelia_module: debug level changed to %d\n", 
                   debug_level);
        }
    }
    
    return count;
}

/* Proc file operations structure */
static const struct proc_ops proc_fops = {
    .proc_read = procfile_read,
    .proc_write = procfile_write,
};

/* Module initialization function */
static int __init amelia_module_init(void)
{
    printk(KERN_INFO "=== Amelia's Kernel Module Loading ===\n");
    printk(KERN_INFO "amelia_module: %s\n", greeting);
    printk(KERN_INFO "amelia_module: Debug level set to %d\n", debug_level);
    
    /* Create proc entry */
    proc_entry = proc_create(PROCFS_NAME, 0666, NULL, &proc_fops);
    if (proc_entry == NULL) {
        printk(KERN_ALERT "amelia_module: Error creating proc entry\n");
        return -ENOMEM;
    }
    
    printk(KERN_INFO "amelia_module: /proc/%s created successfully\n", PROCFS_NAME);
    printk(KERN_INFO "amelia_module: Module loaded successfully!\n");
    
    /* Display system info at load time */
    if (debug_level >= 2) {
        struct system_info info;
        get_system_info(&info);
        printk(KERN_INFO "amelia_module: System - CPUs: %d, Free RAM: %lu KB\n",
               info.cpu_count, info.free_mem);
    }
    
    return 0;
}

/* Module cleanup function */
static void __exit amelia_module_exit(void)
{
    printk(KERN_INFO "=== Amelia's Kernel Module Unloading ===\n");
    
    /* Remove proc entry */
    if (proc_entry) {
        proc_remove(proc_entry);
        printk(KERN_INFO "amelia_module: /proc/%s removed\n", PROCFS_NAME);
    }
    
    printk(KERN_INFO "amelia_module: Module unloaded successfully!\n");
    printk(KERN_INFO "amelia_module: Thank you for using Amelia's kernel module!\n");
}

/* Register module functions */
module_init(amelia_module_init);
module_exit(amelia_module_exit); 