#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "bergenUDPSocket.h"

#define SAMPLE_SIZE 4410

int currentTime(void);


int main(int argc,char **argv)
{
 bergenUDPSocket *socket = new bergenUDPSocket(0);
 struct { short t,s; short data[SAMPLE_SIZE]; } packet;
 int count=0, i, steps, startTime;
 FILE *fp;
 if (argc < 2)
	{
	fprintf(stderr,"Usage: %s file [host [port]]\n",argv[0]);
	exit(1);
	}
 fp = fopen(argv[1],"r");
 if (!fp)
	{
	perror(argv[1]);
	exit(1);
	}
 if (argc > 3)
	socket->setDestination(argv[2],atoi(argv[3]));
 else if (argc > 2)
	socket->setDestination(argv[2],5000);
 else
	socket->setDestination("localhost",5000);
 packet.t = htons(1);
 packet.s = htons(SAMPLE_SIZE*2);
 startTime = currentTime();
 steps = 0;
 while (1)
	{
	int nread = fread(packet.data,sizeof(short),SAMPLE_SIZE,fp);
	if (nread <= 0)
		rewind(fp);
	for (i=0; i < nread; i++, count++)
		packet.data[i] = htons(packet.data[i]);
	for (; i < SAMPLE_SIZE; i++, count++)
		packet.data[i] = 0;
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
