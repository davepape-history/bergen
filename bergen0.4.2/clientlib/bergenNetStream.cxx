#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bergenServer.h"
#include "bergenNetStream.h"

static void getFullPath(char *filename,bergenServer *server,char *fullpath);


bergenNetStream::bergenNetStream(bergenServer *server) :
		bergenSound(server)
	{
	createRemote();
	}


void bergenNetStream::createRemote(void)
	{
	if (server())
		{
		char cmd[256],reply[256];
		sprintf(cmd,"-1 new netstream");
		server()->sendMessage(cmd);
		server_->receiveMessage(reply,sizeof(reply));
		handle_ = atoi(reply);
		}
	else
		handle_ = 0;
	}


void bergenNetStream::setSource(char *host,int port,float duration)
	{
	if (server_)
		{
		char message[512];
		sprintf(message,"%d setsource %s %d %.3f",handle_,host,port,duration);
		server_->sendMessage(message);
		}
	}


void bergenNetStream::setBuffer(float seconds)
	{
	if (server_)
		{
		char message[64];
		sprintf(message,"%d setbuffer %.4f",handle_,seconds);
		server_->sendMessage(message);
		}
	}
