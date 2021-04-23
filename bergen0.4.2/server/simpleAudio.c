/*
 * Simplified audio package
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include "Alib.h"
#include "CUlib.h"
#include "simpleAudio.h"     

static Audio *audioServer = (Audio *) NULL;
static struct protoent *tcpProtocolEntry;

/*
 * this is used to associate a transaction ID and the socket FD for a stream
 */
typedef struct _AStreamInfo {
  ATransID        xid;
  int             fd;
  int             mode;
} AStreamInfo;

static int numStreams = 0;
#define MAX_STREAMS 16
static AStreamInfo activeStreams[MAX_STREAMS];

/*
 * printAudioError - print audio error text given error code
 */
#ifdef __STDC__
static void
printAudioError(
    Audio    *audio,
    char     *message,
    int      errorCode
    )
#else
static void
printAudioError( audio, message, errorCode )
    Audio    *audio;
    char     *message;
    int      errorCode;
#endif
{
    char    errorbuff[132];

    AGetErrorText(audio, errorCode, errorbuff, 131);
    fprintf ( stderr, "%s: %s\n", message, errorbuff);
}

/*
 * error handler for audio
 */
#ifdef __STDC__
static long
myHandler(
  Audio        *audio,
  AErrorEvent  *err_event
  )
#else
static long
myHandler(audio, err_event )
	  Audio  * audio;
	  AErrorEvent  * err_event;
#endif
{
  printAudioError( audio, "Audio error", err_event->error_code ); 
  exit(1);
}


/*
 * open audio connection
 *   returns 0 for success
 */
int
openAudio()
{
  AErrorHandler   prevHandler;  /* pointer to previous handler */
  char server[1];
  int channels;
  int i, fr;

  if ( audioServer ) {
    fprintf( stderr, "openAudio - audio already open\n" );
    return 0;
  }

  /* replace default error handler */
  prevHandler = ASetErrorHandler(myHandler);

  /*
   *  open audio connection
   */
  server[0] = '\0';
  audioServer = AOpenAudio( server, NULL );
  if (audioServer == NULL) {
    fprintf(stderr, "Error: could not open audio\n");
    return 1;
  }

  for ( i = 0; i < MAX_STREAMS; i++ ) {
    /* mark all active stream entries as unused */
    activeStreams[i].fd = -1;
  }

  /*
   * get the tcp protocol name (for setting TCP_NODELAY option later)
   */
  tcpProtocolEntry = getprotobyname("tcp");

  return 0;
}


void
closeAudio()
{
  int i;

  for ( i = 0; i < MAX_STREAMS; i++ ) {
    /* close all active streams */
    if ( activeStreams[i].fd != -1 ) {
      close( activeStreams[i].fd );
      activeStreams[i].fd = -1;
    }
  }
  numStreams = 0;

  /*
   * set close mode to prevent playback from stopping 
   *  when we close audio connection
   */
  ASetCloseDownMode( audioServer, AKeepTransactions, NULL );

  /*
   *  That's all, folks!
   */
  ACloseAudio( audioServer, NULL );
  audioServer = (Audio *) NULL;
}


/*
 * get audio capabilities
 *   returns 0 if succesful
 */
#ifdef __STDC__
int
getAudioCapabilities(
  int  *numChannels,
  int  **sampleRates,
  int  *numRates,
  int  *dataFormats
  )
#else
int
getAudioCapabilities( numChannels, sampleRates, numRates, dataFormats )
  int  *numChannels;
  int  **sampleRates;
  int  *numRates;
  int  *dataFormats;
#endif
{
  int i;
  if ( audioServer ) {
    *numChannels = (audioServer->output_channels & ALeftOutputChMask) ? 2 : 1;
    *sampleRates = ASamplingRates(audioServer);
    *numRates = ANumSamplingRates(audioServer);
    *dataFormats = 0;
    for ( i = 0; i < audioServer->n_data_format; i++ ) {
      switch( audioServer->data_format_list[i] ) {
      case ADFMuLaw:       *dataFormats |= AUDIO_HAS_MULAW;  break;
      case ADFALaw:        *dataFormats |= AUDIO_HAS_ALAW;   break;
      case ADFLin16:       *dataFormats |= AUDIO_HAS_LIN16;  break;
      case ADFLin8:        *dataFormats |= AUDIO_HAS_LIN8;   break;
      case ADFLin8Offset:  *dataFormats |= AUDIO_HAS_LIN8O;  break;
      }
    }
    return 0;
  } else {
    return 1;
  }
}

#ifdef __STDC__
static AStreamInfo
*getAStreamInfo(
  int  fd
  )
#else
static AStreamInfo
*getAStreamInfo( fd )
  int  fd;
#endif
{
  int i;

  for ( i = 0; i < MAX_STREAMS; i++ ) {
    /* find stream info structure with matching FD */
    if ( activeStreams[i].fd == fd ) {
      return &(activeStreams[i]);
    }
  }
  return (AStreamInfo *) NULL;
}

static AStreamInfo
*getFreeAStream()
{
  return getAStreamInfo( -1 );
}

#ifdef __STDC__
void
pauseAStream(
  int  fd
  )
#else
void
pauseAStream( fd )
  int  fd;
#endif
{
  AStreamInfo *aStream;

  aStream = getAStreamInfo( fd );
  if ( aStream ) {
    APauseAudio( audioServer, aStream->xid, NULL, NULL );
  } else {
    fprintf( stderr, "Attempted to pause non-existant audio stream on fd %d\n", fd );
  }
}

#ifdef __STDC__
void
resumeAStream(
  int  fd
  )
#else
void
resumeAStream( fd )
  int  fd;
#endif
{
  AStreamInfo *aStream;

  aStream = getAStreamInfo( fd );
  if ( aStream ) {
    AResumeAudio( audioServer, aStream->xid, NULL, NULL );
  } else {
    fprintf( stderr, "Attempted to resume non-existant audio stream on fd %d\n", fd );
  }
}

#ifdef __STDC__
void
stopAStream(
  int  fd
  )
#else
void
stopAStream( fd )
  int  fd;
#endif
{
  AStreamInfo *aStream;

  aStream = getAStreamInfo( fd );
  if ( aStream ) {
    AStopAudio( audioServer, aStream->xid, ASMThisTrans, NULL, NULL );
  } else {
    fprintf( stderr, "Attempted to stop non-existant audio stream on fd %d\n", fd );
  }
}

/* convert 0...100 to gain value */
#define acPMin  audioServer->min_output_gain
#define acPMax  audioServer->max_output_gain
#define Percent2PlayGain(PERCENT)  (((PERCENT * (acPMax - acPMin)) / 100) + acPMin)

#define acRMin  audioServer->min_input_gain
#define acRMax  audioServer->max_input_gain
#define Percent2RecordGain(PERCENT)  (((PERCENT * (acRMax - acRMin)) / 100) + acRMin)


/*
 * set volume using either DB or percent
 */
#ifdef __STDC__
void
setAStreamVolume(
  int  fd,
  int  volume,
  int  useDB
  )
#else
void
setAStreamVolume( fd, volume, useDB )
  int  fd;
  int  volume;
  int  useDB;
#endif
{
  int  gainDB;
  AStreamInfo *aStream;

  aStream = getAStreamInfo( fd );
  if ( aStream ) {
    if ( useDB ) {
      gainDB = volume;
    } else {
      gainDB = (aStream->mode == RECORD_STREAM)
	? Percent2RecordGain( volume ) : Percent2PlayGain( volume );
    }

    ASetGain( audioServer, aStream->xid, gainDB, NULL);
  } else {
    fprintf( stderr, "Attempted to set volume of non-existant audio stream on fd %d\n",
	    fd );
  }
}

#ifdef __STDC__
void
closeAStream(
  int  fd
  )
#else
void
closeAStream( fd )
  int  fd;
#endif
{
  AStreamInfo *aStream;

  aStream = getAStreamInfo( fd );
  if ( aStream ) {
    aStream->fd = -1;
  } else {
    fprintf( stderr, "Attempted to close non-existant audio stream on fd %d\n", fd );
  }
  close( fd );
}



/****************************************************************************
 *
 * open an audio stream - returns socket fd or negative num for failure
 *   streamMode: PLAY_STREAM or RECORD_STREAM
 *   sampleRate: 
 *   channels: USE_MONO or USE_STEREO (this is same as using the values 1 or 2)
 *   device: USE_INTERNAL_SPEAKER, USE_EXTERNAL_SPEAKER or USE_DEFAULT_SPEAKER
 *   dataFormat: USE_MULAW, USE_LIN16 or USE_ALAW
 *   startPaused: START_PAUSED or START_IMMEDIATELY
 */
#ifdef __STDC__
int
openAStream(
  int streamMode,
  int sampleRate,
  int channels,
  int dataFormat,
  int device,
  int startPaused
  )
#else
int
openAStream( streamMode, sampleRate, channels, dataFormat, device, startPaused )
  int streamMode;
  int sampleRate;
  int channels;
  int dataFormat;
  int device;
  int startPaused;
#endif
{
  long            status;
  SSPlayParams    playParams;
  SSRecordParams  recordParams;
  AGainEntry      gainEntry[4];
  AudioAttrMask   AttribsMask;
  AudioAttributes Attribs;
  SStream	  audioStream;
  int		  streamSocket, i;
  AStreamInfo     *aStream;
  ADataFormat     alibFormat;

  aStream = getFreeAStream();
  if ( aStream == NULL ) {
    fprintf( stderr, "Failed to get audio stream, limit is %d open\n", MAX_STREAMS );
    return -2;
  }

  /*
   * create a stream socket
   */
  streamSocket = socket( AF_INET, SOCK_STREAM, 0 );
  if( streamSocket < 0 ) {
    perror( "Socket creation failed" );
    return -2;
  }
  aStream->fd = streamSocket;
  aStream->mode = streamMode;
  
  switch ( dataFormat ) {
  case USE_ALAW:    alibFormat = ADFALaw;   break;
  case USE_MULAW:   alibFormat = ADFMuLaw;  break;
  case USE_LIN16:   alibFormat = ADFLin16;  break;
  }
    
  Attribs.type = ATSampled;
  Attribs.attr.sampled_attr.sampling_rate = sampleRate;
  Attribs.attr.sampled_attr.channels = channels;
  Attribs.attr.sampled_attr.data_format = alibFormat;
  AttribsMask = ASSamplingRateMask | ASChannelsMask  | ASDataFormatMask;

  /*
   * setup the playback parameters
   */
  switch ( streamMode ) {
  case RECORD_STREAM:
    /*
     * make record stream
     */
    gainEntry[0].u.i.in_ch = AICTMono;
    gainEntry[0].gain = AUnityGain;
    switch ( device ) {
    case USE_MIKE_IN:    gainEntry[0].u.i.in_src = AISTMonoMicrophone; break;
    case USE_LINE_IN:    gainEntry[0].u.i.in_src = AISTMonoAuxiliary;  break;
    default:
    case USE_DEFAULT_IN: gainEntry[0].u.i.in_src = AISTDefaultInput;   break;
    }
    recordParams.gain_matrix.type = AGMTInput;       /* gain matrix */
    recordParams.gain_matrix.num_entries = 1;
    recordParams.gain_matrix.gain_entries = gainEntry;
    recordParams.record_gain = AUnityGain;            /* play volume */
    recordParams.pause_first = startPaused;
    recordParams.event_mask = 0;                      /* don't solicit any events */
/* TBD: check sample rate, #channels before making stream */
    /*
     * create an audio stream
     */
    aStream->xid = ARecordSStream( audioServer, AttribsMask, &Attribs, &recordParams,
				&audioStream, NULL );
    break;

  case PLAY_STREAM:
    /*
     * make play stream
     */
    gainEntry[0].u.o.out_ch = AOCTMono;
    gainEntry[0].gain = AUnityGain;
    switch ( device ) {
    case USE_INTERNAL_SPEAKER: gainEntry[0].u.o.out_dst = AODTMonoIntSpeaker; break;
    case USE_EXTERNAL_SPEAKER: gainEntry[0].u.o.out_dst = AODTMonoJack;       break;
    default:
    case USE_DEFAULT_SPEAKER:  gainEntry[0].u.o.out_dst = AODTDefaultOutput;  break;
    }
    playParams.gain_matrix.type = AGMTOutput;       /* gain matrix */
    playParams.gain_matrix.num_entries = 1;
    playParams.gain_matrix.gain_entries = gainEntry;
    playParams.play_volume = AUnityGain;            /* play volume */
    playParams.priority = APriorityNormal;          /* normal priority */
    playParams.event_mask = 0;                      /* don't solicit any events */
/* TBD: check sample rate, #channels before making stream */
    /*
     * create an audio stream
     */
    aStream->xid = APlaySStream( audioServer, AttribsMask, &Attribs, &playParams,
				&audioStream, NULL );
    if ( startPaused ) {
      APauseAudio( audioServer, aStream->xid, NULL, NULL );
    }
    break;

  default:
    fprintf( stderr, "Invalid mode to openAStream\n" );
    closeAStream( streamSocket );
    return -2;
    break;
  }


  /*
   * connect the stream socket to the audio stream port
   */
  status = connect( streamSocket, (struct sockaddr *)&audioStream.tcp_sockaddr,
		   sizeof(struct sockaddr_in) );
  if( status < 0 ) {
    perror( "Connect failed" );
    closeAStream( streamSocket );
    return -2;
  }

  /*
   * tell TCP to not delay sending data written to this socket
   */
  i = -1;
  setsockopt( streamSocket, tcpProtocolEntry->p_proto, TCP_NODELAY,
	     &i, sizeof(i) );

  return streamSocket;
}

