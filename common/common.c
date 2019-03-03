#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

unsigned char* create_message_with_header(unsigned char* buffer)
{
	unsigned char* message_with_header = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	unsigned int coap_size = count_actual_buffer_size(buffer);
	message_with_header[0] = 0xa1;

	unsigned int ext_len_index = 0;
	if(coap_size > BUFFER_SIZE - 1) {
		message_with_header[++ext_len_index] = 0x00;
		message_with_header[++ext_len_index] = (coap_size & 0xff00) >> 8;
		message_with_header[++ext_len_index] = (coap_size & 0x00ff);
	} else {
		message_with_header[++ext_len_index] = coap_size;
	}

	//hardcoded values
	message_with_header[++ext_len_index] = 128;
	message_with_header[++ext_len_index] = 0;
	message_with_header[++ext_len_index] = 129;
	message_with_header[++ext_len_index] = 0;
	ext_len_index++;

	for(int i = 0; i < coap_size; i++) {
		message_with_header[i + ext_len_index] = buffer[i];
	}
	message_with_header[coap_size + ext_len_index] = '\0';

	return message_with_header;
}

unsigned int count_actual_buffer_size(unsigned char* buffer)
{
	unsigned int size = 0;
	while(!(buffer[size] == '\0'
	        && buffer[size + 1] == '\0'
			&& buffer[size + 2] == '\0'
			&& buffer[size + 3] == '\0')) {
		size++;
	}

	return size;
}

unsigned int count_whole_message_size(unsigned char* buffer)
{
	unsigned int size = 0;
	// there is no case where 4 actual coap bytes in a row will be 0
	while(!(buffer[size] == '\0'
	        && buffer[size + 1] == '\0'
			&& buffer[size + 2] == '\0'
			&& buffer[size + 3] == '\0')) {
		size++;
	}

	return size + 1;
}

unsigned char* data_to_coap(unsigned char* buffer, unsigned int* length, bool DEBUG_FLAG)
{
	int source[2] = { 0 };
	int destination[2] = { 0 };
	unsigned int ext_len_base = 0;
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
	unsigned char* coap_msg = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	for(int i = 0; i < *length; i++) {
		coap_msg[i] = buffer[6 + i + ext_len_base];
	}
	if(DEBUG_FLAG) {
		printf("[DBG] Received message: ");
		for(int i = 0; i < *length; i++) {
			printf("[%i]:%d(%c)\t", i, (unsigned int)coap_msg[i], coap_msg[i]);
		}
		printf("\n");
	}
	printf("[INF] Source: %d.%d\nDestination: %d.%d\n", source[0], source[1], destination[0], destination[1]);
	return coap_msg;
}

bool open_device(int* fd, const char* device)
{
	*fd = open(device,O_RDWR | O_NOCTTY);
	if(*fd > 0) {
		printf("[INF] Device %s opened successfully\n", device);
		return true;
	}
	else {
		printf("[INF] Error in opening device, aborting...\n");
		return false;
	}
}
