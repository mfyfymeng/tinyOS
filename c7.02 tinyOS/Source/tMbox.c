#include "tinyOS.h"

void tMboxInit (tMbox * mbox, void ** msgBuffer, uint32_t maxCount)
{
	tEventInit(&mbox->event, tEventTypeMbox);
	
	mbox->msgBuffer = msgBuffer;
	mbox->maxCount = maxCount;
	mbox->read = 0;
	mbox->write = 0;
	mbox->count = 0;
}

uint32_t tMboxWait (tMbox * mbox, void ** msg, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();

	if (mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if (mbox->read >= mbox->maxCount)
			mbox->read = 0;
		
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		tEventWait(&mbox->event, currentTask, (void *)0, tEventTypeMbox, waitTicks);
		tTaskExitCritical(status);
		tTaskSched();
		*msg = currentTask->eventMsg;
		return currentTask->waitEventResult;
	}
}

uint32_t tMboxNoWait (tMbox * mbox, void ** msg)
{
	uint32_t status = tTaskEnterCritical();
	
	if (mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if (mbox->read >= mbox->maxCount)
			mbox->read = 0;
		
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		tTaskExitCritical(status);
		return tErrorResourseUnavaliable;
	}
}

uint32_t tMboxNotify (tMbox * mbox, void * msg, uint32_t notifyOption)
{
	uint32_t status = tTaskEnterCritical();
	
	if (tEventWaitCount(&mbox->event) > 0)
	{
		tTask * task = tEventWakeUp(&mbox->event, (void *)msg, tErrorNoError);
		if (task->prio < currentTask->prio)
			tTaskSched();
	}
	else
	{
		if (mbox->count >= mbox->maxCount)
		{
			tTaskExitCritical(status);
			return tErrorResourseFull;
		}
		
		if (notifyOption & tMboxSendFront)
		{
			if (mbox->read <= 0)
				mbox->read = mbox->maxCount - 1;
			else
				--mbox->read;
			
			mbox->msgBuffer[mbox->read] = msg;
		}
		else
		{
			mbox->msgBuffer[mbox->write++] = msg;
			if (mbox->write >= mbox->maxCount)
				mbox->write = 0;
		}
		
		mbox->count++;
	}
	
	tTaskExitCritical(status);
	return tErrorNoError;
}


