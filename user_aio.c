#include <aio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#define BUFSIZE 256

int main(void)
{
	int fd, set;
	int ret;
	struct aiocb my_aiocb;

	fd = open("file", O_RDONLY);
	if(fd < 0)
	{
		perror("open");
	}
	
	bzero((char *)&my_aiocb, sizeof(struct aiocb));
	my_aiocb.aio_buf = malloc(BUFSIZE + 1);
	if(!my_aiocb.aio_buf)
	{
		perror("malloc");
	}
	my_aiocb.aio_fildes = fd;
	my_aiocb.aio_nbytes = BUFSIZE;
	my_aiocb.aio_offset = 0;

	ret = aio_read(&my_aiocb);
	if(ret < 0)
	{
		perror("aio_read");
	}
	while(aio_error(&my_aiocb) == EINPROGRESS);

	if(ret = aio_return(&my_aiocb))
	{
		printf("read something %s", (char *)my_aiocb.aio_buf);	
	}
	else
	{
		printf("fail to read\n");
	}


	return 0;
}
