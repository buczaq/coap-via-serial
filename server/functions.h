#ifndef _functions_H_
#define _functions_H_

#include <stdbool.h>
#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "constant.h"

unsigned char* create_message_with_header(char* buffer);
char* send_coap_to_port_and_wait_for_response(unsigned char* buffer);
unsigned char* http_to_coap(char* http_message);
unsigned char* listen_for_http(int sckt, struct addrinfo* res, int accsckt);

typedef enum EMessageType
{
	GET = 1,
	POST = 2
} MessageType;

enum EMessageType recognize_http_message_type(char* http_message);
unsigned char* process_http_get(char* message);
unsigned char* process_http_post(char* message);
unsigned int count_actual_buffer_size(unsigned char* buffer);
unsigned int count_whole_message_size(unsigned char* buffer);
uint16_t receive_response(const char* host, const char* port);

#endif // _functions_H_
