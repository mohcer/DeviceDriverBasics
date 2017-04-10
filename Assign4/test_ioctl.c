#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define SCULL_IO_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IO_MAGIC, 0)
#define SCULL_WRITE _IOW(SCULL_IO_MAGIC, 1, int)
#define SCULL_READ _IOR(SCULL_IO_MAGIC, 2, int)
int lcd;

void test(){
	int k, i, sum;

	char s[3];

	char buf[100];
	memset(s, '2', sizeof(s));

	printf("\n test begin\n");
	
	k = write(lcd, s, sizeof(s));

	printf("written = %d\n",k);

	k = ioctl(lcd, SCULL_HELLO);

	printf("\n result = %d\n",k);

	k = ioctl(lcd, SCULL_WRITE, "1st hello world message !!!!");
	printf("\n result = %d\n",k);

	k = ioctl(lcd, SCULL_WRITE, "2nd hello world message !!!!");
	printf("\n result = %d\n", k);

	k = ioctl(lcd, SCULL_READ, buf);
	printf("\nresult = %d", k);

	printf("\nRead : %s", buf);
}
int main(int argc, char **argv){
	
	lcd = open("/dev/Four_MB_Device", O_RDWR);

	if(lcd == -1){
		perror("unable to open lcd");
		exit(EXIT_FAILURE);
	}

	test();
	close(lcd);

	return 0;
}
