#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>

#define NETLINK_TEST 17
#define MAX_PAYLOAD 1024

int main(void)
{
	int state;
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	struct msghdr msg;
	int sock_fd, retval;
	int state_smg = 0;

	sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
	if(sock_fd == -1)
	{
		printf("error to socket\n");
		return -1;
	}

	memset(&msg, 0, sizeof(msg));
	memset(&src_addr, 0, sizeof(src_addr));

	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();

	src_addr.nl_groups = 0;

	retval = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
	if(retval < 0)
	{
		printf("bind failed:\n");
		return -1;
	}

	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	if(!nlh)
	{
		printf("malloc nlmsghdr error!\n");
		close(sock_fd);
		return -1;
	}

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;
	dest_addr.nl_groups = 0;

	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_flags = 0;

	strcpy(NLMSG_DATA(nlh), "Hello-you!");
	iov.iov_base = (void *)nlh;
	iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

	memset(&msg, 0, sizeof(msg));

	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	printf("state smg\n");
	state_smg = sendmsg(sock_fd, &msg, 0);
	if(state_smg == -1)
	{
		printf("get error sendmsg\n");

	}

	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

	printf("waiting received\n");
#if 1

	while(1)
	{
		printf("while recvmsg\n");
		state = recvmsg(sock_fd, &msg, 0);
		if(state < 0)
		{
			printf("state < 1\n");
		}

		printf("REceived message: %s\n", (char *)NLMSG_DATA(nlh));
	}
#endif

	close(sock_fd);

	return 0;
}
