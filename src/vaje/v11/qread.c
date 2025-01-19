#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* marsikaj */
#include <linux/errno.h> /* kode napak */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_*_user */

#define DEVICE_NAME "qread"
#define MEM_SIZE 128
#define BLK_SIZE 32

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
static int __init qread_init(void);
static void __exit qread_exit(void);
int qread_open(struct inode *, struct file *);
int qread_release(struct inode *, struct file *);
ssize_t qread_read(struct file *, char *, size_t, loff_t *);
ssize_t qread_write(struct file *, const char *, size_t, loff_t *);

struct file_operations fops = {
    .open = qread_open,
    .release = qread_release,
    .read = qread_read,
    .write = qread_write
};

int qread_major;
int qread_minor;

struct qread_dev {
	void **data;
	unsigned Q;
	unsigned S;
	unsigned size;
};

struct qread_dev *qread_dev_ptr;
module_init(qread_init);
module_exit(qread_exit);


static int qread_init(void)
{
    int result; 

    qread_major = register_chrdev(0, DEVICE_NAME, &fops);
    if (qread_major < 0) {
        printk(KERN_ALERT "Registration of character device failed.\n");
        return qread_major;
    }

    qread_minor = 0;
	printk(KERN_INFO "qread: major %d, minor %d\n", qread_major, qread_minor);
	
	sc1_dev_ptr = kmalloc(sizeof(struct sc1_dev), GFP_KERNEL);
	if (!sc1_dev_ptr) { result = -ENOMEM; goto fail; }
    // Memory init
	memset(sc1_dev_ptr, 0, sizeof(struct sc1_dev));
	
	sc1_dev_ptr->Q = SC1_QUANTUM_SIZE;
	sc1_dev_ptr->S = SC1_QSET_SIZE;
	
	return 0;
	
	fail:
	sc1_cleanup();
    printk("Module initialized\n");
	return result;
}

// Renamed exit function
static void qread_exit(void)
{
    printk("Module unloaded\n");
    unregister_chrdev(Major, DEVICE_NAME);
}

// Renamed open function
int qread_open(struct inode *inode, struct file *file)
{
    open_count++;  // Increment counter
    printk("Device open\n");
    return 0;
}

// Renamed release function
int qread_release(struct inode *inode, struct file *file)
{
    printk("Device close\n");
    return 0;
}

// Renamed read function
ssize_t qread_read(struct file *filp, char __user *buff, size_t len, loff_t *offset)
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
ssize_t dqread_write(struct file *filp, const char __user *buff, size_t len, loff_t *offset)
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