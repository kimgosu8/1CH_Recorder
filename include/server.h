/* set tabstop=4 */
/********************************************************************************
 *                                                                              *
 * Copyright(C) 2005  Samsung Techwin                                           *
 *                                                                              *
 * ALL RIGHT RESERVED                                                           *
 *                                                                              *
 * This software is the property of SamsungTechwin and is furnished under       *
 * license by SamsungTechwin. This software may be used only in accordance      *	
 * with the terms of said license. This copyright notice may not be             *
 * removed, modified or obliterated without the prior written permission        *
 * of SamsungTechwin.                                                           *
 *                                                                              *
 * This software may not be copyed, transmitted, provided to or otherwise       *
 * made available to any other person, company, corporation	or other entity     *
 * except as specified in the terms of said license.                            *
 *                                                                              *
 * No right, title, ownership or other interest in the software is hereby       *
 * granted or transferred.                                                      *
 *                                                                              *
 * The information contained herein is subject to change without notice and     *
 * should not be construed as a commitment by Samsung Techwin                   *
 *                                                                              *
 ********************************************************************************
 
 
  MODULE NAME:  SERVER.H
  
  REVISION HISTORY:
  
  Date       Ver Name                              Description
  ---------- --- --------------------------------- -------------------------------

  ................................................................................
  
  DESCRIPTION:
  
  This Module contains definition for SERVER 
  
 ...............................................................................
*/    

#include "typedef.h"
#include "svr_common.h"
 
#ifndef _SERVER_H
#define _SERVER_H


#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <semaphore.h>


/** ************************************************************************* ** 
 ** includes
 ** ************************************************************************* **/
 
/** ************************************************************************* ** 
 ** defines
 ** ************************************************************************* **/
#define NEW_SVR_950


#if defined CODENAME_SVR950 || defined CODENAME_SVR9X0 ||defined CODENAME_SVR1640 || defined CODENAME_SVR160X
#define RING_THREAD_CREATION 
#endif

#ifdef FIX_VOC_0706



/*typedef enum {
	PRIORITY_FIRST 		= 40,		//Lowest priority
	CMD_PRIORITY			= 53,
	DEQUEUE_PRIORITY		= 53,
	OLD_LOW				= 55,
	SEARCH_PRIORITY		=70,
	STREAM_PRIORITY 		= 100,
	CONTROL_PRIORITY 	= 100,
	OLD_NORMAL			= 100,
	OLD_HIGH			= 110,
	CLEANER_PRIORITY		= 110,
	OLD_HIGHEST			= 120
} GLOBALPRIORITY;*/

typedef enum {
	PRIORITY_FIRST 			= 40,		//Lowest priority
	CMD_PRIORITY			= 53,
	DEQUEUE_PRIORITY		= 53,
	OLD_LOW				= 55,
	SEARCH_PRIORITY		= 60,
	STREAM_PRIORITY 		= 65,
	CONTROL_PRIORITY 		= 65,
	OLD_NORMAL				= 65,
	OLD_HIGH				= 70,
	CLEANER_PRIORITY		= 70,
	OLD_HIGHEST			= 80
} GLOBALPRIORITY;



#endif
 
// packet payload
//#define NW_RESET
#define COMMUNICATION_INFO		0xf0
#define FW_FILE_BIOS				0xf1
#define FW_FILE_OS					0xf2
#define FW_FILE_USER2				0xf3
#define	SYSTEM_INFO				0xf4
#define	REMOTE_SETUP_INFO		0xf5		// 1023
#define	REMOTE_SETUP_ENABLE	0xf6		// 1025
#define	REMOTE_SETUP_DISABLE	0xf7		// 1025
#define NW_FRAMTERATE			0xf8
#define PKT_NW_FRAMTERATE		0xf9	// 1008
#define ACK_NW_FRAMTERATE		0xfa	//1008

// added at 2007-08-03

#define PKT_NTP_INFO				0xf9	
#define NTP_RESULT				0xfa	
#define PKT_WEBGATE_SUPPORT		0xfe


// added at 2007-08-07
#define PKT_USER_INFO				0xfb
#define ACK_USER_INFO				0xfc

//added at 2008-03-03
#define PKT_INITIAL_EVENT_INFO		0xfd


#define ACK_FW_FILE_TRANSFER		0xe0
#define NACK_FW_FILE_TRANSFER		0xe1
#define ACK_FW_FILE_ERASE			0xe2
#define NACK_FW_FILE_ERASE			0xe3
#define ACK_FW_FILE_UPDATE			0xe4
#define NACK_FW_FILE_UPDATE		0xe5
#define NBD_INFO					0xe6
#define ACK_PKT_NEXT_IMAGE		0xe7
#define	MACADDR_WRITE			0xe8
#define ACK_MACADDR_WRITE		0xe9
#define NACK_MACADDR_WRITE		0xea

#if 1		// byulkonghj for SDK 1st	
#define ACK_PKT_NEXT_IMAGE_BACKUP	0xeb
#define ACK_PKT_NEXT_PKT_ERROR_BACKUP 0xec
#define ACK_PKT_NEXT_PKT_ERROR 0xed
#endif

#define PTZ_COMMAND				0x61
#define	REQ_RELAY_CTRL			0x63
#define	PKT_RELAY_INFO			0x64	// 1008
#define	REQ_AUDIO_CTRL			0x6b
#define	ACK_AUDIO_CTRL			0x6c
#define	DAT_AUDIO_STREAM		0x6d

#define PKT_EVENT_INFO				0x7f
#define REQ_BACKUP_CTRL			0x81
#define ACK_BACKUP_CTRL			0x82
#define REQ_BACKUP_PAUSE			0x83
#define ACK_BACKUP_PAUSE			0x84
#define REQ_BACKUP_STOP			0x85
#define ACK_BACKUP_STOP			0x86
#define REQ_BACKUP_INIT			0x87
#define ACK_BACKUP_INIT			0x88
#if 1		// byulkonghj for SDK 1st	
#define		REQ_TIMETABLE_DAY	0x89
#define		ACK_TIMETABLE_DAY	0x8a
#define		REQ_TIMETABLE_HOUR	0x8b
#define		ACK_TIMETABLE_HOUR	0x8c
#endif		// byulkonghj for SDK 1st

#define REQ_SEARCH_CTRL			0x91
#define ACK_SEARCH_CTRL			0x92
#define REQ_SEARCH_PAUSE			0x93
#define ACK_SEARCH_PAUSE			0x94
#define REQ_SEARCH_STOP			0x95
#define ACK_SEARCH_STOP			0x96
#define	REQ_SEARCH_TRICK		0x97
#define	ACK_SEARCH_TRICK		0x98

#define REQ_BACKUP_STREAM		0x21
#define ACK_BACKUP_STREAM		0x22
#define REQ_SEARCH_STREAM		0x23
#define ACK_SEARCH_STREAM		0x24

#define	REQ_ENV_INFO				0xA1
#define	ACK_ENV_INFO				0xA2
#define	REQ_ENV_CHANGE			0xA3
#define	ACK_ENV_CHANGE			0xA4
#define REQ_ENV_ACTION			0xA5
#define ACK_ENV_ACTION			0xA6

#define NETWORK_HEADER			"SDVR"

#define NACK						0
#define ACK						1

#define MAX_CLIENT_NUM			4
#define	MAX_SLICE_SIZE			1400
#define	USER_ID_SIZE				2
#define FILE_BUF_SIZE				5600
#if 1		// byulkonghj for SDK 1st
#define MAX_CLIENT_NUM_4SB		2
#endif

// CONTROL define
#define CTRL_VERSION		(0x00 << 6)
#define CTRL_DP_PACKET		(0x01 << 5)
#define CTRL_CP_PACKET		(0x00 << 5)
#define CTRL_MULTIPLE_END	(0x01 << 4)
#define CTRL_ACK			(0x01 << 3)
#define CTRL_EXTEND_HEADER	(0x01 << 0)

#define EX_PAYLOAD			(0x00 << 7)
#define MULTIPLE_PACKET		(0x01 << 7)
#define MULTIPLE_PKT_FILE	(0x01 << 15)
// FRAMEHEADER define
#define IMAGE_PLAY			(0x01 << 4)

// EVENTINFO define
#define EVENT_TT			(0x01 << 7)
#define EVENT_VL			(0x01 << 6)
#define EVENT_MD			(0x01 << 5)
#define EVENT_SD			(0x01 << 4)

#define	VIDEO_LOSS_EVENT			2
#define	MOTION_EVENT				1
#define	SENSOR_EVENT				0

#define	SCREEN_ENV			0x0001
#define	RECORD_ENV			0x0002
#define	EVENT_ENV			0x0003
#define	COMMUNICATION_ENV	0x0004
#define	SYSTEM_ENV			0x0005
#define	RECORD_PROGRAM_ENV	0x0006
#define	NETWORK_FRAMERATE_ENV	0x0007

#define FF_1X           	1
#define FF_2X			2
#define FF_4X           	4
#define FF_8X           	8
#define FF_MAX          	16

#define PLAY_FW         	0
#define PLAY_RW         	1
	 
#define NW_SEARCH			0
#define NW_BACKUP			1

#define PLAY_NOERROR			0
#define PLAY_ERROR			1
#define	PLAY_STOP_BYCOPYING	2
#define	PLAY_STOP_BYFORMAT	3
#define	PLAY_STOP_BYUPDATE	4
#if 1		// byulkonghj for search time
#define	PLAY_STOP_NODATA	5
#endif
#if 1		//inserted by hiyoou for GMT modification 071112
#define 	GMT_ERROR		6
#endif

#define AUDHAL_FRAG_CNT_PLAY	3
#define AUDHAL_FRAG_SIZE_PLAY	2048
#define AUDHAL_MAX_DEV_NAME		64
#define SND_DEV_NAME			"/dev/dsp"
#define FRAME_SIZE_PCM			256   //Arbitary



#define	MAX_NET_BUF				60 * 1024


#if defined CODENAME_SVR950 || defined CODENAME_SVR9X0
// #ifdef CODENAME_SVR950
	#define 	SERVER_NUM		10   // Video 9 + Command 1 = Total 10
#else
	#define 	SERVER_NUM		17   // Video 16 + Command 1 = Total 17
#endif

#define 	VAR_FRAME_RATE_FUNC

#define	MAX_CLIENT_NUM	4

#define 	CONTROL_THREAD	0x00
#define 	VIDEO_CH01		0x01
#define 	VIDEO_CH02		0x02
#define 	VIDEO_CH03		0x03
#define 	VIDEO_CH04		0x04

#define 	VIDEO_CH05		0x05
#define 	VIDEO_CH06		0x06
#define 	VIDEO_CH07		0x07
#define 	VIDEO_CH08		0x08
#define 	VIDEO_CH09		0x09
#define 	VIDEO_CH10		0x0a
#define 	VIDEO_CH11		0x0b
#define 	VIDEO_CH12		0x0c
#define 	VIDEO_CH13		0x0d
#define 	VIDEO_CH14		0x0e
#define 	VIDEO_CH15		0x0f
#define 	VIDEO_CH16		0x10




#define	DEFAULT_PORT 		6000	// for BtoB 



#define 	MAX_SLICE_SIZE		1400

#if defined CODENAME_SVR950 || defined CODENAME_SVR9X0
// #ifdef CODENAME_SVR950
	#define 	SINGLE_MODE_MAX_CHANNEL		9
	#define 	QUAD_MODE_MAX_CHANNEL		4
//	#define	MAX_CONNECTION	90
#else
	#define 	SINGLE_MODE_MAX_CHANNEL		16
	#define 	QUAD_MODE_MAX_CHANNEL		4
//	#define	MAX_CONNECTION	160
#endif
        	
#define 	CH_ON				1
#define 	CH_OFF				0
#define	LISTENQ				100//MAX Listen Queue Size
        	
#define 	PKT_LOGIN			0x71
#define 	PKT_LOGIN_ACK		0x72
#define 	PKT_INFO_REQ			0x73
#define 	PKT_ALIVE_CMD		0x7D
#define	PKT_STOP			0x75
#define	PKT_EVENT_INFO		0x7f
        	
#define 	PKT_NEXT_IMAGE		0x11
#define	ACK_LIVE_STREAM		0x12
#define	PKT_LIVE_I_ONLY		0x13

#define 	PKT_CHANNEL_CHANGE	0x15

#define	PKT_LIVE_PAUSE		0x1b
#define	ACK_LIVE_PAUSE		0x1c        	
        	
#define 	BUF_FREE			0
#define 	BUF_LOCK			1
#define 	BUF_SEND			2

#define	DETECT_NONE			0
#define	DETECT_ERROR		1

#define	PROTO_TCP			0
#define	PROTO_UDP			1

// Control define
#define	CTRL_VERSION		(0x00 << 6)
#define	CTRL_DP_PACKET		(0x01 << 5)
//#define 	CTRL_CP_PACKET		(0x0 << 5)
#define 	CTRL_MULTIPLE_END	(0x01 << 4)
#define 	CTRL_ACK			(0x01 << 3)

#define	EX_PAYLOAD			(0x00 << 7)
#define	MULTIPLE_PACKET		(0x01 << 7)

#define 	EVENT_TT			(0x01 << 7)
#define 	EVENT_VL				(0x01 << 6)
#define 	EVENT_MD			(0x01 << 5)
#define 	EVENT_SD			(0x01 << 4)
#define	NETWORK_HEADER		"SDVR"



#define	SCREEN_ENV			0x0001
#define	RECORD_ENV			0x0002
#define	EVENT_ENV			0x0003
#define	COMMUNICATION_ENV	0x0004
#define	SYSTEM_ENV			0x0005

#define 	SVR1650_STICK_STOP_COMMAND	1
#define 	SVR1650_PRESET_MOVE_COMMAND	15
#define	NACK				0
#define	ACK					1

// DVR Model Code
#define 	SVR1650				0x8001
#define 	SVR1640				0x8002
#define 	SVR950				0x8003  //0x8003  ?„ì„ ?„ê³¼ ?˜ë…¼?´ì„œ 0x8003?¼ë¡œ ?˜ì •? ê²ƒ.

// firmware version == 0.1.0
#define		MAJOR				0
#define		MINOR				1
#define		SUB_VERSION			0
#define		FW_VERSION			"v2.4SDK"


// Video Codec
#define		MPEG4				1
#define		ADPCM				1


/************** For Board to Board Start ***************/
//#define 	MAXBTOBSIZE			1414  //1400 + 12 + 2
#define 		MAXBTOBSIZE			4096 //1400 + 12 + 2
#define		SOCK_TCP			0
#define		SOCK_UDP			1

/*** Audio/Video Command ***/
#define		REQ_BACKUP_STREAM		0x21
#define		ACK_BACKUP_STREAM		0x22
#define		REQ_SEARCH_STREAM		0x23
#define		ACK_SEARCH_STREAM		0x24

/*** Control Command ***/
#define		NETWORK_INFO			0xf0
//#define		FW_FILE_BIOS				0xf1
//#define		FW_FILE_OS				0Xf2
//#define		FW_FILE_USER2			0xf3
#define		SYSTEM_INIT_INFO			0xf4

#define		ACK_FW_FILE_TRANSFER		0xe0
#define		NACK_FW_FILE_TRANSFER	0xe1
#define		ACK_FW_FILE_ERASE		0xe2
#define		NACK_FW_FILE_ERASE		0xe3
#define		ACK_FW_FILE_UPDATE		0xe4
#define		NACK_FW_FILE_UPDATE		0xe5
#define 		NBD_INFO					0xe6
#define 		ACK_PKT_NEXT_IMAGE		0xe7
#define		MACADDR_WRITE			0xe8
#define 		ACK_MACADDR_WRITE		0xe9
#define 		NACK_MACADDR_WRITE		0xea

#define		LOGIN_OK				0x00
#define		LOGIN_FAIL_MIS_PW		0x01
#define		LOGIN_FAIL_ADMIN			0x02
#define		LOGIN_FAIL_USER			0x04
#define 		LOGIN_FAIL_AUTHORITY		0x08 

#define		REQ_PTZ_CTRL			0x61
#define		REQ_RELAY_CTRL			0x63
#define 		REQ_AUDIO_CTRL			0x6b
#define		ACK_AUDIO_CTRL			0x6c
#define		DAT_AUDIO_STREAM		0x6d

#define		REQ_BACKUP_CTRL			0x81
#define		ACK_BACKUP_CTRL			0x82
#define		REQ_BACKUP_PAUSE		0x83
#define		ACK_BACKUP_PAUSE		0x84
#define		REQ_BACKUP_STOP		0x85
#define		ACK_BACKUP_STOP			0x86
#define		REQ_BACKUP_INIT			0x87
#define		ACK_BACKUP_INIT			0x88
#define		REQ_SEARCH_CTRL			0x91
#define		ACK_SEARCH_CTRL			0x92
#define		REQ_SEARCH_PAUSE		0x93
#define		ACK_SEARCH_PAUSE		0x94
#define		REQ_SEARCH_STOP		0x95
#define		ACK_SEARCH_STOP			0x96
#define 		REQ_SEARCH_TRICK		0x97
#define 		ACK_SEARCH_TRICK		0x98

#define		REQ_ENV_INFO				0xA1
#define		ACK_ENV_INFO				0xA2
#define		REQ_ENV_CHANGE			0xA3
#define		ACK_ENV_CHANGE			0xA4
#define		REQ_ENV_ACTION			0xA5
#define		ACK_ENV_ACTION			0xA6
#define 		PKT_EVENT_TIME		0x79



#define 		ENABLE 		1
#define 		DISABLE		0


#define 		LAN			0
#define 		ADSL		1


#define 		SINGLE_MODE 	1
#define 		QUAD_MODE   	0

//2007-08-08
#define 		ADMIN_AUTHORITY		0x7000

#define		CONTROL_AUTHORITY	(0x01 << 14) // PTZ Authority
#define		AUDIO_AUTHORITY		(0x01 << 13) 
#define		VIDEO_AUTHORITY		(0x01 << 12) // Playback Authority

#define		SETUP		0
#define		PTZ			1
#define		SEARCH		3
//#define		PB			3
#define		NW_MIC		6


/** ************************************************************************* ** 
 ** typedefs
 ** ************************************************************************* **/

#pragma pack(1)

typedef struct{
	BYTE		start_code[4];		
	BYTE		Reversed[2];
	WORD		length;	
	BYTE		control;			
	BYTE		payload;
	BYTE		SeqNum;
	BYTE		userid;		
}SLICE_HEADER, *PSLICE_HEADER;

typedef struct{
	BYTE		start_code[4];		
	BYTE		Reversed[2];
	WORD		length;	
	BYTE		control;			
	BYTE		payload;
	WORD		SeqNum;
}SLICE_FILE_HEADER, *PSLICE_FILE_HEADER;

typedef struct          // 40 bytes
{       

        UINT32		PES_Data_Size;
        DWORD		Record_Time;
        BYTE		bFrameSource;
        BYTE		bVideoFrame;
        BYTE		bEventInfo;
        BYTE		bDecFormat;
        DWORD		nFrameNum;
        BYTE		nGOPSize;
        BYTE		nGOPNum; 
	char		cur_timezon1;
	char		cur_timezon2;	//atc;
        //WORD		nTextSize;  
        WORD		nReserved2;  
        //WORD		nReserved3;
	char			Timezone1;
	char			Timezone2;       
        unsigned short			cCHName[8];
} FRAME_HEADER;




typedef struct{
	BYTE		Video_Type;
	long			Current_Time;
	BYTE		user_passwd[11][9];
	BYTE		nmode;	
	unsigned short			scr_title[16][8];
	BYTE		dvr_name[20];	
	char     		signture[8];
	BYTE		Reserved;
} SYSTEM_INFORMATION;    


typedef struct{
	BYTE	event_type;
	BYTE	active;
	WORD	channel;
} EVENT_INFO;   



typedef enum
{
	AUDHAL_IOFMT_8K_8B_MONO,
	AUDHAL_IOFMT_8K_16B_MONO,
	AUDHAL_IOFMT_11K_8B_MONO,
	AUDHAL_IOFMT_11K_16B_MONO,
	AUDHAL_IOFMT_22K_8B_MONO,
	AUDHAL_IOFMT_22K_16B_MONO,
	AUDHAL_IOFMT_44K_8B_MONO,
	AUDHAL_IOFMT_44K_16B_MONO,

	AUDHAL_IOFMT_8K_8B_STEREO,
	AUDHAL_IOFMT_8K_16B_STEREO,
	AUDHAL_IOFMT_11K_8B_STEREO,
	AUDHAL_IOFMT_11K_16B_STEREO,
	AUDHAL_IOFMT_22K_8B_STEREO,
	AUDHAL_IOFMT_22K_16B_STEREO,
	AUDHAL_IOFMT_44K_8B_STEREO,
	AUDHAL_IOFMT_44K_16B_STEREO,
}AUDHAL_IOFMT;

typedef struct{
	int                 nHz; /*!< Sampling rate */
	int                 nBits; /*!< Bits per a sample */
	int                 nChannels;	/*!< channels (stereo=2 or mono=1) */
	int                 nFdDev; /*!< File descriptor for audio device */
} AUDHAL;

typedef struct __PLAY_INFO{
	WORD	play_ch;   
	BYTE    	play_speed;
	BYTE    	pause_onoff;
	BYTE    	play_dir;
	BYTE    	play_act; 
	BYTE	user_id[2]; 
	int		allCH__total_frame;   
	BYTE    	pbORbackup;  
	BYTE	next_image;
	DWORD	nFrameNum[MAX_CH_NUM];
	BYTE	nGOPNum[MAX_CH_NUM]; 
	BYTE	error;
	BYTE	ack_act;
#if 1		// byulkonghj for search time
	time_t	frame_rate_check_time;
	struct timeval tv1;
	struct timeval tv2;
#endif	
	WORD	ifskip_ch;
	unsigned char nb_sync;
} PLAY_INFO, *pPLAY_INFO;

#define AUDIOBUFS 3
typedef struct __audio_buffer{
	unsigned char buf[4*1024+100];
	int size;
}audio_buffer;

typedef struct {
	pthread_mutex_t indexlock;
	pthread_mutex_t buflock[AUDIOBUFS];
	audio_buffer buffer[AUDIOBUFS];
	BYTE    audio_onoff;
	BYTE index;
	AUDHAL	pAudio;	
} AUDIO_INFO;

typedef struct
{
	WORD year;
	WORD mon;
	WORD mday;
	WORD hour;
	WORD min;
#if 1		//inserted by hiyoou 071012 for search time
	WORD sec;
#endif
} time_struc; 	

typedef struct
{
	BYTE mac_addr[6];
	BYTE ip_addr[4];
	BYTE fw_version[10];
	BYTE Reserved[4];
} NBD_INFORMATION; 
#pragma pack ()

typedef struct {
	pthread_mutex_t 	*my_mutex;
	pthread_cond_t	*my_cond; 
	int count;
} MY_SEM_T;	
//#pragma pack () My arnold 2007.09.17

#pragma pack(1)

typedef struct {
	void 				*pPrev;
	void 				*pNext;
//	pthread_mutex_t recbuf_access_wait;
	pthread_t 			ThreadId;
	int				sock;
	int  				id;
	int 				server_id;
	BYTE			proto;
	struct sockaddr_in 	from;	
	char 			client_ip[16];
	int 				client_port;
	BYTE 			buf[MAX_NET_BUF];
	BYTE			lock;	  
	int				length;
	BYTE			detect_stream_err;
	BYTE			send_gop_num;
	BYTE			stop;		/* for client disconnection */
	BYTE			live;		/* Live/Search/Backup/Setup Mode */
	BYTE			userid;
	BYTE			i_only;
	BYTE			pause;	
	UNS16			Alive_channel;
	
	BYTE			user_level_id;		// user1,2,3,4,5¸¦ ±¸ºÐÇÏ±â À§ÇÑ ID

		
} CLIENT_ITEM, *PCLIENT_ITEM;

#pragma pack ()


#pragma pack(1)

typedef struct {
	
	BYTE	Video_Mode;	
	BYTE	net_type;	
	BYTE	dhcp;	
	BYTE	net_mode;		
	
	BYTE	net_mode_change;		
	
    	BYTE	xdsl_id[30];
    	BYTE	xdsl_pw[30];	
	
	BYTE	IP_addr[4];
	int		sm_addr[4];
	int		gw_addr[4];
	int		dns_addr[4];
	BYTE	MAC_Addr[6];
	
	int		port;
	BYTE	ddns;
	BYTE	send_wrs;
	
	BYTE	alarm_mail;
	char		mail_id[21];
	char		mail_server[31];

	BYTE	mail_auth;
	char		smtp_server[31];
	char		smtp_id[13];
	char		smtp_pw[13];

	char		dvr_name[20];	
	
	// added by choco71 --2007-0809
	char 	username[10][8];	
	
} NETWORK_VARIABLE;

#pragma pack()

NETWORK_VARIABLE	gVar; 


#pragma pack(1)
#if 0	//OLD
typedef struct {			// 20 bytes	
	UNS16	Model_Code;
	UNS16	Firmware_Version;
	BYTE	MAC_Address[6];
	BYTE	TCP_UDP;
	BYTE	Quad_Mux;  // 0 : single, 1 : Quad, 2 : Transcoder
	BYTE	Video_Codec;
	BYTE	Audio_Codec;
	BYTE	Max_Video_Channel;
	BYTE	Max_Audio_Channel;	
	UNS16	Active_Video_Channel;
	UNS16	Active_Audio_Channel;
	UNS16	PTZ_Channel;
	BYTE	Current_Time[4];
	BYTE 	reserved[2];

	unsigned short		scr_title[16][8];	
}DVR_INFORMATION;
#else
typedef struct {			// 20 bytes	
	WORD	nModelCode;	
	WORD	nFWVersion;
	BYTE	bMACAddress[6];
	BYTE	bTransType;
	BYTE	bVideoMux;
	BYTE	bVideoCodec;
	BYTE	bAudioCodec;	
	BYTE	bMaxVideoNum;
	BYTE	bMaxAudioNum;
	WORD	nExPTZCH;
	WORD	nActiveAudioCH;
	//WORD	nPTZEnableCH;
	char		timezone2;
	char		timezone1;
	DWORD	nServerCurTime;	
	BYTE	bUnicode;
	BYTE	bReserved;	
	WORD	bCHName[16][8];	
	BYTE	dst_daylight;
	DWORD	dst_start;
	DWORD	dst_end;
	BYTE	dst_duration;
	BYTE	reserve[6];
}DVR_INFORMATION;
#endif
#pragma pack()


#pragma pack(1)

typedef struct{
	BYTE	event_type;
	BYTE	Reserved;
	WORD	channel;
} EVENT_INFO2;   

#pragma pack()

#pragma pack(1)
typedef struct {
	unsigned char Type;
	unsigned char RD;
	long 	Par1;
	char Par2_1;
	char Par2_2;
	unsigned char Par3;
	unsigned char Par4;
	unsigned char Par5;
	unsigned char Par6;
	unsigned char Par7;
	unsigned char Par8;
	unsigned char Par9;
	unsigned char Par20;
}LOG_INFO;
#pragma pack()

typedef struct {
   void *pPrev;
   void *pNext;

   pthread_mutex_t server_wait;
   pthread_t ThreadId;
   int  id;
   BYTE	proto;
} SERVER_ITEM, *PSERVER_ITEM;



typedef struct __GSUPPRESSION {
	unsigned char keyprocessing;
	unsigned char mouseprocessing;
	unsigned char waitingincommingthread;
	unsigned char recordprocessing;
	unsigned char timerprocessing;
	unsigned char reserved[3];
} GSUPPRESSION, *pSUPPRESSION;



/** ************************************************************************* ** 
 ** functions
 ** ************************************************************************* **/

int InitServer(void);
int my_listen(int port,int Proto);

//static int get_new_id(PCLIENT_ITEM *pHead);
CLIENT_ITEM *find_client_id(int id);

void send_cmd(int sock, INT16 cmd, BYTE *data, int size);
//static int create_socket(void);
 
 int server_thread_create();
 void *pthread_server(void *args);
 void *pthread_audio(void *args);
int mic_thread_create();
void *pthread_network_audio(void *args);

void my_sem_post (MY_SEM_T *my_semaphore);
int    my_sem_wait (MY_SEM_T *my_semaphore, int timeout);
void my_sem_init (MY_SEM_T *my_semaphore, int val);
void my_sem_reinit (MY_SEM_T *my_semaphore, int val);

int init_server_variable ();

void AudHalIoFmt2Ctx(AUDHAL_IOFMT eFormat);
int AudHalInitDev(int nLevel);
int AudioOutPlay(unsigned char *pPcmData, int nPcmSz);


extern PLAY_INFO gNetPlay[4];
extern int			my_sockfd;
extern BYTE		network_board_macaddr[6];
extern BYTE		macaddr_for_write[6];
extern NBD_INFORMATION nbd_info_data;

extern sem_t		NetworkAudioWait;

extern pthread_mutex_t 		send_access_wait;
//extern int 					szStream[MAX_CONNECTION][SERVER_NUM];
extern BYTE				NetBdReBootFlag;


extern 	sem_t		NetworkSearchWait[MAX_CLIENT_NUM];
extern	MY_SEM_T 	NetworkNextImage[MAX_CLIENT_NUM];


extern 	SYSTEM_INFORMATION SysInfo;

extern pthread_cond_t mail_send_cond;
extern pthread_mutex_t mail_send_cond_mutex;
/** ************************************************************************* ** 
 ** includes
 ** ************************************************************************* **/
 
int sudo_Do(SLICE_HEADER * cmd_pkt);
int Do(int sockfd, BYTE user_id[], SLICE_HEADER * cmd_pkt, BYTE *buf, int size);
int DoMain (PCLIENT_ITEM pClient, SLICE_HEADER *cmd_pkt, BYTE *send_cmd, BYTE *recv_buf, int recv_size, BYTE *send_buf, int *send_size);
BYTE ring_create_pthread(void* thread_func, int priority, int stacksize, int schedpolicy, void* arg) ;
BYTE  ring_create_pthread_id(pthread_t* thread_t, void* thread_func, int priority, int stacksize, int schedpolicy, void* arg) ;
int my_connect(void);
void reconnect_try (int type);

#endif /* _SERVER_H */
