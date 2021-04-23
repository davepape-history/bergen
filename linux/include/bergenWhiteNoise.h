#ifndef _bergenWhiteNoise_h_
#define _bergenWhiteNoise_h_

#include "bergenSound.h"

class bergenWhiteNoise : public bergenSound
		{
		public:
		 bergenWhiteNoise(bergenServer *server);
		 virtual void createRemote(void);	/* This should only be called by bergenServer */
		private:
		};

#endif
