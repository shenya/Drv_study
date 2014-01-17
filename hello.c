#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

#define GLOBALMEM_SIZE 256
#define MEM_CLEAR 0X001

static int major = 221;
struct globalmem_dev
{
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];
	struct semaphore sem;
};
struct globalmem_dev *globalmem_devp;
int globalmem_open(struct inode *inode, struct file *filp)
{
	filp->private_data = globalmem_devp;

	return 0;
}

int globalmem_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "globalmem release");
	return 0;
}

static ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	
	struct globalmem_dev *dev = filp->private_data;

	if(p >= GLOBALMEM_SIZE)
	{
		return count?-ENXIO:0;
	}
	if(count > GLOBALMEM_SIZE-p)
	{
		count = GLOBALMEM_SIZE - p;
	}
	if(down_interruptible(&dev->sem))
	{
		return -ERESTARTSYS;
	}
	
	if(copy_from_user(dev->mem+p, buf, count))
	{	
		ret = -EFAULT;
	}
	else
	{
		*ppos += count;
		ret = count;
	}

	up(&dev->sem);
	printk(KERN_INFO "write %d bytes", count);
	
	return ret;
}
static ssize_t globalmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;

	if(p >= GLOBALMEM_SIZE)
	{
		return count?-ENXIO: 0;
	}
	if(count > GLOBALMEM_SIZE - p)
	{
		count = GLOBALMEM_SIZE - p;
	}

	if(down_interruptible(&dev->sem))
	{
		return -ERESTARTSYS;
	}
	if(copy_to_user(buf, (void *)(dev->mem + p), count))
	{
		ret = -EFAULT;
	}
	else
	{
		*ppos += count;
		ret = count;
	}
	up(&dev->sem);
	printk(KERN_INFO "read %d bytes", count);
	return ret;
}
static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig)
{
	loff_t ret = 0;

	switch(orig)
	{
		case 0:
				if(offset < 0)
				{
					ret = -EINVAL;
					break;
				}
				if((unsigned int)offset > GLOBALMEM_SIZE)
				{
					ret = -EINVAL;
					break;
				}
				filp->f_pos = (unsigned int)offset;
				ret = filp->f_pos;
				break;

		case 1:
				if(filp->f_pos+offset > GLOBALMEM_SIZE)
				{
					ret = -EINVAL;
					break;
				}
				if(filp->f_pos+offset < 0)
				{
					ret = -EINVAL;
					break;
				}

				filp->f_pos += offset;
				ret = filp->f_pos;
				break;
		default:
				ret = -EINVAL;
				break;
	}

	return ret;
}
static long globalmem_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;

	if(down_interruptible(&dev->sem))
	{
		return -ERESTARTSYS;
	}
	switch(cmd)
	{
		case MEM_CLEAR:
			memset(dev->mem, 0, GLOBALMEM_SIZE);
			printk(KERN_INFO "globalmem is set to zero");
			break;

		default:
			return -EINVAL;	
	}
	up(&dev->sem);
	return 0;
}
static const struct file_operations globalmem_fops = 
{
	.owner = THIS_MODULE,
	.open = globalmem_open,
	.write = globalmem_write,
	.read = globalmem_read,
	.llseek = globalmem_llseek,
	.release = globalmem_release,
	.unlocked_ioctl = globalmem_ioctl,
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

	sema_init(&globalmem_devp->sem, 1);

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
