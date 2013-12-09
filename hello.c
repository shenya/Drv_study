#include <linux/init.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

#define BUF_SIZE 16384

#define NETLINK_TEST 17
#define MAX_MSGSIZE 1024


int stringlength(char *);

void sendnlmsg(char *message);
int pid;
int err;
struct sock *nl_sk = NULL;
int flag = 0;

void sendnlmsg(char *message)
{
	struct sk_buff *skb_l;
	struct nlmsghdr *nlh;
	int len = NLMSG_SPACE(MAX_MSGSIZE);
	int slen = 0;
	if(!message || !nl_sk)
	{
		return;
	}
	skb_l = alloc_skb(len, GFP_KERNEL);
	if(!skb_l)
	{
		printk(KERN_ERR "alloc_skb error\n");
	}
	slen = stringlength(message);
	nlh = nlmsg_put(skb_l, 0, 0, 0, MAX_MSGSIZE, 0);

	NETLINK_CB(skb_l).pid = 0;
	NETLINK_CB(skb_l).dst_group = 0;

	memcpy(NLMSG_DATA(nlh), message, slen+1);

	printk("send message %s \n", (char *)NLMSG_DATA(nlh));
	netlink_unicast(nl_sk, skb_l, pid, MSG_DONTWAIT);
	
//	kfree_skb(skb_l);  //don't use this , because netlink_unicast do 
}

int stringlength(char *s)
{
	int slen = 0;

	for(; *s; s++)
	{
		slen++;
	}

	return slen;
}

void nl_data_ready(struct sk_buff *__skb)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	char str[100];

	struct completion cmpl;
	int i = 4;

	skb = skb_get(__skb);
#if 1
	if(skb->len >= NLMSG_SPACE(0))
	{
		nlh = nlmsg_hdr(skb);
		memcpy(str, NLMSG_DATA(nlh), sizeof(str));
		printk("message received: %s\n", str);
		pid = nlh->nlmsg_pid;
#if 1
		while(i--)
		{
			init_completion(&cmpl);
			wait_for_completion_timeout(&cmpl, 3 *HZ);
			sendnlmsg("I am from kernel!");
		}
#endif

		flag = 1;
		kfree_skb(skb);
	}
#endif

}

#if 0
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


#endif


static int __init hello_init(void)
{
	printk(KERN_ALERT "module init\n");

	nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, 0, nl_data_ready, NULL, THIS_MODULE);
	if(!nl_sk)
	{
		printk("netlink error");
		return 1;
	}

	return 0;
}



static void __exit hello_exit(void)
{
	printk(KERN_ALERT "module exit\n");

	netlink_kernel_release(nl_sk);
}

module_init(hello_init);
module_exit(hello_exit);
