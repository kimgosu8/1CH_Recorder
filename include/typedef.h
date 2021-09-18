#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_



#define INLINE  inline

#define FIX_VOC_0706



// Architecture specific
typedef int 					INT;
typedef unsigned int 			UINT;

typedef short int 				SHORT;
typedef unsigned short int 		USHORT;


typedef unsigned char 			UCHAR;
typedef signed char 			SBYTE;

typedef long 					LONG;
typedef unsigned long 			ULONG;


//try to be architecture independent
// for PPC 4xx, the following is true
typedef signed char 			INT8;
typedef unsigned char 			UINT8;

typedef short int 				INT16;
typedef unsigned short int 		UINT16;

typedef int 					INT32;
typedef unsigned int 			UINT32;

typedef long long int 			INT64;
typedef unsigned long long int 	UINT64;


// only gcc support these

typedef unsigned char 			BYTE;
typedef unsigned char			UNS8;	/* 8-bit */
typedef unsigned short 		WORD;
typedef unsigned short			UNS16;	/* 16-bit */
typedef unsigned int			UNS32;	/* 32-bit */
typedef unsigned long 			DWORD;


typedef struct {
	BYTE 	*pData;
	int 		MAX_NUM;
	int 		FRONT;
	int 		REAR;
} Q_BUF;


#define TRUE 		1
#define FALSE		0

#define ON		1
#define OFF		0

#endif /*_TYPEDEF_H_*/

