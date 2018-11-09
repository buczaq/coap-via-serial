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

unsigned char* create_message_with_header(const char* buffer)
{
	unsigned char* message_with_header = malloc(sizeof(unsigned char) * 256);
	unsigned int coap_size = strlen(buffer);
	message_with_header[0] = 0xa1;

	unsigned int ext_len_index = 0;
	if(coap_size > 255) {
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

	for(int i = 0; i < coap_size + 6; i++) {
		printf("%d ", message_with_header[i]);
	}
	return message_with_header;
}

bool send_data(int fd)
{
	unsigned char write_buffer[32] = {"\0"};
	scanf("%s", write_buffer);
	int bytes_written = 0;

	unsigned char* message_with_header = create_message_with_header(write_buffer);

	bytes_written = write(fd, message_with_header, sizeof(write_buffer) + 6);

	printf("\n +----------------------------------+\n");
	return (bytes_written > 0);
}

bool receive_data(int fd)
{
	tcflush(fd, TCIFLUSH);

	unsigned char read_buffer[32];
	int bytes_read = 0;
	int i = 0;

	bytes_read = read(fd, &read_buffer, 32);
	unsigned int length = 0;
	int source[2] = { 0 };
	int destination[2] = { 0 };
	if(read_buffer[0] == 0xa1) {
		length = (unsigned int)read_buffer[1];
		source[0] = (unsigned int)read_buffer[2];
		source[1] = (unsigned int)read_buffer[3];
		destination[0] = (unsigned int)read_buffer[4];
		destination[1] = (unsigned int)read_buffer[5];
	}

	for(i = 0; i < length; i++) {
		printf("%c", read_buffer[i + 6]);
	}
	printf("\nSource: %d.%d\nDestination: %d.%d", source[0], source[1], destination[0], destination[1]);
	
	printf("\n +----------------------------------+\n");
}

unsigned char* receive_udp_datagram()
{
	const char* hostname = "0.0.0.0";
	const char* portname = "8000";
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_PASSIVE|AI_ADDRCONFIG;
	struct addrinfo* res = 0;
	int err=getaddrinfo(hostname, portname, &hints, &res);
	if (err!=0) {
		printf("failed to resolve local socket address (err = %d)",err);
	}
	int sckt = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if (sckt == -1) {
    	printf("failed to create address (err = %d)",err);
	}
	if (bind(sckt,res->ai_addr,res->ai_addrlen) == -1) {
    	printf("failed to bind (err = %d)",err);
	}

	unsigned char buffer[549] = { '\0' };
	struct sockaddr_storage src_addr;
	socklen_t src_addr_len=sizeof(src_addr);
	ssize_t count=recvfrom(sckt, buffer,sizeof(buffer), 0, (struct sockaddr*)&src_addr,&src_addr_len);
	if (count == -1) {
		printf("%s",strerror(errno));
	} else if (count==sizeof(buffer)) {
		printf("datagram too large for buffer: truncated");
	} else {
		for(int i = 0; i < sizeof(buffer); i++) printf("%c", buffer[i]);
	}

	freeaddrinfo(res);

	const char* desthostname = "0.0.0.0";
	const char* destportname = "9001";
	struct addrinfo desthints;
	memset(&desthints, 0, sizeof(desthints));
	desthints.ai_family = AF_UNSPEC;
	desthints.ai_socktype = SOCK_STREAM;
	desthints.ai_protocol = 0;
	struct addrinfo* destres = 0;
	int desterr=getaddrinfo(desthostname, destportname, &desthints, &destres);
	if (desterr!=0) {
		printf("failed to resolve local socket address (err = %d)",desterr);
	}
	int destsckt = socket(destres->ai_family,destres->ai_socktype,destres->ai_protocol);
	if (destsckt == -1) {
    	printf("failed to create address (err = %d)",desterr);
	}
	connect(destsckt, destres->ai_addr, destres->ai_addrlen);
	char* send_buffer = http_to_coap(buffer);
	write(destsckt, send_buffer, 255);

	return '5';
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
	return true;
}

unsigned char* http_to_coap(char* http_message)
{
	MessageType message_type = recognize_http_message_type(http_message);
	char* message_to_send;
	switch (message_type)
	{
		case GET:
			message_to_send = process_http_get(http_message);
			break;
		case POST:
			message_to_send = process_http_post(http_message);
			break;
		default:
			break;
	}
	return message_to_send;
}

unsigned char* process_http_get(char* message)
{
	unsigned char* coap_get = malloc(sizeof(unsigned char) * 256);
	// hardcoded values
	//assuming no token
	coap_get[0] = 64; // CoAP version 1, confirmable, no token
	coap_get[1] = 1; // GET
	coap_get[2] = 123; // message_id p1
	coap_get[3] = 012; // message_id p2

	int i = 4;
	int coap_index = 4;
	char url[128] = { '\0' };
	bool host_is_specified = false;
	int opt_delta = 0;
	int current_part_length = 0;
	int whole_url_length = 0;
	int url_index = 0;
	bool time_to_break = false;
	// workaround! FIXME
	while(!time_to_break) {
		printf("-%c-", message[i]);
		url[i - 4] = message[i];
		//printf("%c", message[i])
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
				// hostname: max 24 bytes!
				if(current_part_length > 12) {
					ext_opt_len = current_part_length - 12;
					current_part_length -= 12;
				}
				coap_get[coap_index] = 0x30 + current_part_length;
				coap_index++;
				if(ext_opt_len > 0) {
					coap_get[coap_index] = ext_opt_len;
					coap_index++;
				}
				host_is_specified = true;
				for(int m = 0; m < current_part_length; m++) {
					printf("index: %d, char: %c\n", url_index, url[url_index]);
					coap_get[coap_index] = url[url_index];
					url_index++;
					coap_index++;
				}
				current_part_length = 0;
			} else {
				// assuming that option is uri_path (optcode 11)
				opt_delta = abs(11 - opt_delta);
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
					printf("index: %d, char: %c\n", url_index, url[url_index]);
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
	//debug
	for(int i = 0; i < 255; i++)
		printf("%d ", coap_get[i]);
	return coap_get;
}

// TODO
unsigned char* process_http_post(char* message)
{
	return 'x';
}

//2 params(uart) 4 bytes per value --rs-> 


//ser2net