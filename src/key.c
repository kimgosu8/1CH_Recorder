#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <jpeglib.h>
#include <linux/videodev.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h> 
#include <time.h> 



char KEY_INPUT = 'A';




void* Task_pthread_key_process(void* arg)
{

	while(1)
	{
//		printf("pthread_key_process\n");

		usleep(1000000);

		KEY_INPUT = getchar();

		printf("KEY_INPUT ==>%c\n", KEY_INPUT);
	}


}








