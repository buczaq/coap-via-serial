#ifndef _functions_H_
#define _functions_H_
#include <stdbool.h>

bool open_device(int* fd, const char* device);
unsigned char* receive_data(int fd);
unsigned char* data_to_coap(unsigned char* buffer);

#endif // _functions_H_
