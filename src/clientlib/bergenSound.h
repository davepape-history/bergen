#ifndef _bergenSound_h_
#define _bergenSound_h_


class bergenSound
		{
		public:
		 bergenSound(class bergenServer *server);
		 ~bergenSound(void);
		 inline	 int handle(void) const { return handle_; }
		 inline	 class bergenServer * server(void) const { return server_; }
		 virtual void setAmplitude(float amp);
		 virtual void play(void);
		 virtual void stop(void);
		 virtual void pause(void);
		 virtual void kill(void);
		 virtual void killRemote(void);		/* This should only be called by bergenServer */
		 virtual void createRemote(void);	/* This should only be called by bergenServer */
		protected:
		 class bergenServer * server_;
		 int handle_;
		 bool isPlaying_;
		};

#endif
