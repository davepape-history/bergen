include Makedefs.sgi
#include Makedefs.linux

OBJS = main.o snerdServer.o snerdDB.o $(AUDIOIFACE) bergenUDPSocket.o \
	ASound.o SampleFile.o Tone.o WhiteNoise.o

snerd: $(OBJS)
	$(C++) $(LFLAGS) -o snerd $(OBJS) $(LIBS)

