#ifndef _MPEGENC_H_
#define _MPEGENC_H_



#define BUF_FREE		0 
#define BUF_LOCK		1
#define BUF_SEND		2

extern sem_t	MpegEncWait;


typedef struct {				// 32 bytes
//	PACKET_HEADER packet_header;
//	BYTE		  channel;
	BYTE		  size;
	BYTE		  status;
//	BYTE		  i_status;
//	BYTE		  i_index;
	BYTE		  buf[1024 * 257];
}IMAGE_IBUF;


#define 	PREBUFFER_SIZE (24*1024*1024)
#define	MUX_BUF_SIZE	64*1024
#define 	PARKING_SIZE 	256 *1024 // unit: bytes

extern BYTE* preBuffer;



int InitMpegEnc(void);
void *MpegEncTask(void * arg);
void MpegEncStart(void);
void MpegEncStop(void);
#endif
