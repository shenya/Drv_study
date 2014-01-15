#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

#define GLOBALMEM_SIZE 256
static int major = 221;
struct globalmem_dev
{
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];
};
struct globalmem_dev *globalmem_devp;
int globalmem_open(struct inode *inode, struct file *filp)
{
	filp->private_data = globalmem_devp;

	return 0;
}

static ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;

	struct globalmem_dev *dev = filp->private_data;
	
	if(copy_from_user(dev->mem+p, buf, count))
		ret = -EFAULT;

	printk(KERN_INFO "wriet %d bytes", count);
	
}
static const struct file_operations globalmem_fops = 
{
	.owner = THIS_MODULE,
	.open = globalmem_open,
	.write = globalmem_write,
};

static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)
{
	int err, devno = MKDEV(major, index);

	cdev_init(&dev->cdev, &globalmem_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &globalmem_fops;
	err = cdev_add(&dev->cdev, devno, 1);

	if(err)
		printk(KERN_NOTICE "err");
}
static int __init hello_init(void)
{
	int result;

	dev_t devno = MKDEV(major, 0);

	result = register_chrdev_region(devno, 1, "globalmem");
	if(result < 0)
		return result;
	#if 1
	globalmem_devp = kmalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
	if(!globalmem_devp)
	{
		result = -ENOMEM;
		goto fail_malloc;
	}
	#endif

	memset(globalmem_devp, 0, sizeof(struct globalmem_dev));
	globalmem_setup_cdev(globalmem_devp, 0);
	printk(KERN_ALERT "module init\n");

	return 0;
fail_malloc:
	unregister_chrdev_region(devno, 1);
	return result;

}



static void __exit hello_exit(void)
{

	cdev_del(&globalmem_devp->cdev);
	unregister_chrdev_region(MKDEV(major, 0), 1);

	kfree(globalmem_devp);
	printk(KERN_ALERT "module exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
