#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "snerdDB.h"

#define MAX_SOUNDS 32768

snerdDB::snerdDB(int bufsize)
	{
	bufferSize_ = bufsize;
	buffer_ = (short *) malloc(bufferSize_ * sizeof(short));
	memset(buffer_, 0, bufferSize_ * sizeof(short));
	sounds_ = (ASound **) malloc(MAX_SOUNDS * sizeof(ASound *));
	memset(sounds_, 0, MAX_SOUNDS * sizeof(ASound *));
	maxSoundIndex_ = -1;
	sampleRate_ = 32000;
	gain_ = 1.0;
	}

snerdDB::~snerdDB(void)
	{
	free(buffer_);
	free(sounds_);
	}


void snerdDB::setSampleRate(int rate)
	{
	sampleRate_ = rate;
	}


void snerdDB::addSound(ASound * sound)
	{
	int i;
#ifdef DEBUG
	printf("Adding sound %d\n",sound->handle());
#endif
	sound->setSampleRate(sampleRate_);
	for (i=0; i < MAX_SOUNDS; i++)
		if (!sounds_[i])
			{
			sounds_[i] = sound;
			if (i > maxSoundIndex_)
				maxSoundIndex_ = i;
			return;
			}
	fprintf(stderr,"ERROR: snerdDB::addSound() - list is full; cannot add new sound\n");
	}


ASound * snerdDB::findHandle(int h)
	{
	int i;
	for (i=0; i < MAX_SOUNDS; i++)
		if (sounds_[i])
			{
			if (h == sounds_[i]->handle())
				return sounds_[i];
			}
	return NULL;
	}


void snerdDB::update(void)
	{
	int i, j;
	memset(buffer_, 0, bufferSize_ * sizeof(short));
	for (i=0; i <= maxSoundIndex_; i++)
		{
		if ((sounds_[i]) && (sounds_[i]->removable()))
			{
			delete sounds_[i];
			sounds_[i] = NULL;
			if (i == maxSoundIndex_)
				{
				while ((maxSoundIndex_ >= 0) && (sounds_[maxSoundIndex_] == NULL))
					maxSoundIndex_--;
				}
			}
		}
	for (i=0; i <= maxSoundIndex_; i++)
		{
		if ((sounds_[i]) && (sounds_[i]->active()))
			{
			short * sbuf = sounds_[i]->updateBuffer();
			for (j=0; j < bufferSize_; j++)
				buffer_[j] += sbuf[j];
			}
		}
	if (gain_ != 1.0f)
		{
		for (i=0; i < bufferSize_; i++)
			buffer_[i] = (short)(buffer_[i] * gain_);
		}
	}


short * snerdDB::buffer(void)
	{
	return buffer_;
	}


int snerdDB::bufferSize(void)
	{
	return bufferSize_;
	}


void snerdDB::reset(void)
	{
	int i;
	for (i=0; i <= maxSoundIndex_; i++)
		{
		if (sounds_[i])
			{
			sounds_[i]->kill();
			delete sounds_[i];
			}
		}
	memset(sounds_, 0, MAX_SOUNDS * sizeof(ASound *));
	maxSoundIndex_ = -1;
	}


void snerdDB::setGain(float g)
	{
	gain_ = g;
	}
