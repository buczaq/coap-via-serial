#ifndef _functions_H_
#define _functions_H_
#include <stdbool.h>
#include <stdint.h>

struct Resources {
	char* temperature;
	char* humidity;
	int16_t temperature_value;
	int16_t humidity_value;
};

bool open_device(int* fd, const char* device);
unsigned char* receive_data(int fd);
unsigned char* data_to_coap(unsigned char* buffer, unsigned int* length);
unsigned char* process_coap(unsigned char* buffer, unsigned int length);
unsigned char* process_get(unsigned char* buffer, unsigned int length);
unsigned char* process_post(unsigned char* buffer, unsigned int length);

int16_t get_temperature_value(struct Resources* resources);
int16_t get_humidity_value(struct Resources* resources);

void check_resources_and_send_response(int fd, unsigned char* message, struct Resources* resources);

#endif // _functions_H_
