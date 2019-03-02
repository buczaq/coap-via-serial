#include "common.h"

#include <stdlib.h>

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
