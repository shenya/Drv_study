#Makefile

ifneq ($(KERNELRELEASE), )
obj-m := nf_study.o
else

KERNELDIR=/lib/modules/$(shell uname -r)/build

PWD=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc sock_test.c

endif


clean:
	rm *.o *.ko *.order *.symvers
	rm nf_study.mod.c
	rm .nf_study.mod.o.cmd
	rm .nf_study.ko.cmd
	rm .nf_study.o.cmd
	rm .tmp_versions -rf
	rm a.out
