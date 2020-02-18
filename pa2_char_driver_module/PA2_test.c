#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h> //avoid implicit error
///dev/pa2_character_device c 62 0
#define DEVICENAME "/dev/pa2_char_device" //define name and such
#define BUFFER_SIZE 1024

int main(){
	char command_input;
	int length, whence, new_offset;
	char buffer[BUFFER_SIZE];

	int file = open(*DEVICENAME, O_RDWR); //open the device
	printf("file opening is status %d \n", file);

	bool running = true;
	while(running){ //while still running
		printf("Here are the commands you can use:\n");

		printf("press	'r' to read from device\n");
		printf("press	'w' to write to device\n");
		printf("press	's' to seek into device\n");
		printf("press	'e' to exit from device\n");
		printf("press	anything else to keep reading or writing to the device\n");
		printf("command$> ");
		scanf("%c", &command_input);

		switch(command_input){
			case 'r':
				printf("read$> How many bytes to read?: ");
				scanf("%d", &length);
				read(file, buffer, length);
				printf("read$> %s\n", buffer); // print that buffer
				while(getchar() != '\n'); // check for end line
				break;

			case 'w':
				printf("write$> ");
				scanf("%s", buffer);
				write(file, buffer, BUFFER_SIZE); // write the buffer to file
				while (getchar() != '\n'); // check for end line
				break;

			case 's':
				printf("Seek commands are as follows:\n");
				printf("	'0' seek set\n");
				printf("	'1' seek cur\n");
				printf("	'2' seek end\n");
				printf("	anything else returns to the menu\n");
				printf("seek$> Enter whence: ");
				scanf("%d", &whence);
				printf("\nwrite$> Enter an offset value: ");
				scanf("%d", &new_offset);
				llseek(file, new_offset, whence);
				break;

			case 'e':
				printf("exit$> Exiting\n");
				running = false;
				break;

			default:
				printf("\nerror$> error: not a valid command\n");
				break;
		}
	}
	close(file);
	return 0;
}
