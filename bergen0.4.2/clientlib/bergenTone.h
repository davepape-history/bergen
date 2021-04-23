/*
 *  bergen client library, Version 0.4.1  
 */

#ifndef _bergenTone_h_
#define _bergenTone_h_

#include "bergenSound.h"

class bergenTone : public bergenSound
		{
		public:
		 bergenTone(bergenServer *server);
		 virtual void setFrequency(float freq);
		 virtual void createRemote(void);	/* This should only be called by bergenServer */
		private:
		};

#endif
