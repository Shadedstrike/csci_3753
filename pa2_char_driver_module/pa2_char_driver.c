

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/unistd.h> //avoid implicit error
#include <linux/string.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("April Ott");

/* Define device_buffer and other global data structures you will need here */

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence);


#define BUFFER_SIZE 1024

//62 is the device major number

char *device_buffer; //create the buffer, kernel-side

// count open/close functions.
int open_count = 0;
int close_count = 0;


ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
  int max_bytes;
  int bytes_to_read; //bytes in and out
  int bytes_read;

  printk(KERN_ALERT "User has entered the read function \n");

  if (length > BUFFER_SIZE) {
    printk(KERN_ALERT "ERROR:  User's input length is over buffer limits! \n");
    length = BUFFER_SIZE;
  }
  if (length < 0) {
    printk(KERN_ALERT "ERROR: User's length is negative! \n");
    length = BUFFER_SIZE;
  }
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
  //int buff_size = strlen(device_buffer);
  max_bytes = BUFFER_SIZE - *offset;


  // if (strlen(buffer) == 0) {
  //   printk(KERN_ALERT "The buffer is currently empty \n");
  //   return 0;
  // }

  if (max_bytes > length) { //if bigger
    bytes_to_read = length;
  }
  else {
    bytes_to_read = BUFFER_SIZE - length;
  }

  bytes_read = bytes_to_read - copy_to_user(buffer,device_buffer+ *offset,bytes_to_read); //copy to user userspace

  *offset += bytes_read;
	//print out total bytes moved
	printk(KERN_ALERT "Total bytes read = %d , offset is %lld \n", bytes_read, *offset);
	return bytes_read;
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	// int buff_size = sizeof(device_buffer);
  // int maximum_bytes = BUFFER_SIZE - *offset; //max bytes that can be read
  // int bytes_to_write;
  // int bytes_write;
  // loff_t *temp_offset;
  // temp_offset = offset;
  //
	// if (buff_size == 0) {
  //   printk(KERN_ALERT "Buff size is == 0 \n");
	// 	return 0;
	// } //check if empty
  //
  // if (maximum_bytes > length) { //if bigger
  //     bytes_to_write = length;
  // }
  // else {
  //     temp_offset = 0;
  //     bytes_to_write = length;
  // }


//	*offset = sizeof(buffer);
	printk(KERN_ALERT "Now writing to the device \n");

  if (length > BUFFER_SIZE) {
    printk(KERN_ALERT "ERROR:  User's input length is over buffer limits! \n");
    length = BUFFER_SIZE;
  }
  if (length < 0) {
    printk(KERN_ALERT "ERROR: User's length is negative! \n");
    length = BUFFER_SIZE;
  }
	//use copy from user to pull in data
	copy_from_user(device_buffer + *offset, buffer, length);

  *offset += length;
  printk(KERN_ALERT "The device has been written to, bytes: %lu, current offset: %lld  \n", length, *offset);

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
  int current_pos = 0; //hold the current position
  printk(KERN_ALERT "inside %s function\n", __FUNCTION__);
//  starting_address = device_buffer;
  //ending_address = device_buffer + BUFFER_SIZE;

	/* Update open file position according to the values of offset and whence */
	if (whence == 0) { //If the value of whence is 0 (SEEK_SET), the current position is set to the value of the offset.
    current_pos = offset;
  }
	if (whence == 1) {
		current_pos = pfile->f_pos + offset; //increment by offset
	}
	if (whence == 2) { //current position is set to offset bytes before the end of the file
    current_pos = BUFFER_SIZE - offset;
	}
	if (whence != 0 && whence != 1 && whence !=2) { //if user attempts to seek before or after the file
		printk(KERN_ALERT "Wrong whence value inputted! \n");
	}

//check for out of bounds, if so, keep position where it initally was.
	if (current_pos < 0) {
		printk(KERN_ALERT "ERROR: Cannot seek before the buffer! \n");
    current_pos = pfile->f_pos;
	}
	if (current_pos > BUFFER_SIZE) {
		printk(KERN_ALERT "ERROR: Cannot seek past the buffer! \n");
    current_pos = pfile->f_pos;
	}
	printk(KERN_ALERT "The current position in this seek is now: %d \n", current_pos);
  pfile->f_pos = current_pos;
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
	.llseek = pa2_char_driver_seek
};

static int pa2_char_driver_init(void)
{
  int reg_result;
	/* print to the log file that the init function is called.*/
	printk(KERN_ALERT "pa2_char_driver is now initializing, hello!\n");

	/* register the device */
	reg_result = register_chrdev(62, "pa2_char_device", &pa2_char_driver_file_operations);
  if (reg_result == 0) {
    printk(KERN_ALERT "register_chrdev returned succesfully\n");

  }
	/* allocate memory to store data */
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);

//  bzero(device_buffer, BUFFER_SIZE);

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
