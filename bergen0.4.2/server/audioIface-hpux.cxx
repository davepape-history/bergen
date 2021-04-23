#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "hpux-simpleAudio.h"
#include "audioIface.h"

int snerdAudioBufferSize = 1000;

static int audioPort = -1;


void snerdOpenAudioPort(int device,int bufsize,int sampleRate)
	{
	openAudio();
	audioPort = openAStream( PLAY_STREAM, sampleRate, USE_MONO, USE_LIN16,
				 USE_DEFAULT_SPEAKER, START_IMMEDIATELY );
	if (audioPort < 0)
	       {
	       fprintf(stderr,"Failed to open audio port for writing\n");
	       perror("openAStream");
	       exit(-1);
	       }
	}


void snerdWriteToAudioPort(void *buffer,size_t bufferSize)
	{
	write(audioPort, buffer, bufferSize*2);
	}

