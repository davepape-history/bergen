include Makedefs.sgi
#include Makedefs.linux

LIBOBJS = bergenServer.o bergenSound.o bergenSample.o bergenUDPSocket.o \
	bergenTone.o bergenWhiteNoise.o bergenNetStream.o

libbergen.a: $(LIBOBJS)
	rm -f libbergen.a
	ar ru libbergen.a $(LIBOBJS)
