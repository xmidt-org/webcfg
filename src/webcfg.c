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

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include "webcfg.h"
#include "webcfg_multipart.h"
#include "webcfg_notify.h"
#include "webcfg_generic.h"
#include "webcfg_param.h"
#include "webcfg_auth.h"
#include <msgpack.h>
#include <wdmp-c.h>
#include <base64.h>
#include "webcfg_db.h"
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
pthread_mutex_t sync_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sync_condition=PTHREAD_COND_INITIALIZER;
bool g_shutdown  = false;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void *WebConfigMultipartTask(void *status);
int handlehttpResponse(long response_code, char *webConfigData, int retry_count, char* transaction_uuid, char* ct, size_t dataSize);
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

void initWebConfigMultipartTask(unsigned long status)
{
	int err = 0;
	pthread_t threadId;

	err = pthread_create(&threadId, NULL, WebConfigMultipartTask, (void *) status);
	if (err != 0) 
	{
		WebcfgError("Error creating WebConfigMultipartTask thread :[%s]\n", strerror(err));
	}
	else
	{
		WebcfgInfo("WebConfigMultipartTask Thread created Successfully.\n");
	}
}

void *WebConfigMultipartTask(void *status)
{
	pthread_detach(pthread_self());
	int ret = 0;
	int rv=0;
	int forced_sync=0;
        int Status = 0;
	int retry_flag = 0;
	struct timespec ts;
	Status = (unsigned long)status;

	//start webconfig notification thread.
	initWebConfigNotifyTask();
	WebcfgInfo("initDB %s\n", WEBCFG_DB_FILE);

	initDB(WEBCFG_DB_FILE);

	processWebconfgSync((int)Status);

	while(1)
	{
		if(forced_sync)
		{
			WebcfgDebug("Triggered Forced sync\n");
			processWebconfgSync((int)Status);
			WebcfgDebug("reset forced_sync after sync\n");
			forced_sync = 0;
			setForceSync("", "", 0);
		}

		pthread_mutex_lock (&sync_mutex);

		if (g_shutdown)
		{
			WebcfgInfo("g_shutdown is %d, proceeding to kill webconfig thread\n", g_shutdown);
			pthread_mutex_unlock (&sync_mutex);
			break;
		}

		retry_flag = get_doc_fail();
		WebcfgInfo("The retry flag value is %d\n", retry_flag);
		if (retry_flag == 1)
		{
			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_sec += 900;

			WebcfgDebug("B4 sync_condition pthread_cond_timedwait\n");
			rv = pthread_cond_timedwait(&sync_condition, &sync_mutex, &ts);
			WebcfgInfo("The retry flag value is %d\n", get_doc_fail());
			WebcfgInfo("The value of rv %d\n", rv);
		}
		else 
		{
			rv = pthread_cond_wait(&sync_condition, &sync_mutex);
		}

		if(rv == ETIMEDOUT && get_doc_fail() == 1)
		{
			WebcfgDebug("Inside the timedout condition\n");
			set_doc_fail(0);
			failedDocsRetry();
			WebcfgDebug("After the failedDocsRetry\n");
		}
		else if(!rv && !g_shutdown)
		{
			char *ForceSyncDoc = NULL;
			char* ForceSyncTransID = NULL;

			// Identify ForceSync based on docname
			getForceSync(&ForceSyncDoc, &ForceSyncTransID);
			WebcfgDebug("ForceSyncDoc %s ForceSyncTransID. %s\n", ForceSyncDoc, ForceSyncTransID);
			if(ForceSyncTransID !=NULL)
			{
				if((ForceSyncDoc != NULL) && strlen(ForceSyncDoc)>0)
				{
					forced_sync = 1;
					WebcfgDebug("Received signal interrupt to Force Sync\n");
					WEBCFG_FREE(ForceSyncDoc);
					WEBCFG_FREE(ForceSyncTransID);
				}
				else
				{
					WebcfgError("ForceSyncDoc is NULL\n");
					WEBCFG_FREE(ForceSyncTransID);
				}
			}

			WebcfgDebug("forced_sync is %d\n", forced_sync);
		}
		else if(g_shutdown)
		{
			WebcfgInfo("Received signal interrupt to RFC disable. g_shutdown is %d, proceeding to kill webconfig thread\n", g_shutdown);
			pthread_mutex_unlock (&sync_mutex);
			break;
		}
		
		pthread_mutex_unlock(&sync_mutex);

	}
	if(get_global_notify_threadid())
	{
		ret = pthread_join (get_global_notify_threadid(), NULL);
		if(ret ==0)
		{
			WebcfgInfo("pthread_join returns success\n");
		}
		else
		{
			WebcfgError("Error joining thread\n");
		}
	}
	WebcfgInfo("B4 pthread_exit\n");
	pthread_exit(0);
	WebcfgDebug("After pthread_exit\n");
	return NULL;
}

pthread_cond_t *get_global_sync_condition(void)
{
    return &sync_condition;
}

pthread_mutex_t *get_global_sync_mutex(void)
{
    return &sync_mutex;
}

bool get_global_shutdown()
{
    return g_shutdown;
}

void set_global_shutdown(bool shutdown)
{
    g_shutdown = shutdown;
}
/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
void processWebconfgSync(int status)
{
	int retry_count=0;
	int r_count=0;
	int configRet = -1;
	char *webConfigData = NULL;
	long res_code;
	int rv=0;
	char *transaction_uuid =NULL;
	char ct[256] = {0};
	size_t dataSize=0;

	WebcfgDebug("========= Start of processWebconfgSync =============\n");
	while(1)
	{
		if(retry_count >3)
		{
			WebcfgInfo("retry_count has reached max limit. Exiting.\n");
			retry_count=0;
			break;
		}
		configRet = webcfg_http_request(&webConfigData, r_count, status, &res_code, &transaction_uuid, ct, &dataSize);
		if(configRet == 0)
		{
			rv = handlehttpResponse(res_code, webConfigData, retry_count, transaction_uuid, ct, dataSize);
			if(rv ==1)
			{
				WebcfgDebug("No retries are required. Exiting..\n");
				break;
			}
		}
		else
		{
			WebcfgError("Failed to get webConfigData from cloud\n");
			//WEBCFG_FREE(transaction_uuid);
		}
		WebcfgInfo("webcfg_http_request BACKOFF_SLEEP_DELAY_SEC is %d seconds\n", BACKOFF_SLEEP_DELAY_SEC);
		sleep(BACKOFF_SLEEP_DELAY_SEC);
		retry_count++;
		WebcfgInfo("Webconfig retry_count is %d\n", retry_count);
	}
	WebcfgDebug("========= End of processWebconfgSync =============\n");
	return;
}

int handlehttpResponse(long response_code, char *webConfigData, int retry_count, char* transaction_uuid, char *ct, size_t dataSize)
{
	int first_digit=0;
	int msgpack_status=0;
	int err = 0;

	if(response_code == 304)
	{
		WebcfgInfo("webConfig is in sync with cloud. response_code:%ld\n", response_code);
		WEBCFG_FREE(transaction_uuid);
		return 1;
	}
	else if(response_code == 200)
	{
		WebcfgInfo("webConfig is not in sync with cloud. response_code:%ld\n", response_code);

		if(webConfigData !=NULL)
		{
			WebcfgDebug("webConfigData fetched successfully\n");
			WebcfgDebug("parseMultipartDocument\n");
			msgpack_status = parseMultipartDocument(webConfigData, ct, dataSize, transaction_uuid);

			if(msgpack_status == WEBCFG_SUCCESS)
			{
				WebcfgInfo("webConfigData applied successfully\n");
				return 1;
			}
			else
			{
				WebcfgError("Failed to apply root webConfigData received from server\n");
				return 1;
			}
		}
		else
		{
			WebcfgError("webConfigData is empty, need to retry\n");
		}
	}
	else if(response_code == 204)
	{
		WebcfgInfo("No configuration available for this device. response_code:%ld\n", response_code);
		WEBCFG_FREE(transaction_uuid);
		return 1;
	}
	else if(response_code == 403)
	{
		WebcfgError("Token is expired, fetch new token. response_code:%ld\n", response_code);
		createNewAuthToken(get_global_auth_token(), TOKEN_SIZE, get_deviceMAC(), get_global_serialNum() );
		WebcfgDebug("createNewAuthToken done in 403 case\n");
		err = 1;
	}
	else if(response_code == 429)
	{
		WebcfgInfo("No action required from client. response_code:%ld\n", response_code);
		WEBCFG_FREE(transaction_uuid);
		return 1;
	}
	first_digit = (int)(response_code / pow(10, (int)log10(response_code)));
	if((response_code !=403) && (first_digit == 4)) //4xx
	{
		WebcfgInfo("Action not supported. response_code:%ld\n", response_code);
		WEBCFG_FREE(transaction_uuid);
		return 1;
	}
	else //5xx & all other errors
	{
		WebcfgError("Error code returned, need to retry. response_code:%ld\n", response_code);
		if(retry_count == 3 && !err)
		{
			WebcfgDebug("3 retry attempts\n");
			WEBCFG_FREE(transaction_uuid);
			return 0;
		}
		WEBCFG_FREE(transaction_uuid);
	}
	return 0;
}

void webcfgStrncpy(char *destStr, const char *srcStr, size_t destSize)
{
    strncpy(destStr, srcStr, destSize-1);
    destStr[destSize-1] = '\0';
}

void getCurrent_Time(struct timespec *timer)
{
	clock_gettime(CLOCK_REALTIME, timer);
}

long timeVal_Diff(struct timespec *starttime, struct timespec *finishtime)
{
	long msec;
	msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
	msec+=(finishtime->tv_nsec-starttime->tv_nsec)/1000000;
	return msec;
}
