/* All of the ALSA library API is defined in this header */

#include <alsa/asoundlib.h>


#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/soundcard.h>



/* This example reads from the default PCM device and writes to standard output for 5 seconds of data. */
/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API


snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
snd_pcm_uframes_t frames;




void alsa_sound_init()
{
	int rc;
//	int size;
//	unsigned int val2;
	unsigned int val;
	int dir;
	
	/* Open PCM device for recording (capture). */
	rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);
//	rc = snd_pcm_open(&handle, "plughw:7,0", SND_PCM_STREAM_CAPTURE, 0);

	if (rc < 0)
	{
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */
	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(handle, params, 2);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

	/* Set period size to 32 frames. */
//	frames = 32;
//	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

	frames = 1470;
	snd_pcm_hw_params_set_period_size(handle, params, &frames, &dir);

printf("dir ==>%d\n", dir);
printf("frames ==>%d\n", (int)frames);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0)
	{
		fprintf(stderr,	"unable to set hw parameters: %s\n", snd_strerror(rc));
		exit(1);
	}

	/* Use a buffer large enough to hold one period */
//	snd_pcm_hw_params_get_period_size(params, &frames, &dir);



/* Display information about the PCM interface */
/*
	  printf("PCM handle name = '%s'\n", snd_pcm_name(handle));
	  printf("PCM state = %s\n", snd_pcm_state_name(snd_pcm_state(handle)));
	  snd_pcm_hw_params_get_access(params, (snd_pcm_access_t *) &val);
	  printf("access type = %s\n", snd_pcm_access_name((snd_pcm_access_t)val));
	  snd_pcm_hw_params_get_format(params, &val);
	  printf("format = '%s' (%s)\n",  snd_pcm_format_name((snd_pcm_format_t)val), snd_pcm_format_description((snd_pcm_format_t)val));
	  snd_pcm_hw_params_get_subformat(params, (snd_pcm_subformat_t *)&val);
	  printf("subformat = '%s' (%s)\n", snd_pcm_subformat_name((snd_pcm_subformat_t)val), snd_pcm_subformat_description((snd_pcm_subformat_t)val));
	  snd_pcm_hw_params_get_channels(params, &val);
	  printf("channels = %d\n", val);
	  snd_pcm_hw_params_get_rate(params, &val, &dir);
	  printf("rate = %d bps\n", val);
	  snd_pcm_hw_params_get_period_time(params, &val, &dir);
	  printf("period time = %d us\n", val);
	  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	  printf("period size = %d frames\n", (int)frames);
	  snd_pcm_hw_params_get_buffer_time(params, &val, &dir);
	  printf("buffer time = %d us\n", val);
	  snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *) &val);
	  printf("buffer size = %d frames\n", val);
	  snd_pcm_hw_params_get_periods(params, &val, &dir);
	  printf("periods per buffer = %d frames\n", val);
	  snd_pcm_hw_params_get_rate_numden(params, &val, &val2);
	  printf("exact rate = %d/%d bps\n", val, val2);
	  val = snd_pcm_hw_params_get_sbits(params);
	  printf("significant bits = %d\n", val);
	  snd_pcm_hw_params_get_tick_time(params, &val, &dir);
	  printf("tick time = %d us\n", val);
	  val = snd_pcm_hw_params_is_batch(params);
	  printf("is batch = %d\n", val);
	  val = snd_pcm_hw_params_is_block_transfer(params);
	  printf("is block transfer = %d\n", val);
	  val = snd_pcm_hw_params_is_double(params);
	  printf("is double = %d\n", val);
	  val = snd_pcm_hw_params_is_half_duplex(params);
	  printf("is half duplex = %d\n", val);
	  val = snd_pcm_hw_params_is_joint_duplex(params);
	  printf("is joint duplex = %d\n", val);
	  val = snd_pcm_hw_params_can_overrange(params);
	  printf("can overrange = %d\n", val);
	  val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
	  printf("can mmap = %d\n", val);
	  val = snd_pcm_hw_params_can_pause(params);
	  printf("can pause = %d\n", val);
	  val = snd_pcm_hw_params_can_resume(params);
	  printf("can resume = %d\n", val);
	  val = snd_pcm_hw_params_can_sync_start(params);
	  printf("can sync start = %d\n", val);
*/

}


// Listing 4. Simple Sound Recording
int alsa_sound_capture()
{
	long loops;
	int rc;
	int size;
	unsigned int val;
	int dir;
	char* buffer;


	size = frames * 4; // 2 bytes/sample, 2 channels
	buffer = (char *) malloc(size);


	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params, &val, &dir);
printf("val ==>%d\n", val); // val = 21333

	loops = 5000000 / val;
	while (loops > 0)
	{
		loops--;
		rc = snd_pcm_readi(handle, buffer, frames);

		if (rc == -EPIPE)
		{
			printf("overrun occurred\n");
			/* EPIPE means overrun */
			fprintf(stderr, "overrun occurred\n");
			snd_pcm_prepare(handle);
		}
		else if (rc < 0)
		{
			printf("error from read\n");
			fprintf(stderr, "error from read: %s\n", snd_strerror(rc));
		}
		else if (rc != (int)frames)
		{
			printf("short read, read %d frames\n", rc);
			fprintf(stderr, "short read, read %d frames\n", rc);
		}

		rc = write(1, buffer, size);

		if (rc != size)
		fprintf(stderr, "short write: wrote %d bytes\n", rc);
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);

	return 0;

}





#define LENGTH 3    /* how many seconds of speech to store */
#define RATE 8000   /* the sampling rate */
#define SIZE 8      /* sample size: 8 or 16 bits */
#define CHANNELS 1  /* 1 = mono 2 = stereo */

/* this buffer holds the digitized audio */
unsigned char buf[LENGTH*RATE*SIZE*CHANNELS/8];

int test_sound()
{
	int fd;	/* sound device file descriptor */
	int arg;	/* argument for ioctl calls */
	int status;   /* return status of system calls */

	/* open sound device */
	fd = open("/dev/dsp", O_RDWR);

	if (fd < 0)
	{
		perror("open of /dev/dsp failed");
		exit(1);
	}

	/* set sampling parameters */
	arg = SIZE;	   /* sample size */
	status = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg);

	if (status == -1)
		perror("SOUND_PCM_WRITE_BITS ioctl failed");
	if (arg != SIZE)
		perror("unable to set sample size");

	arg = CHANNELS;  /* mono or stereo */
	status = ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg);

	if (status == -1)
		perror("SOUND_PCM_WRITE_CHANNELS ioctl failed");
	if (arg != CHANNELS)
		perror("unable to set number of channels");

	arg = RATE;	   /* sampling rate */
	status = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);
	if (status == -1)
		perror("SOUND_PCM_WRITE_WRITE ioctl failed");


	while (1)
	{
		/* loop until Control-C */
		printf("Say something:\n");

		status = read(fd, buf, sizeof(buf)); /* record some sound */
		if (status != sizeof(buf))
			perror("read wrong number of bytes");

		printf("You said:\n");
		status = write(fd, buf, sizeof(buf)); /* play it back */

		if (status != sizeof(buf))
			perror("wrote wrong number of bytes");

		/* wait for playback to complete before recording again */
		status = ioctl(fd, SOUND_PCM_SYNC, 0); 
		if (status == -1)
		perror("SOUND_PCM_SYNC ioctl failed");
	}
	
}











void test_alsa_main (int argc, char *argv[])
{
	int i;
	int err;
	short buf[128];
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;

	if ((err = snd_pcm_open (&capture_handle, argv[1], SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n", 
			 argv[1],
			 snd_strerror (err));
		exit (1);
	}
	   
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit (1);
	}
			 
	if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, 44100, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 2)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (capture_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
			 snd_strerror (err));
		exit (1);
	}

	for (i = 0; i < 10; ++i) {
		if ((err = snd_pcm_readi (capture_handle, buf, 128)) != 128) {
			fprintf (stderr, "read from audio interface failed (%s)\n",
				 snd_strerror (err));
			exit (1);
		}
	}

	snd_pcm_close (capture_handle);
	exit (0);
}



