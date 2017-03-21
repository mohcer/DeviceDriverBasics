#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

static char *recieve;

int main(int argc, char **argv){
	int ret, fd;

	char charToWrite[3];

	printf("Starting Our One Byte DEvice Driver");

	//open the device file
	fd = open("/dev/one", O_RDWR);

	if( fd < 0 ){
		perror("Failed to open One Byte Device");
		return errno;
	}

	ret = read(fd, recieve, sizeof(char));

	if(ret < 0){
		perror("Failed to read from One Byte Device");
		return errno;
	}
	
	printf("Read : %s", recieve);

	return 0;
}
