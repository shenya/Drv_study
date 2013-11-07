/*function: character deivce driver for study
 *author: sgy
 *date: 2013-11-07
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <asm/uaccess.h>


#define DEVICE_BUSY 1
#define DEVICE_SUCCESS 0
#define KERNEL_DATLEN 1024

//module info
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");
//device name
char *dev_name = "chr_hello";
dev_t dev_id;
int major;   //major device number
int minor;
static int device_open = 0; //open flag

static char kernel_buf[KERNEL_DATLEN];

static int data_byte = 0; //record bytes counter

int f_open(struct inode *inode, struct file *file);
ssize_t f_write(struct file *file, const char __user *buf, size_t length, loff_t * offset);
int f_release(struct inode *inode, struct file *file);
ssize_t f_read(struct file *file, char __user *buf, size_t length, loff_t * offset);

struct file_operations Fops=
{
	.open = f_open,
	.read = f_read,
	.write = f_write,
	.release = f_release,
};

int f_open(struct inode *inode, struct file *file)
{
	if(device_open)
	{
		printk(KERN_ALERT "device busy");
		return -DEVICE_BUSY;
	}

	device_open++;

	return DEVICE_SUCCESS;
}

int f_release(struct inode *inode, struct file *file)
{
	device_open--;

	return 0;
}
ssize_t f_read(struct file *file, char __user *buf, size_t length, loff_t * offset)
{
	int ret = 0;
	unsigned long p = *offset;

	if(length > KERNEL_DATLEN - p)
	{
		length = KERNEL_DATLEN - p;	
	}
	if(length > data_byte)
	{
		length = data_byte;
	}
	printk(KERN_ALERT "buf is: %s K-p %ld\n", kernel_buf, KERNEL_DATLEN - p);
	ret = copy_to_user(buf, kernel_buf, length);
	if(ret == 0)
	{
		
		printk(KERN_ALERT "copy_to_user success\n");
		ret = length;
	}
	else
	{
		printk(KERN_ALERT "copy_to_user fail\n");
	}
	return ret;
}
ssize_t f_write(struct file *file, const char __user *buf, size_t length, loff_t * offset)
{	
	int ret = 0;

	ret = copy_from_user(kernel_buf, buf, length);
	if(ret < 0)
	{
		printk(KERN_ALERT "copy_from_user fail\n");
		return 0;
	}
	else
	{
		*offset += length;
		ret = length;
		data_byte = length;
	}
	
	printk(KERN_ALERT "buf is: %s length %ld\n", kernel_buf, length);
	switch(kernel_buf[0])
	{
		case 'a':
						printk(KERN_ALERT "this is a\n");
						break;
		case 'b':
						printk(KERN_ALERT "this is a\n");
						break;
						
		default:
						printk(KERN_ALERT "not support\n");
						break;

	}
	printk(KERN_ALERT "wite %dbytes\n", ret);

	return ret;

}

static int __init hello_init(void)
{
//	int ret;
	printk(KERN_ALERT "module init\n");
#if 0	
	ret = alloc_chrdev_region(&dev_id, 0, 1, dev_name);
	if(ret < 0)
	{
		printk(KERN_ALERT "alloc_chrdev_region fail \n");
		return ret;
	}
	
	major = MAJOR(dev_id);
	minor = MINOR(dev_id);

	printk(KERN_ALERT "char-dev's major: %d, minor : %d\n", major, minor);
#endif

	major = register_chrdev(0, dev_name, &Fops);
	if(major < 0)
	{

		printk(KERN_ALERT "register dev failure\n");
	}

	printk(KERN_ALERT "char-dev's major: %d\n", major);
	return 0;
}



static void __exit hello_exit(void)
{
//	unregister_chrdev_region(dev_id, 1);

	unregister_chrdev(major, dev_name);
	printk(KERN_ALERT "module exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
