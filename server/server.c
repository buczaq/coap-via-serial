#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <time.h>

#include "functions.h"
#include "../common/common.h"

bool DEBUG_FLAG;

int main(int argc, char *argv[])
{
	srand(time(NULL));

	struct MessageData message_data;

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
		if(strcmp(argv[5], "debug") == 0) {
			DEBUG_FLAG = true;
			printf("[DBG] Debug mode turned ON\n");
		} else {
			DEBUG_FLAG = false;
		}
	}

	else if(connection_type == RAW) {
		portname = argv[2] ? argv[2] : "8001";
		hostname = argv[3] ? argv[3] : "0.0.0.0";
		if(strcmp(argv[4], "debug") == 0) {
			DEBUG_FLAG = true;
			printf("[DBG] Debug mode turned ON\n");
		} else {
			DEBUG_FLAG = false;
		}
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
		char* http_message = (char*)malloc(sizeof(char) * BUFFER_SIZE);
		unsigned char* coap_message = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
		unsigned char* coap_message_with_header = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
		char destination[32] = { "\0" };
		unsigned char* response = (unsigned char*)malloc(sizeof(unsigned char) * BUFFER_SIZE);
		char* value = (unsigned char*)malloc(sizeof(unsigned char) * PAYLOAD_SIZE);

		http_message = listen_for_http(sckt, res, accsckt);
		coap_message = http_to_coap(http_message, devices, destination, &message_data);
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
				printf("[WRN] ser2net implementation is incomplete and might be UNSTABLE!");
				response = send_coap_to_ser2net_port_and_wait_for_response(coap_message_with_header, hostname, uart_portname);
				break;
			case RAW: ;
				char* device = look_for_device(devices, destination);
				response = send_coap_to_raw_device_and_wait_for_response(coap_message_with_header, device);
				break;
			default:
				break;			
		}
		value = validate_message_and_extract_value(response, &message_data, DEBUG_FLAG);
		printf("[INF] Response: %s", value);
		for(int i = 0; i < PAYLOAD_SIZE; i++) printf("%d", (unsigned int)value[i]);
		int bytes_written = write(accsckt, value, PAYLOAD_SIZE);
		if(DEBUG_FLAG) {
			printf("[INF] Responded with %d byte(s).\n", bytes_written);
		}

		free(http_message);
		free(coap_message);
		free(coap_message_with_header);
		free(response);
		free(value);
	}
}
