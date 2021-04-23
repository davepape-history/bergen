#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "bergenUDPSocket.h"

#define DEFAULT_PORT 5000

#define SAMPLE_SIZE 6400

int currentTime(void);


int main(int argc,char **argv)
{
 bergenUDPSocket *socket = new bergenUDPSocket(0);
 struct { short t,s; short data[SAMPLE_SIZE]; } packet;
 int count=0, i, steps, startTime;
 if (argc > 2)
	socket->setDestination(argv[1],atoi(argv[2]));
 else if (argc > 1)
	socket->setDestination(argv[1],DEFAULT_PORT);
 else
	socket->setDestination("localhost",DEFAULT_PORT);
 packet.t = htons(1);
 packet.s = htons(SAMPLE_SIZE*2);
 startTime = currentTime();
 steps = 0;
 while (1)
	{
	short v;
	for (i=0; i < SAMPLE_SIZE; i++, count++)
		{
		v = (short) (sinf(count * (100) * 2.0f * M_PI / 32000)
                                        * 32768.0f * 0.5);
		packet.data[i] = htons(v);
		}
	socket->send((unsigned char *)&packet,sizeof(packet));
	steps++;
	if (steps % 20 == 0)
		{
		int t = currentTime() - startTime;
		printf("%d.%06d:  %d samples sent;  %f samples/sec\n",
			t/1000000,t%1000000,count, ((float)count/t)*1000000.0);
		}
	usleep(190000);
	}
 delete socket;
}

#include <sys/time.h>

int currentTime(void)
	{
	static struct timeval start;
	static bool first=true;
	struct timeval now;
	if (first)
		{
		first = false;
		gettimeofday(&start,NULL);
		}
	gettimeofday(&now,NULL);
	return (now.tv_usec - start.tv_usec) + (now.tv_sec - start.tv_sec) * 1000000;
	}
