#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/socket.h>
#include <linux/netfilter_ipv4.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

static unsigned int hook_nfout(unsigned int hooknum, struct sk_buff *skb,
				const struct net_device *in, const struct net_device *out,
				int (*okfn)(struct sk_buff *))
{
		printk(KERN_ALERT "Hi, I can do it");

		return NF_ACCEPT;
}

static struct nf_hook_ops nfout =
{
		.hook = hook_nfout,
		.owner = THIS_MODULE,
		.pf = AF_INET,
		.hooknum = NF_INET_LOCAL_OUT,
		.priority = NF_IP_PRI_FIRST,
};

static int __init hello_init(void)
{
	int ret;

	printk(KERN_ALERT "module init\n");
	ret = nf_register_hook(&nfout);
	if(ret == 0)
	{
			printk(KERN_ALERT "nf register");
	}
	return 0;
}



static void __exit hello_exit(void)
{
	printk(KERN_ALERT "module exit\n");
	nf_unregister_hook(&nfout);
}

module_init(hello_init);
module_exit(hello_exit);
