#ifndef _bergenServer_h_
#define _bergenServer_h_


class bergenServer
		{
		public:
		 bergenServer(char *server=NULL,int port=0);
		 ~bergenServer(void);
		 void reset(void);
		 void addSound(class bergenSound *);
		 void removeSound(class bergenSound *);
		 void setDirectory(char *);
		 char * directory(void);
		 void sendMessage(char *msg);
		 int receiveMessage(char *msg,int size);
		private:
		 void connect(void);
		 char *serverName_;
		 int serverPort_;
		 struct _sound * soundList_;
		 char * directory_;
		 class bergenUDPSocket * socket_;
		};


extern char * bergenVersion;

#endif
