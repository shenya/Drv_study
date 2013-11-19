#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct nf_proto_port
{
	unsigned int nf_proto;
	unsigned int nf_ip;
	unsigned int nf_port;
}nf_bound_t;

struct net_filter
{
		//icmp
		unsigned int icmp_enable;
		
		nf_bound_t nf_filter; 
};

struct net_filter nf_status;

int main(void)
{
	
	int opt = 1;
	int sockfd;

	socklen_t len;
	nf_status.icmp_enable = 1;
	nf_status.nf_filter.nf_proto = IPPROTO_TCP;
	nf_status.nf_filter.nf_ip = inet_addr("123.125.114.144");
	nf_status.nf_filter.nf_port = htons(80);

	printf("ip %u\n", nf_status.nf_filter.nf_ip);

	len = sizeof(nf_status);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(sockfd < 0)
	{
		perror("fail to socket");
	}

	setsockopt(sockfd, IPPROTO_IP, 0x6001, &nf_status, len);





	return 0;
}
