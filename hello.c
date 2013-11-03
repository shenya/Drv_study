#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");


static int __init hello_init(void)
{
	printk(KERN_ALERT "module init\n");

	return 0;
}



static void __exit hello_exit(void)
{
	printk(KERN_ALERT "module exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
