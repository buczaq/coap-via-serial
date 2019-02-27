#ifndef _client_functions_H_
#define _client_functions_H_
#include <stdbool.h>
#include <stdint.h>

struct Resources {
	char* temperature;
	char* humidity;
	int16_t temperature_value;
	int16_t humidity_value;
};

struct MessageData {
	char device_name[24];
	unsigned char message_id[2];
	unsigned char token[2];
};

bool open_device(int* fd, const char* device);
unsigned char* receive_data(int fd);
unsigned char* data_to_coap(unsigned char* buffer, unsigned int* length, bool DEBUG_FLAG);
char* process_coap(unsigned char* buffer, unsigned int length, char* post_payload);
char* process_get(unsigned char* buffer, unsigned int length);
char* process_post(unsigned char* buffer, unsigned int length, char* post_payload);

int16_t get_temperature_value(struct Resources* resources);
int16_t get_humidity_value(struct Resources* resources);

int16_t set_temperature_value(struct Resources* resources, int16_t value);
int16_t set_humidity_value(struct Resources* resources, int16_t value);

void check_resources_and_send_response(int fd, unsigned char* message, struct Resources* resources);
void set_resources_and_send_response(int fd, unsigned char* message, struct Resources* resources, char* post_payload, bool DEBUG_FLAG);

#endif // _client_functions_H_
