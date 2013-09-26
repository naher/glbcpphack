#ifndef _GLB_SPECTRUM_SHARED
#define _GLB_SPECTRUM_SHARED

#include "gst\gstclock.h"

typedef struct _Message {
	GstClockTime timestamp;
	int spectrum_bands;
	float magnitude[128];
} Message;

#ifdef __cplusplus
extern "C" {
#endif

/**
* Creates a shared memory segment and allocates structures needed.
*/
void createSharedMem();

/**
* Recovers structures allocated from a shared memory segment.
*/
void useSharedMem();

/**
* Writes a message into the shared vector.
*/
void writeMessage(Message*);

/**
* Reads a message from the shared vector.
*/
void readMessage(Message*);

/**
* Locks until other thread calls setInit.
* Used to lock until the reading thread is ready.
*/
void checkInit();

/**
* Signals waiting threads on checkInit().
*/
void setInit(bool);

#ifdef __cplusplus
}
#endif

#endif