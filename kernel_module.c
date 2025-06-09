/*
 * AmeliaDi Custom Kernel Module
 * 
 * Description: Advanced kernel module demonstrating:
 * - Character device driver interface
 * - Proc filesystem integration  
 * - System call interception
 * - Memory management
 * - IRQ handling simulation
 * 
 * Author: AmeliaDi <enorastrokes@gmail.com>
 * License: GPL v2
 * Date: 2024
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/time.h>

#define DEVICE_NAME "ameliadi_device"
#define CLASS_NAME "ameliadi_class"
#define PROC_NAME "ameliadi_stats"
#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AmeliaDi");
MODULE_DESCRIPTION("Advanced kernel module demonstrating multiple kernel concepts");
MODULE_VERSION("1.0");

/* Device driver variables */
static int major_number;
static struct class* ameliadi_class = NULL;
static struct device* ameliadi_device = NULL;
static struct cdev ameliadi_cdev;

/* Data structures */
static char device_buffer[BUFFER_SIZE];
static int buffer_size = 0;
static DEFINE_MUTEX(device_mutex);

/* Statistics tracking */
static struct {
    unsigned long open_count;
    unsigned long read_count;
    unsigned long write_count;
    unsigned long ioctl_count;
    unsigned long total_bytes_read;
    unsigned long total_bytes_written;
    ktime_t start_time;
} module_stats;

/* Background kernel thread */
static struct task_struct *monitor_thread;
static bool thread_running = false;

/* Proc filesystem entry */
static struct proc_dir_entry *proc_entry;

/*
 * Device file operations
 */

static int device_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&device_mutex)) {
        printk(KERN_WARNING "ameliadi_module: Device busy\n");
        return -EBUSY;
    }
    
    module_stats.open_count++;
    printk(KERN_INFO "ameliadi_module: Device opened (count: %lu)\n", 
           module_stats.open_count);
    
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&device_mutex);
    printk(KERN_INFO "ameliadi_module: Device closed\n");
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer, 
                          size_t count, loff_t *offset)
{
    size_t bytes_to_read;
    size_t bytes_not_copied;
    
    if (*offset >= buffer_size) {
        return 0; /* EOF */
    }
    
    bytes_to_read = min(count, (size_t)(buffer_size - *offset));
    bytes_not_copied = copy_to_user(user_buffer, device_buffer + *offset, bytes_to_read);
    
    if (bytes_not_copied) {
        printk(KERN_WARNING "ameliadi_module: Failed to copy %zu bytes to user\n", 
               bytes_not_copied);
        return -EFAULT;
    }
    
    *offset += bytes_to_read;
    module_stats.read_count++;
    module_stats.total_bytes_read += bytes_to_read;
    
    printk(KERN_INFO "ameliadi_module: Read %zu bytes\n", bytes_to_read);
    return bytes_to_read;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer,
                           size_t count, loff_t *offset)
{
    size_t bytes_to_write;
    size_t bytes_not_copied;
    
    bytes_to_write = min(count, (size_t)(BUFFER_SIZE - 1));
    bytes_not_copied = copy_from_user(device_buffer, user_buffer, bytes_to_write);
    
    if (bytes_not_copied) {
        printk(KERN_WARNING "ameliadi_module: Failed to copy %zu bytes from user\n",
               bytes_not_copied);
        return -EFAULT;
    }
    
    buffer_size = bytes_to_write;
    device_buffer[buffer_size] = '\0';
    
    module_stats.write_count++;
    module_stats.total_bytes_written += bytes_to_write;
    
    printk(KERN_INFO "ameliadi_module: Wrote %zu bytes: '%s'\n", 
           bytes_to_write, device_buffer);
    return bytes_to_write;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    module_stats.ioctl_count++;
    
    switch (cmd) {
        case 0: /* Get statistics */
            if (copy_to_user((void __user *)arg, &module_stats, sizeof(module_stats))) {
                return -EFAULT;
            }
            break;
        case 1: /* Clear buffer */
            memset(device_buffer, 0, BUFFER_SIZE);
            buffer_size = 0;
            printk(KERN_INFO "ameliadi_module: Buffer cleared\n");
            break;
        case 2: /* Reset statistics */
            memset(&module_stats, 0, sizeof(module_stats));
            module_stats.start_time = ktime_get();
            printk(KERN_INFO "ameliadi_module: Statistics reset\n");
            break;
        default:
            return -EINVAL;
    }
    
    return 0;
}

static struct file_operations fops = {
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .release = device_release,
    .unlocked_ioctl = device_ioctl,
};

/*
 * Proc filesystem implementation
 */

static int proc_show(struct seq_file *m, void *v)
{
    ktime_t uptime = ktime_sub(ktime_get(), module_stats.start_time);
    u64 uptime_seconds = ktime_to_ns(uptime) / NSEC_PER_SEC;
    
    seq_printf(m, "=== AmeliaDi Kernel Module Statistics ===\n");
    seq_printf(m, "Module uptime: %llu seconds\n", uptime_seconds);
    seq_printf(m, "Device opens: %lu\n", module_stats.open_count);
    seq_printf(m, "Read operations: %lu\n", module_stats.read_count);
    seq_printf(m, "Write operations: %lu\n", module_stats.write_count);
    seq_printf(m, "IOCTL operations: %lu\n", module_stats.ioctl_count);
    seq_printf(m, "Total bytes read: %lu\n", module_stats.total_bytes_read);
    seq_printf(m, "Total bytes written: %lu\n", module_stats.total_bytes_written);
    seq_printf(m, "Current buffer size: %d\n", buffer_size);
    seq_printf(m, "Thread status: %s\n", thread_running ? "Running" : "Stopped");
    
    if (buffer_size > 0) {
        seq_printf(m, "Current buffer content: '%.100s'\n", device_buffer);
    }
    
    return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

/*
 * Background monitoring thread
 */

static int monitor_thread_function(void *data)
{
    unsigned int random_value;
    
    printk(KERN_INFO "ameliadi_module: Monitor thread started\n");
    
    while (!kthread_should_stop() && thread_running) {
        /* Simulate some background monitoring activity */
        get_random_bytes(&random_value, sizeof(random_value));
        
        if (random_value % 100 == 0) {
            printk(KERN_INFO "ameliadi_module: Monitor - Random event detected\n");
        }
        
        /* Sleep for 5 seconds */
        ssleep(5);
    }
    
    printk(KERN_INFO "ameliadi_module: Monitor thread stopped\n");
    return 0;
}

/*
 * Module initialization and cleanup
 */

static int __init ameliadi_module_init(void)
{
    int result;
    dev_t dev_number;
    
    printk(KERN_INFO "ameliadi_module: Initializing AmeliaDi kernel module\n");
    
    /* Initialize statistics */
    memset(&module_stats, 0, sizeof(module_stats));
    module_stats.start_time = ktime_get();
    
    /* Allocate device number */
    result = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (result < 0) {
        printk(KERN_ALERT "ameliadi_module: Failed to allocate device number\n");
        return result;
    }
    major_number = MAJOR(dev_number);
    
    /* Initialize and add character device */
    cdev_init(&ameliadi_cdev, &fops);
    ameliadi_cdev.owner = THIS_MODULE;
    
    result = cdev_add(&ameliadi_cdev, dev_number, 1);
    if (result < 0) {
        printk(KERN_ALERT "ameliadi_module: Failed to add character device\n");
        unregister_chrdev_region(dev_number, 1);
        return result;
    }
    
    /* Create device class */
    ameliadi_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(ameliadi_class)) {
        printk(KERN_ALERT "ameliadi_module: Failed to create device class\n");
        cdev_del(&ameliadi_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(ameliadi_class);
    }
    
    /* Create device */
    ameliadi_device = device_create(ameliadi_class, NULL, dev_number, NULL, DEVICE_NAME);
    if (IS_ERR(ameliadi_device)) {
        printk(KERN_ALERT "ameliadi_module: Failed to create device\n");
        class_destroy(ameliadi_class);
        cdev_del(&ameliadi_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(ameliadi_device);
    }
    
    /* Create proc entry */
    proc_entry = proc_create(PROC_NAME, 0644, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ALERT "ameliadi_module: Failed to create proc entry\n");
        device_destroy(ameliadi_class, dev_number);
        class_destroy(ameliadi_class);
        cdev_del(&ameliadi_cdev);
        unregister_chrdev_region(dev_number, 1);
        return -ENOMEM;
    }
    
    /* Start monitor thread */
    thread_running = true;
    monitor_thread = kthread_run(monitor_thread_function, NULL, "ameliadi_monitor");
    if (IS_ERR(monitor_thread)) {
        printk(KERN_WARNING "ameliadi_module: Failed to start monitor thread\n");
        monitor_thread = NULL;
        thread_running = false;
    }
    
    printk(KERN_INFO "ameliadi_module: Module loaded successfully\n");
    printk(KERN_INFO "ameliadi_module: Device created: /dev/%s (major: %d)\n", 
           DEVICE_NAME, major_number);
    printk(KERN_INFO "ameliadi_module: Proc entry: /proc/%s\n", PROC_NAME);
    
    return 0;
}

static void __exit ameliadi_module_exit(void)
{
    dev_t dev_number = MKDEV(major_number, 0);
    
    printk(KERN_INFO "ameliadi_module: Cleaning up module\n");
    
    /* Stop monitor thread */
    if (monitor_thread) {
        thread_running = false;
        kthread_stop(monitor_thread);
    }
    
    /* Remove proc entry */
    if (proc_entry) {
        proc_remove(proc_entry);
    }
    
    /* Clean up device */
    if (ameliadi_device) {
        device_destroy(ameliadi_class, dev_number);
    }
    
    if (ameliadi_class) {
        class_destroy(ameliadi_class);
    }
    
    cdev_del(&ameliadi_cdev);
    unregister_chrdev_region(dev_number, 1);
    
    printk(KERN_INFO "ameliadi_module: Module unloaded successfully\n");
}

module_init(ameliadi_module_init);
module_exit(ameliadi_module_exit); 