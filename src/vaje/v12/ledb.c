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

#define DEVICE_NAME "ledbblink"

MODULE_LICENSE("GPL");

// Compile ledb.ko
// sudo make -C /usr/src/linux-headers-$(uname -r) M=$(pwd) modules
// Makefile: obj-m := ledb.o
// Load: sudo insmod ./ledb.ko
// Unload: sudo rmmod ./ledb.ko
// file: sudo mknod -m 666 /dev/ledb c 510 0
// printk() - dmesg -w

// # Load with custom sizes
// sudo insmod ledb.ko mem_size=25 blk_size=5
// # Or load with defaults
// sudo insmod ledb.ko

// Change these function declarations
static int __init ledb_init(void);
static void __exit ledb_exit(void);
int ledb_open(struct inode *, struct file *);
int ledb_release(struct inode *, struct file *);
ssize_t ledb_read(struct file *, char *, size_t, loff_t *);
ssize_t ledb_write(struct file *, const char *, size_t, loff_t *);

struct file_operations fops = {
    .open = ledb_open,
    .release = ledb_release,
    .read = ledb_read,
    .write = ledb_write
};

int ledb_major;
int ledb_minor;

struct ledb_dev {
    unsigned char *data;   // Buffer pointer
    unsigned Q;            // Quantum size (32B) - read chunk
    unsigned S;           // Buffer size (128B)
    unsigned size;        // Current amount of data in buffer
};

struct ledb_dev *dev;
module_init(ledb_init);
module_exit(ledb_exit);

struct timer_list my_timer; 
static unsigned long timer_interval = 2000;

int LEDon = 0;
int SWITCHon = 0;

// Timer callback function
static void timer_callback(struct timer_list *t)
{

    LEDon = !LEDon; 
    // !1, !2 -> None zero -> To Zero
    // !0 -> Zero to 1 

    // Restart timer
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_interval));

    printk(KERN_DEBUG "Timer execute: LEDon %d", LEDon);
}


int ledb_init(void)
{
    int result; 

    // Register device in kernel
    ledb_major = register_chrdev(0, DEVICE_NAME, &fops);
    if (ledb_major < 0) {
        printk(KERN_ALERT "Registration of character device failed.\n");
        return ledb_major;
    }

    ledb_minor = 0;
	printk(KERN_INFO "Ledb: major %d, minor %d\n", ledb_major, ledb_minor);
	
    // Kernel memory allocation for device structure
	dev = kmalloc(sizeof(struct ledb_dev), GFP_KERNEL);
	if (!dev)
    { 
        result = -ENOMEM;
        goto fail;
    }

    // Memory init (set zeros to newly allocated memory)
	memset(dev, 0, sizeof(struct ledb_dev));

    // Setup timer
    timer_setup(&my_timer, timer_callback, 0);

    printk(KERN_INFO "LED blink module initialized\n");
	return 0;
	
fail:
    printk(KERN_ALERT "Module initialization failedb\n");
    ledb_exit();
	return result;
}

void ledb_exit(void)
{
    // Clean timer (wait for callback to finish)
    del_timer_sync(&my_timer);

    printk(KERN_INFO "Module unloaded\n");
    if (dev) {
        // Frees dev structure memory allocated with kmalloc
		kfree(dev);
	}

    // Enregisters device (major) from kernel
	unregister_chrdev(ledb_major, DEVICE_NAME);
}

int ledb_open(struct inode *inode, struct file *filp)
{
    // Store device pointer for other operations
    filp->private_data = dev;  
    printk(KERN_INFO "Device open\n");
    return 0;
}

int ledb_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Device close\n");
    return 0;
}

ssize_t ledb_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
//     // retrieves the device structure that was stored during device open
//     struct ledb_dev *dev = filp->private_data;
//     size_t quantum = dev->Q;
//     ssize_t retval = 0;

//     printk(KERN_DEBUG "Read-Start: pos: %lld, count: %zu, size: %u\n",
//            *f_pos, count, dev->size);

//     // Check if we're past end of data
//     if (*f_pos >= dev->size)
//     {
//         printk(KERN_DEBUG "Read-Complete\n");
//         goto out;
//     }

//     // Set max number of bytes to read
//     if (count > dev->size - *f_pos)
//         count = dev->size - *f_pos;

//     // Limit read to quantum size
//     if (count > quantum)
//         count = quantum;  // Read maximum one quantum at a time

//     // Copy to user space from kernel buffer at current position
//     if (copy_to_user(buf, dev->data + *f_pos, count)) {
//         retval = -EFAULT;
//         goto out;
//     }

//     *f_pos += count;  // Update position
//     retval = count;   // Return number of bytes read

//     printk(KERN_DEBUG "Read-End: pos: %lld, read: %zu\n", 
//            *f_pos, count);

// out:
//     return retval;

    return 0;
}

ssize_t ledb_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    unsigned char value; // buffer
    ssize_t retval = -ENOMEM;

    // We expect only 1 byte
    if (count != 1)
        return -EINVAL;

    // Get the value from user space
    if (copy_from_user(&value, buf, 1)) {
        retval = -EFAULT;
        goto out;
    }

    // Handle different cases
    switch(value) {
        case 0:
            // Stop LED blinking
            if (SWITCHon == 0)
                del_timer(&my_timer); // stops timer
            break;

        case 1:
            // Start LED blinking with 0.5s interval
            if (SWITCHon == 0)
            {
                timer_interval = 500;  // 0.5 seconds
                mod_timer(&my_timer, jiffies);
            }
            break;

        case 2:
            // Set SWITCH to 0
            SWITCHon = 0;
            del_timer(&my_timer); // stops timer
            break;

        case 3:
            // Set SWITCH to 1 (2s timer)
            SWITCHon = 1;
            timer_interval = 2000;
            mod_timer(&my_timer, jiffies);
            break;

        default:
            return -EINVAL;
    }

    // Print status after switch
    printk(KERN_DEBUG "Status: Switch: %d, Timer: %s, Interval: %lu\n",
           SWITCHon,
           timer_pending(&my_timer) ? "active" : "not active",
           timer_interval);

    retval = count;
out:
    return retval;
}