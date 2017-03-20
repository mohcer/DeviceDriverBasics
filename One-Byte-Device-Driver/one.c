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
#include <asm/uaccess.h>

#define DEVICE_NAME "OneByte"

/*Forward Declaration*/
int onebyte_open(struct inode *inode, struct file *filep);

int onebyte_release(struct inode *inode, struct file *filep);

ssize_t onebyte_read(struct file *filep, char *buf, size_t
			count, loff_t *f_pos);

ssize_t onebyte_write(struct file *filep, const char *buf,
			size_t count, loff_t *f_pos);

static void onebyte_exit(void);

/* definition of file_operation structure */
struct file_operations onebyte_fops = {
	.read   = onebyte_read,
	.write  = onebyte_write,
	.open   =  onebyte_open,
	.release= onebyte_release
};

static char *onebyte_data = NULL;

static int MAJOR_NUMBER;

static short size_of_message;

int onebyte_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

int onebyte_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

/*
onebyte_read reads One byte from this device and returns to the user
*/
ssize_t onebyte_read(struct file *filep, char *buf, size_t
			count, loff_t *f_pos)
{
	/*please complete the function on your own*/
	int is_error = 0;

	is_error = copy_to_user( onebyte_data, buf, size_of_message );
	if( 0 == is_error ){
		printk(KERN_INFO "Sucessfully Read %d character to User", size_of_message);

		return ( size_of_message = 0 ); //clear the position to start and return 0
	} else {
		printk(KERN_INFO "Failed to Read %d character from Device", is_error);

		return -EFAULT;
	}
}

ssize_t onebyte_write(struct file *filep, const char *buf,
			size_t count, loff_t *f_pos)
{
	/*please complete the function on your own*/
	size_t allowed_to_write = 1;
	char *_onebyte = kmalloc( sizeof(char), GFP_KERNEL );
	*_onebyte = buf[0];

	get_user( onebyte_data, _onebyte );
	if( count > allowed_to_write ){

		return -EFAULT;
	} 

	return 0;
}

static int onebyte_init(void)
{
	printk(KERN_INFO "Initializing the One Byte Charachter Device Driver");

	// register the device
	//Let Kernel allocate us a MAJOR_NUMBER instead of using 61
	MAJOR_NUMBER = register_chrdev(0, "onebyte", &onebyte_fops);

	if ( MAJOR_NUMBER < 0 ) {
		return MAJOR_NUMBER;
	}

	printk(KERN_INFO "Our One Byte Device regustered sucessfully! with %d Major Number", MAJOR_NUMBER);
	// allocate one byte of memory for storage
	// kmalloc is just like malloc, the second parameter is
	// the type of memory to be allocated.
	// To release the memory allocated by kmalloc, use kfree.
	onebyte_data = kmalloc(sizeof(char), GFP_KERNEL);

	if (!onebyte_data) {
		onebyte_exit();
		// cannot allocate memory
		// return no memory error, negative signify a failure
	
		return -ENOMEM;
	}

	// initialize the value to be X
	*onebyte_data = 'X';
	printk(KERN_ALERT "This is a onebyte device module\n");
	printk(KERN_INFO "Create a device file using mknod:\n");
	printk( KERN_INFO "mknod %s c %d 0\n", DEVICE_NAME, MAJOR_NUMBER );

	return 0;
}

static void onebyte_exit(void)
{
	// if the pointer is pointing to something
	if (onebyte_data) {
		// free the memory and assign the pointer to NULL
		kfree(onebyte_data);
		onebyte_data = NULL;
	}

	// unregister the device
	unregister_chrdev(MAJOR_NUMBER, "onebyte");
	printk(KERN_ALERT "Onebyte device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);

