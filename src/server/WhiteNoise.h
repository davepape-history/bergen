#ifndef _WhiteNoise_h_
#define _WhiteNoise_h_

#include "ASound.h"

class WhiteNoise : public ASound
		{
		public:
		 WhiteNoise(int bufsize=1000);
		 ~WhiteNoise(void);
		 virtual short * updateBuffer(void);
		 virtual int removable(void);
		 virtual int active(void);
		 virtual void play(void);
		 virtual void stop(void);
		 virtual void pause(void);
		 virtual void kill(void);
		private:
		 int playing_;
		 int killed_;
		};

#endif
