include Makedefs.sgi
#include Makedefs.linux
#include Makedefs.hpux

OBJS = main.o snerdServer.o snerdDB.o $(AUDIOIFACE) bergenUDPSocket.o \
	ASound.o SampleFile.o Tone.o WhiteNoise.o NetStream.o

snerd: $(OBJS)
	$(C++) $(LFLAGS) -o snerd $(OBJS) $(LIBS)

