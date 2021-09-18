

/** ************************************************************************* ** 
 ** includes
 ** ************************************************************************* **/
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "typedef.h"
#include "svr_common.h"
// #include "PlxApi.h"
#include "server.h"
// #include "simo_ringbuffer.h"
// #include "timer.h"




/** ************************************************************************* ** 
 ** locals
 ** ************************************************************************* **/
pthread_t				g_pthread[PTHREAD_MAX];
// pthread_mutex_t		g_pthread_mutex[PTHREAD_MAX];
// pthread_cond_t		g_pthread_cond[PTHREAD_MAX];





char user_create_pthread(pthread_t *thread_t, void* thread_func, int priority, int stacksize, int schedpolicy, void* arg)
{
	pthread_attr_t thread_attr;
//	struct sched_param scheduling_value;


	pthread_attr_init(&thread_attr);

/*
	pthread_attr_setschedpolicy(&thread_attr, schedpolicy);
	scheduling_value.sched_priority = priority;
	pthread_attr_setschedparam(&thread_attr, &scheduling_value);
	pthread_attr_setstacksize(&thread_attr, stacksize);

*/
	pthread_create(thread_t, &thread_attr, thread_func, arg);

/*
	pthread_detach(thread_t);
	pthread_attr_destroy(&thread_attr);
*/

	return 1;
}







