#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mymodule"
MODULE_LICENSE("GPL");

// Compile my_module.ko
// sudo make -C /usr/src/linux-headers-$(uname -r) M=$(pwd) modules
// Makefile: obj-m := my_module.o
// Load: sudo insmod ./my_module.ko
// Unload: sudo rmmod ./my_module.ko
// file: sudo mknod -m 666 /dev/my_module c 510 0
// printk() - dmesg

// Add global counter
static int open_count = 0;
// Add buffer for our message
static char message[50];

// Change these function declarations
static int my_init(void);
static void my_exit(void);
int device_open(struct inode *, struct file *);
int device_release(struct inode *, struct file *);
ssize_t device_read(struct file *, char *, size_t, loff_t *);
ssize_t device_write(struct file *, const char *, size_t, loff_t *);

int Major;

struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write
};

// Use the new function names here
module_init(my_init);
module_exit(my_exit);

// Renamed init function
static int my_init(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registration of character device failed.\n");
        return Major;
    }
    printk("Module intitialized\n");
    printk(KERN_INFO "Major number is %d.\n", Major);
    return 0;
}

// Renamed exit function
static void my_exit(void)
{
    printk("Module unloaded\n");
    unregister_chrdev(Major, DEVICE_NAME);
}

// Renamed open function
int device_open(struct inode *inode, struct file *file)
{
    open_count++;  // Increment counter
    printk("Device open\n");
    return 0;
}

// Renamed release function
int device_release(struct inode *inode, struct file *file)
{
    printk("Device close\n");
    return 0;
}

// Renamed read function
ssize_t device_read(struct file *filp, char __user *buff, size_t len, loff_t *offset)
{
    // Format our message with current count
    snprintf(message, sizeof(message), "Odprt sem bil %d-krat.\n", open_count);
    int size = strlen(message);

    // char *msg = "Hello!\n";
    // int size = strlen(msg);

    printk("Device read\n");

    if (*offset >= size)
        return 0;
    if (len > size - *offset)
        len = size - *offset;
    if (copy_to_user(buff, message, len))
        return -EFAULT;
    *offset += len;
    return len;
}

// Renamed write function
ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *offset)
{
    char kernel_buffer[100];  // Buffer to store data in kernel space
    
    printk("Device write\n");
    
    // Make sure we don't overflow our buffer
    if (len > sizeof(kernel_buffer) - 1)
        len = sizeof(kernel_buffer) - 1;
        
    // Copy from user space to kernel space
    if (copy_from_user(kernel_buffer, buff, len))
        return -EFAULT;
        
    // Null terminate the string
    kernel_buffer[len] = '\0';
    
    // Print what was written
    printk("Written data: %s\n", kernel_buffer);
    
    // Return number of bytes written
    return len;
}