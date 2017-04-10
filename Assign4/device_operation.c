/*
Problem Domain :
Implement a Simple kernel device driver that can simulate 
Read Operation   : One Byte allowed at a time
Write Operaation : One Byte allowed at a time

Assumption       : One byte operation allowed on this device
 
Error :
In case of more than One byte write operation errors this ddevice 
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#include "scull.h"

#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 0)
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 1, int)
#define SCULL_READ _IOR(SCULL_IOC_MAGIC, 2, int)

#define DEVICE_NAME "Four_MB_Device"

#define DEVICE_SIZE 4 * 1024 * 1024 //4MB

/*Forward Declaration*/
int device_open(struct inode *inode, struct file *filep);

int device_release(struct inode *inode, struct file *filep);

ssize_t device_read(struct file *filep, char *buf, size_t
			count, loff_t *f_pos);

ssize_t device_write(struct file *filep, const char *buf,
			size_t count, loff_t *f_pos);

loff_t scull_llseek(struct file *filep, loff_t off, int whence);

static void device_exit(void);

long ioctl_example(struct file *filp, unsigned int cmd, unsigned long arg);

/* definition of file_operation structure */
struct file_operations device_fops = {
	.read   = device_read,
	.write  = device_write,
	.open   = device_open,
        .llseek = scull_llseek,
	.unlocked_ioctl = ioctl_example,
	.release= device_release
};

static char *device_data = NULL;

static int MAJOR_NUMBER;

static int device_buffer_size = 0;

int device_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

int device_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

/*onebyte_read reads One byte from this device and returns to the user*/
ssize_t device_read(struct file *filep, char *buf, size_t
			count, loff_t *f_pos)
{
	int bytes_read = 0;
	int length = count;
	int remain;
	if(*device_data == 0){
		return 0;
	}

	if(count > DEVICE_SIZE){
		length = DEVICE_SIZE;
		remain = count - DEVICE_SIZE;
	}

	while(*device_data && length){
		put_user( *(device_data++), buf++);
		length--;
		bytes_read++;
	}
	printk(KERN_INFO "Read : %d bytes", bytes_read);

	return bytes_read;
}

/*
onebyte_write writes one byte to this device
if more than one byte is attempted to write it throws error
*/
ssize_t device_write(struct file *filep, const char __user *buf,
			size_t count, loff_t *f_pos)
{
	/*please complete the function on your own*/
	printk(KERN_INFO "onebyte_write(count : %ld)", count);

	device_buffer_size = count;

	//check if it is more than one byte write operation
	if( count > DEVICE_SIZE ){
		device_buffer_size = DEVICE_SIZE;
	}
	
	//Copy incoming buffer to device_data
        if( copy_from_user( device_data, buf, device_buffer_size ) ){
		return -EFAULT;
	}
	
	return device_buffer_size;
}

loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
    struct scull_dev *dev = filp->private_data;
    loff_t newpos;

    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = off;
        break;

      case 1: /* SEEK_CUR */
        newpos = filp->f_pos + off;
        break;

      case 2: /* SEEK_END */
        newpos = dev->size - off;
        break;

      default: /* can't happen */
        return -EINVAL;
    }
    if (newpos < 0) return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}

char dev_msg[100];
char user_msg[100];

long ioctl_example(struct file *filp, unsigned int cmd, unsigned long arg){
	int err = 0;
	int retval = 0, i;

	if(_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;

	if(_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

	if(_IOC_DIR(cmd) && _IOC_READ){
		err = !access_ok(VERIFY_WRITE, (void __user*)arg, _IOC_SIZE(cmd));
	} else if(_IOC_DIR(cmd) && _IOC_WRITE){
		err = !access_ok(VERIFY_READ, (void __user*)arg, _IOC_SIZE(cmd));
	} 

	if(err){
		return -ENOTTY;
	}

	switch(cmd){
		case SCULL_HELLO:
			printk(KERN_WARNING "hello\n");
			break;
		case SCULL_READ:
			printk(KERN_WARNING "Read ioctl");
			copy_to_user((char *)arg, user_msg, 100);
			break;
		case SCULL_WRITE:
			printk(KERN_WARNING "Write ioctl");
			for(i =0; i < 40; i++){
			      user_msg[i] = dev_msg[i];
			}
			copy_from_user(dev_msg, (char *)arg, 100);
                        printk(KERN_WARNING "New vale of dev_msg : %s", dev_msg);
			break;
		default:
			return -ENOTTY;
	}

	return retval;
}

static int device_init(void)
{
	printk(KERN_INFO "Initializing the One Byte Charachter Device Driver");

	// register the device
	//Let Kernel allocate us a MAJOR_NUMBER instead of using 61
	MAJOR_NUMBER = register_chrdev(0, "Four_MB_Device", &device_fops);

	if ( MAJOR_NUMBER < 0 ) {
		return MAJOR_NUMBER;
	}

	printk(KERN_INFO "Our One Byte Device registered sucessfully! with %d Major Number", MAJOR_NUMBER);
	// allocate 4 MB of memory for storage
	// kmalloc is just like malloc, the second parameter is
	// the type of memory to be allocated.
	// To release the memory allocated by kmalloc, use kfree.
	device_data = kmalloc( 4 * 1024 * 1024  * sizeof(char), GFP_KERNEL );  //allocate 4MB of data

	if (!device_data) {
		device_exit();
		// cannot allocate memory
		// return no memory error, negative signify a failure
	
		return -ENOMEM;
	}

	// initialize the value to be X
	*device_data = 'X';
	printk(KERN_ALERT "This is a 4MB device module\n");
	printk(KERN_INFO "Create a device file using mknod:\n");
	printk( KERN_INFO "mknod /dev/%s -m 0777 c %d 0\n", DEVICE_NAME, MAJOR_NUMBER );

	return 0;
}

static void device_exit(void)
{
	// if the pointer is pointing to something
	if (device_data) {
		// free the memory and assign the pointer to NULL
		kfree(device_data);
		device_data = NULL;
	}

	// unregister the device
	unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
	printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(device_init);
module_exit(device_exit);

