#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/socket.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/in.h>
#include "nf_sockopt_ext.h"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

typedef struct nf_proto_port
{
	unsigned int nf_proto;
	unsigned int nf_port;
}nf_bound;

struct net_filter
{
		//icmp
		unsigned int icmp_enable;
		
		//`nf_bound 

};

struct net_filter nf_status;

static int nf_sockopt_ext_set(struct sock *sk, int optval, void __user *user, unsigned int len);

struct nf_sockopt_ops nf_sockopt_ext =
{
		.pf = AF_INET,
		.set_optmin = SOE_BANDIP,
		.set_optmax = SOE_BANDIP + 2,
		.set = nf_sockopt_ext_set,

		.get_optmin = 0,
		.get_optmax = 0,
		.get = NULL,

		.owner = THIS_MODULE,
};

static int nf_sockopt_ext_set(struct sock *sk, int optval, void __user *user, unsigned int len)
{
	
	printk("setopt extern \n");

	switch(optval)
	{
		case SOE_BANDIP :
			copy_from_user(&nf_status, user, len);
			printk("nf_status-icmp_enable  %d\n soe\n", nf_status.icmp_enable);
			break;

		default:
					break;
	}


	return 0;
}
static unsigned int hook_nfout(unsigned int hooknum, struct sk_buff *skb,
				const struct net_device *in, const struct net_device *out,
				int (*okfn)(struct sk_buff *))
{
		struct sk_buff *sk = skb;

		struct iphdr *iph = ip_hdr(sk);
		//8.8.8.8
#if 0
		if(iph->daddr == 134744072)
		{
			printk("drop packet\n");
			return NF_DROP;
		}
		printk("iph->daddr %d\n", iph->daddr);
		printk(KERN_ALERT "Hi, I can do it\n");

#endif

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
			printk(KERN_ALERT "nf register\n");
	}
	nf_register_sockopt(&nf_sockopt_ext);
	return 0;
}



static void __exit hello_exit(void)
{
	printk(KERN_ALERT "module exit\n");
	nf_unregister_hook(&nfout);

	nf_unregister_sockopt(&nf_sockopt_ext);
	
}

module_init(hello_init);
module_exit(hello_exit);
