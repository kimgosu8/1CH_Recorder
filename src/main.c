/********************************************************************************
   INCLUDES
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include <linux/videodev.h>
#include "v4l2uvc.h"
#include "svr_common.h"

#include <semaphore.h>

#include "MpegEnc.h"
#include "env.h"
#include "ide.h"

/********************************************************************************
   DEFINES
********************************************************************************/


/********************************************************************************
   GLOBALS
********************************************************************************/

static const char version[] = VERSION;
int run = 1;
int read_run=0;
int write_run=0;


struct vdIn *videoIn;
int quality = 100;
int g_width = 640;
int g_height = 480;
//int g_width = 960;
//int g_height = 720;
//int g_format = V4L2_PIX_FMT_MJPEG;
int g_format = V4L2_PIX_FMT_YUYV;

pthread_mutex_t 	mutex_vdbuf = PTHREAD_MUTEX_INITIALIZER; // 상수를 해주지 않고 init 함수만 실행시켜줘도 된다!!! 

DVR_ENV gEnv,gEnvTemp;
NEW_DVR_VARIABLE gNewVar;
sem_t sema_1;


/********************************************************************************
   LOCALS
********************************************************************************/
extern pthread_t	g_pthread[PTHREAD_MAX];


/********************************************************************************
   Function Prototype Declare
********************************************************************************/

void* Task_pthread_encoding(void* arg);
void* Task_pthread_key_process(void* arg);
void* pthread_write_hdd(void* arg);
void print_time(time_t target_time);


void init_value();
void my_handler();


void HDD_MAKE_COPY_ROOM(); // make copy room
void ide_identify();
char user_create_pthread(pthread_t* pt,  void* thread_func, int priority, int stacksize, int schedpolicy, void* arg);

void* Task_preBuffer_timeChange(void *);
void* Task_pthread_enc(void *);
void* Task_Timer(void *);

void control_framerate(int fd, int width, int height);
void prepare_timer(void);
void	g_Env_init();
void	g_NewVar_init();


/********************************************************************************
   Function define
********************************************************************************/

int main (int argc, char *argv[])
{
//	char *videodevice[]={"./1ch_dvr","/dev/video0"};
	char *videodevice = "/dev/video0";
	int grabmethod = 1;
	int ii=0;


	// TEST CODE
	system("rm -rf *.avi");


	videoIn = (struct vdIn *)calloc(1, sizeof(struct vdIn));

	init_value();

	if (init_videoIn(videoIn, (char *) videodevice, g_width, g_height, g_format, grabmethod) < 0)
		exit (1);

	if ((pthread_mutex_init(&mutex_vdbuf, NULL)) != 0) 						printf("\t!!! ERROR : pthread_mutex_init !!!\n");
	if ((pthread_mutex_init(&gNewVar.hdd_encode_lock, NULL)) != 0) 			printf("\t!!! ERROR : pthread_mutex_init !!!\n");
	for(ii = 1; ii < MAX_HDD_NUM; ii++)
		if ((pthread_mutex_init(&ide_devs_info[ii].ide_access_wait, NULL)) != 0) 	printf("\t!!! ERROR : pthread_mutex_init !!!\n");


	read_run = 1;
	write_run = 1;


	control_framerate(videoIn->fd, g_width, g_height);


	preBuffer = (BYTE*)malloc(PREBUFFER_SIZE);



	prepare_timer();
//	pthread_create_config(); // log file 처리 시작



	HDD_MAKE_COPY_ROOM();
	ide_identify();


	user_create_pthread(&g_pthread[PTHREAD_KEY], 		Task_pthread_key_process, 	20, 1024*1024, SCHED_FIFO, NULL);
	user_create_pthread(&g_pthread[PTHREAD_RECBUFFER],	Task_preBuffer_timeChange, 	20, 1024*1024, SCHED_FIFO, NULL);
//	pthread_create_playback();
	user_create_pthread(&g_pthread[PTHREAD_ENC], 		Task_pthread_enc, 			20, 1024*1024, SCHED_FIFO, NULL);
//	pthread_create_dec();
	user_create_pthread(&g_pthread[PTHREAD_TIMER], 		Task_Timer, 				20, 1024*1024, SCHED_FIFO, NULL);
//	text_thread_create();




/* 이 것은 다른 Thread로 이전함!!
	// Semaphore Initial (function 부분이 동시에 2번이상 작동 되지 않도록 한다.)
	sem_init( &sema_1, 0, 1 );
	utimerCreate(&my_handler, 20000); //a 20 ms
*/



//	directfb_app(2,videodevice);





	while(1)
	{
		usleep(60000000);
	}
	

	close_v4l2 (videoIn);
	free(videoIn);

	return 0;
}


void init_value()
{
	if (g_width == 0)
		g_width = 640;
	if (g_height == 0)
		g_height = 480;

	if (g_width < 144 || g_width > 720) {
		fprintf(stderr, "Width must be between 144 and 720 pixels.\n");
		exit(1);
	}
	if (g_height < 96 || g_height > 576) {
		fprintf(stderr, "Height must be between 96 and 576 pixels.\n");
		exit(1);
	}
	if ((g_width & 0x0f) != 0 || (g_height & 0x0f) != 0) {
		fprintf(stderr, "Height and g_width must be divisible by 16.\n");
		exit(1);
	}


	g_Env_init();
	g_NewVar_init();
	
}








struct timeval gap_tv1, gap_tv2;

void start_point()
{
	gettimeofday( &gap_tv1, NULL);
}

void end_point()
{
	gettimeofday( &gap_tv2, NULL);

	printf(" ==>Elasped Time : %ld us\n", (gap_tv2.tv_sec - gap_tv1.tv_sec) * 1000000 + (gap_tv2.tv_usec - gap_tv1.tv_usec));
}


void	g_Env_init()
{
	int loop=0;
	
	for(loop=0; loop<16; loop++)
		memcpy(gEnv.channel_title[loop], "TITLEABC", 8);
		
}

void	g_NewVar_init()
{
	gNewVar.Record.rec_onoff = 1; // NORMAL REC 해라. 
}


