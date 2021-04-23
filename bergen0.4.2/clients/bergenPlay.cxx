#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "bergenServer.h"
#include "bergenSample.h"


main(int argc,char **argv)
	{
	bergenServer * server;
	bergenSample * sound;
	server = new bergenServer;
	sound = new bergenSample(argv[1],server);
	sound->play();
	return 0;
	}
