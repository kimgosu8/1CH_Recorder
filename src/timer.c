/********************************************************************************
   INCLUDES
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/ipc.h> // MSG QUEUE
#include <sys/msg.h>

#include "typedef.h"
#include "svr_common.h"
#include "env.h"
#include "timer.h"
//#include "hw_fs1648_api.h"
//#include "server.h"
//#include "backup.h"

//#include <time.h>
#include <errno.h>


#include <stdio.h>
#include <sys/time.h>
#include <signal.h>



/********************************************************************************
   DEFINES
********************************************************************************/







/********************************************************************************
   GLOBALS
********************************************************************************/
int 		fd_vsync;
extern pthread_cond_t      RecBuffer_cond;
extern pthread_mutex_t     RecBuffer_mutex;



extern sem_t sema_1;
time_t timer_cur_time, timer_old_time;
int timer_count_20MS;

static void (*utimer_handler)() = NULL;
static long int utimer_interval;
// static sigset_t usig_new, usig_old; //BLOCK을 풀면 잘 될 수 도 있을 듯. 

int g_temp_count; // 자유롭게 나두면 거의 2000000 개 정도 나옴. 



/********************************************************************************
   LOCALS
********************************************************************************/




/********************************************************************************
   Function Prototype Declare
********************************************************************************/

void do_function_20MS();
void do_function_40MS();
void do_function_100MS();
void do_function_200MS();
void do_function_500MS();
void do_function_1S(time_t cur_time);


void my_handler();
void utimerHandler();
void utimerCreate( void (*handler)(), int interval );
void utimerStart();


/********************************************************************************
   Function define
********************************************************************************/


void* Task_Timer(void * arg)
{
//	int the_minute;
	time_t the_time, old_time;
//	struct tm tm_data;
	int old_count=0;

	old_time = 0;


	// Semaphore Initial (function 부분이 동시에 2번이상 작동 되지 않도록 한다.)
	sem_init( &sema_1, 0, 1 );
	utimerCreate(&my_handler, 20000); //a 20 ms


	gNewVar.old_minute = 60;




	while(1)
	{
//		usleep(1000); // 이 녀석 없으면 제대로 안돌아감.


//		g_temp_count++;

		
		time(&the_time);
		if ( the_time != old_time )
			do_function_1S(the_time);
		old_time = the_time;


		if( timer_count_20MS != old_count ) // 20ms 이내에 두번 들어 올 수 있기 때문에 같은 count를 check
		{
			old_count = timer_count_20MS;


			do_function_20MS();
			
			if( (timer_count_20MS % 2) == 0 && timer_count_20MS != 50 ) // 40ms 마다
				do_function_40MS();

			if( (timer_count_20MS % 5) == 0 && timer_count_20MS != 50 ) // 100ms 마다
				do_function_100MS();

			if( (timer_count_20MS % 10) == 0 && timer_count_20MS != 50 ) // 200ms 마다
				do_function_200MS();

			if( (timer_count_20MS % 25) == 0 && timer_count_20MS != 50 ) // 500ms 마다
				do_function_500MS();
		}



	}

	return 0;

}



void do_function_20MS()
{
//	printf("timer_count_20MS ==>%d\n", timer_count_20MS);
}

void do_function_40MS()
{
//	printf("timer_count_20MS ==>%d\n", timer_count_20MS);
}

void do_function_100MS()
{
//	printf("timer_count_20MS ==>%d\n", timer_count_20MS);
}

void do_function_200MS()
{
//	printf("timer_count_20MS ==>%d\n", timer_count_20MS);
}

void do_function_500MS()
{
//	printf("timer_count_20MS ==>%d\n", timer_count_20MS);
}

void do_function_1S(time_t cur_time)
{
	gNewVar.current_time = cur_time;
//	printf("g_temp_count ==>%d\n", g_temp_count);
	g_temp_count = 0;


//	clock_debug_message(gNewVar.current_time);
	pthread_mutex_lock(&RecBuffer_mutex);
	pthread_cond_signal(&RecBuffer_cond);
	pthread_mutex_unlock(&RecBuffer_mutex);
}





void prepare_timer(void)
{
//	clock_read(); // rtc에서 시간읽어와서 settimeofday로 리눅스에 시간 쓰기
	time(&gNewVar.current_time);
}


void clock_debug_message(time_t target)
{
	struct tm tm_data;

	localtime_r( &target, &tm_data);
	printf("%d/%02d/%02d  %02d:%02d:%02d \n", tm_data.tm_year + 1900
								, tm_data.tm_mon + 1
								, tm_data.tm_mday
								, tm_data.tm_hour
								, tm_data.tm_min
								, tm_data.tm_sec);
}






void utimerCreate( void (*handler)(), int interval )
{
	struct sigaction act;


	// 시간 단위가 백만이 넘는지를 검사 한다.
	if( interval >= 1000000 ) 
	{
		printf("ERROR : The usec is wrong!!!\n");
		return;
	}

	utimer_interval = interval;
	utimer_handler = handler;


	act.sa_handler = utimerHandler;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGALRM, &act, NULL);

	utimerStart();
}


void utimerHandler()
{

	
	if( utimer_handler == NULL ) 
		return;

	utimer_handler();
	utimerStart();
}

void utimerStart()
{
	struct itimerval itv;
//	struct timeval *tv;

	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = utimer_interval; // 몇초(uSec)후 작동 될지를 정해 준다.


	setitimer( ITIMER_REAL, &itv, NULL ); // itv 시간(sec & usec) 후, SIGALRM 을 발생 시킨다.
}

void my_handler()
{
	char *cts;
	time_t tc;
	struct timeval tv;


	// Semaphore 값을 1감소 시킨다.
	sem_wait( &sema_1 );

	// 현재 시간을 보여주기 위한 코드
	gettimeofday(&tv, NULL);
	tc = (time_t)tv.tv_sec;
	cts = (char *)ctime(&tc);
//	printf("\n\n * Timer Start : %s \n", cts);


	timer_count_20MS++;
	timer_cur_time = tc;

	if( timer_cur_time != timer_old_time )
	{
//		printf("%s[%d]\n\n", cts, timer_count_20MS);
		timer_count_20MS = 0;
	}
	timer_old_time = timer_cur_time;


	// Semaphore 값을 1증가 시킨다.
	sem_post( &sema_1 );


	return;
}


