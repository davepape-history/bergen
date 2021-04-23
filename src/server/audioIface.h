extern int snerdAudioBufferSize;

void snerdOpenAudioPort(int device,int bufsize,int sampleRate);

void snerdWriteToAudioPort(void *buffer,size_t bufferSize);
