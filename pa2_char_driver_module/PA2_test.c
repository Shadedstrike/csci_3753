#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h> //avoid implicit error
#include <string.h>
///dev/pa2_character_device c 62 0
#define DEVICE "/dev/pa2_character_device"   //define name and such
#define BUFFER_SIZE 1024

int main(){
	printf("\n");
	printf("welcome to the { B U F F E R } main function\n");
	printf("\n");
	printf("/////////////////////////////////////////////\n");


	char *buffer;
	char command_input;
	int length, whence, new_offset;
	//static char *user_buffer; //declare user buf

	//user_buffer = malloc(BUFFER_SIZE);

	int file = open(DEVICE, O_RDWR); //open the device
	if (file >= 0) {
		printf("file opening status =  %d, good to go \n", file); //verify file is opened succesfully
		printf("/////////////////////////////////////////////\n");
		printf("\n");

	}
	else{
		printf("file opening status =  %d, not good! \n", file);
		printf("\n");

	}

	bool running = true;
	while(running){ //while still running
		printf("Here are the commands you can use:\n");
		printf("press	'r' to read from device\n");
		printf("press	'w' to write to device\n");
		printf("press	's' to seek into device\n");
		printf("press	'e' to exit from device\n");
		printf("press	'anything' else to go back to the menu!!\n");
		printf("command$> ");
		scanf(" %c", &command_input);

		switch(command_input){
			case 'r':
				buffer = malloc(BUFFER_SIZE);
				bzero(buffer,BUFFER_SIZE);

				printf("read$> How many bytes to read?: ");
				scanf("%d", &length);
				printf("entered %d length in read function\n", length );
				read(file, buffer, length);
				printf("read$> %s\n", buffer); // print that user_buffer
				while(getchar() != '\n'); // check for end line
        free(buffer);
				break;

			case 'w':
				buffer = malloc(BUFFER_SIZE);
				bzero(buffer,BUFFER_SIZE);

				printf("write$> ");
				scanf("%s", buffer);
				printf("user buffer is currently %s and buffer size is: %d \n", buffer, strlen(buffer));
			//	int writesize = strlen(buffer);
				write(file, buffer, strlen(buffer)); // write the user_buffer to file
				while (getchar() != '\n'); // check for end line
				free(buffer);
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
				lseek(file, new_offset, whence);
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
