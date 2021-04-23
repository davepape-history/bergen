#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "bergenServer.h"


main(int argc,char **argv)
	{
	bergenServer * server;
	server = new bergenServer(getenv("BERGEN_SERVER"));
	server->sendMessage("-1 reset");
	delete server;
	}
