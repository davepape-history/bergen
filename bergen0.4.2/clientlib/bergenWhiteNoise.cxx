#include <stdio.h>
#include <stdlib.h>
#include "bergenServer.h"
#include "bergenWhiteNoise.h"


bergenWhiteNoise::bergenWhiteNoise(bergenServer *server) : bergenSound(server)
	{
	createRemote();
	}


void bergenWhiteNoise::createRemote(void)
	{
	if (server_)
		{
		char cmd[256], reply[256];
		sprintf(cmd,"-1 newwhitenoise");
		server_->sendMessage(cmd);
		server_->receiveMessage(reply,sizeof(reply));
		handle_ = atoi(reply);
		}
	else
		handle_ = 0;
	}
