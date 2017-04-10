#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

#define FILE_PATH "big.txt"
#define DEVICE_PATH "/dev/Four_MB_Device"
#define BUF_SIZE 7 * 1024 * 1024

//struct FILE *filep = NULL;
//struct FILE *device_filep = NULL;
 
int main(char **argv){
	char * buf = (char *) malloc ( BUF_SIZE * sizeof(char));

	int filep        = open( FILE_PATH, O_RDONLY );
	int device_filep = open( DEVICE_PATH, O_RDWR );

	int bytes_read = 0, bytes_write = 0;
	char ch;

	if( filep == -1 || device_filep == -1 ){
		printf("failed to open %s or %s", FILE_PATH, DEVICE_PATH);
		exit(1);
	} 

	bytes_read = read(filep, buf, BUF_SIZE);

	char str[] = "abc";
	bytes_write = write(device_filep, buf, bytes_read);

	printf("\n Total number of Bytes of input file : %d", bytes_read);
	if(bytes_write){
		printf("\nsuccessfully written %d bytes", bytes_write);
	}
   
        char buff[4*1024*1024*sizeof(char)];
	bytes_read = read(device_filep, &buff, bytes_write);

	if(bytes_read){
		printf("\nsuccessfully read : %d", bytes_read);
	}
	return 0;
}
