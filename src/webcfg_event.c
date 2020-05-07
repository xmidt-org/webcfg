/*
 * Copyright 2020 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "webcfg_multipart.h"
#include "webcfg_notify.h"
#include "webcfg_generic.h"
#include "webcfg.h"
#include <pthread.h>
#include <cJSON.h>
#include <unistd.h>
#include "webcfg_event.h"
#include "webcfg_db.h"
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static pthread_t EventThreadId=0;
static pthread_t processThreadId = 0;
pthread_mutex_t event_mut=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t event_con=PTHREAD_COND_INITIALIZER;
event_data_t *eventDataQ = NULL;
expire_timer_t *event_timer = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void* blobEventHandler();
int parseEventData();
void* processSubdocEvents();

int checkWebcfgTimer();
int addToEventQueue(char *buf);
void sendSuccessNotification(char *name, uint32_t version);
int startWebcfgTimer(uint32_t timeout);
int stopWebcfgTimer();
int checkTimerExpired (expire_timer_t *timer, long timeout_ms);
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

//Webcfg thread listens for blob events from respective components.
void initEventHandlingTask()
{
	int err = 0;
	err = pthread_create(&EventThreadId, NULL, blobEventHandler, NULL);
	if (err != 0)
	{
		WebcfgError("Error creating Webconfig event handling thread :[%s]\n", strerror(err));
	}
	else
	{
		WebcfgInfo("Webconfig event handling thread created Successfully\n");
	}

}

void* blobEventHandler()
{
	pthread_detach(pthread_self());

	int ret = 0;
	WebcfgInfo("registerWebcfgEvent\n");
	ret = registerWebcfgEvent(webcfgCallback);
	if(ret)
	{
		WebcfgInfo("registerWebcfgEvent success\n");
	}
	else
	{
		WebcfgError("registerWebcfgEvent failed\n");
	}

	/* Loop to check timer expiry. When timer is not running, loop is active but wont check expiry until next timer starts. */
	while(1)
	{
		if ((event_timer !=NULL) && (checkTimerExpired (event_timer, (event_timer->timeout)*1000)))
		{
			WebcfgError("Timer expired. No event received in %lu seconds\n", (long)event_timer->timeout);
			//reset timer. Generate internal timer_expiry event with new trans_id to retry. TODO
			memset(event_timer, 0, sizeof(expire_timer_t));
			event_timer->running = false;
		}
		else
		{
			WebcfgInfo("Waiting at timer loop of 30s\n");
			sleep(30);
		}
	}
	return NULL;
}

//Call back function to be executed when webconfigSignal signal is received from component.
void webcfgCallback(char *Info, void* user_data)
{
	char *buff = NULL;
	WebcfgInfo("Received webconfig event signal Info %s user_data %s\n", Info, (char*) user_data);
	
	buff = strdup(Info);
	addToEventQueue(buff);

	WebcfgInfo("After addToEventQueue\n");
}

//Producer adds event data to queue
int addToEventQueue(char *buf)
{
	event_data_t *Data;

	WebcfgInfo ("Add data to event queue\n");
        Data = (event_data_t *)malloc(sizeof(event_data_t));

	if(Data)
        {
            Data->data =buf;
            Data->next=NULL;
            pthread_mutex_lock (&event_mut);
            if(eventDataQ == NULL)
            {
                eventDataQ = Data;

                WebcfgInfo("Producer added Data\n");
                pthread_cond_signal(&event_con);
                pthread_mutex_unlock (&event_mut);
                WebcfgInfo("mutex unlock in producer event thread\n");
            }
            else
            {
                event_data_t *temp = eventDataQ;
                while(temp->next)
                {
                    temp = temp->next;
                }
                temp->next = Data;
                pthread_mutex_unlock (&event_mut);
            }
        }
        else
        {
            WebcfgError("failure in allocation for event data\n");
        }

	return 0;
}


//Webcfg consumer thread to process the events.
void processWebcfgEvents()
{
	int err = 0;
	err = pthread_create(&processThreadId, NULL, processSubdocEvents, NULL);
	if (err != 0)
	{
		WebcfgError("Error creating processWebcfgEvents thread :[%s]\n", strerror(err));
	}
	else
	{
		WebcfgInfo("processWebcfgEvents thread created Successfully\n");
	}

}

//Parse and process sub doc event data.
void* processSubdocEvents()
{
	event_params_t *eventParam = NULL;
	int rv = WEBCFG_FAILURE;
	WEBCFG_STATUS uStatus = WEBCFG_FAILURE;

	while(1)
	{
		pthread_mutex_lock (&event_mut);
		WebcfgInfo("mutex lock in event consumer thread\n");
		if(eventDataQ != NULL)
		{
			event_data_t *Data = eventDataQ;
			eventDataQ = eventDataQ->next;
			pthread_mutex_unlock (&event_mut);
			WebcfgInfo("mutex unlock in event consumer thread\n");

			WebcfgInfo("Data->data is %s\n", Data->data);
			rv = parseEventData(Data->data, &eventParam);
			if(rv == WEBCFG_SUCCESS)
			{
				if ((strcmp(eventParam->status, "ACK")==0) && (eventParam->timeout == 0))
				{
					WebcfgInfo("ACK event. doc apply success, proceed to add to DB\n");
					
					WebcfgInfo("stop Timer\n");
					stopWebcfgTimer();
					//add to DB and tmp lists based on success ack.
					WebcfgInfo("AddToDB subdoc_name %s version %lu\n", eventParam->subdoc_name, (long)eventParam->version);
					checkDBList(eventParam->subdoc_name,eventParam->version);
					WebcfgInfo("blob addNewDocEntry to DB\n");
					addNewDocEntry(get_successDocCount());
					if(checkRootUpdate() == WEBCFG_SUCCESS)
					{
						WebcfgInfo("updateRootVersionToDB\n");
						updateRootVersionToDB();
					}
					WebcfgInfo("B4 sendSuccessNotification.\n");
					sendSuccessNotification(eventParam->subdoc_name, eventParam->version);

				}
				else if ((strcmp(eventParam->status, "NACK")==0) && (eventParam->timeout == 0)) 
				{
					WebcfgInfo("NACK event. doc apply failed, need to retry\n");
					WebcfgInfo("stop Timer ..\n");
					stopWebcfgTimer();
					WebcfgInfo("updateTmpList\n");
					uStatus = updateTmpList(eventParam->subdoc_name, eventParam->version, "failed", "doc_rejected");
					if(uStatus !=WEBCFG_SUCCESS)
					{
						WebcfgError("Failed in updateTmpList for NACK\n");
					}
					addWebConfgNotifyMsg(eventParam->subdoc_name, eventParam->version, "failed", "doc_rejected", "123");
				}
				else if (eventParam->timeout != 0)
				{
					WebcfgInfo("Timeout event. doc apply need time, start timer.\n");
					startWebcfgTimer(eventParam->timeout);
					WebcfgInfo("After startWebcfgTimer\n");
				}
				else
				{
					WebcfgInfo("Crash event. Component restarted after crash, re-send blob.\n");
				}
			}
			else
			{
				WebcfgError("Failed to parse event Data\n");
			}
			//WEBCFG_FREE(Data);
		}
		else
		{
			WebcfgInfo("Before pthread cond wait in event consumer thread\n");
			pthread_cond_wait(&event_con, &event_mut);
			pthread_mutex_unlock (&event_mut);
			WebcfgInfo("mutex unlock in event consumer thread after cond wait\n");
		}
	}
	return NULL;
}

//Extract values from comma separated string and add to event_params_t structure.
int parseEventData(char* str, event_params_t **val)
{
	event_params_t *param = NULL;
	char *tmpStr =  NULL;
	char * trans_id = NULL;
	char *version = NULL;
	char * timeout =NULL;

	if(str !=NULL)
	{
		param = (event_params_t *)malloc(sizeof(event_params_t));
		if(param)
		{
			memset(param, 0, sizeof(event_params_t));
		        tmpStr = strdup(str);
			//WEBCFG_FREE(str);

		        param->subdoc_name = strsep(&tmpStr, ",");
		        trans_id = strsep(&tmpStr,",");
		        version = strsep(&tmpStr,",");
		        param->status = strsep(&tmpStr,",");
		        timeout = strsep(&tmpStr, ",");

			WebcfgInfo("convert string to uint type\n");
			if(trans_id !=NULL)
			{
				param->trans_id = strtoul(trans_id,NULL,0);
			}

			if(version !=NULL)
			{
				param->version = strtoul(version,NULL,0);
			}

			if(timeout !=NULL)
			{
				param->timeout = strtoul(timeout,NULL,0);
			}

			WebcfgInfo("param->subdoc_name %s param->trans_id %lu param->version %lu param->status %s param->timeout %lu\n", param->subdoc_name, (long)param->trans_id, (long)param->version, param->status, (long)param->timeout);
			*val = param;
			return WEBCFG_SUCCESS;
		}
	}
	return WEBCFG_FAILURE;

}

//Update Tmp list and send success notification to cloud .
void sendSuccessNotification(char *name, uint32_t version)
{
	WEBCFG_STATUS uStatus=1, dStatus=1;

	uStatus = updateTmpList(name, version, "success", "none");
	if(uStatus == WEBCFG_SUCCESS)
	{
		WebcfgInfo("B4 addWebConfgNotifyMsg\n"); 
		addWebConfgNotifyMsg(name, version, "success", "none", "123"); //TODO: Add trans id and new blob params if any.

		WebcfgInfo("deleteFromTmpList as doc is applied\n");
		dStatus = deleteFromTmpList(name);
		if(dStatus == WEBCFG_SUCCESS)
		{
			WebcfgInfo("blob deleteFromTmpList success\n");
		}
		else
		{
			WebcfgError("blob deleteFromTmpList failed\n");
		}
	}
	else
	{
		WebcfgError("blob updateTmpList failed\n");
	}

}

//start internal timer when timeout value is received
int startWebcfgTimer(uint32_t timeout)
{
	event_timer = malloc(sizeof(expire_timer_t));
	if(event_timer)
	{
		memset(event_timer, 0, sizeof(expire_timer_t));
		event_timer->running = false;
		if (!event_timer->running)
		{
			getCurrent_Time(&event_timer->start_time);
			event_timer->running = true;
			event_timer->timeout = timeout;
			WebcfgInfo("started webcfg internal timer\n");
			WebcfgInfo("event_timer->timeout %lu\n", (long)event_timer->timeout);
			return true;
		}
		else
		{
			WebcfgError("Timer is already running!!\n");
		}
	}
	WebcfgError("Failed in startWebcfgTimer\n");
	return false;
}


//stop internal timer when ack/nack events received
int stopWebcfgTimer()
{
	if(event_timer)
	{
		if (event_timer->running)
		{
			WebcfgInfo("stopWebcfgTimer\n");
			memset(event_timer, 0, sizeof(expire_timer_t));
			event_timer->running = false;
			WebcfgInfo("stopped webcfg internal timer\n");
			return true;
		}
		else
		{
			WebcfgError("Timer is not running!!\n");
		}
	}
	return false;
}

int checkTimerExpired (expire_timer_t *timer, long timeout_ms)
{
	long time_diff_ms;

	if(timer == NULL)
	{
		return false;
	}

	if (!timer->running)
	{
		return false;
	}

	getCurrent_Time(&timer->end_time);
	time_diff_ms = timeVal_Diff (&timer->start_time, &timer->end_time);
	WebcfgInfo("checking timeout difference:%ld sec timeout value:%ld sec\n", (time_diff_ms/1000), (timeout_ms/1000));
	if(time_diff_ms >= timeout_ms)
	{
		WebcfgError("Internal timer with %ld sec expired, doc apply failed\n", (timeout_ms/1000));
		return true;
	}
	return false;
}
