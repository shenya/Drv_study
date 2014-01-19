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
#include <linux/wait.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

#define GLOBALMEM_SIZE 256
#define MEM_CLEAR 0X001

static int major = 221;
struct globalmem_dev
{
	struct cdev cdev;
	unsigned int current_len;
	unsigned char mem[GLOBALMEM_SIZE];
	struct semaphore sem;
	wait_queue_head_t r_wait;
	wait_queue_head_t w_wait;
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

static ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	//unsigned long p = *ppos;

	int ret = 0;
	
	struct globalmem_dev *dev = filp->private_data;
	DECLARE_WAITQUEUE(wait, current);

	down(&dev->sem);
	add_wait_queue(&dev->w_wait, &wait);
	if(dev->current_len == GLOBALMEM_SIZE)
	{
		if(filp->f_flags &O_NONBLOCK)
		{
			ret = -EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		up(&dev->sem);
		schedule();
		if(signal_pending(current))
		{
			ret = -ERESTARTSYS;
			goto out2;
		}
		down(&dev->sem);
	}
	if(count > GLOBALMEM_SIZE - dev->current_len)
		count = GLOBALMEM_SIZE - dev->current_len;
	if(copy_from_user(dev->mem+dev->current_len, buf, count))
	{	
		ret = -EFAULT;
		goto out;
	}
	else
	{
		dev->current_len += count;
		printk(KERN_INFO "written %d bytes(s), current_len: %d\n", count, dev->current_len);
		wake_up_interruptible(&dev->r_wait);
		ret = count;
	}

	out: up(&dev->sem);
	out2: remove_wait_queue(&dev->w_wait, &wait);

	set_current_state(TASK_RUNNING);

	
	return ret;
}
static ssize_t globalmem_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
//	unsigned long p = *ppos;
	//unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;
#if 0
	if(p >= GLOBALMEM_SIZE)
	{
		return count?-ENXIO: 0;
	}
	if(count > GLOBALMEM_SIZE - p)
	{
		count = GLOBALMEM_SIZE - p;
	}
#endif
	DECLARE_WAITQUEUE(wait, current);
	printk(KERN_INFO "herre\n");
	down(&dev->sem);
	printk(KERN_INFO "herkr 2\n");
	add_wait_queue(&dev->r_wait, &wait);
	printk(KERN_INFO "herkr 3\n");
	if(dev->current_len == 0)
	{
		printk(KERN_INFO" dev->current_len = 0\n");
		if(filp->f_flags &O_NONBLOCK)
		{
			printk(KERN_INFO "O_NONBLOCK is set\n");
			ret = -EAGAIN;
			goto out;
		}
		printk(KERN_INFO "1\n");
		__set_current_state(TASK_INTERRUPTIBLE);
		up(&dev->sem);
		schedule();
		printk(KERN_INFO "2\n");
		if(signal_pending(current))
		{
			ret = -ERESTARTSYS;
			goto out2;
		}

		down(&dev->sem);
	}
	printk(KERN_INFO "3\n");
	if(count > dev->current_len)
		count = dev->current_len;
	if(copy_to_user(buf, dev->mem , count))
	{
		ret = -EFAULT;
		goto out;
	}
	else
	{
		memcpy(dev->mem, dev->mem+count, dev->current_len-count);
		dev->current_len -= count;
		printk(KERN_INFO "read %d bytes(s), current_len: %d\n", count, dev->current_len);
		wake_up_interruptible(&dev->w_wait);
		ret = count;
	}
	out: up(&dev->sem);
	out2: remove_wait_queue(&dev->w_wait, &wait);
	printk(KERN_INFO "read %d bytes", count);

	set_current_state(TASK_RUNNING);
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


	memset(globalmem_devp, 0, sizeof(struct globalmem_dev));
	sema_init(&globalmem_devp->sem, 1);
	globalmem_setup_cdev(globalmem_devp, 0);
	printk(KERN_ALERT "module init\n");
	init_waitqueue_head(&globalmem_devp->r_wait);
	init_waitqueue_head(&globalmem_devp->w_wait);
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
