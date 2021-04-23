/*
 *  bergen client library, Version 0.4.1
 */

#ifndef _bergenSample_h_
#define _bergenSample_h_

#include "bergenSound.h"

class bergenSample : public bergenSound
		{
		public:
		 bergenSample(char *filename,bergenServer *server);
		 virtual void setLoop(int loop);
		 virtual void createRemote(void);	/* This should only be called by bergenServer */
		 virtual float duration(void);
		private:
		 char *filename_;
		};

#endif
