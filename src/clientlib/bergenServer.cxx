#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "bergenUDPSocket.h"
#include "bergenSound.h"
#include "bergenServer.h"


char * bergenVersion = "bergen client library, Version 0.4.1 - Compiled " __DATE__;


struct _sound
		{
		bergenSound * sound;
		struct _sound * next;
		};


bergenServer::bergenServer(char *server,int port)
	{
	soundList_ = NULL;
	directory_ = NULL;
	if (server)
		serverName_ = strdup(server);
	else if (getenv("BERGEN_SERVER"))
		serverName_ = strdup(getenv("BERGEN_SERVER"));
	else
		serverName_ = "localhost";
	if (port)
		serverPort_ = port;
	else if (getenv("BERGEN_PORT"))
		serverPort_ = atoi(getenv("BERGEN_PORT"));
	else
		serverPort_ = 5900;
	socket_ = new bergenUDPSocket(0);
	socket_->setDestination(serverName_,serverPort_);
	connect();
	}


bergenServer::~bergenServer(void)
	{
	struct _sound *s,*next=NULL;
	for (s=soundList_; s; s = next)
		{
		next = s->next;
		delete s->sound;
		delete s;
		}
	if (directory_)
		free(directory_);
	}


void bergenServer::connect(void)
	{
#define NUM_TRIES 2
#define WAIT_TICKS 50
	int tries, wait;
	if (!socket_)
		{
		fprintf(stderr,"ERROR (bergenServer::connect()): don't have a socket to use for connection\n");
		return;
		}
	socket_->setBlocking(0);
	for (tries=0; tries < NUM_TRIES; tries++)
		{
		sendMessage("-1 ping");
		for (wait=0; wait < WAIT_TICKS; wait++)
			{
			char replybuf[256];
			if (socket_->receive(replybuf,sizeof(replybuf)) > 0)
				break;
			usleep(10000);
			}
		if (wait < WAIT_TICKS)
			break;
		}
	if (tries == NUM_TRIES)
		{
		fprintf(stderr,"WARNING: bergenServer failed to connect to server (%s)\n",serverName_);
		delete socket_;
		socket_ = NULL;
		}
	else
		socket_->setBlocking(1);
	}


void bergenServer::reset(void)
	{
	struct _sound *s;
	for (s=soundList_; s; s = s->next)
		s->sound->killRemote();
	if (!socket_)
		{
		socket_ = new bergenUDPSocket(0);
		socket_->setDestination(serverName_,serverPort_);
		}
	connect();
	if (socket_)
		for (s=soundList_; s; s = s->next)
			s->sound->createRemote();
	}


void bergenServer::addSound(bergenSound * sound)
	{
	struct _sound *s = new struct _sound;
	s->sound = sound;
	s->next = soundList_;
	soundList_ = s;
	}


void bergenServer::removeSound(bergenSound * sound)
	{
	struct _sound *s, *removed=NULL;
	if (!soundList_)
		return;
	if (soundList_->sound == sound)
		{
		removed = soundList_;
		soundList_ = soundList_->next;
		}
	else
		for (s=soundList_; s->next; s = s->next)
			{
			if (s->next->sound == sound)
				{
				removed = s->next;
				s->next = s->next->next;
				break;
				}
			}
	if (removed)
		delete removed;
	}


void bergenServer::setDirectory(char *dir)
	{
	if (directory_)
		free(directory_);
	if (dir)
		directory_ = strdup(dir);
	else
		directory_ = NULL;
	}


char * bergenServer::directory(void)
	{
	return directory_;
	}


void bergenServer::sendMessage(char *msg)
	{
	if (socket_)
		socket_->send(msg,strlen(msg)+1);
	}


int bergenServer::receiveMessage(char *msg,int size)
	{
	if (socket_)
		return socket_->receive(msg,size);
	else
		return 0;
	}
