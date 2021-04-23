#ifndef _snerdDB_h_
#define _snerdDB_h_

#include "ASound.h"


class snerdDB
		{
		public:
		 snerdDB(int bufsize=1000);
		 ~snerdDB(void);
		 void addSound(ASound *);
		 ASound * findHandle(int);
		 void update(void);
		 short * buffer(void);
		 int bufferSize(void);
		 void reset(void);
		 void setSampleRate(int rate);
		 void setGain(float);
		private:
		 short * buffer_;
		 int bufferSize_;
		 ASound ** sounds_;
		 int maxSoundIndex_;
		 int sampleRate_;
		 float gain_;
		};

#endif
