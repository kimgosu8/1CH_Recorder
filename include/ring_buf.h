#ifndef _RINGBUF_H_
#define _RINGBUF_H_

struct tagRingbuf {
	BYTE* array;
	DWORD front;
	DWORD rear;
	DWORD length_mask;
	DWORD length; // 이건 필요없을지도...
	DWORD cell_size;
//DWORD tolerance;
};

void* RingBuf_Front(struct tagRingbuf *ptr);
void* RingBuf_Rear(struct tagRingbuf *ptr);
DWORD RingBuf_Count(struct tagRingbuf *ptr);
void RingBuf_Deposit( struct tagRingbuf *ptr);
void RingBuf_Withdraw( struct tagRingbuf *ptr);
int RingBuf_Init(struct tagRingbuf *ptr, int cell_size, int length);

extern struct tagRingbuf key_buffer;
extern struct tagRingbuf gNewEventBuf;
extern struct tagRingbuf gNewRecBuf;
extern struct tagRingbuf gNewNetLiveBuf;
#endif	/* _RINGBUF_H_ */
