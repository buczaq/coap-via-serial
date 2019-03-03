#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "functions.h"

void read_devices_list(struct Device* devices)
{
	FILE* fid;

	fid = fopen("devices.txt","r");
	char alias[100];
	int address;
	char location[100];

	int counter = 0;

	while (!feof(fid))
	{
		fscanf(fid, "%s\t%d\t%s", alias, &address, location);
		strcpy(devices[counter].alias, alias);
		devices[counter].address = address;
		strcpy(devices[counter].location, location);
	}

	fclose(fid);
}

unsigned char* send_coap_to_ser2net_port_and_wait_for_response(unsigned char* buffer, char* hostname, char* portname)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	struct addrinfo* res = 0;
	int err=getaddrinfo(hostname, portname, &hints, &res);
	if (err!=0) {
		printf("[ERR] Failed to resolve local socket address (err = %d)",err);
	}
	int sckt = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sckt == -1) {
    	printf("[ERR] Failed to create address (err = %d)",err);
	}
	connect(sckt, res->ai_addr, res->ai_addrlen);
	int write_bytes = write(sckt, buffer, count_whole_message_size(buffer));
	printf("[INF] Sent %d bytes.\n", write_bytes);
	char feed[] = "\n";
	write(sckt, feed, 1);

	char* tmp_buffer;

	// reading data that has just been sent in order to ignore it
	read(sckt, tmp_buffer, count_whole_message_size(buffer));

	unsigned char* response = (unsigned char*)malloc(sizeof(unsigned char) * PAYLOAD_SIZE);

	read(sckt, response, 4);

	close(sckt);
	freeaddrinfo(res);

	return response;
}

unsigned char* send_coap_to_raw_device_and_wait_for_response(unsigned char* buffer, char* destination)
{
	int fd;

	if(!open_device(&fd, destination)) {
		exit(1);
	}

	fcntl(fd, F_SETFL, 0);
	struct termios SerialPortSettings;
	tcgetattr(fd, &SerialPortSettings);

	cfsetispeed(&SerialPortSettings,B38400);
	cfsetospeed(&SerialPortSettings,B38400);

	SerialPortSettings.c_cflag &= ~PARENB;
	SerialPortSettings.c_cflag &= ~CSIZE;
	SerialPortSettings.c_cflag |= CS8;
	SerialPortSettings.c_iflag &= IXANY;

	SerialPortSettings.c_cflag |= CREAD | CLOCAL;

	SerialPortSettings.c_oflag &= ~OPOST;

	tcsetattr(fd, TCSANOW, &SerialPortSettings);

	int bytes_written = 0;

	bytes_written = write(fd, buffer, count_whole_message_size(buffer));

	printf("[INF] Bytes written: %d\n", bytes_written);

	unsigned char* response = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	fd_set fd_to_check;
	FD_SET(fd, &fd_to_check);

	int check_availability = select(fd+1, &fd_to_check, NULL, NULL, &tv);

	if(check_availability) {
		read(fd, response, BUFFER_SIZE);
	}
	else {
		printf("[WRN] Response has not been received! System might become unstable!");
	}

	close(fd);

	return response;
}

char* listen_for_http(int sckt, struct addrinfo* res, int accsckt)
{
	char* buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);

	struct sockaddr_storage src_addr;
	socklen_t src_addr_len=sizeof(src_addr);
	ssize_t count = 0;
	while(buffer[0] == '\0') {
		count = recvfrom(accsckt, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&src_addr,&src_addr_len);
	}
	if (count == -1) {
		printf("%s",strerror(errno));
	} else if (count==sizeof(buffer)) {
		printf("[WRN] datagram too large for buffer: truncated");
	} else {
		printf("\n[INF] Sucessfully received message\n");
	}

	return buffer;
}

MessageType recognize_http_message_type(char* http_message)
{
	char type[3];
	strncpy(type, http_message, 3);
	if(strcmp(type, "GET") == 0) {
		return GET;
	} else if (strcmp(type, "POS") == 0) {
		return POST;
	}
	else return GET;
}

unsigned char* http_to_coap(char* http_message, struct Device* devices, char* destination, struct MessageData* message_data)
{
	MessageType message_type = recognize_http_message_type(http_message);
	unsigned char* message_to_send = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	switch (message_type)
	{
		case GET:
			message_to_send = process_http_get(http_message, devices, destination, message_data);
			break;
		case POST:
			message_to_send = process_http_post(http_message, devices, destination, message_data);
			break;
		default:
			break;
	}
	return message_to_send;
}

char* look_for_device(struct Device* devices, char* destination)
{
	for(int i = 0; i < 16; i++) {
		if(strcmp(devices[i].alias, destination) == 0) {
			return devices[i].location;
		}
	}
	printf("[ERR] Device %s not found!", destination);
	exit(1);
}

unsigned char* process_http_get(char* message, struct Device* devices, char* destination, struct MessageData* message_data)
{
	unsigned char* coap_get = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	// hardcoded values
	//assuming no token
	coap_get[0] = 64 + 2; // CoAP version 1, confirmable, 2 byte token
	coap_get[1] = 2; // POST
	coap_get[2] = rand() % 256;; // message_id p1
	coap_get[3] = rand() % 256;; // message_id p2
	coap_get[4] = rand() % 256; // token 1
	coap_get[5] = rand() % 256; // token 2

	message_data->message_id[0] = coap_get[2];
	message_data->message_id[1] = coap_get[3];
	message_data->token[0] = coap_get[4];
	message_data->token[1] = coap_get[5];

	int i = 4;
	int coap_index = 4 + 2;
	char url[128] = { '\0' };
	bool host_is_specified = false;
	int opt_delta = 0;
	int last_opt_number = 0;
	int current_part_length = 0;
	int whole_url_length = 0;
	int url_index = 0;
	bool time_to_break = false;

	while(!time_to_break) {
		url[i - 4] = message[i];
		i++;
		current_part_length++;
		whole_url_length++;
		if(message[i] == '/' || message[i] == ' ') {
			if(message[i] == ' ') {
				time_to_break = true;
			}
			if(!host_is_specified) {
				opt_delta = 3;
				int ext_opt_len = 0;
				last_opt_number = 3;
				// hostname: max 24 bytes!
				// assuming that two ext-len bytes will not be used
				if(current_part_length > 13) {
					ext_opt_len = current_part_length - 13;
					current_part_length -= 13;
					coap_get[coap_index] = 0x3d;
					coap_index++;
				} else {
					coap_get[coap_index] = 0x30 + current_part_length;
					coap_index++;
				}

				if(ext_opt_len > 0) {
					coap_get[coap_index] = ext_opt_len;
					coap_index++;
				}
				host_is_specified = true;
				for(int m = 0; m < current_part_length; m++) {
					coap_get[coap_index] = url[url_index];
					destination[m] = url[url_index];
					url_index++;
					coap_index++;
				}
				current_part_length = 0;
			} else {
				// assuming that option is uri_path (optcode 11)
				opt_delta = abs(11 - last_opt_number);
				last_opt_number = 11;
				int ext_opt_len = 0;
				// hostname: max 24 bytes!
				int opt_len = current_part_length;
				if(opt_len > 12) {
					ext_opt_len = opt_len - 12;
					opt_len -= 12;
				}
				coap_get[coap_index] = (opt_delta << 4) + opt_len;
				coap_index++;
				if(ext_opt_len > 0) {
					coap_get[coap_index] = ext_opt_len;
					coap_index++;
				}
				for(int m = 0; m < current_part_length; m++) {
					coap_get[coap_index] = url[url_index];
					url_index++;
					coap_index++;
				}
				current_part_length = 0;
			}
			i++;
			url_index++;
		}
	}

	return coap_get;
}

unsigned char* process_http_post(char* message, struct Device* devices, char* destination, struct MessageData* message_data)
{
	unsigned char* coap_post = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
	// hardcoded values
	// 2 bytes long token
	coap_post[0] = 64 + 2; // CoAP version 1, confirmable, 2 byte token
	coap_post[1] = 2; // POST
	coap_post[2] = rand() % 256;; // message_id p1
	coap_post[3] = rand() % 256;; // message_id p2
	coap_post[4] = rand() % 256; // token 1
	coap_post[5] = rand() % 256; // token 2

	message_data->message_id[0] = coap_post[2];
	message_data->message_id[1] = coap_post[3];
	message_data->token[0] = coap_post[4];
	message_data->token[1] = coap_post[5];

	int i = 5;
	int coap_index = 4 + 2; // 2 for token
	char url[128] = { '\0' };
	bool host_is_specified = false;
	int opt_delta = 0;
	int opt_delta_sum = 0;
	int last_opt_number = 0;
	int current_part_length = 0;
	int whole_url_length = 0;
	int url_index = 0;
	bool time_to_break = false;

	while(!time_to_break) {
		url[i - 5] = message[i];
		i++;
		current_part_length++;
		whole_url_length++;
		if(message[i] == '/' || message[i] == ' ') {
			if(message[i] == ' ') {
				time_to_break = true;
			}
			if(!host_is_specified) {
				opt_delta = 3;
				opt_delta_sum += opt_delta;
				last_opt_number = 3;
				int ext_opt_len = 0;
				// hostname: max 24 bytes!
				// assuming that two ext-len bytes will not be used
				if(current_part_length > 13) {
					ext_opt_len = current_part_length - 13;
					current_part_length -= 13;
					coap_post[coap_index] = 0x3d;
					coap_index++;
				} else {
					coap_post[coap_index] = 0x30 + current_part_length;
					coap_index++;
				}

				if(ext_opt_len > 0) {
					coap_post[coap_index] = ext_opt_len;
					coap_index++;
				}
				host_is_specified = true;
				for(int m = 0; m < current_part_length; m++) {
					coap_post[coap_index] = url[url_index];
					destination[m] = url[url_index];
					url_index++;
					coap_index++;
				}
				current_part_length = 0;
			} else {
				// assuming that option is uri_path (optcode 11)
				opt_delta = abs(11 - last_opt_number);
				last_opt_number = 11;
				opt_delta_sum += opt_delta;
				int ext_opt_len = 0;
				// hostname: max 24 bytes!
				int opt_len = current_part_length;
				if(opt_len > 12) {
					ext_opt_len = opt_len - 12;
					opt_len -= 12;
				}
				coap_post[coap_index] = (opt_delta << 4) + opt_len;
				coap_index++;
				if(ext_opt_len > 0) {
					coap_post[coap_index] = ext_opt_len;
					coap_index++;
				}
				for(int m = 0; m < current_part_length; m++) {
					coap_post[coap_index] = url[url_index];
					url_index++;
					coap_index++;
				}
				current_part_length = 0;
			}
			i++;
			url_index++;
		}
	}

	opt_delta = abs(27 - last_opt_number);
	unsigned int ext_opt_delta = 0;
	if(opt_delta > 12) {
		ext_opt_delta = opt_delta - 13;
		opt_delta = 13;
	}
	coap_post[coap_index] = (opt_delta << 4) + 1;
	coap_index++;
	if(ext_opt_delta) {
		coap_post[coap_index] = ext_opt_delta;
		coap_index++;
	}
	// assuming one block with size of 16 bytes
	coap_post[coap_index] = 0;
	coap_index++;
	// end of options mark
	coap_post[coap_index] = 0xff;
	coap_index++;

	while(message[i] != ' ') {
		coap_post[coap_index] = message[i];
		coap_index++;
		i++;
	}
	
	for(int j = 0; j <= coap_index; j++) {
		printf("%d : %c\t", (unsigned int)coap_post[j], coap_post[j]);
	}

	printf("\n");

	return coap_post;
}

char* validate_message_and_extract_value(unsigned char* response, struct MessageData* message_data, bool DEBUG_FLAG)
{
	if(DEBUG_FLAG) {
		printf("[DBG] Received CoAP response:\n");
		for(int i = 0; i < 32; i++) {
			printf("[%d]:%d(%c)\t", i, (unsigned int)response[i], response[i]);
		}
		printf("\n");
	}
	unsigned int length;
	char* value = (char*)malloc(sizeof(char) * PAYLOAD_SIZE);
	unsigned char* message = data_to_coap(response, &length, DEBUG_FLAG);
	if(message[2] == message_data->message_id[0] &&
	   message[3] == message_data->message_id[1] &&
	   message[4] == message_data->token[0] &&
	   message[5] == message_data->token[1])
	{
		for(int i = 7; i < length + 7; i++) {
			value[i - 7] = message[i];
		}

		return value;
	}
	printf("[ERR] Received incorrect message!\n");
	free(value);
	exit(1);
}
