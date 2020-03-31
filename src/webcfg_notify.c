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
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct _notify_params
{
	char * url;
	long status_code;
	char * application_status;
	int application_details;
	char * request_timestamp;
	char * version;
	char * transaction_uuid;
} notify_params_t;
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static pthread_t NotificationThreadId=0;
pthread_mutex_t notify_mut=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notify_con=PTHREAD_COND_INITIALIZER;
notify_params_t *notifyMsg = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void* processWebConfigNotification();
void free_notify_params_struct(notify_params_t *param);
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
pthread_t get_global_notify_threadid()
{
    return NotificationThreadId;
}

//To handle webconfig notification tasks
void initWebConfigNotifyTask()
{
	int err = 0;
	WebConfigLog("Inside initWebConfigNotifyTask\n");
	err = pthread_create(&NotificationThreadId, NULL, processWebConfigNotification, NULL);
	if (err != 0)
	{
		WebConfigLog("Error creating Webconfig Notification thread :[%s]\n", strerror(err));
	}
	else
	{
		WebConfigLog("Webconfig Notification thread created Successfully\n");
	}

}

void addWebConfigNotifyMsg(char *url, long status_code, char *application_status, int application_details, char *request_timestamp, char *version, char *transaction_uuid)
{
	notify_params_t *args = NULL;
	args = (notify_params_t *)malloc(sizeof(notify_params_t));

	if(args != NULL)
	{
                WebcfgDebug("pthread mutex lock\n");
		pthread_mutex_lock (&notify_mut);
		memset(args, 0, sizeof(notify_params_t));
		if(url != NULL)
		{
			args->url = strdup(url);
			WEBCFG_FREE(url);
		}
		args->status_code = status_code;
		if(application_status != NULL)
		{
			args->application_status = strdup(application_status);
		}
		args->application_details = application_details;
		if(request_timestamp != NULL)
		{
			args->request_timestamp = strdup(request_timestamp);
			WEBCFG_FREE(request_timestamp);
		}
		if(version != NULL)
		{
			args->version = strdup(version);
			WEBCFG_FREE(version);
		}
		if(transaction_uuid != NULL)
		{
			args->transaction_uuid = strdup(transaction_uuid);
			WEBCFG_FREE(transaction_uuid);
		}
		WebcfgDebug("args->url:%s args->status_code:%ld args->application_status:%s args->application_details:%d args->request_timestamp:%s args->version:%s args->transaction_uuid:%s\n", args->url, args->status_code, args->application_status, args->application_details, args->request_timestamp, args->version, args->transaction_uuid );

		notifyMsg = args;

                WebcfgDebug("Before notify pthread cond signal\n");
		pthread_cond_signal(&notify_con);
                WebcfgDebug("After notify pthread cond signal\n");
		pthread_mutex_unlock (&notify_mut);
                WebcfgDebug("pthread mutex unlock\n");
	}
}

//Notify thread function waiting for notify msgs
void* processWebConfigNotification()
{
	char device_id[32] = { '\0' };
	cJSON *notifyPayload = NULL;
	char  * stringifiedNotifyPayload = NULL;
	notify_params_t *msg = NULL;
	char dest[512] = {'\0'};
	char *source = NULL;
	cJSON * reports, *one_report;

	while(1)
	{
                WebConfigLog("processWebConfigNotification Inside while\n");
		pthread_mutex_lock (&notify_mut);
		WebConfigLog("processWebConfigNotification mutex lock\n");
		msg = notifyMsg;
		if(msg !=NULL)
		{
                        WebConfigLog("Processing msg\n");
			if(strlen(get_global_deviceMAC()) == 0)
			{
				WebConfigLog("deviceMAC is NULL, failed to send Webconfig Notification\n");
			}
			else
			{
				snprintf(device_id, sizeof(device_id), "mac:%s", get_global_deviceMAC());
				WebConfigLog("webconfig Device_id %s\n", device_id);

				notifyPayload = cJSON_CreateObject();

				if(notifyPayload != NULL)
				{
					cJSON_AddStringToObject(notifyPayload,"device_id", device_id);

					if(msg)
					{
						cJSON_AddItemToObject(notifyPayload, "reports", reports = cJSON_CreateArray());
						cJSON_AddItemToArray(reports, one_report = cJSON_CreateObject());
						cJSON_AddStringToObject(one_report, "url", (NULL != msg->url) ? msg->url : "unknown");
						cJSON_AddNumberToObject(one_report,"http_status_code", msg->status_code);
                                                if(msg->status_code == 200)
                                                {
						        cJSON_AddStringToObject(one_report,"document_application_status", (NULL != msg->application_status) ? msg->application_status : "unknown");
						        cJSON_AddNumberToObject(one_report,"document_application_details", msg->application_details);
                                                }
						cJSON_AddNumberToObject(one_report, "request_timestamp", (NULL != msg->request_timestamp) ? atoi(msg->request_timestamp) : 0);
						cJSON_AddStringToObject(one_report,"version", (NULL != msg->version && (strlen(msg->version)!=0)) ? msg->version : "NONE");
						cJSON_AddStringToObject(one_report,"transaction_uuid", (NULL != msg->transaction_uuid && (strlen(msg->transaction_uuid)!=0)) ? msg->transaction_uuid : "unknown");
					}
					stringifiedNotifyPayload = cJSON_PrintUnformatted(notifyPayload);
					cJSON_Delete(notifyPayload);
				}

				snprintf(dest,sizeof(dest),"event:config-version-report/%s",device_id);
				WebConfigLog("dest is %s\n", dest);

				if (stringifiedNotifyPayload != NULL && strlen(device_id) != 0)
				{
					source = (char*) malloc(sizeof(char) * sizeof(device_id));
					strncpy(source, device_id, sizeof(device_id));
					WebConfigLog("source is %s\n", source);
					WebConfigLog("stringifiedNotifyPayload is %s\n", stringifiedNotifyPayload);
					sendNotification(stringifiedNotifyPayload, source, dest);
				}
				if(msg != NULL)
				{
					free_notify_params_struct(msg);
					notifyMsg = NULL;
				}
			}
			pthread_mutex_unlock (&notify_mut);
			WebcfgDebug("processWebConfigNotification mutex unlock\n");
		}
		else
		{
			WebConfigLog("Before pthread cond wait in notify thread\n");
			pthread_cond_wait(&notify_con, &notify_mut);
			pthread_mutex_unlock (&notify_mut);
			WebcfgDebug("mutex unlock in notify thread after cond wait\n");
		}
	}
	return NULL;
}

void free_notify_params_struct(notify_params_t *param)
{
    if(param != NULL)
    {
        if(param->url != NULL)
        {
            WEBCFG_FREE(param->url);
        }
        if(param->application_status != NULL)
        {
            WEBCFG_FREE(param->application_status);
        }
        if(param->request_timestamp != NULL)
        {
            WEBCFG_FREE(param->request_timestamp);
        }
        if(param->version != NULL)
        {
            WEBCFG_FREE(param->version);
        }
	if(param->transaction_uuid != NULL)
        {
	    WEBCFG_FREE(param->transaction_uuid);
        }
        WEBCFG_FREE(param);
    }
}

