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


	if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) {
		printf("\nERROR ! in Setting attributes");
		return -1;
	}

	unsigned char* coap_msg = receive_udp_datagram();

	printf("Press 1 to send, 2 to receive:");
	for(;;) {
		char choice;
		scanf("%c", &choice);

		switch(choice)
		{
			case '1':
				send_data(fd);
				break;
			case '2':
				receive_data(fd);
				break;
			default:
				continue;
		}
		fflush(stdin);
		fflush(stdout);
	}
}
