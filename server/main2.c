#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

void main(int argc, char *argv[])
{
	int fd;

	const char* device = argv[1];
	printf("%s", device);
	fd = open(device,O_RDWR | O_NOCTTY);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter */
			 					/* O_RDWR - Read/Write access to serial port */
								/* O_NOCTTY - No terminal will control the process */
								/* Open in blocking mode,read will wait*/



	if(fd == -1)						/* Error Checking */
	printf("\nError! in Opening device");
	else
		printf("\ndevice Opened Successfully ");

	
		/*---------- Setting the Attributes of the serial port using termios structure --------- */

		struct termios SerialPortSettings;	/* Create the structure*/

		tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

		/* Setting the Baud rate */
		cfsetispeed(&SerialPortSettings,B115200); /* Set ReadSpeed as 9600 */
		cfsetospeed(&SerialPortSettings,B115200); /* Set Write Speed as 9600 */

		/* 8N1 Mode */
		SerialPortSettings.c_cflag &= ~PARENB; /* Disables the Parity Enable bit(PARENB),So No Parity */
		SerialPortSettings.c_cflag &= ~CSTOPB; /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
		SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size */
		SerialPortSettings.c_cflag |=CS8;/* Set the data bits = 8 */

		SerialPortSettings.c_cflag &= ~CRTSCTS; /* No Hardware flow Control */
		SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines */ 


		SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);/* Disable XON/XOFF flow control both i/p and o/p */
		SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);/* Non Cannonical mode*/

		SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

		/* Setting Time outs */
		SerialPortSettings.c_cc[VMIN] = 1; /* Read at least 10 characters */
		SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly */


		if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
	printf("\nERROR ! in Setting attributes");

	/*------------------------------- Read data from serial port -----------------------------*/

		tcflush(fd, TCIFLUSH); /* Discards old data in the rx buffer*/

		char read_buffer[32]; /* Buffer to store the data received*/
		intbytes_read = 0;/* Number of bytes read by the read() system call */
	int i = 0;

		bytes_read = read(fd,&read_buffer,32); /* Read the data */

		printf("\n\nBytes Rxed -%d", bytes_read); /* Print the number of bytes read */
	printf("\n\n");

		for(i=0;i<bytes_read;i++)	 /*printing only the received characters*/
	printf("%c",read_buffer[i]);
	
	printf("\n +----------------------------------+\n\n\n");

		close(fd); /* Close the serial port */

}
