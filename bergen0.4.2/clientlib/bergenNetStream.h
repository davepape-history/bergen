/*
 *  bergen client library, Version 0.4.2  
 */

#ifndef _bergenNetStream_h_
#define _bergenNetStream_h_

#include "bergenSound.h"

class bergenNetStream : public bergenSound
		{
		public:
		 bergenNetStream(bergenServer *server);
		 void setSource(char *host,int port,float duration);
		 void setBuffer(float seconds);
		protected:
		 virtual void createRemote(void);
		 friend class bergenServer;
		private:
		};

#endif
