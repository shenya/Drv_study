#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>


#define FIFO_CLEAR 0x1
#define BUFFER_LEN 20
int main(void)
{
	int fd;
	int num;
	char rd_ch[BUFFER_LEN];
	fd_set rfds, wfds;

	fd = open("/dev/globalmem", O_RDONLY | O_NONBLOCK);
	if(fd != -1)
	{
		if(ioctl(fd, FIFO_CLEAR, 0) < 0)
		{
			printf("ioctl failed\n");
		}
		while(1)
		{
			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			FD_SET(fd, &rfds);
			FD_SET(fd, &wfds);

			select(fd+1, &rfds, &wfds, NULL, NULL);
			sleep(2);
			if(FD_ISSET(fd, &rfds))
			{
				printf("poll monitor: can be read\n");
			}

			if(FD_ISSET(fd, &wfds))
			{
				printf("poll monitor: can be written\n");
			}
			
		}

	}
	else
	{
		printf("Device open failure\n");
	}
	

	return 0;
}
