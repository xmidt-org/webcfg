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
#ifdef BUILD_YOCTO
void *bus_handle = NULL;
char* component_id = "ccsp.webconfid";
#endif
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void* blobEventHandler();
int parseEventData();
void* processSubdocEvents();

int checkWebcfgTimer();
int addToEventQueue(char *buf);
void sendSuccessNotification(char *name, uint32_t version);

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

	WebcfgInfo("registerWebcfgEvent\n");
	registerWebcfgEvent(webcfgCallback);

	/* Testing purpose. Remove this once event reg callbck is implemented. */
	/*char data[128] = {0};
	snprintf(data,sizeof(data),"%s,%hu,%u,ACK,%u","portforwarding",123,210666,0);
	WebcfgInfo("data is %s\n", data);
	webcfgCallback(data, NULL);*/
	return NULL;
}

void registerWebcfgEvent(WebConfigEventCallback webcfgEventCB)
{
#ifdef BUILD_YOCTO //should we move this as override fn for rdkb?

	int ret = 0;
	char *pCfg = CCSP_MSG_BUS_CFG;

	ret = CCSP_Message_Bus_Init(component_id, pCfg, &bus_handle,
	(CCSP_MESSAGE_BUS_MALLOC) Ansc_AllocateMemory_Callback, Ansc_FreeMemory_Callback);
	if (ret == -1)
	{
		WebcfgInfo("Message bus init failed\n");
	}

	CcspBaseIf_SetCallback2(bus_handle, "webconfigSignal",
            webcfgEventCB, NULL);

	ret = CcspBaseIf_Register_Event(bus_handle, NULL, "webconfigSignal");
	if (ret != CCSP_Message_Bus_OK)
	{
		WebcfgInfo("CcspBaseIf_Register_Event failed\n");
	}
	else
	{
		WebcfgInfo("Registration with CCSP Bus is success, waiting for events from components\n");
	}
#endif
	WebcfgInfo("webcfgEventCB %s\n", (char*)webcfgEventCB);
}

//Call back function to be executed when webconfigSignal signal is received from component.
void webcfgCallback(char *Info, void* user_data)
{
	WebcfgInfo("Received webconfig event signal Info %s user_data %s\n", Info, (char*) user_data);
	
	char *buff = NULL;
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
					
					//add to DB and tmp lists based on success ack.
					WebcfgInfo("AddToDB subdoc_name %s version %lu\n", eventParam->subdoc_name, (long)eventParam->version);
					checkDBList(eventParam->subdoc_name,eventParam->version);

					sendSuccessNotification(eventParam->subdoc_name, eventParam->version);

				}
				else if ((strcmp(eventParam->status, "NACK")==0) && (eventParam->timeout == 0)) 
				{
					WebcfgInfo("NACK event. doc apply failed, need to retry\n");
					addWebConfgNotifyMsg(eventParam->subdoc_name, eventParam->version, "failed", "failed_reason", "123");
				}
				else if (eventParam->timeout != 0)
				{
					WebcfgInfo("Timeout event. doc apply need time, start timer.\n");
					checkWebcfgTimer();
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

			/*if(ack !=NULL)
			{
				param->ack = atoi(ack);
			}*/

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

//checks subdoc timeouts
int checkWebcfgTimer()
{
	/*int value = 0;
	gettimeofday(&tp, NULL);
	ts.tv_sec = tp.tv_sec;
	ts.tv_nsec = tp.tv_usec * 1000;
	ts.tv_sec += value;*/

	//if(time > timeout ) //time expire
	//{
		WebcfgInfo("Timer expired. doc apply failed\n");
	//}
	return 0;

}


