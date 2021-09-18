#ifndef _TIMER_H_
#define _TIMER_H_

#define AUTO_ONOFF  0
#define AUTO_OPER	1
#define AUTO_INIT	2
#define AUTO_IDENTIFY 3


extern sem_t	timer_33MS;
extern sem_t	timer_100MS;

void clock_debug_message(time_t);
void prepare_timer(void);

void *pthread_ddns(void *args);
int send_message_to_ddns(void);


#endif
