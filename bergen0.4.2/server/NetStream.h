#ifndef _NetStream_h_
#define _NetStream_h_

#include "ASound.h"

class NetStream : public ASound
		{
		public:
		 NetStream(int bufsize=1000);
		 ~NetStream(void);
		 virtual short * updateBuffer(void);
		 virtual int removable(void);
		 virtual int active(void);
		 virtual void play(void);
		 virtual void stop(void);
		 virtual void pause(void);
		 virtual void kill(void);
		 virtual void message(char **msg,int);
		 virtual void setSource(char *,char*,char*);
		private:
		 void parsePacket(int nbytes);
		 int getChunk(char *buffer,int nbytes,int& chunkType,int& dataSize,char*& dataPointer);
		 void appendSampleData(char *data,int size);
		 int playing_;
		 int killed_;
		 class bergenUDPSocket * socket_;
		 short * sampleBuffer_;
		 char * packetBuffer_;
		 int sampleWritePos_, sampleReadPos_;
		 bool buffering_;
		 int initialBufferSamples_;
		};


#define BERGEN_NET_SAMPLEDATA		1
#define BERGEN_NET_REQUESTSTREAM	2


#endif
