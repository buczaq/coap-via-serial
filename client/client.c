#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "functions.h"
#include "constant.h"
#include "../common/MessageData.h"

bool DEBUG_FLAG;

int main(int argc, char *argv[])
{	
	if(strcmp(argv[2], "debug") == 0) {
		DEBUG_FLAG = true;
		printf("[DBG] Debug mode turned ON\n");
	} else {
		DEBUG_FLAG = false;
	}

	// CoAP resources
	struct Resources resources = { "temperature", "humidity", 23, 71 };

	// CoAP message data got from server
	struct MessageData message_data;

	const char* device = argv[1];
	int fd;

	if(!open_device(&fd, device)) {
		return -1;
	}

	struct termios SerialPortSettings; 

	tcgetattr(fd, &SerialPortSettings);

	cfsetispeed(&SerialPortSettings,B38400);
	cfsetospeed(&SerialPortSettings,B38400);

	SerialPortSettings.c_cflag &= ~PARENB;
	SerialPortSettings.c_cflag &= ~CSIZE;
	SerialPortSettings.c_cflag |= CS8;
	SerialPortSettings.c_iflag &= IXANY;

	SerialPortSettings.c_cflag |= CREAD | CLOCAL;

	SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);


	SerialPortSettings.c_oflag &= ~OPOST;

	tcsetattr(fd, TCSANOW, &SerialPortSettings);

	while(true) {
		//initializing new variables every time
		unsigned char* data;
		unsigned char* coap_msg_raw;
		unsigned int length = 0;
		char* coap_msg;
		unsigned char post_payload[PAYLOAD_SIZE];
		for(int i = 0; i < PAYLOAD_SIZE; i++) {
			post_payload[i] = '\0';
		}

		data = receive_data(fd);
		if(DEBUG_FLAG) {
			printf("[INF] Received:\n");
			for(int i = 0; i < 40; i++) {
				printf("%d ", data[i]);
			}
			printf("\n");
		}

		coap_msg_raw = data_to_coap(data, &length, DEBUG_FLAG);
		coap_msg = process_coap(coap_msg_raw, length, post_payload, &message_data);
		if(!post_payload[0]) {
			check_resources_and_send_response(fd, coap_msg, &resources);
		} else {
			set_resources_and_send_response(fd, coap_msg, &resources, post_payload, DEBUG_FLAG);
		}
	}
	close(fd);
}
