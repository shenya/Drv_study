#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void)
{
	
	int opt = 1;
	int sockfd;

	socklen_t len;

	len = sizeof(opt);

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(sockfd < 0)
	{
		perror("fail to socket");
	}

	setsockopt(sockfd, IPPROTO_IP, 0x6001, &opt, len);





	return 0;
}
