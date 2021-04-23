#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include "bergenServer.h"
#include "bergenNetStream.h"


main(int argc,char **argv)
	{
	float duration = 10;
	bergenServer * server;
	bergenNetStream * sound;
	if (argc < 3)
		{
		fprintf(stderr,"Usage: %s hostname port [duration]\n",argv[0]);
		exit(1);
		}
	if (argc > 3)
		duration = atof(argv[3]);
	server = new bergenServer;
	sound = new bergenNetStream(server);
	sound->setSource(argv[1],atoi(argv[2]),duration);
	sound->play();
	sleep((int)(duration+0.5));
	delete sound;
	delete server;
	return 0;
	}
