/*
 *  bergen client library, Version 0.4.1  
 */

#ifndef _bergenUDPSocket_h_
#define _bergenUDPSocket_h_

#include <netinet/in.h>

class bergenUDPSocket
		{
		public:
		 bergenUDPSocket(int port);
		 void setDestination(char *host,int port);
		 void setBlocking(int block);
		 void send(void *buffer,int size);
		 int receive(void *buffer,int size);
		 void reply(void *buffer,int size);
		private:
		 int port_;
		 struct sockaddr_in destAddr_;
		 struct sockaddr_in lastFromAddr_;
		 int socket_;
		};

#endif
