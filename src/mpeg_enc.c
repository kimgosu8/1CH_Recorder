/********************************************************************************
   INCLUDES
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <linux/videodev.h>
#include <errno.h>


#include "typedef.h"
#include "svr_common.h"
#include "MpegEnc.h"
#include "MpegCtrl.h"
#include "ide.h"
#include "env.h"
#include "ring_buf.h"

#include "server.h"


#include "v4l2uvc.h"
#include "avformat.h"
#include "avcodec.h"
#include "rational.h"
#include "swscale.h"
#include "opt.h"



/********************************************************************************
   DEFINES
********************************************************************************/
#define TRUE 1
#define FALSE 0


/********************************************************************************
   GLOBALS
********************************************************************************/

struct tag_sixty_seconds_info {
	time_t the_time;
	struct tagEventInfo *pEvent_offset;
	int validity;
	int num_picture;
};

struct tag_sixty_seconds_info sixty_seconds_info[60];

int front_ptr = 0;
int rear_ptr = 0;

pthread_mutex_t mutex_current = PTHREAD_MUTEX_INITIALIZER;


pthread_cond_t    	RecBuffer_cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t   	RecBuffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t 	at2041_lock;

BYTE* preBuffer;

struct Image_buffer_header {
	unsigned int valid;
	time_t the_time;
	unsigned int size;
	int next_offset;
};

int temp_input_count=0, temp_output_count=0;


/********************************************************************************
   LOCALS
********************************************************************************/
extern struct vdIn *videoIn;
extern char KEY_INPUT;

extern AVPacket g_packet;
extern BYTE g_pict_type;



/********************************************************************************
   Function Prototype Declare
********************************************************************************/
void flush_time_index(void);
void write_cached_buffer();
void err_func();
void init_encoding(int width, int height);
void stop_encoding();
void exit_encoding();
void start_encoding(unsigned char *in_addr);
void record_force_start(void);


/********************************************************************************
   Function define
********************************************************************************/


void init_preBuffer() // should be protected by mutex prior to calling this.
{
	pthread_mutex_lock(&mutex_current);
	rear_ptr = front_ptr;
	pthread_mutex_unlock(&mutex_current);
}



void* Task_preBuffer_timeChange(void * arg)
{
	DWORD group_count = 0;
	int anomaly = 0;
	IMAGE_HEADER* pHeader;
	NEW_GROUP_HEADER* pGH;
	struct Image_buffer_header * pLink;
	WORD* pData;
	int current_buffer_idx;
	struct tagEventInfo **event_ptrs;
	struct tagEventInfo *last_event_offset;

	time_t cur_time;
	time_t work_time;
	time_t next_time;
	int cur_sec, work_sec, next_sec;
	int total_size;
	int event_processing;

	last_event_offset = gNewVar.Event.buffer_addr_current;





	while(1)
	{
		pthread_mutex_lock(&RecBuffer_mutex);
		pthread_cond_wait(&RecBuffer_cond, &RecBuffer_mutex);
		pthread_mutex_unlock(&RecBuffer_mutex);


		cur_time = gNewVar.current_time;
		cur_sec = cur_time % 60;
		pthread_mutex_lock( &gNewVar.hdd_encode_lock );



		pLink = (struct Image_buffer_header *)&preBuffer[rear_ptr];
		if ( (pLink->valid & 0xff0000ff) != ( (((unsigned int)'A')<<24) | ((unsigned int)'Z') ) )
		{
			pthread_mutex_unlock( &gNewVar.hdd_encode_lock );
			printf ("[ENC] RESET PREBUFFER!!! R:%08d F:%08d\n", rear_ptr, front_ptr);
			pthread_mutex_lock(&mutex_current);
			rear_ptr = front_ptr;
			pthread_mutex_unlock(&mutex_current);
			continue;
		}

		pthread_mutex_lock(&ide_devs_info[ ((LOGICAL_VOLUME_GROUP*)gNewVar.Record.LVG_ptr)->hdd_id ].ide_access_wait);




		while (pLink->valid == ( (((unsigned int)'A')<<24) | (((unsigned int)1)<<8) | ((unsigned int)'Z') ) )
		{
			work_time = pLink->the_time;

			if ( cur_time - work_time > 30 ) // time over 버릴것.
			{
				printf ("[ENC] Adjusting Rec Time Interval Forward!!! %d %d \n", (unsigned int)work_time, (unsigned int)cur_time);
				pLink->valid = 0;
				rear_ptr = pLink->next_offset;
				pLink = (struct Image_buffer_header *)&preBuffer[rear_ptr];
				continue;
			}

			if ( cur_time - work_time < gNewVar.preRecordingTime ) //work_time >= cur_time 도 포함되는지 확인할것. (음수가 되는 경우)
			{
				if ( work_time > cur_time )
				{
					printf ("[ENC] Adjusting Rec Time Interval Backward!!! %d %d \n", (unsigned int)work_time, (unsigned int)cur_time);
					pLink->valid = 0;
					rear_ptr = pLink->next_offset;
					pLink = (struct Image_buffer_header *)&preBuffer[rear_ptr];
					continue;
				}
				break; // 너무 빨리 소모해서 한 박자 쉬도록 break
			}

			work_sec = work_time % 60;
			next_time = work_time + 1;
			next_sec = next_time % 60;

			pGH = (NEW_GROUP_HEADER *)gNewVar.Record.pData;
			memset( (void*)pGH, 0, 512);


			if ( sixty_seconds_info[next_sec].validity == 1 && next_time == sixty_seconds_info[next_sec].the_time )
			{
				pGH->event_offset_start = last_event_offset;
				pGH->event_offset_end = sixty_seconds_info[next_sec].pEvent_offset;
				last_event_offset = pGH->event_offset_end;
			}
			else // less reliable information
			{
//				printf ("[ENC] Omitted 1 sec \n");
			}

			pGH->version = 0x09022500;
			pGH->Izone = GROUP_HEADER_SIZE<<9;
			pGH->non_Izone = GROUP_TAIL_SIZE<<9;
			current_buffer_idx = 0;

			event_ptrs = (struct tagEventInfo **) ((BYTE*)pGH + MAX_GROUP_SIZE - ((GROUP_HEADER_SIZE + GROUP_TAIL_SIZE)<<9));
			memset( (void*)event_ptrs, 0, 512);

// printf("work_sec ======================>[%d sec]\n", work_sec);

			do {
// printf("rear_ptr ==>%d\n", rear_ptr);
				pHeader = (IMAGE_HEADER *)(pLink + 1);
				pData = (WORD*)(pHeader + 1);


				if ( check_crc( (BYTE*)pHeader, sizeof(IMAGE_HEADER) - 1) == 1 )
				{
					// printf("CRC Check SUCCESS!!!!!\n");
				}
				else
				{
					printf("CRC check Error during Data Copy!!!\n");
				}

				if ( pHeader->data_type == 2 || pHeader->data_type == 3 || pHeader->data_type == 4 ) //MUX HEADER 8byte 포함
				{
					// 정상적인 데이터
					printd("Normal Data!!!!!!\n");
				}
				else
				{
					// 헤더 이상. 가비지?
					printf("What's wrong? Data is Different!!!!!!\n");
					anomaly++;
					pthread_mutex_lock(&mutex_current);
					rear_ptr = front_ptr;
					pthread_mutex_unlock(&mutex_current);
					break;
				}




				if ( pHeader->event_rec == ON ) // Post Event Record
				{
					printf ("EVENT DATA 2\n");
					event_processing = ON;
				}
				else if ( gNewVar.Event.CH_info[pHeader->channel].record_count > 0 )
				{
					printf ("PRE EVENT DATA 3\n");
					// 이벤트가 발생 후는 아니지만, 녹화해야 할 데이터 ( pre-REC )
					event_processing = ON; 
				}
				else if ( pHeader->normal_rec == ON ) // 채널 구분 없이 전채널 녹화 Noraml Record
				{
					printd ("MANUAL DATA 1\n");
					event_processing = OFF; // 순서에 주의할 것!
				}
				else
				{
					printf ("DO NOT Record\n");
					goto next_image; // Don't Record
				}






				total_size = pLink->size - sizeof(struct Image_buffer_header);

				if ( current_buffer_idx >= MAX_IMAGE_SIZE || pGH->Izone + pGH->non_Izone + total_size > (MAX_GROUP_SIZE) - (4<<9) ) // 버퍼의 마지막 두 블럭은 남겨놓아야 한다. >= (1*1024*512)>>9 - 1
				{
printf("here????????????????????????????????????????????????????????????????????????????????????\n");
exit(1);
					pGH->num_picture = current_buffer_idx;
					pGH->block_size = ((pGH->Izone + 511)>>9) + GROUP_TAIL_SIZE + ((pGH->non_Izone + 511)>>9);
					pGH->last_time = work_time;
					pGH->seq = group_count++;

					memcpy( (BYTE*)pGH + ((pGH->Izone + 511)&0xfffffe00) + 256, gEnv.channel_title, 256);

					write_cached_buffer();
					memset( (void*)pGH, 0, 512);

					pGH->version = 0x06050100;
					pGH->Izone = GROUP_HEADER_SIZE<<9;
					pGH->non_Izone = GROUP_TAIL_SIZE<<9;
					current_buffer_idx = 0;
					event_ptrs = (struct tagEventInfo **) ((BYTE*)pGH + MAX_GROUP_SIZE - ((GROUP_HEADER_SIZE + GROUP_TAIL_SIZE)<<9));
					memset( (void*)event_ptrs, 0, 512);
				}






				if (pHeader->data_type == 0x02 && pHeader->picture_type == 0x01) // I DATA
				{
					memcpy( (BYTE*)pGH + pGH->Izone, pHeader, total_size);

					pGH->index_high[current_buffer_idx] = (BYTE)((pGH->Izone & 0x00ff0000)>>16);
					pGH->index_low[current_buffer_idx] = (WORD)(pGH->Izone & 0x0000ffff);
					pGH->CH_exist |= (WORD)( (0x0001) << (pHeader->channel) );
					pGH->I_exist |= (WORD)( (0x0001) << (pHeader->channel) );

//printf("[I ZONE] ==>%d (%d)\n", pGH->Izone, total_size);
//if( pHeader->channel == 1 )
//printf("[I ZONE] pHeader->channel ==>%d\n", pHeader->channel);

					pGH->Izone += total_size;
				}
				else // non-I DATA
				{
					pGH->non_Izone += total_size;

					memcpy( (BYTE*)pGH + MAX_GROUP_SIZE - (GROUP_HEADER_SIZE<<9) - pGH->non_Izone, pHeader, total_size);
					
					pGH->index_high[current_buffer_idx] = (BYTE)((pGH->non_Izone & 0x00ff0000)>>16) | (BYTE)(0x80);
					pGH->index_low[current_buffer_idx] = (WORD)(pGH->non_Izone & 0x0000ffff);
					pGH->CH_exist |= (WORD)( (0x0001) << (pHeader->channel) );

//printf("[NON-I ZONE] ==>%d  (%d)\n", MAX_GROUP_SIZE - (GROUP_HEADER_SIZE<<9) - pGH->non_Izone, total_size);
//if( pHeader->channel == 1 )
//printf("[NON- I ZONE] pHeader->channel ==>%d\n", pHeader->channel);
				}





				if ( pHeader->event_offset != NULL )
				{
					((struct tagEventInfo *)pHeader->event_offset)->group_index = current_buffer_idx;
					((struct tagEventInfo *)pHeader->event_offset)->group_offset = group_count;
					event_ptrs[(pGH->nr_events)++] = (struct tagEventInfo *)pHeader->event_offset;
					//if ( pHeader->event_type == (0x01<<EVENT_TEXT) )
					//	printd("[EVENT] TEXT Event Added !!!!!!!!!!!!!!!!!!!!!!\n");
				}

				if ( event_processing == ON )
					pGH->EVENT_exist |= (WORD)( (0x0001) << (pHeader->channel) );


				++(current_buffer_idx);


				next_image:

				pLink->valid = 0;
				rear_ptr = pLink->next_offset;
				pLink = (struct Image_buffer_header *)&preBuffer[rear_ptr];
				//printf("3,%d,%d\n",work_time, pLink->the_time);

temp_output_count++;
			} while ( pLink->valid == ( (((unsigned int)'A')<<24) | (((unsigned int)1)<<8) | ((unsigned int)'Z') ) && work_time == pLink->the_time );

// printf("temp_input_count ==>%d\n", temp_input_count);
// printf("temp_output_count ==>%d\n", temp_output_count);
if( temp_output_count < 29 )
	printf("Performance is not good!\n");
temp_input_count = 0;
temp_output_count = 0;







			if ( current_buffer_idx > 0 || pGH->event_offset_start != pGH->event_offset_end )
			{
				pGH->num_picture = current_buffer_idx;
				pGH->block_size = ((pGH->Izone + 511)>>9) + GROUP_TAIL_SIZE + ((pGH->non_Izone + 511)>>9);
				pGH->last_time = work_time;
				pGH->seq = group_count++;

// printf("pGH->num_picture ==>%d\n", pGH->num_picture);
// printf("pGH->block_size ==>%d\n", pGH->block_size);

				memcpy( (BYTE*)pGH + ((pGH->Izone + 511)&0xfffffe00) + 256, gEnv.channel_title, 256);
				write_cached_buffer();
			}

			sixty_seconds_info[work_sec].validity = 0;


			//if ( debug_onoff == 1 )
			//	printd("%02d SAVED, %02d PREPARED, %02d INTERVAL\n", work_sec, cur_sec, gNewVar.preRecordingTime);
		
		} //while


		flush_time_index();
		pthread_mutex_unlock(&ide_devs_info[ ((LOGICAL_VOLUME_GROUP*)gNewVar.Record.LVG_ptr)->hdd_id ].ide_access_wait);
		pthread_mutex_unlock( &gNewVar.hdd_encode_lock );

		calc_hdd_free();

		if ( gNewVar.Record.verbose == 1 )
			fprintf( stderr, "%d-%d Buffer Memory R:%08d F:%08d\n", gNewVar.Record.record_ready, gNewVar.Record.repeat, rear_ptr, front_ptr);

	} //while

}







void MpegEncStart()
{
	if (!videoIn->isstreaming)
		if (video_enable (videoIn))
		{
			err_func();
		}

	memset (&videoIn->buf, 0, sizeof (struct v4l2_buffer));
	videoIn->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	videoIn->buf.memory = V4L2_MEMORY_MMAP;
//	videoIn->buf.flags = V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_TIMECODE;
}


void image_deque_buf()
{
	int ret;
	
	ret = ioctl (videoIn->fd, VIDIOC_DQBUF, &videoIn->buf);
	if (ret < 0) 
	{
		fprintf (stderr, "Unable to dequeue buffer (%d).\n", errno);
		err_func();
	}

//	printf("deque_buf!!!!!!!!!!!\n");
}


void image_enque_buf()
{
	int ret;
	
	ret = ioctl (videoIn->fd, VIDIOC_QBUF, &videoIn->buf);
	if (ret < 0)
	{
		fprintf (stderr, "Unable to requeue buffer (%d).\n", errno);
		err_func();
	}

// 	printf("enque_buf!!!!!!!!!!!\n\n");
}


void err_func()
{
	videoIn->signalquit = 0;

	fprintf (stderr, "Error grabbing!!!!!!!!!!!!!!!\n");
	close_v4l2 (videoIn);
	free(videoIn);
	free(videoIn);
	free(videoIn->framebuffer);
//	free(videoIn->tmpbuffer);
	exit (1);

}








/*
BYTE garbage[MUX_BUF_SIZE];
struct {
	unsigned int accumulated_size;
	BYTE buffer[PARKING_SIZE + 128];
} large_size[MAX_AT2041_NUM];
*/



void* Task_pthread_enc(void * arg)
{
//	int 							loop;
//	WORD 						last_flag;
	IMAGE_HEADER				*pHeader;
	IMAGE_HEADER 				tHeader;
	struct Image_buffer_header 	*pLink;
//	WORD 						*pData;

	unsigned int 					video_count[MAX_CH_NUM];
	unsigned int 					audio_count[MAX_CH_NUM];
	unsigned int 					I_count[MAX_CH_NUM];

	UNS32						frame_number[SINGLE_MODE_MAX_CHANNEL];
	UNS32						audio_frame_number[SINGLE_MODE_MAX_CHANNEL];

	BYTE						enc_gop_num[SINGLE_MODE_MAX_CHANNEL];

	int 							csnum;
	int 							size_error;

	unsigned long 					total_size = 0;



#ifdef MEMORY_ALLOCATION
	if ( preBuffer == NULL )
		printf("Pre-Buffer is TOO Big! Failed to allocate sufficient memory!\n");
#endif




	memset( preBuffer, 0, PREBUFFER_SIZE);
	memset( video_count, 0, sizeof(video_count));
	memset( audio_count, 0, sizeof(audio_count));
	memset( I_count, 0, sizeof(I_count));
//	memset( large_size, 0, sizeof(large_size));
	memset( gNewVar.Data_Accumulated, 0, (MAX_CH_NUM + 4) * sizeof(int));

	memset(frame_number, 0x00, sizeof(UNS32)&SINGLE_MODE_MAX_CHANNEL);
	memset(audio_frame_number, 0x00, sizeof(UNS32)&SINGLE_MODE_MAX_CHANNEL);
	memset(enc_gop_num, 0x00, sizeof(BYTE)&SINGLE_MODE_MAX_CHANNEL);


	pLink = (struct Image_buffer_header *)&preBuffer[front_ptr];
	pLink->valid = (((unsigned int)'A')<<24) | ((unsigned int)'Z');

/*
	for (loop = 0; loop < MAX_CH_NUM; loop ++) 
	{	
		SetMotion(loop, 0, 1, 0, 2);		// motion : ch, flag, sensitivity, area, resolution
	}
*/

/*
	time_t now;  // jphan

	time(&now);


	if ( gNewVar.isdst > 0 )
	{
		for (loop = 0; loop < MAX_AT2041_NUM; loop ++)
		{
			encoding_with_time_embedding( loop, 1); // jphan
			embedding_time_set( loop, now + gNewVar.timezone.offset_sec + 3600); // jphan
		}
	}
	else
	{
		for (loop = 0; loop < MAX_AT2041_NUM; loop ++)
		{
			encoding_with_time_embedding( loop, 1); // jphan
			embedding_time_set( loop, now + gNewVar.timezone.offset_sec); // jphan
		}
	}

*/






again:
	
	gNewVar.bChange = 0;

	
//	EncParam( gNewVar.resolution, gNewVar.NtscPal, gNewVar.Record.rate_control_mode);	// at2041 setting

	memset( gNewVar.Event.event_rec_change, 1, sizeof(gNewVar.Event.event_rec_change) ); 
//	event_condition_change(); // at2041 framerate + quality change
	usleep(100000); 


	MpegEncStart();
	usleep(100000);


	init_encoding(videoIn->width, videoIn->height);









	while(gNewVar.bChange == 0)
	{
		image_deque_buf();
// printf("front_ptr ==>%d\n", front_ptr);

		if( KEY_INPUT == 113) // 'q'
		{
			stop_encoding();
			exit_encoding();
		}

		memcpy (videoIn->framebuffer, videoIn->mem[videoIn->buf.index], (size_t) videoIn->buf.bytesused);
		start_encoding(videoIn->framebuffer);

		size_error = 0;

		memset( &tHeader, 0, sizeof(IMAGE_HEADER) );

		tHeader.size 				= g_packet.size;
		tHeader.channel 			= 0; // 1ch 전용
		tHeader.data_type 			= 2;  // 2:VIDEO    3:AUDIO    4:TEXT
		tHeader.picture_type 		= g_pict_type;

		csnum = 0;


		if( 1 ) // VIDEO
		{
			if ( tHeader.size > PARKING_SIZE )
			{
				printf("[WARNING] TOO BIG IMAGE! DISCARDED!\n");
				printf("tHeader.size ==>%d\n", tHeader.size);
				tHeader.size = PARKING_SIZE;
				size_error = 1;
			}

			video_count[tHeader.channel]++;
			tHeader.seq = video_count[tHeader.channel];

			if ( tHeader.picture_type == I_PICTURE )
			{
				I_count[tHeader.channel]++;
				tHeader.I_seq = I_count[tHeader.channel];
			}

			if ( gNewVar.Event.TYPE_info[EVENT_VLOSS].new_status & ( 0x01 << tHeader.channel ) )
			{
				printf("Video Loss !!!!!!\n");
				continue;
			}

			if ( size_error == 1 )
			{
				printf("TOO BIG !!!!!!\n");
				continue;
			}

			gNewVar.Data_Accumulated[tHeader.channel] += tHeader.size;

		}
		else // AUDIO
		{

printf("AUDIO is Not implemented yet! exit!!!!!!!!!!!\n");
exit(1);

			if ( tHeader.size > PARKING_SIZE ) // large_size[ csnum ].accumulated_size == 0
			{
				printf("[WARNING] TOO BIG AUDIO DATA! DISCARDED!\n");
				printf("tHeader.size ==>%d\n", tHeader.size);
				tHeader.size = PARKING_SIZE;
				size_error = 1;
			}

			audio_count[tHeader.channel]++;
			tHeader.seq = audio_count[tHeader.channel];

			if ( gNewVar.Event.TYPE_info[EVENT_VLOSS].new_status & ( 0x01 << tHeader.channel ) )
			{
				printf("Video Loss !!!!!!\n");
				continue;
			}

			if ( size_error == 1 )
			{
				printf("TOO BIG !!!!!!\n");
				continue;
			}
			
			gNewVar.Data_Accumulated[tHeader.channel + MAX_CH_NUM] += tHeader.size;
		}


		tHeader.normal_rec 		= gNewVar.Record.rec_onoff; //Normal Record
		tHeader.event_rec 			= 0;
		tHeader.version 			= 0x09022500; //Version
		tHeader.event_type 		= 0;
		tHeader.event_offset 		= 0;


		if ( gNewVar.Event.CH_info[tHeader.channel].record_count > 0 )
		{
			tHeader.event_rec = 1; // Event Record : Video + Audio
			if ( tHeader.data_type == 0x02 ) // Video Only
			{
				if ( tHeader.picture_type == 0x01 )
				{
					void * event_ptr;
					event_ptr = (void *) gNewVar.Event.CH_info[tHeader.channel].buf_ptr;
					if ( event_ptr != NULL )
					{
						gNewVar.Event.CH_info[tHeader.channel].buf_ptr = NULL;
						tHeader.event_offset = event_ptr;
					}
				}
				if ( gNewVar.Event.TYPE_info[EVENT_VLOSS].reset_count[tHeader.channel] > 0 )
					tHeader.event_type = tHeader.event_type | (0x01<<EVENT_VLOSS);

				if ( gNewVar.Event.TYPE_info[EVENT_MOTION].reset_count[tHeader.channel] > 0 )
					tHeader.event_type = tHeader.event_type | (0x01<<EVENT_MOTION);

				if ( gNewVar.Event.TYPE_info[EVENT_SENSOR].reset_count[tHeader.channel] > 0 )
					tHeader.event_type = tHeader.event_type | (0x01<<EVENT_SENSOR);

				tHeader.event_msg = gNewVar.Event.event_msg[tHeader.channel];
			}
		}




		total_size = sizeof(struct Image_buffer_header) + sizeof(IMAGE_HEADER) + 8 + tHeader.size + 4; // 2041 MUX 정보 8byte 포함 + decoding시 여유 용량 4byte

		tHeader.datetime 			= gNewVar.current_time;
		tHeader.timezone_offset 	= gNewVar.timezone.offset_sec;
		tHeader.isdst 				= gNewVar.isdst;
		tHeader.resolution 			= gNewVar.resolution[csnum];
		tHeader.NtscPal 			= gNewVar.NtscPal;
		tHeader.frame_rate 		= gNewVar.real_frame_rate[tHeader.channel];
		tHeader.watermark 		= 1;
		tHeader.crc_value 			= make_crc( (BYTE*)&tHeader, sizeof(IMAGE_HEADER) - 1);

		if ( total_size > PARKING_SIZE )
		{
			printf("[WARNING] Data Size is Too Big ? \n");
			continue;
		}




		pthread_mutex_lock(&mutex_current);
		pLink = (struct Image_buffer_header *)&preBuffer[front_ptr];
		if ( front_ptr <= rear_ptr )
		{
			if ( front_ptr == rear_ptr )
			{
				if ( pLink->valid == ( (((unsigned int)'A')<<24) | (((unsigned int)1)<<8) | ((unsigned int)'Z') ) )
				{
					pthread_mutex_unlock(&mutex_current);
					printf("________________________________________Buffer FULL 1!   R:%08d F:%08d\n", rear_ptr, front_ptr);
					usleep(200000);
					continue; // 주의 : return 아님
				}
			}
			else if ( front_ptr + total_size >= rear_ptr )
			{
				pthread_mutex_unlock(&mutex_current);
				printf("________________________________________Buffer FULL 2!   R:%08d F:%08d, %d\n", rear_ptr, front_ptr, (int)total_size);
				record_force_start();
				usleep(200000);
				continue; // 주의 : return 아님
			}		
		}


		pHeader = (IMAGE_HEADER *)(pLink + 1);
		memcpy( pHeader, &tHeader, sizeof(IMAGE_HEADER) );
		memcpy( (BYTE *)(pHeader + 1) + 8, (BYTE*)g_packet.data, tHeader.size);


		pLink->size = total_size;
		pLink->the_time = tHeader.datetime;
		front_ptr += total_size;
		if ( front_ptr + PARKING_SIZE >= PREBUFFER_SIZE )
			front_ptr = 0;


		pLink->next_offset = front_ptr;
		pLink->valid = (((unsigned int)'A')<<24) | (((unsigned int)1)<<8) | ((unsigned int)'Z');
		pLink = (struct Image_buffer_header *)&preBuffer[front_ptr];
		pLink->valid = (((unsigned int)'A')<<24) | ((unsigned int)'Z');
		pthread_mutex_unlock(&mutex_current);


		image_enque_buf();
temp_input_count++;
	}


//	MpegEncStop(); 

	if ( gNewVar.bStop == 0 )
	{
		goto again;
	}

	pthread_mutex_lock( &gNewVar.hdd_encode_lock );

#ifdef MEMORY_ALLOCATION
	free(preBuffer);
	printd("[ENC] MEMORY FREE SUCCESS !!!\n");
#endif

	pthread_exit(NULL);
	return 0;

}


void record_force_start(void)
{
	pthread_mutex_lock(&RecBuffer_mutex);
	pthread_cond_signal(&RecBuffer_cond);
	pthread_mutex_unlock(&RecBuffer_mutex);
}




