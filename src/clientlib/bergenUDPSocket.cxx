#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "bergenUDPSocket.h"

bergenUDPSocket::bergenUDPSocket(int port)
	{
	struct sockaddr_in addr;
	socket_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_ == -1)
		{
		fprintf(stderr,"ERROR: bergenUDPSocket failed to create a socket\n");
		perror("socket");
		return;
		}
	memset(&addr,0,sizeof(addr));
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = port;
	if (bind(socket_, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
		fprintf(stderr,"ERROR: bergenUDPSocket failed to bind to port %d\n",
			port);
		perror("bind");
		socket_ = -1;
		return;
		}
	}


void bergenUDPSocket::setBlocking(int block)
	{
	if (socket_ == -1)
		return;
	if (block)
		fcntl(socket_, F_SETFL, fcntl(socket_, F_GETFL) & (~FNDELAY));
	else
		fcntl(socket_, F_SETFL, fcntl(socket_, F_GETFL) | FNDELAY);
	}


void bergenUDPSocket::setDestination(char *host,int port)
	{
	struct hostent *hp;
	if (!(hp = gethostbyname(host)))
		{
		fprintf(stderr,"ERROR: bergenUDPSocket::setDestination failed "
			"to get address for \"%s\"\n",host);
		perror("gethostbyname");
		return;
		}
	memset(&destAddr_,0,sizeof(destAddr_));
	memcpy(&destAddr_.sin_addr, hp->h_addr, hp->h_length);
	destAddr_.sin_family = hp->h_addrtype;
	destAddr_.sin_port = port;
	}


void bergenUDPSocket::send(void *buffer,int size)
	{
	if (socket_ == -1)
		return;
	sendto(socket_,buffer,size,0,(struct sockaddr *)&destAddr_,sizeof(destAddr_));
	}


int bergenUDPSocket::receive(void *buffer,int size)
	{
	if (socket_ == -1)
		return 0;
	struct sockaddr_in fromAddr;
#ifdef __linux__
	unsigned int fromLen;
#else
	int fromLen;
#endif
	fromLen = sizeof(fromAddr);
	int retval = recvfrom(socket_,buffer,size,0,(struct sockaddr *)&fromAddr,&fromLen);
	lastFromAddr_ = fromAddr;
	return retval;
	}


void bergenUDPSocket::reply(void *buffer,int size)
	{
	if (socket_ == -1)
		return;
	sendto(socket_,buffer,size,0,(struct sockaddr *)&lastFromAddr_,sizeof(lastFromAddr_));
	}
