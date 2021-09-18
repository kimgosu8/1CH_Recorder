#ifndef __IDE_H__
#define __IDE_H__

/** ************************************************************************* **
 ** includes
 ** ************************************************************************* **/
//#include <stdio.h>
//#include <sys/types.h>
//#include <fcntl.h>
#include <linux/hdreg.h>
#include <sys/mount.h>
#include "typedef.h"
//#include "audio.h"

#if defined __IDE_C__
	#define EXTN 
#else
	#define EXTN extern
#endif


//#define MAX_INDEX_COUNT 128
//#define MAX_MPEG_REC_SIZE	200 * 1024
//#define MAX_AUD_REC_SIZE	30 * 1024
//#define MAX_BACKUP_LIST 1000
//#define MAX_BACKUP_LIST2		50000
//#define FULL_HDD_SIZE		(UINT64)25 * (UINT64)1024 * (UINT64)1024
//#define FULL_HEAD			(UINT64)10 * (UINT64)1024 * (UINT64)1024
//#define GIGA_SEARCH_SIZE	(UINT64)50 * (UINT64)1024 * (UINT64)1024
//#define GIGA_DETECT			MAX_MPEG_REC_SIZE * MAX_INDEX_COUNT

//#define GARBAGE_DATA_SIZE 0

//#define GIGA_SEARCH_OFFSET2	(UINT64)10 * (UINT64)1024 * (UINT64)1024
//#define EVENT_LIST_OFFSET2	(UINT64)12 * (UINT64)1024 * (UINT64)1024
//#define HDD_HEADER_OFFSET2	(UINT64)24 * (UINT64)1024 * (UINT64)1024
//#define REC_INFO_OFFSET2	(UINT64)25 * (UINT64)1024 * (UINT64)1024
//#define GREC_INFO_OFFSET2	(UINT64)26 * (UINT64)1024 * (UINT64)1024

//#define BACKUP_LIST_OFFSET	(UINT64)15 * (UINT64)1024 * (UINT64)1024
//#define GIGA_SEARCH_OFFSET	(UINT64)20 * (UINT64)1024 * (UINT64)1024
//#define EVENT_LIST_OFFSET	(UINT64)22 * (UINT64)1024 * (UINT64)1024
//#define HDD_HEADER_OFFSET	(UINT64)24 * (UINT64)1024 * (UINT64)1024
//#define REC_INFO_OFFSET		(UINT64)25 * (UINT64)1024 * (UINT64)1024
//#define GREC_INFO_OFFSET	(UINT64)26 * (UINT64)1024 * (UINT64)1024
//#define START_OFFSET		(UINT64)30 * (UINT64)1024 * (UINT64)1024 

#define MAX_IDE_NUM 4
#define MAX_SCSI_NUM 16
#define MAX_HDD_NUM (MAX_IDE_NUM+MAX_SCSI_NUM) // jphan
#define MAX_NUM_VOLUMES 2048 // DVR supports up to 2TB per each disk... because of the limit in linux kernel 2.4
#define MAX_SATA_PORT	4

#define NEW_HDD_HEADER_OFFSET 10*1024
#define NEW_HDD_HEADER_BACKUP_OFFSET 20*1024
#define HDD_MIRROR_INFO_OFFSET (NEW_HDD_HEADER_OFFSET + 1)

// 2007 12 10 BAD LVG LIST
#define BAD_LVGS_OFFSET (NEW_HDD_HEADER_OFFSET + 2)

#define NEW_DATA_OFFSET		(DWORD)200 * (DWORD)1024
#define GROUP_HEADER_SIZE	1
#define GROUP_TAIL_SIZE		1

//#define MAX_BACKUP_BUFFER_COUNT	 8
//#define MAX_PLAY_BUFFER_COUNT	 10 //20
//#define MAX_REC_BUFFER_COUNT	 100
//#define MAX_AUD_BUFFER_COUNT	 32

//#define MAX_NETPLAY_BUFFER_COUNT 1

//#define SRAM_HEAD_INFO		0x4100
//#define SRAM_TAIL_INFO		0x4200
//#define SRAM_REC_INFO		0x4300
//#define SRAM_PLAY_INFO		0x4300

//#define NETPLAY_BUF(i,loc)	(((PLAY_BUFFER*)(gNetPlayBuf[i].pData))[gNetPlayBuf[i].loc])

//#pragma pack(1)

typedef struct {
	char signature[8];
	int num_volumes;
	UINT64 time_signature;
	int num_bad_volumes; // 2007 12 10
	char reserved1[36];
} NEW_HDD_HEADER;

typedef struct {
	DWORD mirror_signature1;
	UINT64 mirrored_time_signature;
	UINT64 time_signature_of_pair;
	DWORD mirror_signature2;
} MIRROR_INFO_HEADER;

//#pragma pack()

typedef	struct {
	int volume_id;
	int status; 		// LV_STATUS_EMPTY, LV_STATUS_RECORDING, LV_STATUS_FULL
	time_t start_time;
	time_t end_time;
	DWORD last_offset;
	DWORD new_offset;
	int event_count;
} LOGICAL_VOLUME_HEADER;

struct tagLVG {
	DWORD start_offset; //TODO: 64bit로 전환할 것
	int hdd_id;
	int event_former_record_length;
	void* event_offset_start;
	void* event_offset_end;
	struct tagLVG * prev_pLVG;
	struct tagLVG * next_pLVG;
	LOGICAL_VOLUME_HEADER tLVH;
};
typedef struct tagLVG LOGICAL_VOLUME_GROUP;
//dataLVG_list;
//LOGICAL_VOLUME_GROUP* freeLVG_list;

struct tagLVG_list {
	LOGICAL_VOLUME_GROUP* LVG_ptr_head;
	LOGICAL_VOLUME_GROUP* LVG_ptr_tail;
	int num_LVs;
};

typedef	struct _fdisk_info {
	BYTE exist;
	BYTE format;
	BYTE make_mirror;
	BYTE mirror_configured;
	int ide_fd;
	int num_LVs;
	UINT64 sector_size;	/* Number of sectors */
	NEW_HDD_HEADER hdd_header;
	struct hd_driveid info; /* <linux/hdreg.h> */
	pthread_mutex_t ide_access_wait;
	LOGICAL_VOLUME_GROUP *tLVG;
	
	DWORD free_blocks;
	DWORD total_blocks;
	time_t rec_start;
	time_t rec_end;
	UINT64 mirrored_time_signature;
	int pair_id;
	
	int num_FREEs;
	int error_count;
} IDE_DEV_INFO;

typedef struct {
	time_t datetime;
	DWORD version;
	int size;
	void* event_offset;
	//BYTE ch_name[TITLE_LENGTH]; // 10 byte : 
	time_t timezone_offset;
	DWORD reserved10_1;
	WORD reserved10_2;
	BYTE data_type;
	BYTE picture_type;
	BYTE channel;
	BYTE event_type;
	BYTE NtscPal;
	BYTE frame_rate;
	DWORD seq;
	DWORD I_seq;
	BYTE resolution;

	//BYTE reserved[3];
	//BYTE audio_rec;
	////WORD text_size;
	//DWORD audio_size;

	//BYTE reserved[6]; //0: NORMAL,   1: EVENT,   2: Version
	BYTE normal_rec;
	BYTE event_rec;
	BYTE reserved1; // 0xef 란 값을 넣은 적이 있었음.
	BYTE event_msg;
	BYTE isdst;
	BYTE watermark;
	BYTE crc_value;
}IMAGE_HEADER;

#define MAX_IMAGE_SIZE 120 // TODO: 128!
#define MAX_GROUP_SIZE 1024*1024

typedef struct {
	WORD			CH_exist;
	WORD			I_exist;
	WORD			block_size;
	WORD			num_picture;
	BYTE			index_high[MAX_IMAGE_SIZE];
	WORD			index_low[MAX_IMAGE_SIZE];
	DWORD			Izone;
	DWORD			non_Izone;
	DWORD			prev_offset;
	WORD			prev_blocks;
	DWORD			current_offset;
	DWORD			next_offset;
	BYTE			CH_count;
	BYTE			reserved1;
	WORD			EVENT_exist;
	time_t			last_time;
	DWORD			seq;
	BYTE			status; // NO USE
	struct tagEventInfo *event_offset_start;
	struct tagEventInfo *event_offset_end;
	int			nr_events;
	time_t		lvg_time;
	DWORD		version;
} NEW_GROUP_HEADER;

struct PBControlBlock
{
	DWORD	start_addr; // PB시에만 사용함.
	DWORD	end_addr;
	int	buffer_ready;
	//int	LVG_index;
	void*	LVG_ptr;
	time_t	lvg_time;
	BYTE	I_only;
	int	reserved;
};

typedef struct {
	DWORD	dwMake;
	WORD	wServerIndex;
	BYTE	bCHIndex;
	BYTE	bReserved1;
	BYTE	cCHName[16];
	BYTE	cRecordCheck[75];
	BYTE	bReserved2;
	DWORD	dwFrameInfoPos;
} STW_FILE_HEADER;

typedef struct {
	DWORD	dwFrameInfoPos;
	WORD	wFrameSize;	
	BYTE	bIFrameGap;
	BYTE	bReserved;	
} WRITE_FRAME_HEADER;

typedef struct {
	DWORD	dwFrameInfoPos;
	DWORD	dwFrameSize;
	BYTE	bFrameSource;
	BYTE	bVideoFrame;
	BYTE	bIFrameGap;
	BYTE	bReserved;	
} READ_FRAMECHECK_HEADER;

typedef struct
{			
	DWORD	nFrameSize;	
	DWORD	nRecordTime;
	BYTE	bFrameSource;
	BYTE	bVideoFrame;
	BYTE	bEventInfo;
	BYTE	bDecFormat;
	DWORD	nFrameNum;
	WORD	nGOPNum;
	WORD	nTextSize;
	WORD	nReserved2;
	char	Timezone1;
	char	Timezone2;
	char		cCHName[16];
} FRAME_INFO;

EXTN LOGICAL_VOLUME_GROUP *gLVG[MAX_NUM_VOLUMES * MAX_HDD_NUM];
EXTN IDE_DEV_INFO ide_devs_info[MAX_HDD_NUM];
EXTN struct tagLVG_list data_LVG;
EXTN struct tagLVG_list free_LVG;

void print_hdd_checking(int);
char* make_device_string(int);
void make_copy_room(int);
void report_next_HDD_fail(void);
void sataHDD_scanning(void);
void ide_identify(void);
int LVG_sort( const void * mFormer, const void * mLatter);
void hdd_error_handle(int);
int ide_write(BYTE hddid,UINT64 offset,BYTE * buf,DWORD size,BYTE sync);
int ide_read(BYTE hddid,UINT64 offset,BYTE * buf,DWORD size);
int ide_block_read( BYTE hddid, DWORD offset, BYTE* buf, DWORD size);
int ide_block_write( BYTE hddid, DWORD offset, BYTE* buf, DWORD size, BYTE sync);
int ide_block_scan( BYTE hddid);
void calc_hdd_free(void);
void probe_HDD_info(int force);
void write_cached_buffer(void);
int read_hdd_info( NEW_HDD_HEADER* pHddHeader, int hdd_id, int force);
int format_hdd( int, NEW_HDD_HEADER*);
int configure_LogicalVolumeGroup( int, NEW_HDD_HEADER*);
void calc_hdd_free(void);
int calc_each_disk(int);
void read_mirror_info(int);
LOGICAL_VOLUME_GROUP* find_proper_position( time_t target_time);
void* pop_free_LVG(void);
UINT64 make_time_signature(void);



#define DIVISION_PER_DAY 24
#define SEC_PER_DIVISION1 (60*60)
#define DIVISION_PER_HOUR 60
#define SEC_PER_DIVISION2 60

EXTN struct {
	BYTE CalendarMatrixNumber[6][7];
	BYTE CalendarMatrixExist[42];
	WORD CalendarMatrixTimeline1[DIVISION_PER_DAY];
	WORD CalendarMatrixEventline1[DIVISION_PER_DAY];
	WORD CalendarMatrixTimeline2[DIVISION_PER_HOUR];
	WORD CalendarMatrixEventline2[DIVISION_PER_HOUR];
	WORD CalendarMatrixNetworkTimeline[DIVISION_PER_DAY * DIVISION_PER_HOUR];
	WORD CalendarMatrixNetworkEventline[DIVISION_PER_DAY * DIVISION_PER_HOUR];

	void* Timeline_lvgptr1[16][DIVISION_PER_DAY];
	DWORD Timeline_offset1[16][DIVISION_PER_DAY];
	void* Timeline_lvgptr2[16][DIVISION_PER_HOUR];
	DWORD Timeline_offset2[16][DIVISION_PER_HOUR];

	int this_year, this_month, this_day;
} CalendarSearch;
/*
EXTN struct {
	BYTE CalendarMatrixNumber[6][7];
	BYTE CalendarMatrixExist[42];
	WORD CalendarMatrixTimeline[24 * 2]; // 2: 30분마다   3: 20분마다   4: 15분마다

	void* Timeline_lvgptr[16][24*2];
	DWORD Timeline_offset[16][24*2];

	int this_year, this_month;
} CalendarSearch;
*/
EXTN BYTE bad_volume_table[ MAX_NUM_VOLUMES ];

#define PBBufferSize (4*1024)
EXTN BYTE MP_Buffer[PBBufferSize*512*4 + 8192];
//BYTE New_Buffer1[PBBufferSize*512 + 1024];
//BYTE New_Buffer2[PBBufferSize*512 + 1024];
//BYTE New_Buffer3[PBBufferSize*512 + 1024];
//BYTE New_Buffer4[PBBufferSize*512 + 1024];
//BYTE New_Buffer5[PBBufferSize*512 + 1024];
//BYTE New_Buffer6[PBBufferSize*512 + 1024];
//BYTE New_Buffer7[PBBufferSize*512 + 1024];

#undef EXTN
#endif //__IDE_H__
