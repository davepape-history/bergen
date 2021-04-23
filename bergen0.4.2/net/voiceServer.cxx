#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include "bergenUDPSocket.h"

#define DEFAULT_PORT 5000

#define MAX_SAMPLE_SIZE 44100
#define DEVICE_NAME "/dev/dsp"


int openMicrophone(int sampleRate);


static int blockSize=1024;


int main(int argc,char **argv)
	{
	bergenUDPSocket *socket = new bergenUDPSocket(0);
	struct { short t,s; short data[MAX_SAMPLE_SIZE]; } packet;
	int micPort, i, max;
	if (argc > 2)
		socket->setDestination(argv[1],atoi(argv[2]));
	else if (argc > 1)
		socket->setDestination(argv[1],DEFAULT_PORT);
	else
		socket->setDestination("localhost",DEFAULT_PORT);
	micPort = openMicrophone(32000);
	packet.t = htons(1);
	while (1)
		{
		int nbytes = read(micPort, packet.data, blockSize);
		packet.s = htons(nbytes);
		for (i=0, max=0; i < nbytes/2; i++)
			{
			if (packet.data[i] > max)
				max = packet.data[i];
			packet.data[i] = htons(packet.data[i]);
			}
		socket->send((unsigned char *)&packet,nbytes+4);
		}
	delete socket;
	close(micPort);
	}


int openMicrophone(int sampleRate)
	{
	int port;
	int prof=APF_NORMAL, sampleSize=16, stereo=0;
	if ((port = open(DEVICE_NAME, O_RDONLY)) == -1)
	       {
	       fprintf(stderr,"Failed to open audio port for reading\n");
	       perror(DEVICE_NAME);
	       exit(-1);
	       }
	if (ioctl(port, SNDCTL_DSP_SAMPLESIZE, &sampleSize) == -1)  
		{
		perror("SNDCTL_DSP_SAMPLESIZE");  
		exit(-1);
		}
	printf("sampleSize = %d\n",sampleSize);
	ioctl(port, SNDCTL_DSP_PROFILE, &prof);
	if (ioctl(port, SNDCTL_DSP_STEREO, &stereo) == -1)  
		{
		perror("SNDCTL_DSP_STEREO");  
		exit(-1);
		}
	printf("stereo = %d\n",stereo);
	if (ioctl(port, SNDCTL_DSP_SPEED, &sampleRate) == -1)  
		{
		perror("SNDCTL_DSP_SPEED");  
		exit(-1);
		}
	printf("sampleRate = %d\n",sampleRate);
	/* Tell hardware to use 3 fragments, of size 1024 bytes (2^10) */
	int fragSize = 0x0003000A;
	if (ioctl(port, SNDCTL_DSP_SETFRAGMENT, &fragSize) == -1)  
		perror("SNDCTL_DSP_SETFRAGMENT");  
	/* Find out what the resulting fragment size actually is */
	if (ioctl(port, SNDCTL_DSP_GETBLKSIZE, &blockSize) == -1)
		{
		blockSize = 1024;
		}
	printf("blockSize = %d\n",blockSize);
	return port;
	}
