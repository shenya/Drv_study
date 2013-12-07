#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/sched.h>
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

#define NETLINK_TEST 17
#define BUF_SIZE 16384

static struct sock *netlink_exam_sock;
static unsigned char buffer[BUF_SIZE];
static unsigned int buffer_tail = 0;
static int exit_flag = 0;
static DECLARE_COMPLETION(exit_completion);
static int process_message_thread(void * data);

static void recv_handler(struct sk_buff *sk)
{
	struct sk_buff *skb = sk;
	struct nlmsghdr *nlhdr = NULL;
	int len;
	int dst_pid;
	int ret;

	nlhdr = (struct  nlmsghdr *)skb->data;
	if(nlhdr->nlmsg_len < sizeof(struct nlmsghdr))
	{
		printk("netlink message. error\n");
	}
	len = nlhdr->nlmsg_len - NLMSG_LENGTH(0);


	if(len + buffer_tail > BUF_SIZE)
	{
		printk("netlink buffer is full.\n");
		
	}
	else
	{
		printk("get message\n");
		memcpy(buffer + buffer_tail, NLMSG_DATA(nlhdr), len);
		buffer_tail += len;
	}
	dst_pid = nlhdr->nlmsg_pid;
	printk("len:%d, type:%d, flag:%d, seq :%d, pid: %d\n", nlhdr->nlmsg_len, nlhdr->nlmsg_type, nlhdr->nlmsg_flags, nlhdr->nlmsg_seq, nlhdr->nlmsg_pid);
	printk("%s\n", buffer);

		nlhdr->nlmsg_pid = 0;
		nlhdr->nlmsg_flags = 0;

		NETLINK_CB(skb).pid = 0;
//		NETLINK_CB(skb).dst_pid = 0;
		NETLINK_CB(skb).dst_group = 1;

#if 0
		ret = netlink_broadcast(netlink_exam_sock, skb, 0, 1, GFP_KERNEL);
		if(ret < 0)
		{
			printk("no send success\n");
		}
		kfree(skb);
#endif
#if 0
		ret = netlink_unicast(netlink_exam_sock, skb, dst_pid, 1);
		if(ret < 0)
		{
			printk("no send success\n");
		}
#endif
}
#if 0
static int process_message_thread(void * data)
{
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlhdr = NULL;
	int len;

//	DEFINE_WAIT(wait);

	//daemonize("mynetlink");

	//while(exit_flag == 0)
	{
#if 0
		prepare_to_wait(netlink_exam_sock->sk_sleep, &wait, TASK_INTERRUPTIBLE);
		schedule();
		finish_wait(netlink_exam_sock->sk_sleep, &wait);
#endif
		while((skb = skb_dequeue(&netlink_exam_sock->sk_receive_queue)) != NULL)
		{
			nlhdr = (struct  nlmsghdr *)skb->data;
			if(nlhdr->nlmsg_len < sizeof(struct nlmsghdr))
			{
				printk("netlink message. error\n");
				continue;
			}
			len = nlhdr->nlmsg_len - NLMSG_LENGTH(0);

#if 1
			if(len + buffer_tail > BUF_SIZE)
			{
				printk("netlink buffer is full.\n");
				
			}
			else
			{
				printk("get message");
				memcpy(buffer + buffer_tail, NLMSG_DATA(nlhdr), len);
				buffer_tail += len;
			}
#endif
	#if 0
			nlhdr->nlmsg_pid = 0;
			nlhdr->nlmsg_flags = 0;
			NETLINK_CB(skb).pid = 0;
			NETLINK_CB(skb).dst_pid = 0;
			NETLINK_CB(skb).dst_group = 1;

			netlink_broadcast(netlink_exam_sock, skb, 0, 1, GFP_KERNEL);
	#endif
		}
	}

	//complete(&exit_completion);
	printk("exit thread\n");
	return 0;
}

#endif





static int __init hello_init(void)
{
	printk(KERN_ALERT "module init\n");

	netlink_exam_sock = netlink_kernel_create(&init_net, NETLINK_TEST, 0, recv_handler, NULL, THIS_MODULE);
	if(netlink_exam_sock == NULL)
		printk("netlink error");

//	kernel_thread(process_message_thread, NULL, CLONE_KERNEL);

	return 0;
}



static void __exit hello_exit(void)
{
	printk(KERN_ALERT "module exit\n");

	netlink_kernel_release(netlink_exam_sock);
}

module_init(hello_init);
module_exit(hello_exit);
