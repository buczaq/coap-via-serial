#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../common/common.h"
#include "functions.h"

/**
  * @brief Constantly listen for data on device.
  * 
  * @return Data received from server.
**/
unsigned char* receive_data(int fd)
{
	unsigned char* read_buffer_to_return = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	unsigned char read_buffer[256] = { '\0' };
	int bytes_read = 0;
	while(read_buffer[0] != 0xa1) {
		bytes_read = read(fd, &read_buffer, 1);
	}
	bytes_read = read(fd, &read_buffer[1], BUFFER_SIZE);
	for(int i = 0; i < bytes_read; i++) {
		read_buffer_to_return[i] = read_buffer[i];
	}
	return read_buffer_to_return;
}

/**
  * @brief Process received CoAP message.
  * 
  * @return Path to resources referred in server's request.
**/
char* process_coap(unsigned char* buffer, unsigned int length, char* post_payload, struct MessageData* message_data)
{
	char* path = (char*)malloc(sizeof(char) * BUFFER_SIZE);

	switch(buffer[1])
	{
		case 1:
			path = process_get(buffer, length, message_data);
			break;
		case 2:
			path = process_post(buffer, length, post_payload, message_data);
			break;
		default:
			break;
	}

	return path;
}

/**
  * @brief Process CoAP GET request.
  * 
  * @return Path to resources referred in server's request.
**/
char* process_get(unsigned char* buffer, unsigned int length, struct MessageData* message_data)
{
	// assuming that token is 2 bytes long
	unsigned int header_len = 6;
	bool host_processed = false;

	char* get_path = (char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	unsigned int get_path_iterator = 0;

	save_message_parameters(message_data, &buffer[2], &buffer[4]);

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

/**
  * @brief Process CoAP POST request.
  * 
  * @return Path to resources referred in server's request.
**/
char* process_post(unsigned char* buffer, unsigned int length, char* post_payload, struct MessageData* message_data)
{
	// assuming that token is 2 bytes long and whole header is 5 bytes
	unsigned int header_len = 6;
	bool host_processed = false;

	char* post_path = (char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	unsigned int post_path_iterator = 0;
	unsigned int last_delta = 0;
	int opt_delta_sum = 0;

	save_message_parameters(message_data, &buffer[2], &buffer[4]);

	for(int i = header_len; i < header_len + length;) {
		last_delta = buffer[i] >> 4;
		opt_delta_sum += last_delta;
		if(last_delta == 13) {
			i++;
			opt_delta_sum += buffer[i];
		}
		// check for block indicator
		if(opt_delta_sum == 27) {
			// ignore block info and just go for data
			while(buffer[i] != 0xff) {
				i++;
			}
			i++;
			int j = 0;
			while(buffer[i] != '\0') {
				post_payload[j] = buffer[i];
				i++;
				j++;
			}
			break;
		}
		// check for uri_host option
		if((last_delta == 3) && !host_processed) {
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
				post_path[post_path_iterator] = buffer[i];
				i++;
				post_path_iterator++;
			}
			post_path[post_path_iterator] = '/';
			post_path_iterator++;
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
				post_path[post_path_iterator] = buffer[i];
				i++;
				post_path_iterator++;
			}
			post_path[post_path_iterator] = '/';
			post_path_iterator++;
		} else {
			break;
		}
	}
	//delete last slash
	post_path[--post_path_iterator] = '\0';

	return post_path;
}

/**
  * @brief Get CoAP message parameters (MID, token) and use them when building response.
**/
void save_message_parameters(struct MessageData* message_data, unsigned char* message_id, unsigned char* token)
{
	message_data->message_id[0] = message_id[0];
	message_data->message_id[1] = message_id[1];
	message_data->token[0] = token[0];
	message_data->token[1] = token[1];
}

/**
  * @brief Example function for getting resources.
**/
int16_t get_temperature_value(struct Resources* resources)
{
	return resources->temperature_value;
}

/**
  * @brief Example function for getting resources.
**/
int16_t get_humidity_value(struct Resources* resources)
{
	return resources->humidity_value;
}

/**
  * @brief Example function for setting resources.
**/
int16_t set_temperature_value(struct Resources* resources, int16_t value)
{
	resources->temperature_value = value;
	return resources->temperature_value;
}

/**
  * @brief Example function for setting resources.
**/
int16_t set_humidity_value(struct Resources* resources, int16_t value)
{
	resources->humidity_value = value;
	return resources->humidity_value;
}

/**
  * @brief Part of GET request processing. Check value under path reffered by server's request and send it via CoAP as a response.
**/
void check_resources_and_send_response(int fd, unsigned char* message, struct Resources* resources, struct MessageData* message_data)
{
	int16_t resource_to_send;
	if(strstr((const char*)message, resources->temperature) != NULL) {
		resource_to_send = get_temperature_value(resources);
		printf("[INF] Sending temperature: \"%d\"...\n", resource_to_send);
	} else if(strstr((const char*)message, resources->humidity) != NULL) {
		resource_to_send = get_humidity_value(resources);
		printf("[INF] Sending humidity: \"%d\"...\n", resource_to_send);
	}
	char* response = (char*)malloc(sizeof(unsigned char) * 4);
	for(int i = 0; i < 4; i++) {
		response[i] = '\0';
	}

	sprintf(response, "%d", resource_to_send);

	unsigned char* coap_response = build_coap_response_with_header(message_data, response);
	int response_length = count_whole_message_size(coap_response);

	int bytes_written = 0;
	bytes_written = write(fd, coap_response, response_length);
	printf("[INF] Bytes written: %d\n", bytes_written);
	free(response);
}

/**
  * @brief Part of POST request processing. Set value under path reffered by server's request and send it via CoAP as a response (as a confirmation).
**/
void set_resources_and_send_response(int fd, unsigned char* message, struct Resources* resources, char* post_payload, bool DEBUG_FLAG, struct MessageData* message_data)
{
	int16_t resource_to_set = (int)strtol(post_payload, (char **)NULL, 10);
	if(DEBUG_FLAG) {
		printf("\n[DBG] Post payload is: %d\n", resource_to_set);
	}
	if(strstr((const char*)message, resources->temperature) != NULL) {
		resource_to_set = set_temperature_value(resources, resource_to_set);
		printf("[INF] Setting temperature to: \"%d\"...\n", resource_to_set);
	} else if(strstr((const char*)message, resources->humidity) != NULL) {
		resource_to_set = set_humidity_value(resources, resource_to_set);
		printf("[INF] Setting humidity to: \"%d\"...\n", resource_to_set);
	}

	char* response = (char*)malloc(sizeof(unsigned char) * 4);
	for(int i = 0; i < 4; i++) {
		response[i] = '\0';
	}

	sprintf(response, "%d", resource_to_set);

	unsigned char* coap_response = build_coap_response_with_header(message_data, response);
	int response_length = count_whole_message_size(coap_response);

	int bytes_written = 0;
	bytes_written = write(fd, coap_response, response_length);
	printf("[INF] Bytes written: %d\n", bytes_written);
	free(response);
}

/**
  * @brief Build a response and wrap it with shim header.
  * 
  * @return CoAP message with a shim sheader.
**/
unsigned char* build_coap_response_with_header(struct MessageData* message_data, char* response)
{
	unsigned char* message = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	message[0] = 62; // CoAP version 1, ACK, token length 2
	message[1] = 69; // 2.05 Content marker
	message[2] = message_data->message_id[0];
	message[3] = message_data->message_id[1];
	message[4] = message_data->token[0];
	message[5] = message_data->token[1];
	message[6] = 255; // end of options marker
	for(int i = 0; i < PAYLOAD_SIZE; i++) {
		message[i + 7] = response[i];
	}

	unsigned char* message_with_header = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	message_with_header = create_message_with_header(message);
	free(message);
	return message_with_header;
}
