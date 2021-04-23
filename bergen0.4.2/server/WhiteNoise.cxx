#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include "WhiteNoise.h"

WhiteNoise::WhiteNoise(int bufsize) : ASound(bufsize)
	{
	playing_ = 0;
	killed_ = 0;
	}

WhiteNoise::~WhiteNoise(void)
	{
	kill();
	}


short * WhiteNoise::updateBuffer(void)
	{
	if (playing_)
		{
		int i;
		for (i=0; i < bufferSize_; i++)
			buffer_[i] = (short)((random() % 65536 - 32768) * amplitude_);
		}
	return buffer_;
	}


int WhiteNoise::removable(void)
	{
	return killed_;
	}


int WhiteNoise::active(void)
	{
	return playing_;
	}


void WhiteNoise::play(void)
	{
	playing_ = 1;
	}


void WhiteNoise::stop(void)
	{
	playing_ = 0;
	}


void WhiteNoise::pause(void)
	{
	playing_ = 0;
	}


void WhiteNoise::kill(void)
	{
	stop();
	killed_ = 1;
	}
