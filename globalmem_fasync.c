#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>


#define FIFO_CLEAR 0x1
#define BUFFER_LEN 20

void input_handler(int signum)
{
	printf("receive a signal from globalfifo, signalnum: %d\n", signum);
}
int main(void)
{
	int fd, oflags;
	int num;
	char rd_ch[BUFFER_LEN];
	fd_set rfds, wfds;

	fd = open("/dev/globalmem", O_RDONLY | O_NONBLOCK);
	if(fd != -1)
	{
		signal(SIGIO, input_handler);
		fcntl(fd, F_SETOWN, getpid());
		oflags = fcntl(fd, F_GETFL);
		fcntl(fd, F_SETFL, oflags | FASYNC);
		while(1)
		{
			sleep(100);
		}
	}
	else
	{
		printf("Device open failure\n");
	}
	

	return 0;
}
