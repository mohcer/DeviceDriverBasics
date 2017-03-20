/*
Problem Domain
Build a simple hello world! kernel module 
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohsin-Sayyed");
MODULE_DESCRIPTION("A simple hello world kernel module");

static char *who = "Mohsin-A0163131W";

module_param(who, charp, S_IRUGO);
MODULE_PARM_DESC(who, "Parameter to show who is running this module");


static int hello_init(void){
	printk(KERN_ALERT "Hello, %s this is Kernel Module init\n", who);
	return 0;
}

static void hello_exit(void){
	printk(KERN_ALERT "Good bye, %s kernel module exits\n", who);
}

module_init( hello_init );

module_exit( hello_exit );
																																																					
