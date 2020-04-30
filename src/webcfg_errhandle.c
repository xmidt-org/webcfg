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
pthread_mutex_t error_mut=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t error_con=PTHREAD_COND_INITIALIZER;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void* blobErrorHandler();
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
pthread_t get_global_notify_threadid()
{
    return ErrThreadId;
}

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

	registerWebcfgEvent();

	return NULL;
}

//TODO:
static void registerWebcfgEvent()
{
	CcspBaseIf_Register_Event(bus_handle, NULL, "webconfigSignal");

		CcspBaseIf_SetCallback2
		(
		bus_handle,
		"webconfigSignal",
		WebConfigEventCallback,
		NULL
		);
}

//TODO:
//Call back function to be executed webconfigSignal signal is received from component.
static void WebConfigEventCallback(char Info, void* user_data)
{
	WalInfo("Received webconfig event signal Info %s\n", Info);
	processSubdocEvents(Info);
}

//Function to parse and process the sub doc event data received from component.
int processSubdocEvents(char *blobInfo)
{
	parseEventData();
	checkWebcfgTimer();
}

//extract comma separated values for subdoc name, version, ACK, NACK, timeout, trans id
void parseEventData()
{

}

//checks subdoc timeouts
int checkWebcfgTimer()
{
	pthread_mutex_lock (&periodicsync_mutex);
	gettimeofday(&tp, NULL);
	ts.tv_sec = tp.tv_sec;
	ts.tv_nsec = tp.tv_usec * 1000;
	ts.tv_sec += value;

	

}
