#include Makedefs.sgi
include Makedefs.linux
#include Makedefs.hpux


POBJS = bergenPlay.o

bergenPlay: $(POBJS)
	$(C++) $(LFLAGS) -o bergenPlay $(POBJS) $(LIBS)



ROBJS = bergenReset.o

bergenReset: $(ROBJS)
	$(C++) $(LFLAGS) -o bergenReset $(ROBJS) $(LIBS)


KOBJS = bergenKill.o

bergenKill: $(KOBJS)
	$(C++) $(LFLAGS) -o bergenKill $(KOBJS) $(LIBS)


DOBJS = bergenDemo.o

bergenDemo: $(DOBJS)
	$(C++) $(LFLAGS) -o bergenDemo $(DOBJS) $(LIBS)


TOBJS = bergenTest.o

bergenTest: $(TOBJS)
	$(C++) $(LFLAGS) -o bergenTest $(TOBJS) $(LIBS)



GOBJS = bergenGain.o

bergenGain: $(GOBJS)
	$(C++) $(LFLAGS) -o bergenGain $(GOBJS) $(LIBS) -lXm -lXt -lX11

