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

#include "functions.h"

int main(int argc, char *argv[])
{
	const char* hostname = "0.0.0.0";
	const char* portname = argv[1] ? argv[1] : "8001";
	const char* uart_portname = argv[2] ? argv[2] : "9001";

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

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

	listen(sckt, 128);
	int accsckt = accept(sckt, (struct sockaddr *) &res->ai_addr, &res->ai_addrlen);
	while(true) {
	printf("Initializing new sequence...\n");
	unsigned char* http_message;
	unsigned char* coap_message;
	unsigned char* coap_message_with_header;
	char* response;

	http_message = listen_for_http(sckt, res, accsckt);
	coap_message = http_to_coap(http_message);
	coap_message_with_header = create_message_with_header(coap_message);
	printf("[DBG]Sending:\n");
	for(int i = 0; i < 25; i++) {
		printf("%d ", coap_message_with_header[i]);
	}
	printf("\n");
	response = send_coap_to_port_and_wait_for_response(coap_message_with_header);
	printf("Response: %s", response);
	for(int i = 0; i < 4; i++) printf("%d", (unsigned int)response[i]);
	int bytes_written = write(accsckt, response, 4);
	printf("Responded with %d byte(s).\n", bytes_written);
	}
}
