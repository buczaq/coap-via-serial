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
	//while(true) {
		// initializing new variables every time
		printf("Initializing new sequence...\n");
		const char* hostname = "0.0.0.0";
		const char* portname = "8001";
		const char* uart_portname = "9001";
		unsigned char* http_message;
		unsigned char* coap_message;
		unsigned char* coap_message_with_header;

		http_message = listen_for_http(hostname, portname);
		coap_message = http_to_coap(http_message);
		coap_message_with_header = create_message_with_header(coap_message);
		printf("[DBG]Sending:\n");
		for(int i = 0; i < 25; i++) {
			printf("%d ", coap_message_with_header[i]);
		}
		printf("\n");
		send_coap_to_port(coap_message_with_header);
		//printf("Waiting for response...\n");
		//uint16_t response = receive_response(hostname, uart_portname);
		//printf("\nResponse: %d\n", response);
	//}
}
