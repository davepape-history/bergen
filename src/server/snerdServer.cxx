#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "snerdDB.h"
#include "bergenUDPSocket.h"
#include "SampleFile.h"
#include "Tone.h"
#include "WhiteNoise.h"
#include "audioIface.h"
#include "snerdServer.h"

snerdDB * snerdServer::db_ = NULL;
bergenUDPSocket * snerdServer::socket_ = NULL;
bool snerdServer::looping_ = false;
 

void snerdServer::init(int bufSize, int sampleRate, bergenUDPSocket *sock)
	{
	db_ = new snerdDB(bufSize);
	db_->setSampleRate(sampleRate);
	socket_ = sock;
	}


void snerdServer::mainLoop(void)
	{
	looping_ = true;
	while (looping_)
		{
		processMessages();
		db_->update();
		snerdWriteToAudioPort(db_->buffer(),db_->bufferSize());
		}
	}


void snerdServer::processMessages(void)
	{
	char message[1024], *msgStrings[32];
	int numStrings;
	while (socket_->receive(message,sizeof(message)) > 0)
		{
#ifdef DEBUG
		printf("got message '%s'\n",message);
#endif
		numStrings = splitMessage(message, msgStrings, 32);
		if (numStrings <= 0)
			continue;
		int handle = atoi(msgStrings[0]);
		if (handle == -1)
			parseMessage(msgStrings,numStrings);
		else
			{
			ASound * sound = db_->findHandle(handle);
			if (sound)
				sound->message(msgStrings+1,numStrings-1);
			else
				printf("warning: couldn't find sound with handle %d\n",handle);
			}
		while (numStrings)
			{
			numStrings--;
			free(msgStrings[numStrings]);
			}
		}
	}


int snerdServer::splitMessage(char *message,char **strings,int max)
	{
	int i=0, start, len, count=0;
	while ((message[i]) && (count < max))
		{
		i += strspn(message+i, " \t\n");
		if (!message[i])
			break;
		start = i;
		len = strcspn(message+start, " \t\n");
		strings[count] = (char *) malloc(len+1);
		strncpy(strings[count],message+start,len);
		strings[count][len] = '\0';
		count++;
		i += len;
		}
	return count;
	}


void snerdServer::parseMessage(char **message,int numStrings)
	{
	if (numStrings < 2)
		return;
	if (!strcasecmp(message[1],"new"))
		parseNewSoundMessage(message,numStrings);
/**** Backwards compatibility for older bergen clients ****/
	else if (!strcasecmp(message[1],"newsample"))
		{
		SampleFile * sound;
		if (numStrings > 2)
			sound = new SampleFile(message[2],db_->bufferSize());
		else
			sound = new SampleFile("",db_->bufferSize());
		db_->addSound(sound);
		returnInt(sound->handle());
		}
	else if (!strcasecmp(message[1],"newtone"))
		{
		Tone * sound;
		sound = new Tone(db_->bufferSize());
		db_->addSound(sound);
		returnInt(sound->handle());
		}
	else if (!strcasecmp(message[1],"newwhitenoise"))
		{
		WhiteNoise * sound;
		sound = new WhiteNoise(db_->bufferSize());
		db_->addSound(sound);
		returnInt(sound->handle());
		}
/****  End backwards compatibility section ****/
	else if (!strcasecmp(message[1],"gain"))
		{
		if (numStrings > 2)
			db_->setGain(atof(message[2]));
		}
	else if (!strcasecmp(message[1],"cd"))
		{
		if (numStrings > 2)
			chdir(message[2]);
		}
	else if (!strcasecmp(message[1],"kill"))
		{
		looping_ = false;
		}
	else if (!strcasecmp(message[1],"reset"))
		{
		db_->reset();
		}
	else if (!strcasecmp(message[1],"ping"))
		{
		char * reply = "pong";
		returnMessage(reply,strlen(reply)+1);
		}
	else
		fprintf(stderr,"ERROR: unknown message \"%s\"\n",message[1]);
	}


void snerdServer::parseNewSoundMessage(char **message,int numStrings)
	{
	if (numStrings < 3)
		return;
	ASound * sound = NULL;
	if (!strcasecmp(message[2],"sample"))
		{
		if (numStrings > 3)
			sound = new SampleFile(message[3],db_->bufferSize());
		else
			sound = new SampleFile("",db_->bufferSize());
		}
	else if (!strcasecmp(message[2],"tone"))
		sound = new Tone(db_->bufferSize());
	else if (!strcasecmp(message[2],"whitenoise"))
		sound = new WhiteNoise(db_->bufferSize());
	else
		fprintf(stderr,"ERROR: asked to create unknown sound type '%s'\n",message[2]);
	if (sound)
		{
		db_->addSound(sound);
		returnInt(sound->handle());
		}
	}


void snerdServer::returnMessage(void *msg,int len)
	{
	socket_->reply(msg,len);
	}


void snerdServer::returnInt(int val)
	{
	char msg[32];
	sprintf(msg,"%d",val);
	socket_->reply(msg,strlen(msg)+1);
	}


void snerdServer::returnFloat(float val)
	{
	char msg[32];
	sprintf(msg,"%f",val);
	socket_->reply(msg,strlen(msg)+1);
	}
