#include <stdio.h>
#include <malloc.h>
#include "bergenServer.h"
#include "bergenSound.h"


bergenSound::bergenSound(bergenServer *server)
	{
	server_ = server;
	handle_ = 0;
	isPlaying_ = false;
	if (server)
		server->addSound(this);
	else
		fprintf(stderr,"ERROR: a sound was created with a NULL server pointer; "
			"this may core dump\n");
	}


bergenSound::~bergenSound(void)
	{
	kill();
	}


void bergenSound::setAmplitude(float amp)
	{
	if (server_)
		{
		char message[256];
		sprintf(message,"%d amplitude %.4f", handle_, amp);
		server_->sendMessage(message);
		}
	}


void bergenSound::play(void)
	{
	if (server_)
		{
		char message[256];
		sprintf(message,"%d play", handle_);
		server_->sendMessage(message);
		}
	isPlaying_ = true;
	}


void bergenSound::stop(void)
	{
	if (server_)
		{
		char message[256];
		sprintf(message,"%d stop", handle_);
		server_->sendMessage(message);
		}
	isPlaying_ = false;
	}


void bergenSound::pause(void)
	{
	if (server_)
		{
		char message[256];
		sprintf(message,"%d pause", handle_);
		server_->sendMessage(message);
		}
	isPlaying_ = false;
	}


void bergenSound::kill(void)
	{
	killRemote();
	if (server_)
		server_->removeSound(this);
	isPlaying_ = false;
	}


void bergenSound::killRemote(void)
	{
	if (server_)
		{
		char message[256];
		sprintf(message,"%d kill", handle_);
		server_->sendMessage(message);
		}
	}


void bergenSound::createRemote(void)
	{
	}
