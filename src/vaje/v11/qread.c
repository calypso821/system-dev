#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* marsikaj */
#include <linux/errno.h> /* kode napak */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_*_user */
#include <linux/moduleparam.h>

#define DEVICE_NAME "qread"
#define MEM_SIZE 128
#define BLK_SIZE 32

// Module parameters
static unsigned long mem_size = MEM_SIZE;
static unsigned long blk_size = BLK_SIZE;

// Expose parameters to module loading
module_param(mem_size, ulong, 0644);
MODULE_PARM_DESC(mem_size, "Size of memory buffer (default 128 bytes)");
module_param(blk_size, ulong, 0644);
MODULE_PARM_DESC(blk_size, "Size of quantum read block (default 32 bytes)");

MODULE_LICENSE("GPL");

// Compile qread.ko
// sudo make -C /usr/src/linux-headers-$(uname -r) M=$(pwd) modules
// Makefile: obj-m := qread.o
// Load: sudo insmod ./qread.ko
// Unload: sudo rmmod ./qread.ko
// file: sudo mknod -m 666 /dev/qread c 510 0
// printk() - dmesg -w

// # Load with custom sizes
// sudo insmod qread.ko mem_size=25 blk_size=5
// # Or load with defaults
// sudo insmod qread.ko

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
    unsigned char *data;   // Buffer pointer
    unsigned Q;            // Quantum size (32B) - read chunk
    unsigned S;           // Buffer size (128B)
    unsigned size;        // Current amount of data in buffer
};

struct qread_dev *dev;
module_init(qread_init);
module_exit(qread_exit);


int qread_init(void)
{
    int result; 

    // Register device in kernel
    qread_major = register_chrdev(0, DEVICE_NAME, &fops);
    if (qread_major < 0) {
        printk(KERN_ALERT "Registration of character device failed.\n");
        return qread_major;
    }

    qread_minor = 0;
	printk(KERN_INFO "Qread: major %d, minor %d\n", qread_major, qread_minor);

    // Validate parameters
    if (mem_size == 0) {
        printk(KERN_WARNING "Invalid mem_size, using default %d\n", MEM_SIZE);
        mem_size = MEM_SIZE;
    }
    if (blk_size == 0 || blk_size > mem_size) {
        printk(KERN_WARNING "Invalid blk_size, using default %d\n", BLK_SIZE);
        blk_size = BLK_SIZE;
    }
	
    // Kernel memory allocation for device structure
	dev = kmalloc(sizeof(struct qread_dev), GFP_KERNEL);
	if (!dev)
    { 
        result = -ENOMEM;
        goto fail;
    }

    // Memory init (set zeros to newly allocated memory)
	memset(dev, 0, sizeof(struct qread_dev));

    // Allocate the buffer in kernel
    dev->data = kmalloc(mem_size, GFP_KERNEL);
    if (!dev->data) {
        result = -ENOMEM;
        goto fail;
    }

    dev->Q = blk_size;
    dev->S = mem_size;
    dev->size = 0;

    printk(KERN_INFO "Quantum read module initialized with buffer size: %lu, block size: %lu\n", 
           mem_size, blk_size);
	return 0;
	
fail:
    printk(KERN_ALERT "Module initialization failed\n");
    qread_exit();
	return result;
}

void qread_exit(void)
{
    printk(KERN_INFO "Module unloaded\n");
    if (dev) {
        // First free the buffer
        if (dev->data)          
            kfree(dev->data);
        // Frees dev structure memory allocated with kmalloc
		kfree(dev);
	}
    // Enregisters device (major) from kernel
	unregister_chrdev(qread_major, DEVICE_NAME);
}

int qread_open(struct inode *inode, struct file *filp)
{
    // Store device pointer for other operations
    filp->private_data = dev;  
    printk(KERN_INFO "Device open\n");
    return 0;
}

int qread_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Device close\n");
    return 0;
}

ssize_t qread_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    // retrieves the device structure that was stored during device open
    struct qread_dev *dev = filp->private_data;
    size_t quantum = dev->Q;
    ssize_t retval = 0;

    printk(KERN_DEBUG "Read-Start: pos: %lld, count: %zu, size: %u\n",
           *f_pos, count, dev->size);

    // Check if we're past end of data
    if (*f_pos >= dev->size)
    {
        printk(KERN_DEBUG "Read-Complete\n");
        goto out;
    }

    // Set max number of bytes to read
    if (count > dev->size - *f_pos)
        count = dev->size - *f_pos;

    // Limit read to quantum size
    if (count > quantum)
        count = quantum;  // Read maximum one quantum at a time

    // Copy to user space from kernel buffer at current position
    if (copy_to_user(buf, dev->data + *f_pos, count)) {
        retval = -EFAULT;
        goto out;
    }

    *f_pos += count;  // Update position
    retval = count;   // Return number of bytes read

    printk(KERN_DEBUG "Read-End: pos: %lld, read: %zu\n", 
           *f_pos, count);

out:
    return retval;
}

// Replace write
// ssize_t qread_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
// {
//     // retrieves the device structure that was stored during device open
//     struct qread_dev *dev = filp->private_data;
//     ssize_t retval = -ENOMEM;

//     printk(KERN_DEBUG "Write-Start: count: %zu\n", count);

//     if (count > dev->S) {
//         retval = -ENOSPC;  // No space for complete write
//         goto out;
//     }

//     // Copy data from user space to kernel space starting at beginning of buffer
//     // dev->data: Destination in kernel buffer (always start at beginning)
//     // buf: Source buffer in user space
//     // count: Number of bytes to copy
//     if (copy_from_user(dev->data, buf, count)) {
//         goto out;
//     }

//     // In replace mode, position and size are always same as write size
//     *f_pos = count;
//     dev->size = count;
//     retval = count;

//     printk(KERN_DEBUG "Write-End: written: %zu, size: %u\n", 
//            count, dev->size);

// out:
//     return retval;
// }

// Sequential write
ssize_t qread_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    // retrieves the device structure that was stored during device open
    struct qread_dev *dev = filp->private_data;
    ssize_t retval = -ENOMEM;
    size_t available_space;

    printk(KERN_DEBUG "Write-Start: pos: %lld, count: %zu, size: %u\n", 
           dev->size + *f_pos, count, dev->size);

    // Check if we're trying to write past the buffer end
    if (dev->size >= dev->S) {
        retval = -ENOSPC;  // No space for complete write
        goto out;
    }

    // Calculate available space (MaxSize - current size)
    available_space = dev->S - dev->size;
    if (count > available_space)
        count = available_space;  // Truncate the write to available space

    // Copy data from user space to kernel space
    // dev->data + *f_pos: Destination in kernel buffer, offset by current position
    // buf: Source buffer in user space
    // count: Number of bytes to copy
    if (copy_from_user(dev->data + dev->size + *f_pos, buff, count)) {
        retval = -EFAULT;
        goto out;
    }

    printk(KERN_DEBUG "Write-End: pos: %lld, written: %zu, size: %zu\n", 
        dev->size + *f_pos, count, dev->size + count);

    // Update current position, size
    *f_pos += count;
    dev->size += count;
    retval = count;

out:
    return retval;
}