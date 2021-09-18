#ifndef _CONFIG_H_
#define _CONFIG_H_

//#include "typedef.h"
/** ************************************************************************* ** 
 ** defines
 ** ************************************************************************* **/
#define MEMORY_ALLOCATION
#define IVNET_FONT
#define DISPLAY_DECODER_CLOCK

//#define BT4000
//
//#ifndef CODENAME_SVR950
//#define F007
//#endif


#define MAX_PLAY_STEP	4
#if defined CODENAME_SVR1640 || defined CODENAME_SVR164X || defined CODENAME_SVR1641
	#define MAX_AT2041_NUM	1
	#define UNIT_CH_NUM	16
	#define RECORDPATH_MAX 	1
	
	#define MAX_CH_NUM	16
	#define MAX_SENSOR_NUM	16
#elif defined CODENAME_SVR950 || defined CODENAME_SVR9X0
//#elif defined CODENAME_SVR950
	#define MAX_AT2041_NUM	1
	#define UNIT_CH_NUM	9
	#define RECORDPATH_MAX	1
	
	#define MAX_CH_NUM	9
	#define MAX_SENSOR_NUM	9
#else
	#define MAX_AT2041_NUM	4
	#define UNIT_CH_NUM	4
	#define RECORDPATH_MAX	4
	
	#define MAX_CH_NUM	16
	#define MAX_SENSOR_NUM	16
#endif

#define UART_MAX	3
#define FS1648_INTERFACE

typedef enum {	/* DUP */
	NTSC = 0,
	PAL
} VIDEO_MODE;

typedef enum {	/* DUP */
	FULL_D1,
	HALF_D1,
	CIF,
	VHALF_D1
} VIDEO_SIZE;

typedef enum {	/* DUP */
	D_PLAY = 0,
	D_STOP,
	D_PAUSE,
	D_SLOW,
	D_REW,
	D_STEP_B,
	D_FF,
	D_STEP_F,
	D_IDLE
} PLAY_MODE;

#endif  /* _CONFIG_H_ */
