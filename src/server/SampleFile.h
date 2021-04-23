#ifndef _SampleFile_h_
#define _SampleFile_h_

#ifdef AUDIOFILE_LIBRARY

#ifdef __sgi
#include <dmedia/audiofile.h>
#else
/* Linux */
#include <audiofile.h>
#endif

#else
/* No SGI audiofile library */
typedef int AFfilehandle;
#define AF_NULL_FILEHANDLE 0
#define AF_SAMPFMT_TWOSCOMP 1
#define AF_SAMPFMT_UNSIGNED 2

#endif


#include "ASound.h"

class SampleFile : public ASound
		{
		public:
		 SampleFile(char *filename,int bufsize=1000);
		 ~SampleFile(void);
		 virtual void setLoop(int loop);
		 virtual int loop(void);
		 virtual char * filename(void);
		 virtual void play(void);
		 virtual void stop(void);
		 virtual void pause(void);
		 virtual void kill(void);
		 virtual short * updateBuffer(void);
		 virtual int removable(void);
		 virtual int active(void);
		 virtual void message(char **,int);
		private:
		 char *filename_;
		 int isRawFile_;
		 int rawfd_;
		 AFfilehandle afFile_;
		 int loop_;
		 int playing_;
		 int channels_;
		 int sampleFormat_;
		 int sampleWidth_;
		 int bytesPerSample_;
		 int fileRate_;
		 unsigned char * tempBuf_;
		 short * inputBuf_;
		 int inputFrames_, frameSize_;
		 int numFrames_;
		 int killed_;
		 int readFrames(void);
		 void convertFrames(void);
		 void initializeRawFile(void);
		 void initializeAFFile(void);
		 int readRawFrames(void);
		 int readAFFrames(void);
		};

#endif
