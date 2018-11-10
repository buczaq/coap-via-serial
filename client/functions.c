#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <sys/types.h>

#include "constant.h"
#include "functions.h"

bool open_device(int* fd, const char* device)
{
	*fd = open(device,O_RDWR | O_NOCTTY);
	if(*fd > 0) {
		printf("\nDevice %s opened successfully\n", device);
		return true;
	}
	else {
		printf("\nError in opening device, aborting...\n");
		return false;
	}
}

unsigned char* receive_data(int fd)
{
	tcflush(fd, TCIFLUSH);
	unsigned char* read_buffer_to_return = malloc(sizeof(unsigned char) * BUFFER_SIZE);
	unsigned char read_buffer[256] = { '\0' };
	int bytes_read = 0;
	bytes_read = read(fd, &read_buffer, BUFFER_SIZE);
	for(int i = 0; i < bytes_read; i++) {
		read_buffer_to_return[i] = read_buffer[i];
	}
	return read_buffer_to_return;
}

unsigned char* data_to_coap(unsigned char* buffer)
{
	unsigned int length = 0;
	int source[2] = { 0 };
	int destination[2] = { 0 };
	unsigned int ext_len_base = 0;
	printf("%d", (unsigned int) buffer[0]);
	if(buffer[0] == 0xa1) {
		length = (unsigned int)buffer[1];
		if(length == 0) {
			ext_len_base = 2;
			length = (unsigned int)buffer[2] + (unsigned int)buffer[3];
		}
		source[0] = (unsigned int)buffer[2 + ext_len_base];
		source[1] = (unsigned int)buffer[3 + ext_len_base];
		destination[0] = (unsigned int)buffer[4 + ext_len_base];
		destination[1] = (unsigned int)buffer[5 + ext_len_base];
	}
	unsigned char* coap_msg = malloc(sizeof(unsigned char) * BUFFER_SIZE);
	for(int i = 0; i < length; i++) {
		coap_msg[i] = buffer[6 + i + ext_len_base];
	}
	printf("Received message: ");
	for(int i = 0; i < length; i++) {
		printf("%d ", (unsigned int)coap_msg[i]);
	}
	return coap_msg;
	//printf("\nSource: %d.%d\nDestination: %d.%d", source[0], source[1], destination[0], destination[1]);
}
