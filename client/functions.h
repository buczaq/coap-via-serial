#ifndef _functions_H_
#define _functions_H_
#include <stdbool.h>

bool open_device(int* fd, const char* device);
unsigned char* receive_data(int fd);
unsigned char* data_to_coap(unsigned char* buffer, unsigned int* length);
unsigned char* process_coap(unsigned char* buffer, unsigned int length);
unsigned char* process_get(unsigned char* buffer, unsigned int length);
unsigned char* process_post(unsigned char* buffer, unsigned int length);

#endif // _functions_H_
