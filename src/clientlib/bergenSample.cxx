#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bergenServer.h"
#include "bergenSample.h"

static void getFullPath(char *filename,bergenServer *server,char *fullpath);


bergenSample::bergenSample(char *filename,bergenServer *server) : bergenSound(server)
	{
	if (filename)
		{
		filename_ = strdup(filename);
		createRemote();
		}
	else
		{
		fprintf(stderr,"ERROR: bergenSample created with NULL file name\n");
		filename_ = NULL;
		}
	}


void bergenSample::createRemote(void)
	{
	if (server_)
		{
		char cmd[256], reply[256], fullpath[256];
		getFullPath(filename_,server_,fullpath);
		sprintf(cmd,"-1 newsample %s",fullpath);
		server_->sendMessage(cmd);
		server_->receiveMessage(reply,sizeof(reply));
		handle_ = atoi(reply);
		}
	else
		handle_ = 0;
	}


void bergenSample::setLoop(int loop)
	{
	if (server_)
		{
		char message[256];
		sprintf(message,"%d loop %d", handle_, loop);
		server_->sendMessage(message);
		}
	}


static void getFullPath(char *filename,bergenServer *server,char *fullpath)
	{
	if (!filename)
		fullpath[0] = 0;
	else if ((filename[0] == '/') || ((filename[0] == '.') && (filename[1] == '/')))
		strcpy(fullpath,filename);
	else if ((server) && (server->directory()))
		sprintf(fullpath,"%s/%s",server->directory(),filename);
	else
		strcpy(fullpath,filename);
	}


float bergenSample::duration(void)
	{
	if (server_)
		{
		char message[256], reply[256];
		sprintf(message,"%d getduration", handle_);
		server_->sendMessage(message);
		server_->receiveMessage(reply,sizeof(reply));
		return atof(reply);
		}
	return 0;
	}
