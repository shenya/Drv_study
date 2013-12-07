#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>

#define NETLINK_TEST 17
#define MAX_MSGSIZE 1024
int main(void)
{
	
	int sd;
	int ret;
	struct sockaddr_nl saddr, daddr;
	struct nlmsghdr *nlhdr = NULL;
	struct msghdr msg;
	struct iovec iov;

	char text_line[MAX_MSGSIZE] = "hello";

	sd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
	if(sd < 0)
	{
		perror("fail to socket");
		exit(1);

	}

	memset(&saddr, 0, sizeof(saddr));
	memset(&daddr, 0, sizeof(daddr));

	saddr.nl_family = AF_NETLINK;
	saddr.nl_pid = getpid();
	printf("pid: %d\n", getpid());
	saddr.nl_groups = 0;
	ret = bind(sd, (struct sockaddr *)&saddr, sizeof(saddr));
	if(ret < 0)
	{
		printf("error\n");
	}

	daddr.nl_family = AF_NETLINK;
	daddr.nl_pid = 0;
	daddr.nl_groups = 0;

	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSGSIZE));
	
	memcpy(NLMSG_DATA(nlhdr), text_line, strlen(text_line));
	memset(&msg, 0, sizeof(struct msghdr));

	nlhdr->nlmsg_len = NLMSG_LENGTH(strlen(text_line));
	nlhdr->nlmsg_pid = getpid();
	nlhdr->nlmsg_flags = 0;

	iov.iov_base = (void *)nlhdr;
	iov.iov_len = nlhdr->nlmsg_len;

	msg.msg_name = (void *)&daddr;
	msg.msg_namelen = sizeof(daddr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	ret = sendmsg(sd, &msg, 0);
	if(ret == -1)
	{
		printf("error \n");
	}
	printf("end\n");
	return 0;
}
