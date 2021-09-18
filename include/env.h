#ifndef _ENV_H_
#define _ENV_H_

#include "typedef.h"
#include "svr_common.h"

#define 	FRAM_SIZE				0x8000 // 32KB, 256Kb
#define 	REC_ADDR 				0x0000
#define 	HWINFO_ADDR 			0x0010
#define 	TIMEZONE_ADDR 		0x0020
#define 	WTIME_ADDR 			0x0030
#define 	BWTIME_ADDR 			0x0040 // bootloader에서 사용함. 
#define 	REBOOT_CHECK 			0x0050
#define 	ENV_ADDR 				0x0400
#define 	LOGFILE_INFO_OFFSET	0x4000
#define 	LOGFILE_VER_OFFSET	0x3990
//#define TEMPERATURE_ADDR 0x6012
#define 	BACKUP		0
#define 	FIXED		1
#define	SENSOR_MAX_NUM	16

#define	MAX_SCHEDULE		50
#define	MAX_PROGRAM		20
#define	MAX_TIMEZONE		50 // 이것을 고치려면 엑셀파일에서 R_sch_index, R_sch 수정해줘야 함!!!
#define	MAX_RELAY_NUM		7
#define	MAX_EVE_TYPE		5
#define 	MAX_LOG_LIST		1001


#define 	LAN			0
#define 	ADSL		1

#define ENV_DATA_VERSION1 0
#define ENV_DATA_VERSION2 0x06050100
#define ENV_DATA_VERSION3 0x06110300

typedef enum {
	LOGFILE_POWER_ON,
	LOGFILE_POWER_OFF,
	LOGFILE_POWER_ON_REC,
	LOGFILE_REC_ON,
	LOGFILE_REC_OFF,
	LOGFILE_SETUP,
	LOGFILE_CLOCK,
	LOGFILE_FORMAT,
	LOGFILE_HDD_ERROR,
	LOGFILE_EXT_HDD_ERROR,
	LOGFILE_PLAYBACK,
	LOGFILE_ADMIN_PASSWORD,
	LOGFILE_USER_PASSWORD,
	LOGFILE_NTP_SUCCESS,
	LOGFILE_NTP_FAIL,
	LOGFILE_BACKUP
} LOGFILE_ID;

#define SAVE_CH_NUM 16

#pragma pack(1)

typedef struct{
	BYTE split_4E_screen[4];
	BYTE split_9B_screen[9];
} USER_SPLIT;

typedef struct{
	BYTE			scr_ch_number;
	BYTE			scr_title[16][12];
	BYTE			scr_brightness[SAVE_CH_NUM];
	BYTE			scr_contrast[SAVE_CH_NUM];
	BYTE			scr_sharpness[SAVE_CH_NUM];
	
	BYTE			scr_seq_ch;
	BYTE			scr_dweltime[24];
	BYTE			scr_auto_order[24];
	BYTE			scr_multimode;
	USER_SPLIT		user_split_screen;
	BYTE			scr_vlossskip;
	
	BYTE			scr_clock;
	BYTE			scr_hddspace;
	BYTE			scr_recstatus;
	BYTE			scr_title_disp;
	BYTE			scr_remoteid;
	BYTE			scr_textmtr;

	BYTE			scr_covert_ch;
	BYTE			live_covert[SAVE_CH_NUM];
	BYTE			play_covert[SAVE_CH_NUM];
	BYTE			nw_covert[SAVE_CH_NUM];

	BYTE			scr_spot_tdisp;
	BYTE			scr_spot_mode;
	BYTE			scr_spot_event;

	BYTE			auto_ch[SAVE_CH_NUM + 1];
	BYTE			auto_multi;
	BYTE			vga_mode;
	BYTE			reserved[223];
	BYTE			covert[SAVE_CH_NUM];

} DVR_CONFIG_SCREEN;



typedef struct{
	BYTE			quality[SAVE_CH_NUM];
	BYTE			frame_rate[SAVE_CH_NUM];
} DVR_CONFIG_MANUAL_REC;

typedef struct{
	BYTE			quality[SAVE_CH_NUM];
	BYTE			frame_rate[SAVE_CH_NUM];
} DVR_CONFIG_EVENT_REC;

typedef struct{
	BYTE 			image_size;
	DVR_CONFIG_MANUAL_REC	manual;
	DVR_CONFIG_EVENT_REC	event;
} DVR_CONFIG_VIDEO_MODE;

typedef struct{
	BYTE			used;
	BYTE			num;
	DVR_CONFIG_VIDEO_MODE	nt_pal_mode[2];
} DVR_CONFIG_PROGRAM;

typedef struct{
	BYTE			day;
	BYTE 			program_num;
	int			start_time;
	int			end_time;
	BYTE			audio_onoff;
} DVR_CONFIG_SCH_EVE;


typedef struct{
	BYTE			rec_mode;
	BYTE			manual_prog_num;
	BYTE			pre_rec;
	BYTE			post_rec;
	BYTE			repeat_rec;
	BYTE			warning_msg;
	BYTE			current_program;
	DVR_CONFIG_SCH_EVE	sch_event[MAX_SCHEDULE];
	DVR_CONFIG_PROGRAM 	program[MAX_PROGRAM];
	BYTE			audio_onoff;
	BYTE			audio_ch;
} DVR_CONFIG_RECORD;


// SAVE_CH_NUM에 +1 해주어야 할 변수들을 다시 정리해야함!!!!!!!!!!!!


typedef struct{
	BYTE			day;
	int			start_time;
	int			end_time;
} DVR_CONFIG_TIMEZONE;


typedef struct{
	BYTE			event_check;
	DVR_CONFIG_TIMEZONE	timezone[MAX_TIMEZONE];
	BYTE			event_msg;
	BYTE			event_buzzer;	
	BYTE    			switch_to_event;
	BYTE			event_scr_time_min;
	BYTE			event_scr_time_max;
	BYTE			text_monitor;
	BYTE			sensor_num;
	BYTE			sensor_nonc[SAVE_CH_NUM+1];

	BYTE			md_ch_onoff[SAVE_CH_NUM+1];
	BYTE			md_area_show[SAVE_CH_NUM+1];
	BYTE			md_sensitivity[SAVE_CH_NUM+1];
	unsigned short	md_zone[SAVE_CH_NUM+1][16];

	BYTE			sensor_pset;
	BYTE			md_pset;

	BYTE			relay_num;
	BYTE			relay[MAX_RELAY_NUM][MAX_EVE_TYPE];

//	short			sensor_ch_preset[SENSOR_MAX_NUM][6];
	short			reserve[149+16*6];
	
	BYTE			name[TITLE_LENGTH+1];
	BYTE			id[TITLE_LENGTH+1];
	BYTE			sync_ch;
	BYTE			device;
	BYTE			header_check;
	BYTE			header1[TITLE_LENGTH+1];
	BYTE			header2[TITLE_LENGTH+1];
	BYTE			delimiter[TITLE_LENGTH+1];
	int				timeout;
	BYTE			lines;

} DVR_CONFIG_EVENT;


typedef struct{
	BYTE		net_type;
	BYTE		xdsl_id[TITLE_LENGTH30+1];
	BYTE		xdsl_pw[TITLE_LENGTH30+1];
	BYTE		dhcp;
	int			port;
	BYTE		swr;
	int			ip_addr[4];
	int			sm_addr[4];
	int			gw_addr[4];
	int			dns_addr[4];
} DVR_CONFIG_NETWORK;

typedef struct{
	BYTE			baud_rate;
	BYTE			data_bit;
	BYTE			parity_bit;
	BYTE			stop_bit;
} DVR_CONFIG_RS232;

typedef enum {
	THROUGH_CONTROL_ON = 0,
	THROUGH_CONTROL_OFF,
} DVR_CONTROL_TYPE;


typedef struct{
	BYTE 			Svr1650_Dvr_id;
	BYTE 			uart_port_num;
	BYTE 			uart_baud[UART_MAX];
 	BYTE 			uart_data_bit[UART_MAX];
 	BYTE 			uart_parity_bit[UART_MAX];
 	BYTE 			uart_stop_bit[UART_MAX];
	BYTE 			PTZ_ch_num;
	BYTE 			PTZ_id[SAVE_CH_NUM+1];
	BYTE 			PTZ_Model[SAVE_CH_NUM+1];
 	DVR_CONTROL_TYPE 	uart_control_mode;
 	BYTE 			serial_test;
} DVR_CONFIG_RS485;


typedef struct{
	DVR_CONFIG_NETWORK 	net;
	DVR_CONFIG_RS232 	rs232;
	DVR_CONFIG_RS485 	rs485;
	BYTE			swr2;
	BYTE			reserved[39];
	//BYTE			reserved[40];
	BYTE			mail_check;
	BYTE			mail_to[TITLE_LENGTH30+1];
	BYTE			mail_id[21];
	BYTE			mail_auth;
	BYTE			smtp_server[TITLE_LENGTH20+1];
	BYTE			smtp_id[TITLE_LENGTH13+1];
	BYTE			smtp_pw[TITLE_LENGTH13+1];

	//Deleted by arnold at 2008/04/08
	//BYTE	swr2_server[TITLE_LENGTH30+1];
	//BYTE	swr2_id[TITLE_LENGTH30+1];
	//BYTE	swr2_pw[TITLE_LENGTH30+1];
} DVR_CONFIG_COMMUNICATION;

typedef struct{
	DVR_CONFIG_NETWORK 	net;
	DVR_CONFIG_RS232 	rs232;
	DVR_CONFIG_RS485 	rs485;
	BYTE			reserved[40];
	BYTE			mail_check;
	BYTE			mail_to[TITLE_LENGTH30+1];
	BYTE			mail_id[21];
	BYTE			mail_auth;
	BYTE			smtp_server[TITLE_LENGTH20+1];
	BYTE			smtp_id[TITLE_LENGTH13+1];
	BYTE			smtp_pw[TITLE_LENGTH13+1];
	
	BYTE	bDDNSUSe;
	BYTE 	bReserved[3];
	BYTE	bDDNS_Server[32];
	BYTE	bDDNS_ID[32];
	BYTE	bDDNS_PW[32];
} DVR_CONFIG_COMM_SUPPORT_WEBGATECMS;
	

typedef struct {
	BYTE			month;
	BYTE			ordinal;
	BYTE			weekday;
	BYTE			hour;
} DVR_CONFIG_DST;


typedef struct {
	BYTE 				recording;
	BYTE					daylight;
	DVR_CONFIG_DST 		dst_start;
	DVR_CONFIG_DST 		dst_end;
} WEBGATE_SUPPORT_VALUE;


typedef struct{
	BYTE			language;
	BYTE			key_buz_onoff;
	BYTE			video_std;
	BYTE			firm_down;
	BYTE			nmode;
	BYTE			set_default;
	BYTE			user_passwd[9];
	BYTE			admin_passwd[9];
	BYTE			menu_setup_check;
	BYTE			power_on_check;
	BYTE			power_off_check;
	BYTE			rec_off_check;
	BYTE			play_check;
	BYTE			remote_setup;
	BYTE			buzzer_check_fail;
	BYTE			mon_format;
	BYTE			daylight;
	DVR_CONFIG_DST 		dst_start;
	DVR_CONFIG_DST 		dst_end;


	DVR_CONFIG_RS485 	rs485;
	BYTE			r_hdd_mode;

	BYTE			remote_relay;
	BYTE			date_mode;
	BYTE 			mon_type;
	BYTE			daylight_save;
	BYTE			dst_start_year;
	BYTE			dst_end_year;
	BYTE			ntp_check;
	BYTE			reserved[17];
	BYTE			audio_input_type;
	BYTE			remote_id;

	BYTE			dvr_name[20];

} DVR_CONFIG_SYSTEM;

/*
typedef struct {
	BYTE 			version;
	BYTE 			supported_language;
	BYTE 			reserved[26];
} DVR_CONFIG_HEADER;
*/

typedef struct {
	BYTE 			time;
	BYTE 			reserved[26];
} DVR_CONFIG_SEARCH;


typedef struct {
	BYTE 			copy_channel;
	BYTE 			reserved[26];
} DVR_CONFIG_COPY;


// ==========================================================================================================
typedef struct{
	unsigned int version;

	DVR_CONFIG_SCREEN disinfo;
	DVR_CONFIG_RECORD recinfo;
	DVR_CONFIG_EVENT eventinfo;
	DVR_CONFIG_COMMUNICATION cominfo;
	DVR_CONFIG_SEARCH searchinfo;
	DVR_CONFIG_COPY	copyinfo;
	//DVR_CONFIG_HEADER dcHeader;
	DVR_CONFIG_SYSTEM systeminfo;
	unsigned short		channel_title[16][8];
	BYTE 		NW_framerate[32];
	BYTE	ntp_server[TITLE_LENGTH20+1];
	BYTE		user_passwd[11][9];
	unsigned short	user_auth[10];
	unsigned short	pass_check;
	BYTE 		reserved[2016-748];

	// 추가 kimyw 071121
	BYTE	swr2_server[TITLE_LENGTH30+1];
	BYTE	swr2_id[TITLE_LENGTH30+1];
	BYTE	swr2_pw[TITLE_LENGTH30+1];

	// 추가 kimyw 071114 
	BYTE			sensor_ch_preset[16][16];
	BYTE			md_ch_preset[16][16];


	BYTE		NW_auto_check;
	BYTE 		signature[8];
} DVR_ENV;

typedef struct{
	unsigned int version;

	DVR_CONFIG_SCREEN disinfo;
	DVR_CONFIG_RECORD recinfo;
	DVR_CONFIG_EVENT eventinfo;
	DVR_CONFIG_COMMUNICATION cominfo;
	DVR_CONFIG_SEARCH searchinfo;
	DVR_CONFIG_COPY	copyinfo;
	//DVR_CONFIG_HEADER dcHeader;
	DVR_CONFIG_SYSTEM systeminfo;
	BYTE reserved[256];
	BYTE signature[8];
} DVR_ENV_OLD;

//modify khgon
typedef struct {
	int		Front;
	int		Rear;
	int 		Head;

	DWORD	dwReserved;
	BYTE	crc_check;
} LOGFILE_INFO;

#pragma pack(0)

typedef struct {
	BYTE 		kind;
	time_t 		datetime;
} LOGFILE_TYPE_OLD;

typedef struct {
	BYTE 		kind;
	time_t 	datetime;
// kimyw For TB_EYE (07_0404)
	BYTE		menu_auth;
	int		menu_status;
} LOGFILE_TYPE;


extern DVR_ENV gEnv,gEnvTemp;

void set_default( DVR_ENV *env);
void nvram_wirte_byte(WORD addr,BYTE data);
BYTE make_crc(BYTE *buf,int len);
int check_crc( BYTE *buf, int len);

//modify khgon
void write_log_header(LOGFILE_INFO *info);
void read_log_header(LOGFILE_INFO *info) ;
void write_logfile_list(int front,LOGFILE_TYPE *log_list);
void read_logfile_list(int rear,LOGFILE_TYPE *log_list);
void add_logfile(BYTE kind,time_t datetime, BYTE menu_auth, int menu_status);
void erase_logfile_list();
void logfile_list_init(BYTE print_list);


//modified by choco71
void Network_Setup(void);
int network_save(void);
int net_save_pppoe(BYTE * pUser, BYTE * pPassword);
int net_save_dhcp(void);

#endif  /*KEY_H_*/
