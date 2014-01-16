#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>

int main(void)
{
	int fd;
	int num;
	char buf[20];

	fd = open("/dev/globalmem", O_RDWR);
	if(fd < 0)
	{
		perror("fail to open");
		exit(1);
	}
	
	write(fd, "hello", sizeof("hello"));

	close(fd);

	fd = open("/dev/globalmem", O_RDWR);
	if(fd < 0)
	{
		perror("fail to open");
		exit(1);
	}
	
//	ioctl(fd, 0x001);
	lseek(fd, 1, SEEK_SET);
	num = read(fd, buf, sizeof(buf));
	buf[num] = '\0';
	printf("%s\n", buf);
	close(fd);

	return 0;
}
