#ifndef _COMMON_H_
#define _COMMON_H_

// #include "PlxTypes.h"
#include "svr_config.h"
#include "typedef.h"

/** ************************************************************************* ** 
 ** defines
 ** ************************************************************************* **/
//#define HDD_TEST_MODE
//#define FRAMERATE_CHANGE_TEST
//#define COPY_TEST_MODE
//#define PRINT_DEBUG 
//#define SIMPLE_NW_TEST
//#define DIRECT_PLAYBACK
//#define USE_WATCHDOG

//#define MUX_REC_OSD
//#define ENV_PATH_NAME           "/user_2/env.bin"
#define TITLE_LENGTH 		10
#define TITLE_LENGTH8 		8
#define	TITLE_LENGTH13		13
#define TITLE_LENGTH20 		20
#define TITLE_LENGTH30		30
//#define MAXCLIENT 	 	10     	// how many pending connections queue will hold
//#define MAX_CLIENT 	 	3
#define EVENT_COUNTDOWN_FACTOR  5

#ifdef PRINT_DEBUG
#define printd(format, args...)  printf(format, ## args)
#else
#define printd(format, args...)
#endif

//#define m_DEBUG(format, args...)  printf(format, ## args) 
#define m_DEBUG(format, args...)  
//#define m_MSG(format, args...)  printf(format, ## args)
#define m_MSG(format, args...)
#define m_ERROR(format, args...)  printf(format, ## args)

#define DecToBcd(value) ((((value) / 10) << 4) + ((value) % 10))
#define BcdToDec(value) ((((value) >> 4) * 10) + ((value) & 0x0F))

/*
#define ENCODER_PRIORITY		99
#define DECODER_PRIORITY		99
#define VIDOE_PRIORITY 			98
#define AUDIO_READ_PRIORITY 		97
#define AUDIO_WRITE_PRIORITY 	96
#define EVENT_PRIORITY 			95
#define KEY_PRIORITY				94
#define TIMER_PRIORITY			93
#define SERVER_PRIORITY			0
#define CLIENT_PRIORITY			0
#define MAIN_PRIORITY			0
*/
#define ENCODER_PRIORITY		99
#define DECODER_PRIORITY		99
#define VIDOE_PRIORITY 			99
#define AUDIO_READ_PRIORITY 	99
#define AUDIO_WRITE_PRIORITY 	99
#define EVENT_PRIORITY 			99
#define KEY_PRIORITY			99
#define TIMER_PRIORITY			99
#define SERVER_PRIORITY			1
#define CLIENT_PRIORITY			1
#define MAIN_PRIORITY			0

/** ************************************************************************* ** 
 ** typedefs
 ** ************************************************************************* **/
typedef enum {
	DT_SYSTEM = 1,
	DT_VIDEO,
	DT_AUDIO
} DATA_TYPE;

typedef enum {
	I_PICTURE = 1,
	P_PICTURE,
	B_PICTURE
} PICTURE_TYPE;

typedef enum {
	PTHREAD_MAIN = 0,
	PTHREAD_KEY,
	PTHREAD_RECBUFFER,
	PTHREAD_TIMER,


	PTHREAD_EVENT,	
	PTHREAD_ENC,
	PTHREAD_DEC,
	PTHREAD_RECORD,
	PTHREAD_PLAYBACK,
	PTHREAD_SEARCH,
	PTHREAD_COPY,
	PTHREAD_UART,
	PTHREAD_CONFIG,
#if 1		//modified by byulkonghj	
	PTHREAD_NETWORKSEARCH1,
	PTHREAD_NETWORKSEARCH2,
	PTHREAD_NETWORKSEARCH3,
	PTHREAD_NETWORKSEARCH4,			
#endif
	PTHREAD_NTP,
	PTHREAD_DDNS,
	PTHREAD_MAX
} PTHREAD_ID;


#define LED_REC			0x00010000
#define LED_ALARM		0x00020000
#define LED_COPY		0x00080000

/* ==================TYPEDEF for EVENT=============== */

typedef enum {
	EVENT_SENSOR = 0,
	EVENT_MOTION,
	EVENT_VLOSS,
	EVENT_TEXT,
	MAX_EVENT_TYPE,
	EVENT_NO
} EVENT_TYPE;

struct tagEventInfo {
	time_t datetime;
	unsigned int group_offset;
	BYTE group_index;
	BYTE  type;
	BYTE  channel;
	BYTE  record_exist;
//	BYTE  reserved;
};

struct tagElistInformation {
	//int LVG_index;
	void* LVG_ptr;
	int event_count;
	int ordinal_number;
	int exist;
	struct tagEventInfo tEventInfo;
};

typedef enum {
	_INIT_MODE_,
	_SETUP_MODE_,
	_MOTIONSETUP_MODE_,
	_LIVE_MODE_,
	_LIVEPIP_MODE_,
	_LIVEZOOM_MODE_,
	_STANDBY_MODE_,
	_PLAYBACK_MODE_,
	_PLAYBACKPIP_MODE_,
	_EVENT_MODE_,
	_PTZ_MODE_,
	_STATUS_MODE_,
	_DIRECTPB_MODE_,
	_HOT_MODE_
} SCREEN_MODE;

typedef struct {              //khgon
	unsigned char channel;
	unsigned char split; 
	unsigned char pip_mode;
	unsigned char zoom_mode;
	unsigned char pip_channel;
} SCREEN_INFO;

typedef enum {
	P_BASE,
	P_SETUP,
	P_PTZ,
	P_REC_OFF,
	P_PLAY,
	P_POWER_OFF,
	P_POWER_ON,
	P_NET_MIC	
} PASSWORD_MODE;

union tagVersion {
	unsigned int dwVersion;
	BYTE bVersion[4];
};

/* ================INDEX OFFSET & SIZE================== */
#define LVG_BLOCKS 2*1024*1024
#define LVG_DATA_OFFSET	8*1024

#define LVG_TIMEINDEX_OFFSET 2*1024
#define LVG_TIMEINDEX_BLOCKS 768 // 12 hours

#define LVG_EVENTINDEX_OFFSET 5*1024
#define LVG_EVENTINDEX_BLOCKS 128

#define HASHING_INFO_SIZE 8

#define MAX_HOURS 43200L // 12시간 ?????

/* ================GLOBAL VARIABLE================== */

typedef struct {
	//int num_LVs;
	long long int free_blocks;
	long long int total_blocks;
	int	hdd_free_space;
	int	Data_Rate[MAX_CH_NUM + 4];
	int	Data_Accumulated[MAX_CH_NUM + 4];
	int	Data_Total;
	pthread_mutex_t hdd_encode_lock;
	pthread_mutex_t hdd_network_lock;
	struct {
		time_t offset_sec;
		BYTE direction;
		BYTE offset_hh;
		BYTE offset_mm;
		BYTE reserved1;
		char Timezone1;
		char Timezone2;
		BYTE reserved2;
		BYTE reserved3;
	} timezone;
	struct {
		time_t copying_time;
		time_t copy_start_time;
		time_t copy_end_time;
		//int LVG_index;
		void* LVG_ptr;
		time_t lvg_time;
		unsigned int Copy_offset;
		unsigned short ch_mask;
		BYTE pTimeIndex[LVG_TIMEINDEX_BLOCKS<<9];
//		BYTE Copy_buffer[1*1024*1024];
#if 1			// added by byulkonghj copy 2nd
		BYTE Copy_buffer[((2*1024 + 1)<<9)+10];
#endif		
		//int watch_point;
	} Copy;
#if 1 //modified by byulkonghj
	struct {
		time_t sending_time;
		time_t send_end_time;
		//int LVG_index;
		void* LVG_ptr;
#if 1		// byulkonghj for SDK 1st								
		BYTE 	ch_name[16][16];		
#endif		// byulkonghj for SDK 1st												
		BYTE* pTimeIndex;
		time_t lvg_time;
		unsigned int Send_offset;
//		BYTE Send_buffer[1*1024*1024];
		BYTE *Send_buffer;
	} Network[4];
#endif	
	struct {
		//int search_onoff;
		//time_t search_time;
		time_t search_hashing;
		//int LVG_index;
		void* LVG_ptr;
		BYTE pTimeIndex[LVG_TIMEINDEX_BLOCKS<<9];
		unsigned short ch_mask;
	} Search;
	struct {
		int last_modified;
		int rec_onoff;
		int index_dirty;
		unsigned int Prev_offset;
		unsigned int Rec_offset;
		unsigned short last_blocks;
		void* LVG_ptr;
		BYTE repeat;
		BYTE forced_flush;
		BYTE pData[1*1024*1024];
		BYTE pTimeIndex[LVG_TIMEINDEX_BLOCKS<<9];
		BYTE pEventIndex[LVG_EVENTINDEX_BLOCKS<<9];
		int verbose;
		BYTE rec_mark[MAX_CH_NUM];
		BYTE rate_control_mode;
		BYTE program;
		BYTE record_ready;
		BYTE table_in[16];
		BYTE table_out[16];
	} Record;
	struct {
		time_t timezone_offset;
		time_t playback_time;
		time_t decoded_time[16];
		struct {
			time_t buffer_time[32]; // ?
			time_t buffer_tz_offset[32];
			BYTE channel[32];
			BYTE isdst[32];
			int index;
			int prev_depth;
		} buffer_queue[MAX_AT2041_NUM];
		//PLAY_MODE play_mode;
		//unsigned short play_status;
		//unsigned short play_step;
		BYTE resolution[MAX_AT2041_NUM];
		BYTE deinterlace_onoff[MAX_AT2041_NUM];
		BYTE Engine_onoff[MAX_AT2041_NUM];
		BYTE isdst;
		BYTE channel;
		BYTE split;
		BYTE zoom_mode;
		BYTE pip_mode;
		BYTE pip_channel;
		BYTE pause_onoff;;
		BYTE direction;
		BYTE no_more_data;
		unsigned short ch_mask;
		unsigned short CH_exist;
		int CH_count;
		int Buffer_index;
		void* pGroup_addr;
		int group_index;
		struct {
			//int LVG_index;
			void* LVG_ptr;
			unsigned int PB_offset;
		} target;
		struct {
			//int LVG_index;
			void* LVG_ptr;
			unsigned int PB_offset;
			time_t lvg_time;
		} forward;
		struct {
			//int LVG_index;
			void* LVG_ptr;
			unsigned int PB_offset;
			time_t lvg_time;
		} backward;
		BYTE *PB_buffer[4];
		BYTE PB_string[16][16];
		unsigned short msg_clear;
		BYTE screen_clear;
		signed char clear_timeout[16];
		unsigned short start;
		signed char text_timeout;
		//int temp_count[MAX_CH_NUM];
		//int temp_total_count;
		int verbose;
	} Playback;
	struct {
		struct {
			int reset_count[MAX_CH_NUM];
			unsigned short new_status;
			//unsigned short old_status;
			unsigned short detected;
			//struct tagEventInfo *buf_ptr[MAX_CH_NUM];
		} TYPE_info[MAX_EVENT_TYPE];
		struct {
			int record_count;
			struct tagEventInfo *buf_ptr;
			//BYTE type;
		} CH_info[MAX_CH_NUM];
		unsigned short rec_bitmask;
		int tolerance_count;
		//int record_count[MAX_CH_NUM];
		unsigned short event_msg_string[MAX_EVENT_TYPE][8];
		int event_notice_max_time;
		int event_notice_min_time;
		int event_notice_countdown;
		int event_message_countdown[MAX_CH_NUM];
		int Pre_REC_time;
		int Post_REC_time;
		void* buffer_addr_start;
		void* buffer_addr_end;
		void* buffer_addr_current;
		int index_dirty;
		unsigned int search_block;
		BYTE check_onoff;
		BYTE pEventIndex[LVG_EVENTINDEX_BLOCKS<<9];
		//int LVG_index;
		void* LVG_ptr;
		unsigned short sensor_mask;
		BYTE md_prohibit;
		BYTE event_rec_change[16];
		BYTE event_msg[16];
		BYTE event_rec_apply;
	} Event;
	struct {
		//SCREEN_INFO current;
		//SCREEN_INFO last;
		SCREEN_INFO live_request_info;
		//SCREEN_INFO event_request_info;
		SCREEN_INFO current_info;
		SCREEN_INFO backup_info;
		SCREEN_MODE request_mode;
		SCREEN_MODE current_mode;
		BYTE refresh;
	} ScreenInfo[2];
	struct {
		BYTE clock;
		BYTE rec_mark;
		BYTE hdd_msg;
		BYTE live_status;
		BYTE channel_title;
		BYTE status_msg;
		BYTE warning_msg;
		BYTE remote_id;
		unsigned short msg_clear;
	} SCREEN_REFRESH;
	struct {
		//struct {
		//	BYTE channel;
		//	BYTE split;
		//	BYTE dwell_time;
		//} seq_nr[24];
		int seq_now;
		//int seq_total;
		int dwell_countdown;
		int second_ok;
		void (* sequencing_handler)(int);
		void (* sequencing_backup)(int);
	} SEQUENCING[2];
	struct {
	        BYTE      datetime[6];
	        unsigned short     key_msg;
	        BYTE      test_msg;
	        BYTE      uart_test;
	        BYTE	title_status[MAX_CH_NUM];
	} temp_var;
	unsigned short	channel_bitmask[24];
	unsigned short	adhoc_channel;
	BYTE	Record_schedule[7][24*60];
	BYTE	Event_schedule[7][24*60];
	BYTE	resolution[MAX_AT2041_NUM];
	BYTE	frame_rate[MAX_CH_NUM];
	BYTE	real_frame_rate[MAX_CH_NUM];
	BYTE	mpeg_quality[MAX_CH_NUM];
	BYTE	date_string[16];
	BYTE	clock_string[16];
	BYTE	HDD_IO_ERROR;
	BYTE	ERROR_HDD_ID;
	BYTE	sataHDD_fail;
	signed char	sataHDD_err_indicator;
	BYTE	HDD_free_space_warning;
	char		HDDMsg[16];
	BYTE	zoom_msg[16];
	int	preRecordingTime;
	int	test_value1;
	time_t	current_time;
	unsigned int	ntp_count;
	int	test_value2;
	BYTE	isdst;
	BYTE	bStop;
	BYTE	bChange;
	//int minutes_change;
	//time_t hashing_index;
	//int	the_second;
	BYTE	event_buzzer;
	//BYTE	spot_filter;
	BYTE	logfile_erase;
	BYTE	log_init;
	BYTE	NtscPal;
	//SETUP_PARAM setup;
	unsigned int	version;
	BYTE	signature[8];
	BYTE	trcode_exist;
	BYTE	security_failure;
	BYTE	security_count;
	int	log_erase;
	char	WarningMsg[0x40];
	int	WarningTimeout;
	int	old_minute;
	BYTE	copy_onoff;
	BYTE	fw_update_onoff;
	BYTE	copy_room_exist;
	BYTE	board_version; // Extention board=1, old board=0 (main.c 에서 314 Line에 값을 정의해놓았습니다.)
} NEW_DVR_VARIABLE;

extern NEW_DVR_VARIABLE gNewVar;

#endif  /* _COMMON_H_ */




