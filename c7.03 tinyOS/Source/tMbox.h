#ifndef TMBOX_H
#define TMBOX_H

#include "tEvent.h"

#define tMboxSendNormal      0x00
#define tMboxSendFront       0x01

typedef struct _tMbox
{
	tEvent event;
	uint32_t count;
	uint32_t read;
	uint32_t write;
	uint32_t maxCount;
	void ** msgBuffer;
}tMbox;

void tMboxInit (tMbox * mbox, void ** msgBuffer, uint32_t maxCount);
uint32_t tMboxWait (tMbox * mbox, void ** msg, uint32_t waitTicks);
uint32_t tMboxNoWait (tMbox * mbox, void ** msg);
uint32_t tMboxNotify (tMbox * mbox, void * msg, uint32_t notifyOption);
void tMboxFlush (tMbox * mbox);
uint32_t tMboxDestroy (tMbox * mbox);

#endif
