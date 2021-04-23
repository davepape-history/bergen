#include <stdio.h>
#include <stdlib.h>
#include <dmedia/audio.h>
#include "audioIface.h"


int snerdAudioBufferSize = 1000;

static ALport audioPort;


void snerdOpenAudioPort(int device,int bufsize,int sampleRate)
	{
	long params[] = { AL_OUTPUT_RATE, 32000 };
	ALconfig config = ALnewconfig();
	ALsetchannels(config, 1);
	ALsetwidth(config, AL_SAMPLE_16);
	ALsetqueuesize(config,bufsize * 2);
	audioPort = ALopenport("snerd","w",config);
	if (!audioPort)
		{
		perror("ALopenport");
		exit(-1);
		}
	params[1] = sampleRate;
	ALsetparams(AL_DEFAULT_DEVICE,params,2);
	}


void snerdWriteToAudioPort(void *buffer,size_t bufferSize)
	{
	ALwritesamps(audioPort, buffer, bufferSize);
	}
