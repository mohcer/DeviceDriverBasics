CONFIG_MODULE_SIG=n

obj-m += helloworld_module.o
KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVRSION)/build M=$(PWD) clean
