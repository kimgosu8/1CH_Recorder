/********************************************************************************
   INCLUDES
********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "avformat.h"
#include "avcodec.h"
#include "rational.h"
#include "swscale.h"
#include "opt.h"

#include "ffmpeg.h"
#include <alsa/asoundlib.h>

/* #if LIBAVCODEC_BUILD > 4680
 * FFmpeg after build 4680 doesn't have support for mpeg1 videos with non-standard framerates. Previous builds contained a broken hack
 * that padded with B frames to obtain the correct framerate.
    **************************************************
	 Your version of FFmpeg is newer than version 0.4.8
	 Newer versions of ffmpeg do not support MPEG1 with
	 non-standard framerate. MPEG1 will be disabled for
	 normal video output. You can still use mpeg4 and
	 and mpeg4ms which are both better in terms of size
	 and quality. MPEG1 is always used for timelapse.
	 Please read the Motion Guide for more information.
	 Note that this is not an error message!
     **************************************************
 */



/********************************************************************************
   DEFINES
********************************************************************************/

/* 5 seconds stream duration */
#define STREAM_DURATION   5.0
#define STREAM_FRAME_RATE 30 				/* 25 images/s */
#define STREAM_NB_FRAMES  ((int)(STREAM_DURATION * STREAM_FRAME_RATE))
#define STREAM_PIX_FMT PIX_FMT_YUV420P		/* default pix_fmt */
#define STREAM_GOP_SIZE 15

/* For Make a avifile */
#define AVI_FILE_MAKE


/********************************************************************************
   GLOBALS
********************************************************************************/

int g_temp_total_size;
enum CodecID g_codec_num=0;

AVOutputFormat *fmt;
AVFormatContext *pFormatCtx;
AVStream *audio_st, *video_st;
double audio_pts, video_pts;

// uint8_t *temp_y, *temp_u, *temp_v; // calloc
// uint8_t *yuv420_buf; // calloc

static int sws_flags = SWS_BICUBIC;

/* video output */
AVFrame *picture, *tmp_picture;
uint8_t *video_outbuf;
int video_outbuf_size;


/* audio output */
float t, tincr, tincr2;
int16_t *samples;
uint8_t *audio_outbuf;
int samples_size;
int audio_outbuf_size;
int audio_input_frame_size;

AVPacket g_packet;
unsigned char g_pict_type=0;



/********************************************************************************
   LOCALS
********************************************************************************/

extern int g_width;
extern int g_height;

extern char codec_name[20];


extern snd_pcm_t *handle;
extern snd_pcm_hw_params_t *params;
extern snd_pcm_uframes_t frames;





/********************************************************************************
   Function Prototype Declare
********************************************************************************/

void alsa_sound_init();
void yuy2toyv12(unsigned char * _y, unsigned char * _u, unsigned char * _v, unsigned char * input, int width, int height);








/********************************************************************************
   Function define
********************************************************************************/






/*************************
 * add an audio output stream *
 *************************/
 
AVStream *add_audio_stream(AVFormatContext *pFormatCtx, int codec_id)
{
	AVCodecContext *pCodecCtx;
	AVStream *audio_st;

//	alsa_sound_init();
//	test_sound();

	audio_st = av_new_stream(pFormatCtx, 1);
	if (!audio_st) 
	{
		fprintf(stderr, "Could not alloc stream\n");
		exit(1);
	}

	pCodecCtx = audio_st->codec;
//	pCodecCtx->codec_id = codec_id;
	pCodecCtx->codec_id = CODEC_ID_PCM_S16LE ;
//	pCodecCtx->codec_id = CODEC_ID_PCM_S32LE ;

	pCodecCtx->codec_type = CODEC_TYPE_AUDIO;

	/* put sample parameters */
	pCodecCtx->bit_rate = 64000;
//	pCodecCtx->bit_rate = 32000;
//	pCodecCtx->bit_rate = 7056000;	
	pCodecCtx->sample_rate = 44100;
	pCodecCtx->channels = 2;

// printf("pCodecCtx->frame_size ==>%d\n", pCodecCtx->frame_size);

//	pCodecCtx->frame_size = frames;





	return audio_st;
}


void open_audio(AVFormatContext *pFormatCtx, AVStream *audio_st)
{
	AVCodecContext *pCodecCtx;
	AVCodec *codec;

	pCodecCtx = audio_st->codec;

	/* find the audio encoder */
	codec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!codec)
	{
		fprintf(stderr, "codec not found\n");
		exit(1);
	}

	/* open it */
	if (avcodec_open(pCodecCtx, codec) < 0)
	{
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	/* init signal generator */
	t = 0;
	tincr = 2 * M_PI * 110.0 / pCodecCtx->sample_rate;
	/* increment frequency by 110 Hz per second */
	tincr2 = 2 * M_PI * 110.0 / pCodecCtx->sample_rate / pCodecCtx->sample_rate;


// 	pCodecCtx->frame_size = frames;

	/* the codec gives us the frame size, in samples */
//	samples_size = pCodecCtx->frame_size * 2 * pCodecCtx->channels;
//	samples = (short *)malloc(samples_size);

//	samples_size = pCodecCtx->frame_size * 4;
	samples_size = 1470*4;
	samples = (short *)malloc(samples_size);

/*
	audio_outbuf_size = samples_size * 2;
	audio_outbuf = (unsigned char *)malloc(audio_outbuf_size);
*/

	audio_outbuf_size = samples_size*8;
	audio_outbuf = av_malloc(audio_outbuf_size);


	/* ugly hack for PCM codecs (will be removed ASAP with new PCM support to compute the input frame size in samples */
	if (pCodecCtx->frame_size <= 1)
	{
		printf("ugly hack for PCM codecs ~~~~~~~~~~~~~~~~~~~~~~~~\n");
		audio_input_frame_size = audio_outbuf_size / pCodecCtx->channels;

		switch(audio_st->codec->codec_id)
		{
			case CODEC_ID_PCM_S16LE:
			case CODEC_ID_PCM_S16BE:
			case CODEC_ID_PCM_U16LE:
			case CODEC_ID_PCM_U16BE:
				audio_input_frame_size >>= 1;
				break;
			default:
				printf("default PCM!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				break;
		}
	}
	else
	{
		audio_input_frame_size = pCodecCtx->frame_size;
	}

//	samples = av_malloc(audio_input_frame_size * 2 * pCodecCtx->channels);




printf("\n============================\n");
printf("pCodecCtx->codec_id ==>0x%x\n", pCodecCtx->codec_id); // CODEC_ID_MP2
printf("pCodecCtx->sample_rate ==>%d\n", pCodecCtx->sample_rate);
printf("pCodecCtx->frame_size ==>%d\n", pCodecCtx->frame_size);
printf("pCodecCtx->channels ==>%d\n\n", pCodecCtx->channels);

}


/* prepare a 16 bit dummy audio frame of 'frame_size' samples and 'nb_channels' channels */
void get_audio_frame(int16_t *samples, int frame_size, int nb_channels)
{
	int j, i, v;
	int16_t *q;

	q = samples;

	for(j=0;j<frame_size;j++)
	{
		v = (int)(sin(t) * 10000);
		for(i = 0; i < nb_channels; i++)
			*q++ = v;

		t += tincr;
		tincr += tincr2;
	}
}

int temp_packet_count=0;

void write_audio_frame(AVFormatContext *pFormatCtx, AVStream *audio_st)
{
	AVCodecContext *pCodecCtx;
//	AVPacket packet;

// printf(" [Audio Frame]\n");

	av_init_packet(&g_packet);
	int ret=0;
	pCodecCtx = audio_st->codec;

	/* capture */
	ret = snd_pcm_readi(handle, samples, frames); // (char *)samples
//  printf("ret [%d] ", ret);

	if (ret == -EPIPE)
	{
		printf("overrun occurred\n");
		// EPIPE means overrun 
		fprintf(stderr, "overrun occurred\n");
		snd_pcm_prepare(handle);
	}
	else if (ret < 0)
	{
		printf("error from read\n");
		fprintf(stderr, "error from read: %s\n", snd_strerror(ret));
	}
	else if (ret != (int)frames)
	{
		printf("short read, read %d frames\n", ret);
		fprintf(stderr, "short read, read %d frames\n", ret);
	}

//	get_audio_frame(samples, audio_input_frame_size, pCodecCtx->channels);


	g_packet.size= avcodec_encode_audio(pCodecCtx, audio_outbuf, samples_size, (short *)samples);
pCodecCtx->coded_frame->pts = temp_packet_count;
pCodecCtx->time_base.num = 1;
pCodecCtx->time_base.den = 30;
	g_packet.pts= av_rescale_q(pCodecCtx->coded_frame->pts, pCodecCtx->time_base, audio_st->time_base);
temp_packet_count++;


//printf("\tpCodecCtx->coded_frame->pts ==>%d\n", pCodecCtx->coded_frame->pts);
//printf("\tpCodecCtx->time_base.num ==>%d\n", pCodecCtx->time_base.num);
//printf("\tpCodecCtx->time_base.den ==>%d\n", pCodecCtx->time_base.den);
	g_packet.flags |= PKT_FLAG_KEY;
	g_packet.stream_index= audio_st->index;
	g_packet.data= audio_outbuf;

#ifdef AVI_FILE_MAKE
	/* write the compressed frame in the media file */
	if (av_write_frame(pFormatCtx, &g_packet) != 0)
	{
		fprintf(stderr, "Error while writing audio frame\n");
		exit(1);
	}
#endif

}


void close_audio(AVFormatContext *pFormatCtx, AVStream *audio_st)
{
	avcodec_close(audio_st->codec);

	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	av_free(samples);
	av_free(audio_outbuf);
}











/*************************
 * add a video output stream *
 *************************/

AVStream *add_video_stream(AVFormatContext *pFormatCtx, int codec_id, int width, int height)
{
	AVCodecContext *pCodecCtx;
	AVStream *video_st;

	video_st = av_new_stream(pFormatCtx, 0);
	if (!video_st)
	{
		fprintf(stderr, "Could not alloc stream\n");
		exit(1);
	}

	pCodecCtx = video_st->codec;
	pCodecCtx->codec_id = codec_id; // codec_id : 13
	pCodecCtx->codec_type = CODEC_TYPE_VIDEO;

	/* put sample parameters */
	pCodecCtx->bit_rate = 800000; // 400000
	/* resolution must be a multiple of two */
	pCodecCtx->width = width;
	pCodecCtx->height = height;
	/* time base: this is the fundamental unit of time (in seconds) in terms of which frame timestamps are represented. 
	    for fixed-fps content, timebase should be 1/framerate and timestamp increments should be identically 1. */
	pCodecCtx->time_base.den = STREAM_FRAME_RATE;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->gop_size = STREAM_GOP_SIZE; /* emit one intra frame every twelve frames at most */
	pCodecCtx->pix_fmt = STREAM_PIX_FMT;
	if (pCodecCtx->codec_id == CODEC_ID_MPEG2VIDEO)
		pCodecCtx->max_b_frames = 2;/* just for testing, we also add B frames */
	if (pCodecCtx->codec_id == CODEC_ID_MPEG1VIDEO)
	{
		/* Needed to avoid using macroblocks in which some coeffs overflow.
		This does not happen with normal video, it just happens here as the motion of the chroma plane does not match the luma plane. */
		pCodecCtx->mb_decision=2;
	}

	// some formats want stream headers to be separate
	if(!strcmp(pFormatCtx->oformat->name, "mp4") || !strcmp(pFormatCtx->oformat->name, "mov") || !strcmp(pFormatCtx->oformat->name, "3gp"))
		pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return video_st;
}

static AVFrame *alloc_picture(int pix_fmt, int width, int height)
{
	AVFrame *picture;
	uint8_t *picture_buf;
	int size;

	picture = avcodec_alloc_frame();
	if (!picture)
		return NULL;

	size = avpicture_get_size(pix_fmt, width, height);
	picture_buf = av_malloc(size);

	if (!picture_buf)
	{
		av_free(picture);
		return NULL;
	}

	avpicture_fill((AVPicture *)picture, picture_buf, pix_fmt, width, height);

	return picture;
}

void open_video(AVFormatContext *pFormatCtx, AVStream *video_st)
{
	AVCodec *codec;
	AVCodecContext *pCodecCtx;

	pCodecCtx = video_st->codec;

	/* find the video encoder */
	codec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!codec)
	{
		fprintf(stderr, "codec not found\n");
		exit(1);
	}

	/* open the codec */
	if (avcodec_open(pCodecCtx, codec) < 0)
	{
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}


	/* allocate output buffer */
	/* XXX: API change will be done */
	/* buffers passed into lav* can be allocated any way you prefer, as long as they're aligned enough for the architecture,  
	    and they're freed appropriately (such as using av_free for buffers allocated with av_malloc) */
	video_outbuf = NULL;
	if (!(pFormatCtx->oformat->flags & AVFMT_RAWPICTURE))
	{
		video_outbuf_size = 1024*100; // 한장당 17kbyte 정도가 MPEG4의 기본
		video_outbuf = av_malloc(video_outbuf_size);
	}

	/* allocate the encoded raw picture */
	picture = alloc_picture(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height); // PIX_FMT_YUV420P
	if (!picture)
	{
		fprintf(stderr, "Could not allocate picture\n");
		exit(1);
	}

	/* if the output format is not YUV420P, then a temporary YUV420P picture is needed too. 
	     It is then converted to the required output format */
	tmp_picture = alloc_picture(PIX_FMT_YUYV422, pCodecCtx->width, pCodecCtx->height);
/*
	tmp_picture = NULL;
	if (pCodecCtx->pix_fmt != PIX_FMT_YUV420P)
	{
		tmp_picture = alloc_picture(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
		if (!tmp_picture)
		{
			fprintf(stderr, "Could not allocate temporary picture\n");
			exit(1);
		}
	}

	// converted buf for YUV 420
	yuv420_buf = calloc(pCodecCtx->width * pCodecCtx->height * 3 /2, 1);
	temp_y = calloc(pCodecCtx->width * pCodecCtx->height, 1);
	temp_u = calloc(pCodecCtx->width * pCodecCtx->height * 1 /4, 1);
	temp_v = calloc(pCodecCtx->width * pCodecCtx->height * 1 /4, 1);
*/
}

/* prepare a dummy image */
void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
{
    int x, y, i;

    i = frame_index;

    /* Y */
    for(y=0;y<height;y++) {
        for(x=0;x<width;x++) {
            pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
        }
    }

    /* Cb and Cr */
    for(y=0;y<height/2;y++) {
        for(x=0;x<width/2;x++) {
            pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
            pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
        }
    }
}



void write_video_frame(AVFormatContext *pFormatCtx, AVStream *video_st,  unsigned char const *in_addr)
{
	int out_size, ret=0;
	AVCodecContext *pCodecCtx;
	static struct SwsContext *img_convert_ctx;

	pCodecCtx = video_st->codec;

//	fflush(stdout);


/* 1차 성공 (약 1700 us) 인코딩 avcodec_encode_video() speed도 올라감. */
	avpicture_fill((AVPicture *)tmp_picture, in_addr, PIX_FMT_YUYV422, pCodecCtx->width, pCodecCtx->height);
	img_convert((AVPicture *)picture, PIX_FMT_YUV420P, (AVPicture *)tmp_picture, PIX_FMT_YUYV422, pCodecCtx->width, pCodecCtx->height);


/*
	yuy2toyv12( temp_y,  temp_u,  temp_v, in_addr, pCodecCtx->width, pCodecCtx->height);

	memcpy(yuv420_buf, 										temp_y, pCodecCtx->width*pCodecCtx->height);
	memcpy(yuv420_buf+pCodecCtx->width*pCodecCtx->height, 		temp_u, pCodecCtx->width*pCodecCtx->height*1/4);
	memcpy(yuv420_buf+pCodecCtx->width*pCodecCtx->height*5/4, 	temp_v, pCodecCtx->width*pCodecCtx->height*1/4);


	if (pCodecCtx->pix_fmt != PIX_FMT_YUV420P)
	{
		printf("pCodecCtx->pix_fmt != PIX_FMT_YUV420P\n");
		// as we only generate a YUV420P picture, we must convert it to the codec pixel format if needed
		if (img_convert_ctx == NULL)
		{
			img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P,pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,sws_flags, NULL, NULL, NULL);
			if (img_convert_ctx == NULL)
			{
				fprintf(stderr, "Cannot initialize the conversion context\n");
				exit(1);
			}
		}

//		fill_yuv_image(tmp_picture, 1, pCodecCtx->width, pCodecCtx->height);
		sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize, 0, pCodecCtx->height, picture->data, picture->linesize);
	}
	else
	{
		// picture 에서 free 하면 yuv420_buf 도 같이 해제하여 주는 듯 함. 
		avpicture_fill((AVPicture *)picture, yuv420_buf, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	}
*/


	if (pFormatCtx->oformat->flags & AVFMT_RAWPICTURE)
	{
		/* raw video case. The API will change slightly in the near futur for that */
//		AVPacket packet;
		av_init_packet(&g_packet);

		g_packet.flags |= PKT_FLAG_KEY;
		g_packet.stream_index= video_st->index;
		g_packet.data= (uint8_t *)picture;
		g_packet.size= sizeof(AVPicture);
#ifdef AVI_FILE_MAKE
		ret = av_write_frame(pFormatCtx, &g_packet);
#endif
	}
	else
	{
// start_point();
		/* encode the image */
		out_size = avcodec_encode_video(pCodecCtx, video_outbuf, video_outbuf_size, picture);
// end_point();
//  printf("out_size ==>%d\n", out_size);

		g_pict_type = pCodecCtx->coded_frame->pict_type;


		/* if zero size, it means the image was buffered */
		if (out_size > 0)
		{
//			AVPacket g_packet;
			av_init_packet(&g_packet);

			g_packet.pts= av_rescale_q(pCodecCtx->coded_frame->pts, pCodecCtx->time_base, video_st->time_base);
			if(pCodecCtx->coded_frame->key_frame) // GOP 단위 마다 1 설정됨.
				g_packet.flags |= PKT_FLAG_KEY;
			g_packet.stream_index= video_st->index;
			g_packet.data= video_outbuf;
			g_packet.size= out_size;

#ifdef AVI_FILE_MAKE
			/* write the compressed frame in the media file */
			ret = av_write_frame(pFormatCtx, &g_packet);
#endif

// printf("\tpCodecCtx->coded_frame->pts ==>%d\n", pCodecCtx->coded_frame->pts);
// printf("\tpCodecCtx->time_base.num ==>%d\n", pCodecCtx->time_base.num);
// printf("\tpCodecCtx->time_base.den ==>%d\n", pCodecCtx->time_base.den);
// printf("g_packet.flags : %d\n", g_packet.flags);
// printf("g_packet.size : %d\n", g_packet.size);
// printf("g_packet.stream_index : %d\n", g_packet.stream_index);
// printf("\n");

		} 
		else
		{
			printf("encoding size is 0 !!!!\n");
			ret = 0;
		}
	}

#ifdef AVI_FILE_MAKE
	if (ret != 0)
	{
		fprintf(stderr, "Error while writing video frame\n");
		exit(1);
	}
#endif

}

void close_video(AVFormatContext *pFormatCtx, AVStream *video_st)
{
	avcodec_close(video_st->codec);
	av_free(picture->data[0]);
	av_free(picture);

	if (tmp_picture)
	{
		av_free(tmp_picture->data[0]);
		av_free(tmp_picture);
	}
	av_free(video_outbuf);

/*
// picture 에서 free 하면 yuv420_buf 도 같이 해제하여 주는 듯 함.  그래서 free 해주면 안됨. 
//	free(yuv420_buf);
	free(temp_y);
	free(temp_u);
	free(temp_v);
*/
}





void init_encoding(int width, int height)
{
	time_t now;
	char outputfile[100];

	memset(outputfile, 0, 100);
	sprintf (outputfile, "%d.avi", (int)time(&now));



	/* initialize libavcodec, and register all codecs and formats */
	av_register_all();

	/* auto detect the output format from the name. default is mpeg. */
	fmt = guess_format(NULL, outputfile, NULL); // H.264를 하려면 포멧부터 check 해봐야 할듯...... outputfile 이름가지고 안됨..... Googling 해보자. 이 함수... 
	if (!fmt)
	{
		printf("Could not deduce output format from file extension: using MPEG.\n");
		fmt = guess_format("mpeg", NULL, NULL);
	}
	if (!fmt)
	{
		fprintf(stderr, "Could not find suitable output format\n");
		exit(1);
	}

	/* allocate the output media context */
	pFormatCtx = av_alloc_format_context();
	if (!pFormatCtx)
	{
		fprintf(stderr, "Memory error\n");
		exit(1);
	}
	pFormatCtx->oformat = fmt;
	snprintf(pFormatCtx->filename, sizeof(pFormatCtx->filename), "%s", outputfile);




  	/* add the audio and video streams using the default format codecs and initialize the codecs */
	video_st = NULL;
	audio_st = NULL;
	if (fmt->video_codec != CODEC_ID_NONE)
	{
		video_st = add_video_stream(pFormatCtx, fmt->video_codec, width, height);
	}
	if (fmt->audio_codec != CODEC_ID_NONE)
	{
		audio_st = add_audio_stream(pFormatCtx, fmt->audio_codec);
	}

	/* set the output parameters (must be done even if no parameters). */
	if (av_set_parameters(pFormatCtx, NULL) < 0)
	{
		fprintf(stderr, "Invalid output format parameters\n");
		exit(1);
	}

//	dump_format(pFormatCtx, 0, outputfile, 1);

	/* now that all the parameters are set, we can open the audio and
	video codecs and allocate the necessary encode buffers */
	if (video_st)
		open_video(pFormatCtx, video_st);
//	if (audio_st)
//		open_audio(pFormatCtx, audio_st);


	// ================================
	// FOR DEBUG
	// ================================



#ifdef AVI_FILE_MAKE
	// open the output file, if needed
	if (!(fmt->flags & AVFMT_NOFILE)) 
	{
		if (url_fopen(&pFormatCtx->pb, outputfile, URL_WRONLY) < 0)
		{
			fprintf(stderr, "Could not open '%s'\n", outputfile);
			exit(1);
		}
	}

	/* write the stream header, if any */
	av_write_header(pFormatCtx);
#endif

}



void start_encoding(unsigned char *in_addr)
{
/*
	// compute current audio and video time
	if (audio_st)
		audio_pts = (double)audio_st->pts.val * audio_st->time_base.num / audio_st->time_base.den;
	else
		audio_pts = 0.0;

	if (video_st)
		video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
	else
		video_pts = 0.0;
*/


// printf("audio_pts ==>%lf\n", audio_pts);
// printf("video_pts ==>%lf\n", video_pts);

	write_video_frame(pFormatCtx, video_st, in_addr);
//	write_audio_frame(pFormatCtx, audio_st);




/*
	if (!video_st || (video_st && audio_st && audio_pts < video_pts)) 
	{
printf("write audio!!!!!!!!!!!\n");
		write_audio_frame(pFormatCtx, audio_st);
	} 
	else
	{
printf("write video!!!!!!!!!!!\n");
		write_video_frame(pFormatCtx, video_st, in_addr);
	}
*/

}




void stop_encoding()
{
//	int ret=0;

//	ret = fclose(video_fd);
//	printf("ret ==>%d\n", ret);
}



void exit_encoding()
{
	int i=0;

	printf("\n stop encoding!\n");
	printf("pFormatCtx->nb_streams ==>%d\n", pFormatCtx->nb_streams);


	/* close each codec */
	if (video_st)
		close_video(pFormatCtx, video_st);
//	if (audio_st)
//		close_audio(pFormatCtx, audio_st);

#ifdef AVI_FILE_MAKE
	/* write the trailer, if any */
	av_write_trailer(pFormatCtx);
#endif

	/* free the streams */
	for(i = 0; i < pFormatCtx->nb_streams; i++)
	{
		av_freep(&pFormatCtx->streams[i]->codec);
		av_freep(&pFormatCtx->streams[i]);
	}

#ifdef AVI_FILE_MAKE
	if (!(fmt->flags & AVFMT_NOFILE))
	{
		/* close the output file */
		url_fclose(pFormatCtx->pb);
	}
#endif

	/* free the stream */
	av_free(pFormatCtx);


	printf("TEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	exit(0);
}


 





















void yuy2toyv12(unsigned char * _y, unsigned char * _u, unsigned char * _v, unsigned char * input, int width, int height) 
{ 
	int i, j, w2; 
	unsigned char *y, *u, *v; 

	w2 = width / 2; 

	//I420 
	y = _y; 
	v = _v; 
	u = _u; 

	for (i = 0; i < height; i += 4)
	{ 
		/* top field scanline */ 
		for (j = 0; j < w2; j++)
		{ 
			/* packed YUV 422 is: Y[i] U[i] Y[i+1] V[i] */ 
			*(y++) = *(input++);
			*(u++) = *(input++); 
			*(y++) = *(input++); 
			*(v++) = *(input++); 
		} 
		for (j = 0; j < w2; j++) 
		{
			*(y++) = *(input++); 
			*(u++) = *(input++); 
			*(y++) = *(input++); 
			*(v++) = *(input++); 
		}

		/* next two scanlines, one frome each field , interleaved */ 
		for (j = 0; j < w2; j++) 
		{ 
			/* skip every second line for U and V */ 
			*(y++) = *(input++); 
			input++; 
			*(y++) = *(input++); 
			input++; 
		} 

		/* bottom field scanline*/ 
		for (j = 0; j < w2; j++)
		{ 
			/* skip every second line for U and V */ 
			*(y++) = *(input++); 
			input++; 
			*(y++) = *(input++); 
			input++; 
		} 

	} 
 } 







