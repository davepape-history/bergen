#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ASound.h"

int ASound::NextHandle = 1;

ASound::ASound(int bufsize)
	{
	handle_ = NextHandle++;
	bufferSize_ = bufsize;
	buffer_ = (short *) malloc(bufferSize_ * sizeof(short));
	amplitude_ = 1;
	sampleRate_ = 32000;
	}

ASound::~ASound(void)
	{
	free(buffer_);
	}


short * ASound::updateBuffer(void)
	{
	return buffer_;
	}


int ASound::removable(void)
	{
	return 1;
	}


int ASound::active(void)
	{
	return 0;
	}


int ASound::handle(void)
	{
	return handle_;
	}


short * ASound::buffer(void)
	{
	return buffer_;
	}


int ASound::bufferSize(void)
	{
	return bufferSize_;
	}


void ASound::setAmplitude(float amp)
	{
	amplitude_ = amp;
	}


float ASound::amplitude(void)
	{
	return amplitude_;
	}


void ASound::play(void)
	{
	}


void ASound::stop(void)
	{
	}


void ASound::pause(void)
	{
	}


void ASound::kill(void)
	{
	}


void ASound::message(char **msg,int numStrings)
	{
	if (!strcasecmp(msg[0],"play"))
		play();
	else if (!strcasecmp(msg[0],"stop"))
		stop();
	else if (!strcasecmp(msg[0],"pause"))
		pause();
	else if (!strcasecmp(msg[0],"kill"))
		kill();
	else if (!strcasecmp(msg[0],"amplitude"))
		{
		if (numStrings > 1)
			setAmplitude(atof(msg[1]));
		}
	else
		printf("sound %d got unrecognized message \"%s\"\n",handle_,msg[0]);
	}


void ASound::setSampleRate(int rate)
	{
	sampleRate_ = rate;
	}
