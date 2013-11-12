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
	-rm *.o *.ko *.order *.symvers
	-rm hello.mod.c .hello.o.cmd
	-rm -rf .tmp_versions
	-rm .hello.ko.cmd .hello.mod.o.cmd

