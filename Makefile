#Makefile

ifneq ($(KERNELRELEASE), )
obj-m := hello.o
else

KERNELDIR=/lib/modules/$(shell uname -r)/build

PWD=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif


clean:
	rm *.o *.ko *.order *.symvers
	rm hello.mod.c
	rm .hello.mod.o.cmd
	rm .hello.ko.cmd
	rm .hello.o.cmd
	rm .tmp_versions -rf
