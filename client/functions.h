#ifndef _client_functions_H_
#define _client_functions_H_
#include <stdbool.h>
#include <stdint.h>

#include "../common/common.h"

struct Resources {
	char* temperature;
	char* humidity;
	int16_t temperature_value;
	int16_t humidity_value;
};

unsigned char* receive_data(int fd);
char* process_coap(unsigned char* buffer, unsigned int length, char* post_payload, struct MessageData* message_data);
char* process_get(unsigned char* buffer, unsigned int length, struct MessageData* message_data);
char* process_post(unsigned char* buffer, unsigned int length, char* post_payload, struct MessageData* message_data);
void save_message_parameters(struct MessageData* message_data, unsigned char* message_id, unsigned char* token);

int16_t get_temperature_value(struct Resources* resources);
int16_t get_humidity_value(struct Resources* resources);

int16_t set_temperature_value(struct Resources* resources, int16_t value);
int16_t set_humidity_value(struct Resources* resources, int16_t value);

void check_resources_and_send_response(int fd, unsigned char* message, struct Resources* resources, struct MessageData* message_data);
void set_resources_and_send_response(int fd, unsigned char* message, struct Resources* resources, char* post_payload, bool DEBUG_FLAG, struct MessageData* message_data);

unsigned char* build_coap_response_with_header(struct MessageData* message_data, char* response);

#endif // _client_functions_H_
