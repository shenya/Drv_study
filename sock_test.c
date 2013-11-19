#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct net_filter
{
		//icmp
		unsigned int icmp_enable;

};

struct net_filter nf_status;

int main(void)
{
	
	int opt = 1;
	int sockfd;

	socklen_t len;
	nf_status.icmp_enable = 3;

	len = sizeof(nf_status);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(sockfd < 0)
	{
		perror("fail to socket");
	}

	setsockopt(sockfd, IPPROTO_IP, 0x6001, &nf_status, len);





	return 0;
}
