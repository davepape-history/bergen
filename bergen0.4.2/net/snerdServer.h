#ifndef _snerdServer_h_
#define _snerdServer_h_


class snerdClient;


class snerdServer
	{
	public:
	 static void init(class audioInterface*, class bergenUDPSocket*);
	 static void mainLoop(void);
	 static void returnMessage(const snerdClient&,char *msg);
	 static void returnMessage(const snerdClient&,void *msg,int len);
	private:
	 static void processMessages(void);
	 static int splitMessage(char *message,char **strings,int max);
	 static void parseMessage(const snerdClient&,char **message,int numStrings);
	 static void parseNewSoundMessage(const snerdClient&,char **message,int numStrings);
	 static class snerdDB * db_;
	 static class bergenUDPSocket * socket_;
	 static class audioInterface * interface_;
	 static bool looping_;
	};


#endif
