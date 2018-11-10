#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>

#include "functions.h"

int main(int argc, char *argv[])
{	
	const char* device = argv[1];
	int fd;

	if(!open_device(&fd, device)) {
		return -1;
	}

	struct termios SerialPortSettings; 

	tcgetattr(fd, &SerialPortSettings);

	cfsetispeed(&SerialPortSettings,B115200);
	cfsetospeed(&SerialPortSettings,B115200);

	SerialPortSettings.c_cflag &= ~PARENB;
	SerialPortSettings.c_cflag &= ~CSTOPB;
	SerialPortSettings.c_cflag &= ~CSIZE;
	SerialPortSettings.c_cflag |=CS8;

	SerialPortSettings.c_cflag &= ~CRTSCTS;
	SerialPortSettings.c_cflag |= CREAD | CLOCAL;

	SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
	SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);


	SerialPortSettings.c_oflag &= ~OPOST;

	unsigned char* data;
	data = receive_data(fd);
	//printf("Received:\n");
	//for(int i = 0; i < 25; i++) {
	//	printf("%d ", data[i]);
	//}
	//printf("\n");
	data_to_coap(data);
}
