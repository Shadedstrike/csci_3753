

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("April Ott");

/* Define device_buffer and other global data structures you will need here */


#define BUFFER_SIZE 1024

//62 is the device major number

static char *device_buffer; //create the buffer, kernel-side

// count open/close functions.
int open_count = 0;
int close_count = 0;


ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
  printk(KERN_ALERT "User has entered the read function \n");

	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
  int buff_size = strlen(device_buffer);
  int max_bytes = BUFFER_SIZE - *offset;
  int bytes_input; //bytes in and out
  int bytes_read;

  if (buff_size == 0) {
    printk(KERN_ALERT "The buffer is currently empty \n");
    return 0;
  }

  if (max_bytes > length) { //if bigger
    bytes_input = length;
  }
  else {
    bytes_input = max_bytes;
  }

  bytes_read = bytes_input - copy_to_user(buffer,device_buffer + *offset,bytes_input); //copy to user userspace

  *offset += bytes_read;
	//print out total bytes moved
	printk(KERN_ALERT "Total bytes read = %d , offset is %lld \n", bytes_read, *offset);
	return 0;
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	int buff_size = strlen(device_buffer);

	if (buff_size == 0) {
    printk(KERN_ALERT "Buff size is == 0 \n");
		return 0;
	} //check if empty

	*offset = buff_size;
	printk(KERN_ALERT "Now writing to the device \n");
	//use copy from user to pull in data
	copy_from_user(device_buffer + *offset, buffer, length);

	printk(KERN_ALERT "The device has been written to, bytes: %d, current offset: %lld  \n", strlen(buffer), *offset);
	return length;
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT "The device is now open \n");

	open_count++; //add one to open var
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	printk(KERN_ALERT "The device has been opened this many times: %d \n", open_count);

	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	printk(KERN_ALERT "The device is now closed \n");
  close_count++;
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	printk(KERN_ALERT "The device has been closed this many times: %d \n", close_count);

	return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
      printk(KERN_ALERT "inside %s function\n", __FUNCTION__);
	int current_pos = 0; //hold the current position
//  starting_address = device_buffer;
  //ending_address = device_buffer + BUFFER_SIZE;

	/* Update open file position according to the values of offset and whence */
	if (whence == 0) { //If the value of whence is 0 (SEEK_SET), the current position is set to the value of the offset.
		current_pos = offset;
	}
	if (whence == 1) {
		current_pos += offset; //increment by offset
	}
	if (whence == 3) { //current position is set to offset bytes before the end of the file
    current_pos = BUFFER_SIZE - offset;
	}
	if (whence != 0 && whence != 1 && whence !=2) { //if user attempts to seek before or after the file
		printk(KERN_ALERT "Wrong whence value inputted! \n");
	}
	if (current_pos < 0) {
		printk(KERN_ALERT "ERROR: Cannot seek before the buffer! \n");
	}
	if (current_pos > BUFFER_SIZE) {
		printk(KERN_ALERT "ERROR: Cannot after before the buffer! \n");
	}
	printk(KERN_ALERT "The current offset in this seek is: %lld \n", offset);

	return current_pos;
}

struct file_operations pa2_char_driver_file_operations = {
	/*  we need to have  open, release, read, write and seek */
	.owner   = THIS_MODULE,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
	.read = pa2_char_driver_read,
	.write = pa2_char_driver_write,
	.open = pa2_char_driver_open,
	.release = pa2_char_driver_close,
	.llseek = pa2_char_driver_seek,
};

static int pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	printk(KERN_ALERT "pa2_char_driver is now initializing, hello!\n");

	/* register the device */
  int reg_result;
	reg_result = register_chrdev(62, "pa2_char_device", &pa2_char_driver_file_operations);
  if (reg_result == 0) {
    printk(KERN_ALERT "register_chrdev returned succesfully\n");

  }
	/* allocate memory to store data */
	device_buffer = kmalloc(sizeof *device_buffer, GFP_KERNEL);

	printk(KERN_ALERT "device buffer allocated!, device registered\n");

	return 0;
}

static void pa2_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	printk(KERN_ALERT "pa2_char_driver is now exiting, goodbye!\n");

	/*free previously allocated memory*/
	 kfree(device_buffer);

	/* unregister  the device using the register_chrdev() function. */
	unregister_chrdev(62, "pa2_char_device");

}

/* add module_init and module_exit to point to the corresponding init and exit function*/

module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);
