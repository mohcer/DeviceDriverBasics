CONFIG_MODULE_SIG=n

#obj-m += ./Simple-Device-Driver/helloworld_module.o

obj-m += one.o

KVERSION = $(shell uname -r)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
