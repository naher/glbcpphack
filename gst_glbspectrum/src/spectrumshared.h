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



//typedef struct _Message Message;

void createSharedMem();

void useSharedMem();

void writeMessage(Message*);

void readMessage(Message*);

void checkInit();

void setInit(bool);

#ifdef __cplusplus
}
#endif

#endif