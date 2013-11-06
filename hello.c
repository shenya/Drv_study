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

static int __init hello_init(void)
{
	int ret;
	printk(KERN_ALERT "module init\n");
	
	ret = alloc_chrdev_region(&dev_id, 0, 1, dev_name);
	if(ret < 0)
	{
		printk(KERN_ALERT "alloc_chrdev_region fail \n");
		return ret;
	}
	
	major = MAJOR(dev_id);
	minor = MINOR(dev_id);

	printk(KERN_ALERT "char-dev's major: %d, minor : %d\n", major, minor);

	return 0;
}



static void __exit hello_exit(void)
{
	unregister_chrdev_region(dev_id, 1);

	printk(KERN_ALERT "module exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
