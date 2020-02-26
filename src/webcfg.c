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
#include "multipart.h"
#include "webcfg_notify.h"
#include "webcfg_generic.h"
#include "webcfgparam.h"
#include "webcfg_auth.h"
#include <msgpack.h>
#include <wdmp-c.h>
#include <base64.h>
#include "macbindingdoc.h"
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
pthread_mutex_t periodicsync_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t periodicsync_condition=PTHREAD_COND_INITIALIZER;
bool g_shutdown  = false;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void *WebConfigMultipartTask();
int processMsgPackDocument(char *jsonData, int *retStatus, char **docVersion);
int handleHttpResponse(long response_code, char *webConfigData, int retry_count, int index, char* transaction_uuid);
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

void initWebConfigMultipartTask()
{
	int err = 0;
	pthread_t threadId;

	err = pthread_create(&threadId, NULL, WebConfigMultipartTask, NULL);
	if (err != 0) 
	{
		WebConfigLog("Error creating WebConfigMultipartTask thread :[%s]\n", strerror(err));
	}
	else
	{
		WebConfigLog("WebConfigMultipartTask Thread created Successfully\n");
	}
}

void *WebConfigMultipartTask()
{
	pthread_detach(pthread_self());
	int index=0;
	int ret = 0;
	int rv=0;
        struct timeval tp;
        struct timespec ts;
        time_t t;
	int wait_flag=0;
	int forced_sync=0;//, syncIndex = 0;
        int value=0;
	value =Get_PeriodicSyncCheckInterval();

	//start webconfig notification thread.
	initWebConfigNotifyTask();

	while(1)
	{
		if(forced_sync)
		{
			//trigger sync only for particular index
			/*WebConfigLog("Trigger Forced sync for index %d\n", syncIndex);
			processWebconfigSync(syncIndex, (int)status);
			WebConfigLog("reset forced_sync and syncIndex after sync\n");
			forced_sync = 0;
			syncIndex = 0;
			WebConfigLog("reset ForceSyncCheck after sync\n");
			setForceSyncCheck(index, false, "", 0);*/
		}
		else if(!wait_flag)
		{
			processWebconfigSync(index);
		}

		pthread_mutex_lock (&periodicsync_mutex);
		gettimeofday(&tp, NULL);
		ts.tv_sec = tp.tv_sec;
		ts.tv_nsec = tp.tv_usec * 1000;
		ts.tv_sec += value;

		if (g_shutdown)
		{
			WebConfigLog("g_shutdown is %d, proceeding to kill webconfig thread\n", g_shutdown);
			pthread_mutex_unlock (&periodicsync_mutex);
			break;
		}
		value=Get_PeriodicSyncCheckInterval();
		WebConfigLog("PeriodicSyncCheckInterval value is %d\n",value);
		if(value == 0)
		{
			WebcfgDebug("B4 periodicsync_condition pthread_cond_wait\n");
			pthread_cond_wait(&periodicsync_condition, &periodicsync_mutex);
			rv =0;
		}
		else
		{
			WebcfgDebug("B4 periodicsync_condition pthread_cond_timedwait\n");
			rv = pthread_cond_timedwait(&periodicsync_condition, &periodicsync_mutex, &ts);
		}

		if(!rv && !g_shutdown)
		{
			time(&t);
			//BOOL ForceSyncEnable;
			//char* ForceSyncTransID = NULL;

			// Iterate through all indexes to check which index needs ForceSync
			/*count = getConfigNumberOfEntries();
			WebConfigLog("count returned is:%d\n", count);

			for(i = 0; i < count; i++)
			{
				WebcfgDebug("B4 getInstanceNumberAtIndex for i %d\n", i);
				index = getInstanceNumberAtIndex(i);
				WebcfgDebug("getForceSyncCheck for index %d\n", index);
				getForceSyncCheck(index,&ForceSyncEnable, &ForceSyncTransID);
				WebcfgDebug("ForceSyncEnable is %d\n", ForceSyncEnable);
				if(ForceSyncTransID !=NULL)
				{
					if(ForceSyncEnable)
					{
						wait_flag=0;
						forced_sync = 1;
						syncIndex = index;
						WebConfigLog("Received signal interrupt to getForceSyncCheck at %s\n",ctime(&t));
						WEBCFG_FREE(ForceSyncTransID);
						break;
					}
					WebConfigLog("ForceSyncEnable is false\n");
					WEBCFG_FREE(ForceSyncTransID);
				}
			}*/
			WebConfigLog("forced_sync is %d\n", forced_sync);
			if(!forced_sync)
			{
				wait_flag=1;
				value=Get_PeriodicSyncCheckInterval();
				WebConfigLog("Received signal interrupt to change the sync interval to %d\n",value);
			}
		}
		else if (rv == ETIMEDOUT && !g_shutdown)
		{
			time(&t);
			wait_flag=0;
			WebConfigLog("Periodic Sync Interval %d sec and syncing at %s\n",value,ctime(&t));
		}
		else if(g_shutdown)
		{
			WebConfigLog("Received signal interrupt to RFC disable. g_shutdown is %d, proceeding to kill webconfig thread\n", g_shutdown);
			pthread_mutex_unlock (&periodicsync_mutex);
			break;
		}
		pthread_mutex_unlock(&periodicsync_mutex);

	}
	if(get_global_notify_threadid())
	{
		ret = pthread_join (get_global_notify_threadid(), NULL);
		if(ret ==0)
		{
			WebConfigLog("pthread_join returns success\n");
		}
		else
		{
			WebConfigLog("Error joining thread\n");
		}
	}
	WebConfigLog("B4 pthread_exit\n");
	pthread_exit(0);
	WebcfgDebug("After pthread_exit\n");
	return NULL;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
void processWebconfigSync(int index)
{
	int retry_count=0;
	//int r_count=0;
	int configRet = -1;
	//char *webConfigData = NULL;
	//long res_code;
	int rv=0;
	//char *transaction_uuid =NULL;

	WebcfgDebug("========= Start of processWebconfigSync =============\n");
	while(1)
	{
		if(retry_count >3)
		{
			WebConfigLog("retry_count has reached max limit. Exiting.\n");
			retry_count=0;
			break;
		}
		printf("index is %d\n", index);
		//configRet = webcfg_http_request(&webConfigData, r_count, &res_code, &transaction_uuid);
		WebConfigLog("processMultipartDocument\n");
		configRet = processMultipartDocument();
		WebConfigLog("processMultipartDocument complete\n");
		if(configRet == 0)
		{
			//rv = handleHttpResponse(res_code, webConfigData, retry_count, index, transaction_uuid); :TODO
			rv  = 1;
			if(rv ==1)
			{
				WebConfigLog("No retries are required. Exiting..\n");
				break;
			}
		}
		else
		{
			WebConfigLog("Failed to get webConfigData from cloud\n");
		}
		WebConfigLog("webcfg_http_request BACKOFF_SLEEP_DELAY_SEC is %d seconds\n", BACKOFF_SLEEP_DELAY_SEC);
		sleep(BACKOFF_SLEEP_DELAY_SEC);
		retry_count++;
		WebConfigLog("Webconfig retry_count is %d\n", retry_count);
	}
	WebcfgDebug("========= End of processWebconfigSync =============\n");
	return;
}

int handleHttpResponse(long response_code, char *webConfigData, int retry_count, int index, char* transaction_uuid)
{
	int first_digit=0;
	int msgpack_status=0;
	int setRet = 0;
	int ret=0, getRet = 0;
	char *configURL = NULL;
	char *configVersion = NULL;
	char *RequestTimeStamp=NULL;
	char *newDocVersion = NULL;
	int err = 0;

        //get common items for all status codes and send notification.
        getRet = _getConfigURL(index, &configURL); //remove the index and add the sub doc names
	if(getRet)
	{
		WebcfgDebug("configURL for index %d is %s\n", index, configURL);
	}
	else
	{
		WebConfigLog("getConfigURL failed for index %d\n", index);
	}

	getRet = _getConfigVersion(index, &configVersion);
	if(getRet)
	{
		WebConfigLog("configVersion for index %d is %s\n", index, configVersion);
	}
	else
	{
		WebConfigLog("getConfigVersion failed for index %d\n", index);
	}

        ret = _setRequestTimeStamp(index);
	if(ret == 0)
	{
		WebcfgDebug("RequestTimeStamp set successfully for index %d\n", index);
	}
	else
	{
		WebConfigLog("Failed to set RequestTimeStamp for index %d\n", index);
	}

        getRet = _getRequestTimeStamp(index, &RequestTimeStamp);
	if(getRet)
	{
		WebcfgDebug("RequestTimeStamp for index %d is %s\n", index, RequestTimeStamp);
	}
	else
	{
		WebConfigLog("RequestTimeStamp get failed for index %d\n", index);
	}

	if(response_code == 304)
	{
		WebConfigLog("webConfig is in sync with cloud. response_code:%d\n", response_code);
		_setSyncCheckOK(index, true);
		addWebConfigNotifyMsg(configURL, response_code, NULL, 0, RequestTimeStamp , configVersion, transaction_uuid);
		return 1;
	}
	else if(response_code == 200)
	{
		WebConfigLog("webConfig is not in sync with cloud. response_code:%d\n", response_code);

		if(webConfigData !=NULL)
		{
			WebcfgDebug("webConfigData fetched successfully\n");
			msgpack_status = processMsgPackDocument(webConfigData, &setRet, &newDocVersion);
			WebConfigLog("setRet after process msgPack is %d\n", setRet);
			WebcfgDebug("newDocVersion is %s\n", newDocVersion);

			if(msgpack_status == 1)
			{
				WebcfgDebug("processMsgPackDocument success\n");
				if(configURL!=NULL && newDocVersion !=NULL)
				{
					WebConfigLog("Configuration settings from %s version %s were applied successfully\n", configURL, newDocVersion );
				}

				WebcfgDebug("set version and syncCheckOK for success\n");
				ret = _setConfigVersion(index, newDocVersion);//get new version from msgpack doc
				if(ret == 0)
				{
					WebcfgDebug("Config Version %s set successfully for index %d\n", newDocVersion, index);
				}
				else
				{
					WebConfigLog("Failed to set Config version %s for index %d\n", newDocVersion, index);
				}

				ret = _setSyncCheckOK(index, true );
				if(ret == 0)
				{
					WebcfgDebug("SyncCheckOK set successfully for index %d\n", index);
				}
				else
				{
					WebConfigLog("Failed to set SyncCheckOK for index %d\n", index);
				}

				addWebConfigNotifyMsg(configURL, response_code, "success", setRet, RequestTimeStamp , newDocVersion, transaction_uuid);
				return 1;
			}
			else
			{
				WebConfigLog("Failure in processJsonDocument\n");
				ret = _setSyncCheckOK(index, false);
				if(ret == 0)
				{
					WebcfgDebug("SyncCheckOK set to false for index %d\n", index);
				}
				else
				{
					WebConfigLog("Failed to set SyncCheckOK to false for index %d\n", index);
				}

				WebConfigLog("Configuration settings from %s version %s FAILED\n", configURL, newDocVersion );
				WebConfigLog("Sending Webconfig apply Failure Notification\n");
				addWebConfigNotifyMsg(configURL, response_code, "failed", setRet, RequestTimeStamp , newDocVersion, transaction_uuid);
				return 1;
			}
		}
		else
		{
			WebConfigLog("webConfigData is empty, need to retry\n");
		}
	}
	else if(response_code == 204)
	{
		WebConfigLog("No configuration available for this device. response_code:%d\n", response_code);
		addWebConfigNotifyMsg(configURL, response_code, NULL, 0, RequestTimeStamp , configVersion, transaction_uuid);
		return 1;
	}
	else if(response_code == 403)
	{
		WebConfigLog("Token is expired, fetch new token. response_code:%d\n", response_code);
		createNewAuthToken(get_global_auth_token(), sizeof(get_global_auth_token()), get_global_deviceMAC(), get_global_serialNum() );
		WebcfgDebug("createNewAuthToken done in 403 case\n");
		err = 1;
	}
	else if(response_code == 429)
	{
		WebConfigLog("No action required from client. response_code:%d\n", response_code);
		WEBCFG_FREE(configURL);
		WEBCFG_FREE(configVersion);
		WEBCFG_FREE(RequestTimeStamp);
		WEBCFG_FREE(transaction_uuid);
		return 1;
	}
	first_digit = (int)(response_code / pow(10, (int)log10(response_code)));
	if((response_code !=403) && (first_digit == 4)) //4xx
	{
		WebConfigLog("Action not supported. response_code:%d\n", response_code);
		addWebConfigNotifyMsg(configURL, response_code, NULL, 0, RequestTimeStamp , configVersion, transaction_uuid);
		return 1;
	}
	else //5xx & all other errors
	{
		WebConfigLog("Error code returned, need to retry. response_code:%d\n", response_code);
		if(retry_count == 3 && !err)
		{
			WebcfgDebug("Sending Notification after 3 retry attempts\n");
			addWebConfigNotifyMsg(configURL, response_code, NULL, 0, RequestTimeStamp , configVersion, transaction_uuid);
			return 0;
		}
		WEBCFG_FREE(configURL);
		WEBCFG_FREE(configVersion);
		WEBCFG_FREE(RequestTimeStamp);
		WEBCFG_FREE(transaction_uuid);
	}
	return 0;
}

int processMsgPackDocument(char *jsonData, int *retStatus, char **docVersion)
{
	printf("jsonData %s retStatus %d docVersion %s\n" , jsonData, *retStatus, *docVersion);
	WebConfigLog("--------------processMsgPackDocument----------------\n");
	return 0;
}

int processMultipartDocument()
{
	int r_count=0;
	int configRet = -1;
        webcfgparam_t *pm;
	char *webConfigData = NULL;
	long res_code;
          
        int len =0, i=0;
	void* subdbuff;
	char *subfileData = NULL;
	param_t *reqParam = NULL;
	WDMP_STATUS ret = WDMP_FAILURE;
	int ccspStatus=0;
	char* b64buffer =  NULL;
	size_t encodeSize = 0;
	size_t subLen=0;
	struct timespec start,end,*startPtr,*endPtr;
        startPtr = &start;
        endPtr = &end;

	configRet = webcfg_http_request(&webConfigData, r_count, &res_code, &subfileData, &len);
	if(configRet == 0)
	{
		WebConfigLog("config ret success\n");
		subLen = (size_t) len;
		subdbuff = ( void*)subfileData;
		WebConfigLog("subLen is %ld\n", subLen);

		/*********** base64 encode *****************/
		getCurrent_Time(startPtr);
		WebConfigLog("-----------Start of Base64 Encode ------------\n");
		encodeSize = b64_get_encoded_buffer_size( subLen );
		WebConfigLog("encodeSize is %d\n", encodeSize);
		b64buffer = malloc(encodeSize + 1);
		b64_encode((const uint8_t *)subfileData, subLen, (uint8_t *)b64buffer);
		b64buffer[encodeSize] = '\0' ;

		WebConfigLog("---------- End of Base64 Encode -------------\n");
		getCurrent_Time(endPtr);
                WebConfigLog("Base64 Encode Elapsed time : %ld ms\n", timeVal_Diff(startPtr, endPtr));

		WebConfigLog("Final Encoded data: %s\n",b64buffer);
		WebConfigLog("Final Encoded data length: %d\n",strlen(b64buffer));
		/*********** base64 encode *****************/


		WebConfigLog("Proceed to setValues\n");
		reqParam = (param_t *) malloc(sizeof(param_t));

		reqParam[0].name = "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.portMappingData";
		reqParam[0].value = b64buffer;
		reqParam[0].type = WDMP_BASE64;

		WebConfigLog("Request:> param[0].name = %s\n",reqParam[0].name);
		WebConfigLog("Request:> param[0].value = %s\n",reqParam[0].value);
		WebConfigLog("Request:> param[0].type = %d\n",reqParam[0].type);

		WebcfgInfo("WebConfig SET Request\n");

		setValues(reqParam, 1, 0, NULL, NULL, &ret, &ccspStatus);
		WebcfgInfo("Processed WebConfig SET Request\n");
		WebcfgInfo("ccspStatus is %d\n", ccspStatus);
                if(ret == WDMP_SUCCESS)
                {
                        WebConfigLog("setValues success. ccspStatus : %d\n", ccspStatus);
                }
                else
                {
                      WebConfigLog("setValues Failed. ccspStatus : %d\n", ccspStatus);
                }
		//Test purpose to decode config doc from webpa. This is to confirm data sent from webpa is proper
		WebConfigLog("--------------decode config doc from webpa-------------\n");

		//decode root doc
		WebConfigLog("--------------decode root doc-------------\n");
		pm = webcfgparam_convert( subdbuff, len+1 );

		if ( NULL != pm)
		{
			for(i = 0; i < (int)pm->entries_count ; i++)
			{
				WebConfigLog("pm->entries[%d].name %s\n", i, pm->entries[i].name);
				WebConfigLog("pm->entries[%d].value %s\n" , i, pm->entries[i].value);
				WebConfigLog("pm->entries[%d].type %d\n", i, pm->entries[i].type);
			}
			WebConfigLog("--------------decode root doc done-------------\n");
			WebConfigLog("blob_size is %d\n", pm->entries[0].value_size);

			/************ portmapping inner blob decode ****************/

			/*portmappingdoc_t *rpm;
			WebConfigLog("--------------decode blob-------------\n");
			rpm = portmappingdoc_convert( pm->entries[0].value, pm->entries[0].value_size );

			if(NULL != rpm)
			{
				WebConfigLog("rpm->entries_count is %ld\n", rpm->entries_count);

				for(i = 0; i < (int)rpm->entries_count ; i++)
				{
					WebConfigLog("rpm->entries[%d].InternalClient %s\n", i, rpm->entries[i].internal_client);
					WebConfigLog("rpm->entries[%d].ExternalPortEndRange %s\n" , i, rpm->entries[i].external_port_end_range);
					WebConfigLog("rpm->entries[%d].Enable %s\n", i, rpm->entries[i].enable?"true":"false");
					WebConfigLog("rpm->entries[%d].Protocol %s\n", i, rpm->entries[i].protocol);
					WebConfigLog("rpm->entries[%d].Description %s\n", i, rpm->entries[i].description);
					WebConfigLog("rpm->entries[%d].external_port %s\n", i, rpm->entries[i].external_port);
				}

				portmappingdoc_destroy( rpm );

			}*/
			/************ portmapping inner blob decode ****************/

			/************ macbinding inner blob decode ****************/

			macbindingdoc_t *rpm;
			printf("--------------decode blob-------------\n");
			rpm = macbindingdoc_convert( pm->entries[0].value, pm->entries[0].value_size );
			if(NULL != rpm)
			{
				printf("rpm->entries_count is %zu\n", rpm->entries_count);

				for(i = 0; i < (int)rpm->entries_count ; i++)
				{
					printf("rpm->entries[%d].Yiaddr %s\n", i, rpm->entries[i].yiaddr);
					printf("rpm->entries[%d].Chaddr %s\n" , i, rpm->entries[i].chaddr);
				}

				macbindingdoc_destroy( rpm );
			}
			/************ macbinding inner blob decode ****************/

			webcfgparam_destroy( pm );
			/*WAL_FREE(reqParam);
			if(b64buffer != NULL)
			{
				free(b64buffer);
				b64buffer = NULL;
			}
			*/
			return 0;
		}
		else
		{
			WebConfigLog("pm is NULL, root doc decode failed\n");
			return -1;
		}
	}	
	else
	{
		WebConfigLog("webcfg_http_request failed\n");
		return -1;
	}
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

int Get_PeriodicSyncCheckInterval()
{
	return 0;
}

