#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

char *dev_name = "chr_hello";
dev_t dev_id;
int major;
int minor;

int f_open(struct inode *inode, struct file *file);
ssize_t f_write(struct file *file, const char __user *buf, size_t length, loff_t * offset);
int f_release(struct inode *inode, struct file *file);

struct file_operations Fops=
{
	.open = f_open,
	.write = f_write,
	.release = f_release,
};

int f_open(struct inode *inode, struct file *file)
{


	return 0;
}

int f_release(struct inode *inode, struct file *file)
{


	return 0;
}
ssize_t f_write(struct file *file, const char __user *buf, size_t length, loff_t * offset)
{
	switch(buf[0])
	{
		case 'a':
						printk(KERN_ALERT "this is a\n");
						break;
		case 'b':
						printk(KERN_ALERT "this is a\n");
						break;
						


	}

	return 0;

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
