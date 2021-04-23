#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "snerdClient.h"
#include "snerdDB.h"
#include "bergenUDPSocket.h"
#include "SampleFile.h"
#include "Tone.h"
#include "WhiteNoise.h"
#include "NetStream.h"
#include "audioInterface.h"
#include "snerdServer.h"

snerdDB * snerdServer::db_ = NULL;
bergenUDPSocket * snerdServer::socket_ = NULL;
audioInterface * snerdServer::interface_ = NULL;
bool snerdServer::looping_ = false;
 

void snerdServer::init(audioInterface *interface, bergenUDPSocket *sock)
	{
	interface_ = interface;
	db_ = new snerdDB(interface_->bufferSize());
	db_->setSampleRate(interface_->sampleRate());
	socket_ = sock;
	}


void snerdServer::mainLoop(void)
	{
	looping_ = true;
	while (looping_)
		{
		processMessages();
		db_->update();
		interface_->writeSamples(db_->buffer(), db_->bufferSize());
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
		snerdClient client(socket_->lastFrom());
		numStrings = splitMessage(message, msgStrings, 32);
		if (numStrings <= 0)
			continue;
		int handle = atoi(msgStrings[0]);
		if (handle == -1)
			parseMessage(client,msgStrings,numStrings);
		else
			{
			ASound * sound = db_->findSound(client,handle);
			if (sound)
				sound->message(msgStrings+1,numStrings-1);
			else
				printf("Warning: couldn't find sound with handle"
					" %d\n",handle);
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


void snerdServer::parseMessage(const snerdClient& client,char **message,
				int numStrings)
	{
	if (numStrings < 2)
		return;
	if (!strcasecmp(message[1],"new"))
		parseNewSoundMessage(client,message,numStrings);
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
		returnMessage(client,"snerd 500");  /* i.e. version 0.05.00 */
		}
	else
		fprintf(stderr,"ERROR: unknown message \"%s\"\n",message[1]);
	}


void snerdServer::parseNewSoundMessage(const snerdClient& client,char **message,
					int numStrings)
	{
	if (numStrings < 4)
		return;
	ASound * sound = NULL;
	int handle = atoi(message[3]);
	if (!strcasecmp(message[2],"sample"))
		{
		char *file="";
		if (numStrings > 4)
			file = message[4];
		sound = new SampleFile(file, client, handle, db_->bufferSize());
		}
	else if (!strcasecmp(message[2],"netstream"))
		sound = new NetStream(client, handle, db_->bufferSize());
	else if (!strcasecmp(message[2],"tone"))
		sound = new Tone(client, handle, db_->bufferSize());
	else if (!strcasecmp(message[2],"whitenoise"))
		sound = new WhiteNoise(client, handle, db_->bufferSize());
	else
		fprintf(stderr,"ERROR: asked to create unknown sound type "
			"'%s'\n", message[2]);
	if (sound)
		db_->addSound(sound);
	}


void snerdServer::returnMessage(const snerdClient& client,char *msg)
	{
	returnMessage(client,msg,strlen(msg)+1);
	}


void snerdServer::returnMessage(const snerdClient& client,void *msg,int len)
	{
	socket_->setDestination(client.address());
	socket_->send(msg,len);
	}
