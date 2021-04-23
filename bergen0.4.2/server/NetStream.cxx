#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/param.h>
#include "bergenUDPSocket.h"
#include "NetStream.h"

#define MAX_SAMPLEBUFFER_LENGTH (3 * 48000)   /* Up to 3 seconds of 48k sound */
#define MAX_PACKET_SIZE 65536


NetStream::NetStream(int bufsize) : ASound(bufsize)
	{
	playing_ = 0;
	killed_ = 0;
	socket_ = new bergenUDPSocket(0);
	socket_->setBlocking(0);
	sampleBuffer_ = (short *) malloc(MAX_SAMPLEBUFFER_LENGTH * sizeof(short));
	packetBuffer_ = (char *) malloc(MAX_PACKET_SIZE);
	sampleWritePos_ = sampleReadPos_ = 0;
	buffering_ = true;
	initialBufferSamples_ = 1000;
	}

NetStream::~NetStream(void)
	{
	kill();
	free(packetBuffer_);
	free(sampleBuffer_);
	delete socket_;
	}


short * NetStream::updateBuffer(void)
	{
	short * outbuf = buffer();
	if (socket_)
		{
		int nbytes;
		while ((nbytes = socket_->receive(packetBuffer_,MAX_PACKET_SIZE)) > 0)
			parsePacket(nbytes);
		}
	if (buffering_)
		{
		int diff = sampleWritePos_ - sampleReadPos_;
		if (diff < 0)
			diff = MAX_SAMPLEBUFFER_LENGTH + diff;
		if (diff > initialBufferSamples_)
			{
			buffering_ = false;
//			printf("done buffering\n");
			}
		}
	if ((playing_) && (!buffering_))
		{
		int i, size=bufferSize();
		float amp = amplitude();
		for (i=0; i < size; i++)
			{
			if (sampleReadPos_ == sampleWritePos_)
				break;
			outbuf[i] = (short)(sampleBuffer_[sampleReadPos_] * amp);
			sampleReadPos_++;
			if (sampleReadPos_ >= MAX_SAMPLEBUFFER_LENGTH)
				sampleReadPos_ = 0;
			}
		if ((i>0) && (i < size))
			{
//			printf("buffer underrun\n");
			buffering_ = true;
			}
		for (; i < size; i++)
			outbuf[i] = 0;
		}
	else
		{
		int i, size=bufferSize();
		for (i=0; i < size; i++)
			outbuf[i] = 0;
		}
	return outbuf;
	}


void NetStream::parsePacket(int nbytes)
	{
	int pos=0, chunkType, dataSize;
	char *dataPointer;
	while (pos < nbytes)
		{
		int chunkSize = getChunk(packetBuffer_+pos,nbytes-pos,chunkType,dataSize,dataPointer);
		if (chunkSize <= 0)
			break;
		pos += chunkSize;
		if (chunkType == BERGEN_NET_SAMPLEDATA)
			appendSampleData(dataPointer,dataSize);
		}
	}


int NetStream::getChunk(char *buffer,int nbytes,int& chunkType,int& dataSize,char*& dataPointer)
	{
	short temp;
	if (nbytes < 2*sizeof(short))
		return 0;
	memcpy(&temp,buffer,sizeof(short));
	chunkType = htons(temp);
	memcpy(&temp,buffer+sizeof(short),sizeof(short));
	dataSize = htons(temp);
	if (nbytes < dataSize + 2*sizeof(short))
		return 0;
	dataPointer = buffer + 2*sizeof(short);
	return (dataSize+2*sizeof(short));
	}


void NetStream::appendSampleData(char *data,int size)
	{
	short * data16 = (short *) data;
	int dataLength = size/sizeof(short), i;
	if ((sampleWritePos_ == sampleReadPos_ - 1) ||
	    ((sampleWritePos_ == MAX_SAMPLEBUFFER_LENGTH-1) && (sampleReadPos_ == 0)))
		return;
	for (i=0; i < dataLength; i++)
		{
		sampleBuffer_[sampleWritePos_] = htons(data16[i]);
		sampleWritePos_++;
		if (sampleWritePos_ >= MAX_SAMPLEBUFFER_LENGTH)
			sampleWritePos_ = 0;
		if ((sampleWritePos_ == sampleReadPos_ - 1) ||
		    ((sampleWritePos_ == MAX_SAMPLEBUFFER_LENGTH-1) && (sampleReadPos_ == 0)))
			break;
		}
//	if (i < dataLength)
//		printf("write overrun\n");
	}


void NetStream::setSource(char *sourceHost,char *portStr,char *durationStr)
	{
	int port;
	float duration;
	port = atoi(portStr);
	duration = atof(durationStr);
	if (socket_)
		{
		struct { short chunkType, dataSize; char data[32]; } packet;
//!! Be aware of the use of htons() on port number here !!!
		socket_->setDestination(sourceHost,htons(port));
		packet.chunkType = htons(BERGEN_NET_REQUESTSTREAM);
		sprintf(packet.data,"%.3f ",duration);
		packet.dataSize = htons(strlen(packet.data)+1);
		socket_->send((unsigned char *)&packet, strlen(packet.data)+5);
		}
	}


int NetStream::removable(void)
	{
	return killed_;
	}


int NetStream::active(void)
	{
	return playing_;
	}


void NetStream::play(void)
	{
	playing_ = 1;
	}


void NetStream::stop(void)
	{
	playing_ = 0;
	}


void NetStream::pause(void)
	{
	playing_ = 0;
	}


void NetStream::kill(void)
	{
	stop();
	killed_ = 1;
	}


void NetStream::message(char **msg,int numStrings)
	{
	if (!strcasecmp(msg[0],"setsource"))
		{
		if (numStrings > 3)
			setSource(msg[1],msg[2],msg[3]);
		}
	else if (!strcasecmp(msg[0],"setbuffer"))
		{
		if (numStrings > 1)
			{
			initialBufferSamples_ = (int)(atof(msg[1])*sampleRate_);
			if (initialBufferSamples_ > MAX_SAMPLEBUFFER_LENGTH)
				initialBufferSamples_ = MAX_SAMPLEBUFFER_LENGTH;
			}
		}
	else
		ASound::message(msg,numStrings);
	}
