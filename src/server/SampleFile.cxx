#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "snerdServer.h"
#include "SampleFile.h"


static int stringEndsWith(char *string, char *ending)
	{
	int endlen = strlen(ending), stringlen = strlen(string);
	if (stringlen < endlen)
		return 0;
	else
		return !strcmp(string+stringlen-endlen,ending);
	}



SampleFile::SampleFile(char *filename,int bufsize) : ASound(bufsize)
	{
	killed_ = 0;
	filename_ = filename;
	loop_ = 0;
	playing_ = 0;
	inputBuf_ = NULL;
	inputFrames_ = bufsize;
	isRawFile_ = stringEndsWith(filename,".raw");
	if (isRawFile_)
		initializeRawFile();
	else
		initializeAFFile();
	}


void SampleFile::initializeRawFile(void)
	{
	rawfd_ = open(filename_, O_RDONLY);
	if (rawfd_ == -1)
		{
		fprintf(stderr,"ERROR: failed to open sample file \"%s\"\n",filename_);
		perror(filename_);
		}
	if (rawfd_ != -1)
		{
		struct stat statbuf;
		stat(filename_,&statbuf);
		numFrames_ = statbuf.st_size / sizeof(short);
		}
	channels_ = 1;
	sampleFormat_ = AF_SAMPFMT_TWOSCOMP;
	sampleWidth_ = 16;
	bytesPerSample_ = 2;
	fileRate_ = -1;
	frameSize_ = 2;
	}


void SampleFile::initializeAFFile(void)
	{
#ifdef AUDIOFILE_LIBRARY
	afFile_ = afOpenFile(filename_, "r", NULL);
	if (afFile_ == AF_NULL_FILEHANDLE)
		{
		fprintf(stderr,"ERROR: failed to open sample file \"%s\"\n",filename_);
		perror("afOpenFile");
		}
	if (afFile_ != AF_NULL_FILEHANDLE)
		{
		int bad=0;
		channels_ = afGetChannels(afFile_, AF_DEFAULT_TRACK);
		afGetSampleFormat(afFile_, AF_DEFAULT_TRACK, &sampleFormat_, &sampleWidth_);
		fileRate_ = (int)(afGetRate(afFile_, AF_DEFAULT_TRACK));
		numFrames_ = afGetFrameCount(afFile_, AF_DEFAULT_TRACK);
		if (sampleWidth_ == 8)
			bytesPerSample_ = 1;
		else if (sampleWidth_ == 16)
			bytesPerSample_ = 2;
		else
			{
			fprintf(stderr,"ERROR: \"%s\" has unsupported width of %d bits (only"
				" 8 and 16 are supported\n", filename_, sampleWidth_);
			bad = 1;
			}
		frameSize_ = channels_ * bytesPerSample_;
		if ((sampleFormat_ != AF_SAMPFMT_TWOSCOMP) && (sampleFormat_ != AF_SAMPFMT_UNSIGNED))
			{
			fprintf(stderr,"ERROR: \"%s\" is in an unsupported format (only 2scomp"
				" and unsigned are supported)\n",filename_);
			bad = 1;
			}
		if (fileRate_ <= 0)
			{
			fprintf(stderr,"ERROR: problem getting sample rate for \"%s\"\n",
				filename_);
			bad = 1;
			}
		if (bad)
			{
			afCloseFile(afFile_);
			afFile_ = AF_NULL_FILEHANDLE;
			}
		}
#else
	afFile_ = AF_NULL_FILEHANDLE;
	fprintf(stderr,"No SGI audiofile library support - cannot handle file \"%s\"\n",
		filename_);
#endif
	}

SampleFile::~SampleFile(void)
	{
	if (isRawFile_)
		{
		if (rawfd_ != -1)
			close(rawfd_);
		}
#ifdef AUDIOFILE_LIBRARY
	else
		if (afFile_ != AF_NULL_FILEHANDLE)
			afCloseFile(afFile_);
#endif
	}


void SampleFile::setLoop(int loop)
	{
	loop_ = loop;
	}


int SampleFile::loop(void)
	{
	return loop_;
	}


char * SampleFile::filename(void)
	{
	return filename_;
	}


void SampleFile::play(void)
	{
	playing_ = 1;
	}


void SampleFile::stop(void)
	{
	playing_ = 0;
	if (isRawFile_)
		{
		if (rawfd_ != -1)
			lseek(rawfd_, 0, SEEK_SET);
		}
#ifdef AUDIOFILE_LIBRARY
	else
		if (afFile_ != AF_NULL_FILEHANDLE)
			afSeekFrame(afFile_, AF_DEFAULT_TRACK, 0);
#endif
	}


void SampleFile::pause(void)
	{
	playing_ = 0;
	}


void SampleFile::kill(void)
	{
	killed_ = 1;
	if (isRawFile_)
		{
		if (rawfd_ != -1)
			close(rawfd_);
		rawfd_ = -1;
		}
#ifdef AUDIOFILE_LIBRARY
	else
		{
		if (afFile_ != AF_NULL_FILEHANDLE)
			afCloseFile(afFile_);
		afFile_ = AF_NULL_FILEHANDLE;
		}
#endif
	}


int SampleFile::readFrames(void)
	{
	if (isRawFile_)
		return readRawFrames();
#ifdef AUDIOFILE_LIBRARY
	else
		return readAFFrames();
#else
	return 0;
#endif
	}


int SampleFile::readRawFrames(void)
	{
	int bytesToRead = inputFrames_ * sizeof(short);
	int numBytesRead = read(rawfd_, inputBuf_, bytesToRead);
	if (numBytesRead < 0)
		{
		perror(filename_);
		stop();
		return 0;
		}
	if (numBytesRead < bytesToRead)
		{
		if (loop_)
			{
			while (numBytesRead < bytesToRead)
				{
				lseek(rawfd_, 0, SEEK_SET);
				int n = read(rawfd_,
						inputBuf_ + numBytesRead/sizeof(short),
						bytesToRead - numBytesRead);
				if (n <= 0)
					{
					perror(filename_);
					stop();
					return 0;
					}
				numBytesRead += n;
				}
			}
		else
			{
			memset(inputBuf_ + numBytesRead/sizeof(short), 0,
				bytesToRead-numBytesRead);
			stop();
			}
		}
	return 1;
	}


int SampleFile::readAFFrames(void)
	{
#ifdef AUDIOFILE_LIBRARY
	int numRead = afReadFrames(afFile_, AF_DEFAULT_TRACK, tempBuf_, inputFrames_);
	if (numRead < 0)
		{
		perror(filename_);
		stop();
		return 0;
		}
	if (numRead < inputFrames_)
		{
		if (loop_)
			{
			while (numRead < inputFrames_)
				{
				afSeekFrame(afFile_, AF_DEFAULT_TRACK, 0);
				int n = afReadFrames(afFile_, AF_DEFAULT_TRACK,
						tempBuf_ + numRead*frameSize_,
						inputFrames_ - numRead);
				if (n <= 0)
					{
					perror(filename_);
					stop();
					return 0;
					}
				numRead += n;
				}
			}
		else
			{
			memset(tempBuf_ + numRead*frameSize_, 0, (inputFrames_ - numRead)*frameSize_);
			stop();
			}
		}
	convertFrames();
	return 1;
#else
	return 0;
#endif
	}


void SampleFile::convertFrames(void)
	{
	int i;
	if (sampleWidth_ == 16)
		{
		if (sampleFormat_ == AF_SAMPFMT_TWOSCOMP)
			{
			for (i=0; i < inputFrames_; i++)
				inputBuf_[i] = ((signed short *)tempBuf_)[i*channels_];
			}
		else /* if (sampleFormat_ == AF_SAMPFMT_UNSIGNED) */
			{
			for (i=0; i < inputFrames_; i++)
				inputBuf_[i] = ((unsigned short *)tempBuf_)[i*channels_]/2 - 32768;
			}
		}
	else /* if (sampleWidth_ == 8) */
		{
		if (sampleFormat_ == AF_SAMPFMT_TWOSCOMP)
			{
			for (i=0; i < inputFrames_; i++)
				inputBuf_[i] = ((signed char *)tempBuf_)[i*channels_] * 256;
			}
		else /* if (sampleFormat_ == AF_SAMPFMT_UNSIGNED) */
			{
			for (i=0; i < inputFrames_; i++)
				inputBuf_[i] = (((unsigned char *)tempBuf_)[i*channels_]/2 - 128)
						* 256;
			}
		}
	}


short * SampleFile::updateBuffer(void)
	{
	if (!inputBuf_)
		{
		if (isRawFile_)
			fileRate_ = sampleRate_;
		inputFrames_ = bufferSize_ * fileRate_ / sampleRate_;
		tempBuf_ = (unsigned char *) malloc(inputFrames_ * frameSize_);
		inputBuf_ = (short *) malloc(inputFrames_ * 2);
		}
	if (active())
		{
		if (readFrames())
			{
			int i;
			if (sampleRate_ == fileRate_)
				for (i=0; i < bufferSize_; i++)
					buffer_[i] = (short)(inputBuf_[i] * amplitude_);
			else
				for (i=0; i < bufferSize_; i++)
					buffer_[i] = (short)(inputBuf_[i*fileRate_/sampleRate_] * amplitude_);
			}
		}
	else
		memset(buffer_, 0, bufferSize_ * sizeof(short));
	return buffer_;
	}


int SampleFile::removable(void)
	{
	return killed_;
	}


int SampleFile::active(void)
	{
	if (isRawFile_)
		return ((playing_) && (rawfd_ != -1));
#ifdef AUDIOFILE_LIBRARY
	else
		return ((playing_) && (afFile_ != AF_NULL_FILEHANDLE));
#else
	return 0;
#endif
	}


void SampleFile::message(char **msg,int numStrings)
	{
	if (!strcasecmp(msg[0],"loop"))
		{
		if (numStrings > 1)
			setLoop(atoi(msg[1]));
		}
	else if (!strcasecmp(msg[0],"getduration"))
		{
		if (fileRate_ > 0)
			snerdServer::returnFloat(((float)numFrames_) / fileRate_);
		else
			snerdServer::returnFloat(0);
		}
	else
		ASound::message(msg,numStrings);
	}
