#ifdef __cplusplus
extern "C" {
#endif

/*
 * Simplified audio stream playing package
 */

/*
 * open connection to the audio server
 */
extern int openAudio();

/*
 * close connection to the audio server
 */
extern void closeAudio();

#define PLAY_STREAM   0
#define RECORD_STREAM 1

#define USE_MONO   1
#define USE_STEREO 2

#define USE_MULAW  1
#define USE_LIN16  0
#define USE_ALAW  -1

#define USE_INTERNAL_SPEAKER   1
#define USE_EXTERNAL_SPEAKER   0
#define USE_DEFAULT_SPEAKER   -1

#define USE_MIKE_IN           1
#define USE_LINE_IN           0
#define USE_DEFAULT_IN       -1

#define START_PAUSED       1
#define START_IMMEDIATELY  0

/*
 * These are the data format masks returned by getAudioMasks in *dataFormats
 */
#define AUDIO_HAS_LIN16  1
#define AUDIO_HAS_MULAW  2
#define AUDIO_HAS_ALAW   4
#define AUDIO_HAS_LIN8   8
#define AUDIO_HAS_LIN8O  0x10


/*
 * open an audio stream - returns socket fd or negative num for failure
 *   streamMode: PLAY_STREAM or RECORD_STREAM
 *   sampleRate: 
 *   channels: USE_MONO or USE_STEREO (this is same as using the values 1 or 2)
 *   device:
 *     for PLAY_STREAM: USE_INTERNAL_SPEAKER, USE_EXTERNAL_SPEAKER or USE_DEFAULT_SPEAKER
 *     for RECORD_STREAM: USE_MIKE_IN, USE_LINE_IN or USE_DEFAULT_IN
 *   dataFormat: USE_MULAW, USE_LIN16 or USE_ALAW
 *   startPaused: START_PAUSED or START_IMMEDIATELY
 */
extern int
openAStream(
  int streamMode,
  int sampleRate,
  int channels,
  int dataFormat,
  int device,
  int startPaused
  );

/*
 * close an audio stream
 */
extern void
closeAStream(
  int  fd
  );

/*
 * get audio capabilities - mono/stereo, sample rates and data formats
 * dataFormats is a mask with set bits for each supported format (see AUDIO_HAS_* above)
 */
extern int
getAudioCapabilities(
  int  *numChannels,
  int  **sampleRates,
  int  *numRates,
  int  *dataFormats
  );

/*
 * pause an audio stream
 */
extern void
pauseAStream(
  int  fd
  );


/*
 * unpause an audio stream
 */
extern void
resumeAStream(
  int  fd
  );

/*
 * stop an audio stream
 */
extern void
stopAStream(
  int  fd
  );

/*
 * set volume using either DB or percent
 */
extern void
setAStreamVolume(
  int  fd,
  int  volume,
  int  useDB
  );

#ifdef __cplusplus
}
#endif
