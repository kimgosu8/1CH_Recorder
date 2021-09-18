
/*******************************************************************************
#             uvccapture: USB UVC Video Class Snapshot Software                #
#This package work with the Logitech UVC based webcams with the mjpeg feature  #
#.                                                                             #
# 	Orginally Copyright (C) 2005 2006 Laurent Pinchart &&  Michel Xhaard   #
#       Modifications Copyright (C) 2006  Gabriel A. Devenyi                   #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; either version 2 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/videodev.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "v4l2uvc.h"

int debug = 0;


static int init_v4l2 (struct vdIn *vd);

int init_videoIn (struct vdIn *vd, char *device, int width, int height, int format, int grabmethod)
{

	if (vd == NULL || device == NULL)
		return -1;
	if (width == 0 || height == 0)
		return -1;
	if (grabmethod < 0 || grabmethod > 1)
		grabmethod = 1;		//mmap by default;

	vd->videodevice = NULL;
	vd->status = NULL;
	vd->pictName = NULL;
	vd->videodevice = (char *)calloc(1, 16 * sizeof (char));
	vd->status = (char *)calloc(1, 100 * sizeof (char));
	vd->pictName = (char *)calloc(1, 80 * sizeof (char));

	snprintf (vd->videodevice, 12, "%s", device);
	vd->toggleAvi = 0;
	vd->getPict = 0;
	vd->signalquit = 1;
	vd->width = width;
	vd->height = height;
	vd->formatIn = format;
	vd->grabmethod = grabmethod;

	if (init_v4l2 (vd) < 0) 
	{
		fprintf (stderr, " Init v4L2 failed !! exit fatal \n");
		goto error;;
	}


	/* alloc a temp buffer to reconstruct the pict */
	vd->framesizeIn = (vd->width * vd->height << 1);
	switch (vd->formatIn)
	{
/*
		case V4L2_PIX_FMT_MJPEG:
			vd->tmpbuffer = (unsigned char *)calloc(1, (size_t) vd->framesizeIn);
			if (!vd->tmpbuffer)
				goto error;
			vd->framebuffer = (unsigned char *)calloc(1, (size_t) vd->width * (vd->height + 8) * 2);
			break;
*/
			case V4L2_PIX_FMT_YUYV:
//			case V4L2_PIX_FMT_YUV420:

			vd->framebuffer = (unsigned char *)calloc(1, (size_t) vd->framesizeIn);
			break;

		default:
			fprintf (stderr, " should never arrive exit fatal !!\n");
			goto error;
			break;
	}

	if (!vd->framebuffer)
		goto error;

	return 0;

	error:
	free (vd->videodevice);
	free (vd->status);
	free (vd->pictName);
	close (vd->fd);

	return -1;
}


static int init_v4l2 (struct vdIn *vd)
{
	int i;
	int ret = 0;


// printf("vd->videodevice ==>%s\n", vd->videodevice);

	/*
	Two methods exist to suspend execution of the application until one or more buffers can be dequeued. 
	By default VIDIOC_DQBUF blocks when no buffer is in the outgoing queue. 
	When the O_NONBLOCK flag was given to the open() function, 
	VIDIOC_DQBUF returns immediately with an EAGAIN error code when no buffer is available. 
	The select() or poll() function are always available.
	*/
	if ((vd->fd = open (vd->videodevice, O_RDWR)) == -1) 
	{
		perror ("ERROR opening V4L interface \n");
		exit (1);
	}
	memset (&vd->cap, 0, sizeof(struct v4l2_capability));

	ret = ioctl (vd->fd, VIDIOC_QUERYCAP, &vd->cap);
	if (ret < 0)
	{
		fprintf (stderr, "Error opening device %s: unable to query device.\n", vd->videodevice);
		goto fatal;
	}

	if ((vd->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0) 
	{
		fprintf (stderr, "Error opening device %s: video capture not supported.\n", vd->videodevice);
		goto fatal;;
	}

	if (vd->grabmethod)
	{
		if (!(vd->cap.capabilities & V4L2_CAP_STREAMING)) 
		{
			fprintf (stderr, "%s does not support streaming i/o\n", vd->videodevice);
			goto fatal;
		}
	}
	else
	{
		if (!(vd->cap.capabilities & V4L2_CAP_READWRITE))
		{
			fprintf (stderr, "%s does not support read i/o\n", vd->videodevice);
			goto fatal;
		}
	}

	/* set format in */
	memset (&vd->fmt, 0, sizeof (struct v4l2_format));
	vd->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vd->fmt.fmt.pix.width = vd->width;
	vd->fmt.fmt.pix.height = vd->height;
	vd->fmt.fmt.pix.pixelformat = vd->formatIn;
	vd->fmt.fmt.pix.field = V4L2_FIELD_ANY;

/*
printf("vd->fmt.fmt.pix.width ==>%d\n", vd->fmt.fmt.pix.width);
printf("vd->fmt.fmt.pix.height ==>%d\n", vd->fmt.fmt.pix.height);
printf("vd->fmt.fmt.pix.pixelformat ==>%d\n", vd->fmt.fmt.pix.pixelformat);
printf("vd->fmt.fmt.pix.bytesperline ==>%d\n", vd->fmt.fmt.pix.bytesperline);
printf("vd->fmt.fmt.pix.field ==>%d\n", vd->fmt.fmt.pix.field);
*/

	ret = ioctl (vd->fd, VIDIOC_S_FMT, &vd->fmt);
	if (ret < 0) 
	{
		fprintf (stderr, "Unable to set format: %d.\n", errno);
		goto fatal;
	}

	if ((vd->fmt.fmt.pix.width != vd->width) ||(vd->fmt.fmt.pix.height != vd->height)) 
	{
		fprintf (stderr, " format asked unavailable get width %d height %d \n", vd->fmt.fmt.pix.width, vd->fmt.fmt.pix.height);
		vd->width = vd->fmt.fmt.pix.width;
		vd->height = vd->fmt.fmt.pix.height;

		/* look the format is not part of the deal ??? */
		//vd->formatIn = vd->fmt.fmt.pix.pixelformat;
	}



	/* request buffers */
	memset (&vd->rb, 0, sizeof(struct v4l2_requestbuffers));
	vd->rb.count = NB_BUFFER;
	vd->rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vd->rb.memory = V4L2_MEMORY_MMAP;
//	vd->buf.flags = V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_TIMECODE;

	ret = ioctl (vd->fd, VIDIOC_REQBUFS, &vd->rb);
	if (ret < 0) 
	{
		fprintf (stderr, "Unable to allocate buffers: %d.\n", errno);
		goto fatal;
	}



	/* map the buffers */
	for (i = 0; i < NB_BUFFER; i++)
	{
		memset (&vd->buf, 0, sizeof(struct v4l2_buffer));
		vd->buf.index = i;
		vd->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vd->buf.memory = V4L2_MEMORY_MMAP;
//		vd->buf.flags = V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_TIMECODE;

		ret = ioctl (vd->fd, VIDIOC_QUERYBUF, &vd->buf);   
		if (ret < 0) 
		{
			fprintf (stderr, "Unable to query buffer (%d).\n", errno);
			goto fatal;
		}

		if (debug)
			fprintf (stderr, "length: %u offset: %u\n", vd->buf.length, vd->buf.m.offset);

		vd->mem[i] = mmap (0, vd->buf.length, PROT_READ, MAP_SHARED, vd->fd, vd->buf.m.offset);
						     /* start anywhere */ 

		if (vd->mem[i] == MAP_FAILED) 
		{
			fprintf (stderr, "Unable to map buffer (%d)\n", errno);
			goto fatal;
		}

		if (debug)
			fprintf (stderr, "Buffer mapped at address %p.\n", vd->mem[i]);
	}



	/* Queue the buffers. */
	for (i = 0; i < NB_BUFFER; ++i) 
	{
		memset (&vd->buf, 0, sizeof (struct v4l2_buffer));
		vd->buf.index = i;
		vd->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vd->buf.memory = V4L2_MEMORY_MMAP;
//		vd->buf.flags = V4L2_BUF_FLAG_MAPPED | V4L2_BUF_FLAG_TIMECODE;


		ret = ioctl (vd->fd, VIDIOC_QBUF, &vd->buf);
		if (ret < 0) 
		{
			fprintf (stderr, "Unable to queue buffer (%d).\n", errno);
			goto fatal;;
		}
	}

	return 0;

	fatal:
		return -1;

}


int video_enable (struct vdIn *vd)
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int ret;

	ret = ioctl (vd->fd, VIDIOC_STREAMON, &type);
	if (ret < 0) 
	{
		fprintf (stderr, "Unable to %s capture: %d.\n", "start", errno);
		return ret;
	}
	vd->isstreaming = 1;
	return 0;
}

int video_disable (struct vdIn *vd)
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int ret;

	ret = ioctl (vd->fd, VIDIOC_STREAMOFF, &type);
	if (ret < 0) 
	{
		fprintf (stderr, "Unable to %s capture: %d.\n", "stop", errno);
		return ret;
	}
	vd->isstreaming = 0;
	return 0;
}


void control_framerate(int fd, int width, int height)
{
	struct v4l2_streamparm parm;
	struct v4l2_fract frameperiod;
	int frame_rate = 30; // 25로 하면 고정적으로 나온다.


	/* Query the (input) frame rate */
	memset(&parm, 0, sizeof(parm));
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_G_PARM, &parm) < 0)
	{
		fprintf(stderr, "Unable to query the new frame rate\n");
		exit(1);
	}
// printf("\n");
// printf("before Capturing video at %dx%d, %.2f FPS\n", width, height, (double)frameperiod.denominator / (double)frameperiod.numerator);



	/* Multiple the frame period by the drop rate and set it */
	frameperiod = parm.parm.capture.timeperframe;
	frameperiod.denominator = frame_rate;
	memset(&parm, 0, sizeof(parm));
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parm.parm.capture.timeperframe = frameperiod;
	if (ioctl(fd, VIDIOC_S_PARM, &parm) < 0)
	{
		fprintf(stderr, "Unable to set a new frame rate\n");
		exit(1);
	}

// printf("after Capturing video at %dx%d, %.2f FPS\n", width, height, (double)frameperiod.denominator / (double)frameperiod.numerator);

/*
	if (format == FMT_MPEG1 || format == FMT_MPEG2)
		fprintf(stderr, "Warning: non-standard frame rates with MPEG1 and MPEG2 may not be compatible\n"
		                    " with some AVI players!\n");
*/



	/* Get the frame period */
	memset(&parm, 0, sizeof(parm));
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_G_PARM, &parm) < 0)
	{
		fprintf(stderr, "Unable to query the frame rate\n");
		exit(1);
	}
	frameperiod = parm.parm.capture.timeperframe;

	fprintf(stderr, "\nCapturing video at %dx%d, %.2f FPS\n", width, height, (double)frameperiod.denominator / (double)frameperiod.numerator);


}


int close_v4l2 (struct vdIn *vd)
{
	int i;

	if (vd->isstreaming)
		video_disable (vd);

	// If the memory maps are not released the device will remain opened even after a call to close();
	for (i = 0; i < NB_BUFFER; i++) 
		munmap (vd->mem[i], vd->buf.length);

/*
	if (vd->tmpbuffer)
		free (vd->tmpbuffer);
	vd->tmpbuffer = NULL;
*/
	free (vd->framebuffer);
	vd->framebuffer = NULL;
	free (vd->videodevice);
	free (vd->status);
	free (vd->pictName);
	vd->videodevice = NULL;
	vd->status = NULL;
	vd->pictName = NULL;

	close (vd->fd);

	return 0;
}








/*
int uvcGrab (struct vdIn *vd)
{
#define HEADERFRAME1 0xaf
	int ret;

	if (!vd->isstreaming)
		if (video_enable (vd))
			goto err;

	memset (&vd->buf, 0, sizeof (struct v4l2_buffer));
	vd->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vd->buf.memory = V4L2_MEMORY_MMAP;

	ret = ioctl (vd->fd, VIDIOC_DQBUF, &vd->buf);
	if (ret < 0) 
	{
		fprintf (stderr, "Unable to dequeue buffer (%d).\n", errno);
		goto err;
	}

	switch (vd->formatIn) 
	{
		case V4L2_PIX_FMT_MJPEG:
			memcpy (vd->tmpbuffer, vd->mem[vd->buf.index], HEADERFRAME1);
			memcpy (vd->tmpbuffer + HEADERFRAME1, dht_data, DHT_SIZE);
			memcpy (vd->tmpbuffer + HEADERFRAME1 + DHT_SIZE, vd->mem[vd->buf.index] + HEADERFRAME1, (vd->buf.bytesused - HEADERFRAME1));

			if (debug)
				fprintf (stderr, "bytes in used %d \n", vd->buf.bytesused);
			break;
		
		case V4L2_PIX_FMT_YUYV:
			if (vd->buf.bytesused > vd->framesizeIn)
				memcpy (vd->framebuffer, vd->mem[vd->buf.index], (size_t) vd->framesizeIn);
			else
				memcpy (vd->framebuffer, vd->mem[vd->buf.index], (size_t) vd->buf.bytesused);
			break;

		default:
			goto err;
			break;
	}


	ret = ioctl (vd->fd, VIDIOC_QBUF, &vd->buf);
	if (ret < 0) 
	{
		fprintf (stderr, "Unable to requeue buffer (%d).\n", errno);
		goto err;
	}

	return 0;
	err:
		vd->signalquit = 0;

	return -1;

}
*/



/*
// return >= 0 ok otherwhise -1 
static int isv4l2Control (struct vdIn *vd, int control, struct v4l2_queryctrl *queryctrl)
{
	int err = 0;

	queryctrl->id = control;

	if ((err = ioctl (vd->fd, VIDIOC_QUERYCTRL, queryctrl)) < 0) 
		fprintf (stderr, "ioctl querycontrol error %d \n", errno);
	else if (queryctrl->flags & V4L2_CTRL_FLAG_DISABLED) 
		fprintf (stderr, "control %s disabled \n", (char *) queryctrl->name);
	else if (queryctrl->flags & V4L2_CTRL_TYPE_BOOLEAN) 
		return 1;
	else if (queryctrl->type & V4L2_CTRL_TYPE_INTEGER) 
		return 0;
	else 
		fprintf (stderr, "contol %s unsupported  \n", (char *) queryctrl->name);

	return -1;
}

int v4l2GetControl (struct vdIn *vd, int control)
{
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control_s;
	int err;

	if (isv4l2Control (vd, control, &queryctrl) < 0)
		return -1;
	control_s.id = control;

	if ((err = ioctl (vd->fd, VIDIOC_G_CTRL, &control_s)) < 0) 
	{
		fprintf (stderr, "ioctl get control error\n");
		return -1;
	}
	return control_s.value;
}

int v4l2SetControl (struct vdIn *vd, int control, int value)
{
	struct v4l2_control control_s;
	struct v4l2_queryctrl queryctrl;
	int min, max, step, val_def;
	int err;

	if (isv4l2Control (vd, control, &queryctrl) < 0)
		return -1;
	min = queryctrl.minimum;
	max = queryctrl.maximum;
	step = queryctrl.step;
	val_def = queryctrl.default_value;

	if ((value >= min) && (value <= max)) 
	{
		control_s.id = control;
		control_s.value = value;
		if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) 
		{
			fprintf (stderr, "ioctl set control error\n");
			return -1;
		}
	}

	return 0;
}

int v4l2UpControl (struct vdIn *vd, int control)
{
	struct v4l2_control control_s;
	struct v4l2_queryctrl queryctrl;
	int min, max, current, step, val_def;
	int err;

	if (isv4l2Control (vd, control, &queryctrl) < 0)
		return -1;
	min = queryctrl.minimum;
	max = queryctrl.maximum;
	step = queryctrl.step;
	val_def = queryctrl.default_value;
	current = v4l2GetControl (vd, control);
	current += step;

	if (current <= max) 
	{
		control_s.id = control;
		control_s.value = current;
		if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) 
		{
			fprintf (stderr, "ioctl set control error\n");
			return -1;
		}
	}

	return control_s.value;
}

int v4l2DownControl (struct vdIn *vd, int control)
{
	struct v4l2_control control_s;
	struct v4l2_queryctrl queryctrl;
	int min, max, current, step, val_def;
	int err;

	if (isv4l2Control (vd, control, &queryctrl) < 0)
		return -1;
	min = queryctrl.minimum;
	max = queryctrl.maximum;
	step = queryctrl.step;
	val_def = queryctrl.default_value;
	current = v4l2GetControl (vd, control);
	current -= step;
	
	if (current >= min) 
	{
		control_s.id = control;
		control_s.value = current;
		if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) 
		{
			fprintf (stderr, "ioctl set control error\n");
			return -1;
		}
	}

	return control_s.value;
}

int v4l2ToggleControl (struct vdIn *vd, int control)
{
	struct v4l2_control control_s;
	struct v4l2_queryctrl queryctrl;
	int current;
	int err;

	if (isv4l2Control (vd, control, &queryctrl) != 1)
		return -1;
	current = v4l2GetControl (vd, control);
	control_s.id = control;
	control_s.value = !current;

	if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) 
	{
		fprintf (stderr, "ioctl toggle control error\n");
		return -1;
	}
	
	return control_s.value;
}


int v4l2ResetControl (struct vdIn *vd, int control)
{
	struct v4l2_control control_s;
	struct v4l2_queryctrl queryctrl;
	int val_def;
	int err;

	if (isv4l2Control (vd, control, &queryctrl) < 0)
		return -1;
	val_def = queryctrl.default_value;
	control_s.id = control;
	control_s.value = val_def;

	if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) 
	{
		fprintf (stderr, "ioctl reset control error\n");
		return -1;
	}

	return 0;
}


int v4l2ResetPanTilt (struct vdIn *vd, int pantilt)
{
	int control = V4L2_CID_PANTILT_RESET;
	struct v4l2_control control_s;
	struct v4l2_queryctrl queryctrl;
	unsigned char val;
	int err;

	if (isv4l2Control (vd, control, &queryctrl) < 0)
	return -1;
	val = (unsigned char) pantilt;
	control_s.id = control;
	control_s.value = val;

	if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) 
	{
		fprintf (stderr, "ioctl reset Pan control error\n");
		return -1;
	}

	return 0;
}

union pantilt 
{
	struct 
	{
		short pan;
		short tilt;
	} s16;
	int value;
} pantilt;


int v4L2UpDownPan (struct vdIn *vd, short inc)
{
	int control = V4L2_CID_PANTILT_RELATIVE;
	struct v4l2_control control_s;
	struct v4l2_queryctrl queryctrl;
	int err;

	union pantilt pan;

	control_s.id = control;
	if (isv4l2Control (vd, control, &queryctrl) < 0)
		return -1;

	pan.s16.pan = inc;
	pan.s16.tilt = 0;

	control_s.value = pan.value;
	if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) 
	{
		fprintf (stderr, "ioctl pan updown control error\n");
		return -1;
	}
	return 0;
}

int v4L2UpDownTilt (struct vdIn *vd, short inc)
{
	int control = V4L2_CID_PANTILT_RELATIVE;
	struct v4l2_control control_s;
	struct v4l2_queryctrl queryctrl;
	int err;
	union pantilt pan;

	control_s.id = control;
	if (isv4l2Control (vd, control, &queryctrl) < 0)
		return -1;

	pan.s16.pan = 0;
	pan.s16.tilt = inc;

	control_s.value = pan.value;
	if ((err = ioctl (vd->fd, VIDIOC_S_CTRL, &control_s)) < 0) 
	{
		fprintf (stderr, "ioctl tiltupdown control error\n");
		return -1;
	}
	return 0;
}

*/





