#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/sched.h>
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

#define NETLINK_TEST 17

static struct sock *netlink_exam_sock;

static void recv_handler(struct sk_buff *sk)
{
	printk(KERN_ALERT "hello netlink\n");
}
static int __init hello_init(void)
{
	printk(KERN_ALERT "module init\n");

	netlink_exam_sock = netlink_kernel_create(&init_net, NETLINK_TEST, 0, recv_handler, NULL, THIS_MODULE);
	if(netlink_exam_sock == NULL)
		printk("netlink error");

	return 0;
}



static void __exit hello_exit(void)
{
	printk(KERN_ALERT "module exit\n");

	netlink_kernel_release(netlink_exam_sock);
}

module_init(hello_init);
module_exit(hello_exit);
