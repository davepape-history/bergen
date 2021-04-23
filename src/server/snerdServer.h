#ifndef _snerdServer_h_
#define _snerdServer_h_


class snerdServer
	{
	public:
	 static void init(int bufSize, int sampleRate, class bergenUDPSocket *sock);
	 static void mainLoop(void);
	 static void returnMessage(void *msg,int len);
	 static void returnInt(int val);
	 static void returnFloat(float val);
	private:
	 static void processMessages(void);
	 static int splitMessage(char *message,char **strings,int max);
	 static void parseMessage(char **message,int numStrings);
	 static void parseNewSoundMessage(char **message,int numStrings);
	 static class snerdDB * db_;
	 static class bergenUDPSocket * socket_;
	 static bool looping_;
	};


#endif
