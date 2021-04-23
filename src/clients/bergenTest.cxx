#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "bergenServer.h"
#include "bergenTone.h"
#include "bergenWhiteNoise.h"


int main(int argc,char **argv)
	{
	bergenServer * server = new bergenServer;
	bergenTone * tone = new bergenTone(server);
	tone->setAmplitude(0.25);
	tone->play();
	sleep(3);
	tone->setFrequency(2000.0);
	sleep(1);
	tone->setFrequency(500.0);
	sleep(1);
	delete tone;
	bergenSound * sound = new bergenWhiteNoise(server);
	sound->setAmplitude(0.25);
	sound->play();
	sleep(3);
	delete sound;
	delete server;
	return 0;
	}
