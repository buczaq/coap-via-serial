#ifndef _functions_H_
#define _functions_H_

#include <stdbool.h>
#include <stdint.h>
#include <netdb.h>

#include "constant.h"
#include "../common/MessageData.h"

typedef enum EConnectionType
{
	SER2NET = 1,
	RAW = 2
} ConnectionType;

struct Device {
	char alias[16];
	int16_t address;
	char location[32];
};

typedef enum EMessageType
{
	GET = 1,
	POST = 2
} MessageType;

bool open_device(int* fd, const char* device);
void read_devices_list(struct Device* devices);
char* look_for_device(struct Device* devices, char* destination);

char* listen_for_http(int sckt, struct addrinfo* res, int accsckt);
enum EMessageType recognize_http_message_type(char* http_message);
unsigned char* http_to_coap(char* http_message, struct Device* devices, char* destination, struct MessageData* message_data);

unsigned char* create_message_with_header(unsigned char* buffer);
unsigned char* send_coap_to_ser2net_port_and_wait_for_response(unsigned char* buffer, char* hostname, char* portname);
unsigned char* send_coap_to_raw_device_and_wait_for_response(unsigned char* buffer, char* destination);
uint16_t receive_response(const char* host, const char* port);

unsigned char* process_http_get(char* message, struct Device* devices, char* destination, struct MessageData* message_data);
unsigned char* process_http_post(char* message, struct Device* devices, char* destination, struct MessageData* message_data);

unsigned int count_actual_buffer_size(unsigned char* buffer);
unsigned int count_whole_message_size(unsigned char* buffer);

#endif // _functions_H_
