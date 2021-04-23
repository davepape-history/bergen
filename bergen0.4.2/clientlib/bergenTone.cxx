#include <stdio.h>
#include <stdlib.h>
#include "bergenServer.h"
#include "bergenTone.h"


bergenTone::bergenTone(bergenServer *server) : bergenSound(server)
	{
	createRemote();
	}


void bergenTone::createRemote(void)
	{
	if (server_)
		{
		char cmd[256], reply[256];
		sprintf(cmd,"-1 newtone");
		server_->sendMessage(cmd);
		server_->receiveMessage(reply,sizeof(reply));
		handle_ = atoi(reply);
		}
	else
		handle_ = 0;
	}


void bergenTone::setFrequency(float freq)
	{
	if (server_)
		{
		char message[256];
		sprintf(message,"%d setfrequency %.2f",handle_,freq);
		server_->sendMessage(message);
		}
	}
