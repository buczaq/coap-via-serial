#ifndef _functions_H_
#define _functions_H_

#include <stdbool.h>

#include "constant.h"

bool open_device(int* fd, const char* device);
unsigned char* create_message_with_header(const char* buffer);
bool send_coap_to_port(unsigned char* buffer);
unsigned char* http_to_coap(char* http_message);
unsigned char* listen_for_http(const char* host, const char* port);

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

#endif // _functions_H_
