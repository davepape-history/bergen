#ifndef _Tone_h_
#define _Tone_h_

#include "ASound.h"

class Tone : public ASound
		{
		public:
		 Tone(int bufsize=1000);
		 ~Tone(void);
		 virtual short * updateBuffer(void);
		 virtual int removable(void);
		 virtual int active(void);
		 virtual void play(void);
		 virtual void stop(void);
		 virtual void pause(void);
		 virtual void kill(void);
		 virtual void message(char **msg,int);
		 virtual void setFrequency(int freq);
		private:
		 int frequency_;
		 int playing_;
		 int killed_;
		 unsigned int count_;
		};

#endif
