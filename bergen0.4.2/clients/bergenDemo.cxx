#include <unistd.h>
#include "bergenServer.h"
#include "bergenSample.h"


main(int argc,char **argv)
	{
	bergenServer * server;
	bergenSample * sound;
	server = new bergenServer;
	sound = new bergenSample(argv[1],server);
	sound->setLoop(1);
	sound->play();
	sleep(1);
	sound->setAmplitude(0.25);
	sleep(1);
	delete sound;
	delete server;
	}
