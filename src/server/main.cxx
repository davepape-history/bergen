#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snerdServer.h"
#include "bergenUDPSocket.h"
#include "audioIface.h"


main(int argc,char **argv)
	{
	printf("snerd (bergen sound server) - Version 0.4.1 - Compiled " __DATE__ "\n");
	int sampleRate = 32000, device=0, udpPort=5900, i;
	for (i=1; i < argc-1; i++)
		{
		if (!strcmp(argv[i],"-srate"))
			sampleRate = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-device"))
			device = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-udpport"))
			udpPort = atoi(argv[++i]);
		else
			fprintf(stderr,"Warning: unknown command-line option '%s'\n",
				argv[i]);
		}
	snerdOpenAudioPort(device,snerdAudioBufferSize,sampleRate);
	bergenUDPSocket * socket = new bergenUDPSocket(udpPort);
	socket->setBlocking(0);
	snerdServer::init(snerdAudioBufferSize, sampleRate, socket);
	snerdServer::mainLoop();
	delete socket;
	}
