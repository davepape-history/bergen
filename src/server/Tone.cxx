#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "Tone.h"

Tone::Tone(int bufsize) : ASound(bufsize)
	{
	frequency_ = 1000;
	playing_ = 0;
	killed_ = 0;
	count_ = 0;
	}

Tone::~Tone(void)
	{
	kill();
	}


short * Tone::updateBuffer(void)
	{
	if (playing_)
		{
		int i;
		for (i=0; i < bufferSize_; i++, count_++)
			buffer_[i] = (short) (sinf(count_ * frequency_ * 2.0f * M_PI / sampleRate_)
					* 32768.0f * amplitude_);
		}
	return buffer_;
	}


int Tone::removable(void)
	{
	return killed_;
	}


int Tone::active(void)
	{
	return playing_;
	}


void Tone::play(void)
	{
	playing_ = 1;
	}


void Tone::stop(void)
	{
	playing_ = 0;
	}


void Tone::pause(void)
	{
	playing_ = 0;
	}


void Tone::kill(void)
	{
	stop();
	killed_ = 1;
	}


void Tone::message(char **msg,int numStrings)
	{
	if (!strcasecmp(msg[0],"setfrequency"))
		{
		if (numStrings > 1)
			setFrequency(atoi(msg[1]));
		}
	else
		ASound::message(msg,numStrings);
	}


void Tone::setFrequency(int freq)
	{
	frequency_ = freq;
	}
