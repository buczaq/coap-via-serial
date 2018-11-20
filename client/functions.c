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

struct Resources {
	char* temperature;
	char* humidity;
	int16_t temperature_value;
	int16_t humidity_value;
};

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
	while(read_buffer[0] != 0xa1) {
		bytes_read = read(fd, &read_buffer, BUFFER_SIZE);
	}
	for(int i = 0; i < bytes_read; i++) {
		read_buffer_to_return[i] = read_buffer[i];
	}
	return read_buffer_to_return;
}

unsigned char* data_to_coap(unsigned char* buffer, unsigned int* length)
{
	int source[2] = { 0 };
	int destination[2] = { 0 };
	unsigned int ext_len_base = 0;
	printf("%d", (unsigned int) buffer[0]);
	if(buffer[0] == 0xa1) {
		*length = (unsigned int)buffer[1];
		if(*length == 0) {
			ext_len_base = 2;
			*length = (unsigned int)buffer[2] + (unsigned int)buffer[3];
		}
		source[0] = (unsigned int)buffer[2 + ext_len_base];
		source[1] = (unsigned int)buffer[3 + ext_len_base];
		destination[0] = (unsigned int)buffer[4 + ext_len_base];
		destination[1] = (unsigned int)buffer[5 + ext_len_base];
	}
	unsigned char* coap_msg = malloc(sizeof(unsigned char) * BUFFER_SIZE);
	for(int i = 0; i < *length; i++) {
		coap_msg[i] = buffer[6 + i + ext_len_base];
	}
//	printf("Received message: ");
//	for(int i = 0; i < *length; i++) {
//		printf("[%i]:%d(%c)\t", i, (unsigned int)coap_msg[i], coap_msg[i]);
//	}
	printf("\n");
	return coap_msg;
	//printf("\nSource: %d.%d\nDestination: %d.%d", source[0], source[1], destination[0], destination[1]);
}

unsigned char* process_coap(unsigned char* buffer, unsigned int length)
{
	unsigned char* message_to_send = malloc(sizeof(unsigned char) * BUFFER_SIZE);

	switch(buffer[1])
	{
		case 1:
			message_to_send = process_get(buffer, length);
			break;
		case 2:
			message_to_send = process_post(buffer, length);
			break;
		default:
			break;
	}

	return message_to_send;
}

// TODO: add processing logic
unsigned char* process_get(unsigned char* buffer, unsigned int length)
{
	// assuming that token is not set and header is 4 bytes
	unsigned int header_len = 4;
	bool host_processed = false;

	char* get_path = malloc(sizeof(unsigned char) * BUFFER_SIZE);;
	unsigned int get_path_iterator = 0;

	for(int i = header_len; i < header_len + length;) {
		// check for uri_host option
		if(((buffer[i] >> 4) == 3) && !host_processed) {
			host_processed = true;
			unsigned int ext_uri_length = 0;
			unsigned int regular_uri_length = buffer[i] & 0x0f;
			if(regular_uri_length == 13) {
				i++;
				ext_uri_length = buffer[i];
			}
			unsigned int whole_length = regular_uri_length + ext_uri_length;
			i++;
			for(int n = 0; n < whole_length; n++) {
				get_path[get_path_iterator] = buffer[i];
				i++;
				get_path_iterator++;
			}
			get_path[get_path_iterator] = '/';
			get_path_iterator++;
		} else if(buffer[i]) {
			unsigned int ext_uri_length = 0;
			unsigned int regular_uri_length = buffer[i] & 0x0f;
			if(regular_uri_length == 13) {
				i++;
				ext_uri_length = buffer[i];
			}
			unsigned int whole_length = regular_uri_length + ext_uri_length;
			i++;
			for(int n = 0; n < whole_length; n++) {
				get_path[get_path_iterator] = buffer[i];
				i++;
				get_path_iterator++;
			}
			get_path[get_path_iterator] = '/';
			get_path_iterator++;
		} else {
			break;
		}
	}
	//delete last slash
	get_path[--get_path_iterator] = '\0';
	return get_path;
}

// TODO: add processing logic
unsigned char* process_post(unsigned char* buffer, unsigned int length)
{
	return "dummy";
}

int16_t get_temperature_value()
{
	struct Resources r = { "0.0.0.0/temperature", "0.0.0.0/humidity", 23, 71 };
	return r.temperature_value;
}
int16_t get_humidity_value()
{
	struct Resources r = { "0.0.0.0/temperature", "0.0.0.0/humidity", 23, 71 };
	return r.humidity_value;
}

void check_resources_and_send_response(int fd, unsigned char* message)
{
	struct Resources r = { "0.0.0.0/temperature", "0.0.0.0/humidity", 23, 71 };
	uint16_t resource_to_send;
	if(strcmp(message, r.temperature) == 0) {
		resource_to_send = get_temperature_value();
		printf("Sending temperature: \"%d\"...\n", resource_to_send);
	} else if(strcmp(message, r.humidity) == 0) {
		resource_to_send = get_humidity_value();
		printf("Sending humidity: \"%d\"...\n", resource_to_send);
	}
	unsigned char* write_buffer = malloc(sizeof(unsigned char) * 4);
	for(int i = 0; i < 4; i++) {
		write_buffer[i] = '\0';
	}

	sprintf(write_buffer, "%d", resource_to_send);

	int bytes_written = 0;

	bytes_written = write(fd, write_buffer, 4);
	printf("bytes written: %d\n", bytes_written);
}
