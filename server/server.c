#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <stdlib.h>

#include "functions.h"

bool DEBUG_FLAG;

int main(int argc, char *argv[])
{
	if(strcmp(argv[5], "debug") == 0) {
		DEBUG_FLAG = true;
		printf("[DBG] Debug mode turned ON\n");
	} else {
		DEBUG_FLAG = false;
	}

	char* hostname = (char*)calloc(BUFFER_SIZE, sizeof(char));
	char* portname = (char*)calloc(5, sizeof(char));
	char* uart_portname = (char*)calloc(5, sizeof(char));
	ConnectionType connection_type;
	if(strcmp(argv[1], "raw") == 0) {
		connection_type = RAW;
	}

	if(strcmp(argv[1], "ser2net") == 0) {
		connection_type = SER2NET;
	}

	if(connection_type == SER2NET) {
		portname = argv[2] ? argv[2] : "8001";
		uart_portname = argv[3] ? argv[3] : "9001";
		hostname = argv[4] ? argv[4] : "0.0.0.0";
	}

	else if(connection_type == RAW) {
		portname = argv[2] ? argv[2] : "8001";
		hostname = argv[3] ? argv[3] : "0.0.0.0";
	}

	struct Device* devices = malloc(16 * sizeof(struct Device));

	read_devices_list(devices);

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	struct addrinfo* res = 0;
	int err=getaddrinfo(hostname, portname, &hints, &res);
	if (err!=0) {
		printf("[ERR] Failed to resolve local socket address (err = %d)",err);
	}
	int sckt = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	if (sckt == -1) {
    	printf("[ERR] Failed to create address (err = %d)",err);
	}
	if (bind(sckt,res->ai_addr,res->ai_addrlen) == -1) {
		printf("[ERR] Failed to bind (err = %d)",err);
	}

	listen(sckt, 128);
	int accsckt = accept(sckt, (struct sockaddr *) &res->ai_addr, &res->ai_addrlen);
	while(true) {
		printf("[INF] Initializing new sequence...\n");
		unsigned char* http_message;
		unsigned char* coap_message;
		unsigned char* coap_message_with_header;
		char destination[32] = { "\0" };
		char* response;

		http_message = listen_for_http(sckt, res, accsckt);
		coap_message = http_to_coap(http_message, devices, destination);
		coap_message_with_header = create_message_with_header(coap_message);
		if(DEBUG_FLAG) {
			printf("[DBG] Sending:\n");
			for(int i = 0; i < 40; i++) {
				printf("%d ", coap_message_with_header[i]);
			}
		}
		printf("\n");
		switch(connection_type)
		{
			case SER2NET:
				response = send_coap_to_ser2net_port_and_wait_for_response(coap_message_with_header);
				break;
			case RAW: ;
				char* device = look_for_device(devices, destination);
				response = send_coap_to_raw_device_and_wait_for_response(coap_message_with_header, device);
				break;
			default:
				break;			
		}
		printf("[INF] Response: %s", response);
		for(int i = 0; i < 4; i++) printf("%d", (unsigned int)response[i]);
		int bytes_written = write(accsckt, response, 4);
		if(DEBUG_FLAG) {
			printf("[INF] Responded with %d byte(s).\n", bytes_written);
		}
	}
	//close(fd);
}
