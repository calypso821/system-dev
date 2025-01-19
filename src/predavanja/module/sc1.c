#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* marsikaj */
#include <linux/errno.h> /* kode napak */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_*_user */

#define DEVICE_NAME "sc1"
#define SC1_QUANTUM_SIZE 10
#define SC1_QSET_SIZE 5

MODULE_LICENSE("GPL");

struct sc1_dev {
	void **data;
	unsigned Q;
	unsigned S;
	unsigned size;
};

int sc1_init(void);
void sc1_cleanup(void);
int sc1_trim(struct sc1_dev *dev);
int sc1_open(struct inode *inode, struct file *filp);
int sc1_release(struct inode *inode, struct file *filp);
ssize_t sc1_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t sc1_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

struct file_operations sc1_fops = {
	.owner = THIS_MODULE,
	.llseek = NULL,
	.read = sc1_read,
	.write = sc1_write,
	.open = sc1_open,
	.release = sc1_release,
};

int sc1_major;
int sc1_minor;

struct sc1_dev *sc1_dev_ptr;
module_init(sc1_init);
module_exit(sc1_cleanup);

int sc1_init(void)
{
	int result;
	
	sc1_major = register_chrdev(0, DEVICE_NAME, &sc1_fops);
	if (sc1_major < 0) {
		printk(KERN_ALERT "Registracija spodletela.\n");
		return sc1_major;
	}

	sc1_minor = 0;
	printk(KERN_INFO "sc1: major %d, minor %d\n", sc1_major,sc1_minor);
	
	sc1_dev_ptr = kmalloc(sizeof(struct sc1_dev), GFP_KERNEL);
	if (!sc1_dev_ptr) { result = -ENOMEM; goto fail; }
	memset(sc1_dev_ptr, 0, sizeof(struct sc1_dev));
	
	sc1_dev_ptr->Q = SC1_QUANTUM_SIZE;
	sc1_dev_ptr->S = SC1_QSET_SIZE;
	
	return 0;
	
	fail:
	sc1_cleanup();
	return result;
}

void sc1_cleanup(void)
{
	if (sc1_dev_ptr) {
		sc1_trim(sc1_dev_ptr);
		kfree(sc1_dev_ptr);
	}
	unregister_chrdev(sc1_major, DEVICE_NAME);
}

int sc1_trim(struct sc1_dev *dev)
{
	int i;
	if (dev->data) {
		for ( i=0; i < dev->S; i++)
			if (dev->data[i])
				kfree(dev->data[i]);
		kfree(dev->data);
		dev->data = NULL;
	}
	dev->size = 0;
	return 0;
}

int sc1_open(struct inode *inode, struct file *filp)
{
	filp->private_data = sc1_dev_ptr;
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
	sc1_trim(sc1_dev_ptr);
	
	return 0;
}

int sc1_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t sc1_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct sc1_dev *dev = filp->private_data;
	int Q = dev->Q;
	int S = dev->S;
	int s_pos, q_pos;
	ssize_t retval = 0;
	
	printk("Read-Zacetek: *f_pos: %ld, count: %d, size: %d. ", *f_pos, count, dev->size);

	if (*f_pos >= dev->size)
		goto out;
	
	if ( count > dev->size - *f_pos)
		count = dev->size - *f_pos;
	
	s_pos = (long)*f_pos / Q;
	q_pos = (long)*f_pos % Q;
	
	if (count > Q - q_pos) // beremo le do konca trenutnega kvanta
		count = Q - q_pos;
	
	if ( copy_to_user(buf, dev->data[s_pos] + q_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;
	
	printk("Konec: *f_pos: %ld, count: %d, size: %d. ", *f_pos, count, dev->size);

	out:
	return retval;
}

ssize_t sc1_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct sc1_dev *dev = filp->private_data;
	int Q = dev->Q;
	int S = dev->S;
	int s_pos, q_pos;
	ssize_t retval = -ENOMEM;
	
	printk("Write-Zacetek: *f_pos: %ld, count: %d, size: %d. ", *f_pos, count, dev->size);
	
	s_pos = (long)*f_pos / Q; // pozicija v kvantni množici
	q_pos = (long)*f_pos % Q; // pozicija v kvantu
	if ( s_pos >= S)
		return count;// št. kvantov je omejeno na S (qset_size)
	// alokacija kvantne mnozice (qset)
	if (!dev->data) {
		dev->data = kmalloc(S * sizeof(char *), GFP_KERNEL);
		if (!dev->data)
			goto out;
		memset(dev->data, 0, S * sizeof(char *));
	}

	if (!dev->data[s_pos]) {
		dev->data[s_pos] = kmalloc(Q * sizeof(char), GFP_KERNEL);
		if (!dev->data[s_pos])
			goto out;
	}
	
	if (count > Q - q_pos) // pišemo le do konca trenutnega kvanta
		count = Q - q_pos;
	
	if (copy_from_user( dev->data[s_pos] + q_pos, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	
	*f_pos += count;
	retval = count;
	
	if (dev->size < *f_pos) // posodobimo velikost
		dev->size = *f_pos;

	printk("Konec: *f_pos: %ld, count: %d, size: %d.\n", *f_pos, count, dev->size);

	out:
	return retval;
}

