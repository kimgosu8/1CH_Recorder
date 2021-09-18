/* set tabstop=4 */
/********************************************************************************
 *                                                                              *
 * Copyright(C) 2002  Penta-Micro                                               *
 *                                                                              *
 * ALL RIGHT RESERVED                                                           *
 *                                                                              *
 * This software is the property of Penta-Micro and is furnished under          *
 * license by Penta-Micro. This software may be used only in accordance         *	
 * with the terms of said license. This copyright notice may not be             *
 * removed, modified or obliterated without the prior written permission        *
 * of Penta-Micro.                                                              *
 *                                                                              *
 * This software may not be copyed, transmitted, provided to or otherwise       *
 * made available to any other person, company, corporation	or other entity     *
 * except as specified in the terms of said license.                            *
 *                                                                              *
 * No right, title, ownership or other interest in the software is hereby       *
 * granted or transferred.                                                      *
 *                                                                              *
 * The information contained herein is subject to change without notice and     *
 * should not be construed as a commitment by Penta-Micro.                      *
 *                                                                              *
 ********************************************************************************
 
  MODULE NAME:  MDEC_API.H
  
  REVISION HISTORY:
  
  Date       Ver Name                  Description
  ---------- --- --------------------- -----------------------------------------
  06/24/2003 1.0 CheulBeck(whitefe)    Created
 ...............................................................................
 
  DESCRIPTION:
  
  This Module contains definitions for Pallas Mpeg Decoder API function.
  
 ...............................................................................
*/  
 
#ifndef _MPEG_CTRL_H_
#define _MPEG_CTRL_H_


/** ************************************************************************* ** 
 ** includes
 ** ************************************************************************* **/

/** ************************************************************************* ** 
 ** defines
 ** ************************************************************************* **/
#define trick_mode(a, b, c, d)		((a & 0x03) << 8) | ((b & 0x07) << 5) | ((c & 0x01) << 4) | (d & 0x0f)

//#define CHECK_PERFORM
#define HW_DEBUG
#define AUTO_DISPLAY_MODE

//#define m_DEBUG(format, args...)  printf(format, ## args) 
#define m_DEBUG(format, args...)  
//#define m_MSG(format, args...)  printf(format, ## args)
#define m_MSG(format, args...)
#define m_ERROR(format, args...)  printf(format, ## args) 
/** ************************************************************************* ** 
 ** typedefs
 ** ************************************************************************* **/
typedef struct __WATER_MARK__ {
	unsigned char	flag;
	unsigned char	strength;
	unsigned char	key[16];
} WATER_MARK;

typedef struct __GLOBAL_PARAM__ {
	unsigned int	video_form;
	unsigned int	resolution[MAX_AT2041_NUM];
	WATER_MARK		wm[MAX_AT2041_NUM];
} GLOBAL_PARAM;

#define MOTION_LINE_NUM		18
typedef struct __MOTION_PARAM__ {
	unsigned int	flag;
	unsigned int	sensitivity;
	unsigned int	area[MOTION_LINE_NUM];
} MOTION_PARAM;

typedef struct __ENCODER_PARAM__ {
	unsigned int	frame_rate;
	unsigned int	bit_rate;	
	unsigned int	vbr_q;	
	unsigned int	cbr_q;	
	unsigned int	gop_nm;
	unsigned int	gop_m;
	MOTION_PARAM	motion;
} ENCODER_PARAM;

typedef struct __DISPLAY_PARAM__ {
	unsigned int	DispOn;
	unsigned int	DispPriority;
	unsigned int	DispOffsetX;
	unsigned int	DispOffsetY;
	unsigned int	ScaleX;
	unsigned int	ScaleY;
	unsigned int	ZoomX;
	unsigned int	ZoomY;
	unsigned int ModeA;
	unsigned int ModeB;
} DISPLAY_PARAM;

typedef struct __DECODER_PARAM__ {
	unsigned int	de_interlace;
	unsigned int	AudioCh;
	DISPLAY_PARAM	dp[UNIT_CH_NUM];	
} DECODER_PARAM;

typedef struct __SETUP_PARAM__ {
	signed int		magic_num;	
	GLOBAL_PARAM	gp;
	ENCODER_PARAM	enc_ch[MAX_AT2041_NUM * UNIT_CH_NUM];
	DECODER_PARAM	dec[MAX_AT2041_NUM];
} SETUP_PARAM;

/*
#ifndef AT2041_DUP
typedef enum {
	NTSC = 0,
	PAL
} VIDEO_MODE;

typedef enum {
	FULL_D1,
	HALF_D1,
	CIF,
	VHALF_D1
} VIDEO_SIZE;

typedef enum {
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
#endif
*/

typedef enum {
	TM_PLAY = 0,
	TM_FAST,
	TM_SLOW,
	TM_STEP,
	TM_PAUSE,
	TM_NON_REAL_TIME
} TRICK_MODE;

typedef enum {
	TD_FORWARD = 0,
	TD_BACKWARD
} TRICK_DIR;

typedef enum {
	TME_NORMAL = 0,
	TME_REAL
} TRICK_METHOD;

/** ************************************************************************* ** 
 ** function prototypes
 ** ************************************************************************* **/
void EncParam(BYTE*,unsigned char,unsigned int);
void DecParam(unsigned char mode, unsigned char NtscPal);
void DecMode(unsigned char mode, unsigned char ch, BYTE* resolution);
void PlayMode(int mode);
void SetMotion(unsigned char ch, unsigned char flag, unsigned char sensitivity, unsigned char area, unsigned char resolution);
void motion_sensitivity(unsigned char nCs, unsigned char ch_id, unsigned char level);
void motion_area(unsigned char nCs, unsigned char ch_id, unsigned int *area);
char	At2041EncParamSet(SETUP_PARAM *psp, unsigned char nCs);
char	At2041DecParamSet(SETUP_PARAM *psp, unsigned char nCs);
char	change_decoding_display_mode(SETUP_PARAM *psp, unsigned char nCs);

#endif //_MPEG_CTRL_H_
/* end of xxx.h */
