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
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define DEVICE_NAME "rpiledb"

MODULE_LICENSE("GPL");

// Compile rpiledb.ko
// sudo make -C /usr/src/linux-headers-$(uname -r) M=$(pwd) modules
// Makefile: obj-m := rpiledb.o
// Load: sudo insmod ./rpiledb.ko
// Unload: sudo rmmod ./rpiledb.ko
// file: sudo mknod -m 666 /dev/rpiledb c 510 0
// printk() - dmesg -w

// # Load with custom sizes
// sudo insmod rpiledb.ko mem_size=25 blk_size=5
// # Or load with defaults
// sudo insmod rpiledb.ko

// Change these function declarations
static int __init rpiledb_init(void);
static void __exit rpiledb_exit(void);
int rpiledb_open(struct inode *, struct file *);
int rpiledb_release(struct inode *, struct file *);
ssize_t rpiledb_read(struct file *, char *, size_t, loff_t *);
ssize_t rpiledb_write(struct file *, const char *, size_t, loff_t *);

struct file_operations fops = {
    .open = rpiledb_open,
    .release = rpiledb_release,
    .read = rpiledb_read,
    .write = rpiledb_write
};

int rpiledb_major;
int rpiledb_minor;

module_init(rpiledb_init);
module_exit(rpiledb_exit);

static void __iomem *gpio_base;        // Virtual base address
static bool got_mem_region = false;    // Track if we got memory region
static unsigned long GPIO_BASE = 0x3F200000;  // Physical base address
static unsigned long GPIO_SIZE = 0xB4;        // Size of memory region

struct timer_list my_timer; 

int LEDon = 0;
int SWITCHon = 0;
bool active = true;

// Timer callback function
static void timer_callback(struct timer_list *t)
{
    LEDon = !LEDon;  // Toggle LED state
    
    // Set next timer interval based on SWITCHon state
    if (SWITCHon == 1) { // SWITCH is ON
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));  // 2s interval
    } else if (active) { // SWITCH is OFF + active
        mod_timer(&my_timer, jiffies + msecs_to_jiffies(500));   // 0.5s interval
    } else { // SWITCH is OFF + not active
        LEDon = 1;  // Set LED on if not active
        printk(KERN_DEBUG "LED on");
        return;     // Don't restart timer
    }

    printk(KERN_DEBUG "Timer execute: LEDon %d", LEDon);
}


int rpiledb_init(void)
{
    int result; 

    // Register device in kernel
    rpiledb_major = register_chrdev(0, DEVICE_NAME, &fops);
    if (rpiledb_major < 0) {
        printk(KERN_ALERT "Registration of character device failed.\n");
        return rpiledb_major;
    }

    rpiledb_minor = 0;
	printk(KERN_INFO "RPILedb: major %d, minor %d\n", rpiledb_major, rpiledb_minor);

    // Request memory region
    if (!request_mem_region(GPIO_BASE, GPIO_SIZE, DEVICE_NAME)) {
        printk(KERN_INFO "Memory region 0x%lx already in use\n", GPIO_BASE);
        got_mem_region = false;
    } else {
        got_mem_region = true;
    }

    // Map physical addresses to virtual addresses
    gpio_base = ioremap(GPIO_BASE, GPIO_SIZE);
    if (!gpio_base) {
        printk(KERN_ALERT "Failed to map GPIO memory\n");
        result = -ENOMEM;
        goto fail;
    }

    // Setup GPIO pins
    // GPIO 15 as output
    // GPFSEL1: GPIO 15 is controlled by bits 17-15, set to 001 for output
    iowrite32((ioread32(gpio_base + 0x04) & ~(7 << 15)) | (1 << 15), gpio_base + 0x04);

    // GPIO 14 as input
    // GPFSEL1: GPIO 14 is controlled by bits 14-12, set to 000 for input
    iowrite32(ioread32(gpio_base + 0x04) & ~(7 << 12), gpio_base + 0x04);

    // Setup timer
    timer_setup(&my_timer, timer_callback, 0);

    printk(KERN_INFO "LED blink module initialized\n");
	return 0;
	
fail:
    printk(KERN_ALERT "Module initialization failed\n");
    rpiledb_exit();
	return result;
}

void rpiledb_exit(void)
{
    // Clean timer (wait for callback to finish)
    del_timer_sync(&my_timer);

   // Reset GPIO pins to input (all bits to 0)
   if (gpio_base) {
       // Reset GPIO 15 to input (clear bits 17-15 in GPFSEL1)
       iowrite32(ioread32(gpio_base + 0x04) & ~(7 << 15), gpio_base + 0x04);
       
       // GPIO 14 is already input, but can reset anyway (clear bits 14-12)
       iowrite32(ioread32(gpio_base + 0x04) & ~(7 << 12), gpio_base + 0x04);

       // Unmap GPIO memory
       iounmap(gpio_base);
   }

   // Release memory region only if we got it
   if (got_mem_region)
       release_mem_region(GPIO_BASE, GPIO_SIZE);

    // Enregisters device (major) from kernel
	unregister_chrdev(rpiledb_major, DEVICE_NAME);

    printk(KERN_INFO "Module unloaded\n");
}

int rpiledb_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Device open\n");
    return 0;
}

int rpiledb_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Device close\n");
    return 0;
}

ssize_t rpiledb_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t rpiledb_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
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
            active = false;
            break;

        case 1:
            // Start LED blinking with 0.5s interval
            active = true;
            mod_timer(&my_timer, jiffies); // Call timer
            break;

        case 2:
            // Set SWITCH to 0
            SWITCHon = 0;
            break;

        case 3:
            // Set SWITCH to 1 (2s timer)
            SWITCHon = 1;
            mod_timer(&my_timer, jiffies); // Call timer
            break;

        default:
            return -EINVAL;
    }

    // Print status after switch
    printk(KERN_DEBUG "Status: Switch: %d, Timer: %s\n",
           SWITCHon,
           !SWITCHon && active ? "active" : "not active");

    retval = count;
out:
    return retval;
}