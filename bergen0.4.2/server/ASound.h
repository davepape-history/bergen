#ifndef _ASound_h_
#define _ASound_h_

class ASound
		{
		public:
		 ASound(int bufsize=1000);
		 virtual ~ASound(void);
		 virtual short * updateBuffer(void);
		 virtual int removable(void);
		 virtual int active(void);
		 	 int handle(void);
		 	 short * buffer(void);
		 	 int bufferSize(void);
		 virtual void setAmplitude(float amp);
		 virtual float amplitude(void);
		 virtual void play(void);
		 virtual void stop(void);
		 virtual void pause(void);
		 virtual void kill(void);
		 virtual void message(char **msg,int num);
		 	 void setSampleRate(int rate);
		protected:
		 short * buffer_;
		 int bufferSize_;
		 int handle_;
		 float amplitude_;
		 int sampleRate_;
		private:
		 static int NextHandle;
		};

#endif
