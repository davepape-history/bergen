#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include "audioIface.h"

#define DEVICE_NAME "/dev/dsp"

int snerdAudioBufferSize = 1024;

static int audioPort;


void snerdOpenAudioPort(int device,int bufsize,int sampleRate)
	{
	int prof=APF_NORMAL, sampleSize=16, blockSize=0, stereo=0;
	if ((audioPort = open(DEVICE_NAME, O_WRONLY)) == -1)
	       {
	       fprintf(stderr,"Failed to open audio port for writing\n");
	       perror(DEVICE_NAME);
	       exit(-1);
	       }
	if (ioctl(audioPort, SNDCTL_DSP_SAMPLESIZE, &sampleSize) == -1)  
		{
		perror("SNDCTL_DSP_SAMPLESIZE");  
		exit(-1);
		}
	ioctl(audioPort, SNDCTL_DSP_PROFILE, &prof);
	if (ioctl(audioPort, SNDCTL_DSP_STEREO, &stereo) == -1)  
		{
		perror("SNDCTL_DSP_STEREO");  
		exit(-1);
		}
	if (ioctl(audioPort, SNDCTL_DSP_SPEED, &sampleRate) == -1)  
		{
		perror("SNDCTL_DSP_SPEED");  
		exit(-1);
		}
	/* Tell hardware to use 3 fragments, of size 1024 bytes (2^10) */
	int fragSize = 0x0003000A;
	if (ioctl(audioPort, SNDCTL_DSP_SETFRAGMENT, &fragSize) == -1)  
		perror("SNDCTL_DSP_SETFRAGMENT");  
	/* Find out what the resulting fragment size actually is */
	if (ioctl(audioPort, SNDCTL_DSP_GETBLKSIZE, &blockSize) != -1)
		{
		if (blockSize > 0)
			snerdAudioBufferSize = blockSize/2;
#ifdef DEBUG
		printf("got blockSize=%d\n",blockSize);
#endif
		}
	}


void snerdWriteToAudioPort(void *buffer,size_t bufferSize)
	{
	write(audioPort, buffer, bufferSize*2);
	}
