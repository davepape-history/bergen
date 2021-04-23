## Do a 'setenv ARCHITECTURE linux' or 'setenv ARCHITECTURE sgi'
#
include Makedefs.$(ARCHITECTURE)

all: bergenNetTest voiceServer fileServer toneServer

bergenNetTest: bergenNetTest.o
	$(C++) $(LFLAGS) -o bergenNetTest bergenNetTest.o -L../clientlib -lbergen $(LIBS)

voiceServer: voiceServer.o
	$(C++) $(LFLAGS) -o voiceServer voiceServer.o -L../clientlib -lbergen $(LIBS)

toneServer: toneServer.o
	$(C++) $(LFLAGS) -o toneServer toneServer.o -L../clientlib -lbergen $(LIBS)

fileServer: fileServer.o
	$(C++) $(LFLAGS) -o fileServer fileServer.o -L../clientlib -lbergen $(LIBS)

