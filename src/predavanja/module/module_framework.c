#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mymodule"
MODULE_LICENSE("GPL");

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
    printk(KERN_INFO "Major number is %d.\n", Major);
    return 0;
}

// Renamed exit function
static void my_exit(void)
{
    unregister_chrdev(Major, DEVICE_NAME);
}

// Renamed open function
int device_open(struct inode *inode, struct file *file)
{
    return 0;
}

// Renamed release function
int device_release(struct inode *inode, struct file *file)
{
    return 0;
}

// Renamed read function
ssize_t device_read(struct file *filp, char __user *buff, size_t len, loff_t *offset)
{
    char *msg = "Hello!\n";
    int size = strlen(msg);
    if (*offset >= size)
        return 0;
    if (len > size - *offset)
        len = size - *offset;
    if (copy_to_user(buff, msg, len))
        return -EFAULT;
    *offset += len;
    return len;
}

// Renamed write function
ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *offset)
{
    return 0;
}