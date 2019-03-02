#ifndef _common_H_
#define _common_H_

#define BUFFER_SIZE 256
#define PAYLOAD_SIZE 4

struct MessageData {
	unsigned char message_id[2];
	unsigned char token[2];
};

unsigned char* create_message_with_header(unsigned char* buffer);

unsigned int count_actual_buffer_size(unsigned char* buffer);
unsigned int count_whole_message_size(unsigned char* buffer);

#endif // _common_H_
