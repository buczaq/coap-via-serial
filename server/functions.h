#include <stdbool.h>

bool open_device(int* fd, const char* device);
unsigned char* create_message_with_header(const char* buffer);
bool send_data(int fd);
bool receive_data(int fd);
unsigned char* receive_udp_datagram();
unsigned char* http_to_coap(char* http_message);
typedef enum EMessageType
{
	GET = 1,
	POST = 2
} MessageType;
enum EMessageType recognize_http_message_type(char* http_message);
unsigned char* process_http_get(char* message);
unsigned char* process_http_post(char* message);
