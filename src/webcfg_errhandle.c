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
#include "webcfg_errhandle.h"
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static pthread_t ErrThreadId=0;
pthread_mutex_t event_mut=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t event_con=PTHREAD_COND_INITIALIZER;
event_data_t *eventDataQ = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void* blobErrorHandler();
int parseEventData();
int processSubdocEvents();
void WebConfigEventCallback(char *Info, void* user_data);
int checkWebcfgTimer();
int addToEventQueue(char *buf);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

//Performs Webconfig blob error handling
void initErrorHandlingTask()
{
	int err = 0;
	err = pthread_create(&ErrThreadId, NULL, blobErrorHandler, NULL);
	if (err != 0)
	{
		WebcfgError("Error creating Webconfig error handling thread :[%s]\n", strerror(err));
	}
	else
	{
		WebcfgInfo("Webconfig error handling thread created Successfully\n");
	}

}

//webconfig thread waiting for BLOB events from each component.
void* blobErrorHandler()
{
	pthread_detach(pthread_self());

	WebcfgInfo("registerWebcfgEvent\n");
	registerWebcfgEvent();

	/* Testing purpose. Remove this once event reg callbck is implemented. */
	char data[128] = {0};
	snprintf(data,sizeof(data),"%s,%hu,%u,ACK,%u","portforwarding",123,210666,120);
	WebcfgInfo("data is %s\n", data);
	WebConfigEventCallback(data, NULL);
	return NULL;
}

//TODO:
void registerWebcfgEvent()
{
#ifdef BUILD_YOCTO //should we move this as override fn for rdkb?
	CcspBaseIf_Register_Event(bus_handle, NULL, "webconfigSignal");

		CcspBaseIf_SetCallback2
		(
		bus_handle,
		"webconfigSignal",
		WebConfigEventCallback,
		NULL
		);
#endif
}

//Call back function to be executed when webconfigSignal signal is received from component.
void WebConfigEventCallback(char *Info, void* user_data)
{
	WebcfgInfo("Received webconfig event signal Info %s user_data %s\n", Info, (char*) user_data);

	addToEventQueue(Info);

	processSubdocEvents(); //add this as new consumer thread
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
            //Producer adds the event data into queue
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

//Consumer to parse and process the sub doc event data.
int processSubdocEvents()
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

			WebcfgInfo("Data->data %s\n", Data->data);
			rv = parseEventData(Data->data, &eventParam);
			if(rv == WEBCFG_SUCCESS)
			{

				//TODO: Use uint32_t eventParam->trans_id. Add new blob params if any.
				addWebConfgNotifyMsg(eventParam->subdoc_name, eventParam->version, "success", "none", "123");

				//add and update DB , tmp lists based on success/failure acks. :TODO

				checkWebcfgTimer();
			}
			else
			{
				WebcfgError("Failed to parse event Data\n");
				return WEBCFG_FAILURE;
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
	return WEBCFG_SUCCESS;
}

//Extract values from comma separated string and add to event_params_t structure.
int parseEventData(char* str, event_params_t **val)
{
	event_params_t *param = NULL;
	char *tmpStr =  NULL;
	char * trans_id = NULL;
	char *version = NULL;
	char * ack;
	char * timeout;

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
		        ack = strsep(&tmpStr,",");
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

			if(ack !=NULL)
			{
				param->ack = atoi(ack);
			}

			if(timeout !=NULL)
			{
				param->timeout = strtoul(timeout,NULL,0);
			}

			WebcfgInfo("param->subdoc_name %s param->trans_id %lu param->version %lu param->ack %d param->timeout %lu\n", param->subdoc_name, (long)param->trans_id, (long)param->version, param->ack, (long)param->timeout);
			*val = param;
			return WEBCFG_SUCCESS;
		}
	}
	return WEBCFG_FAILURE;

}

//checks subdoc timeouts
int checkWebcfgTimer()
{
	/*int value = 0;
	gettimeofday(&tp, NULL);
	ts.tv_sec = tp.tv_sec;
	ts.tv_nsec = tp.tv_usec * 1000;
	ts.tv_sec += value;*/

	return 0;

}
