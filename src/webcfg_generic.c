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
#include <string.h>
#include <unistd.h>
#include <wdmp-c.h>
#include "webcfg_generic.h"
#include "webcfg_log.h"
#include "webcfg_rbus.h"
#include <rbus.h>
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define UNUSED(x) (void )(x)
#define WEBCFG_EVENT_NAME "webconfigSignal"
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
char *__attribute__((weak)) getDeviceBootTime(void);
char *__attribute__((weak)) getSerialNumber(void);
char *__attribute__((weak)) getProductClass(void);
char *__attribute__((weak)) getModelName(void);
char *__attribute__((weak)) getPartnerID(void);
char *__attribute__((weak)) getAccountID(void);
char *__attribute__((weak)) getRebootReason(void);
char *__attribute__((weak)) getConnClientParamName(void);
char *__attribute__((weak)) getFirmwareVersion(void);
char *__attribute__((weak)) get_deviceMAC(void);
char *__attribute__((weak)) getFirmwareUpgradeStartTime(void);
char *__attribute__((weak)) getFirmwareUpgradeEndTime(void);
char *__attribute__((weak)) get_global_systemReadyTime(void);
int __attribute__((weak)) setForceSync(char* pString, char *transactionId,int *session_status);
int __attribute__((weak)) getForceSync(char** pString, char **transactionId);
int __attribute__((weak)) Get_Webconfig_URL( char *pString);
int __attribute__((weak)) Set_Webconfig_URL( char *pString);
int __attribute__((weak)) Get_Supplementary_URL( char *name, char *pString);
int __attribute__((weak)) Set_Supplementary_URL( char *name, char *pString);
void __attribute__((weak)) setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus);
void __attribute__((weak)) sendNotification(char *payload, char *source, char *destination);
int __attribute__((weak)) registerWebcfgEvent(WebConfigEventCallback webcfgEventCB);
int __attribute__((weak)) unregisterWebcfgEvent();
WDMP_STATUS __attribute__((weak)) mapStatus(int ret);
void __attribute__((weak)) setAttributes(param_t *attArr, const unsigned int paramCount, money_trace_spans *timeSpan, WDMP_STATUS *retStatus);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

char *getDeviceBootTime(void)
{
    return NULL;
}

char *getSerialNumber(void)
{
    return NULL;
}

char *getProductClass(void)
{
    return NULL;
}

char *getModelName(void)
{
    return NULL;
}

char *getPartnerID(void)
{
    return NULL;
}

char *getAccountID(void)
{
    return NULL;
}

char *getRebootReason(void)
{
    return NULL;
}

char *getConnClientParamName(void)
{
    return NULL;
}
char *getFirmwareVersion(void)
{
    return NULL;
}

char* get_deviceMAC(void)
{
	return NULL;
}

char *getFirmwareUpgradeStartTime(void)
{
    return NULL;
}

char *getFirmwareUpgradeEndTime(void)
{
    return NULL;
}

char *get_global_systemReadyTime(void)
{
    return NULL;
}

int setForceSync(char* pString, char *transactionId,int *session_status)
{
    UNUSED(pString);
    UNUSED(transactionId);
    UNUSED(session_status);
    return 0;
}

int getForceSync(char** pString, char **transactionId)
{
    UNUSED(pString);
    UNUSED(transactionId);
    return 0;
}

int Get_Webconfig_URL( char *pString)
{
    UNUSED(pString);
    return 0;
}

int Set_Webconfig_URL( char *pString)
{
    UNUSED(pString);
    return 0;
}

int Get_Supplementary_URL( char *name, char *pString)
{
    UNUSED(name);
    UNUSED(pString);
    return 0;
}

int Set_Supplementary_URL( char *name, char *pString)
{
    UNUSED(name);
    UNUSED(pString);
    return 0;
}

void setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus)
{
	UNUSED(paramVal);
	UNUSED(paramCount);
	UNUSED(setType);
	UNUSED(transactionId);
	UNUSED(timeSpan);
	UNUSED(retStatus);
	UNUSED(ccspStatus);
	return;
}

void sendNotification(char *payload, char *source, char *destination)
{
	UNUSED(payload);
	UNUSED(source);
	UNUSED(destination);
	return;
}

int registerWebcfgEvent(WebConfigEventCallback webcfgEventCB)
{
	int ret = 0;
	if(isRbusEnabled())
	{
		char user_data[64] = {0};
		strncpy(user_data,WEBCFG_EVENT_NAME,sizeof(user_data)-1);

		rbusHandle_t rbus_handle = get_global_rbus_handle();
		ret = rbusEvent_Subscribe(rbus_handle, WEBCFG_EVENT_NAME,(rbusEventHandler_t) webcfgEventCB, user_data, 0);
		if(ret != RBUS_ERROR_SUCCESS)
		{
			WebcfgError("Unable to subscribe to event %s with rbus error code : %d\n", WEBCFG_EVENT_NAME, ret);
		}
		WebcfgInfo("registerWebcfgEvent : subscribe to %s ret value is %d\n",WEBCFG_EVENT_NAME,ret);
	}
	else
	{
		UNUSED(webcfgEventCB);
	}
	return ret;
}

int unregisterWebcfgEvent()
{
	int ret = 0 ;
	if(isRbusEnabled())
	{
		rbusHandle_t rbus_handle = get_global_rbus_handle();
		ret = rbusEvent_Unsubscribe(rbus_handle, WEBCFG_EVENT_NAME);
		if ( ret != RBUS_ERROR_SUCCESS )
		{
			WebcfgError("%s Unsubscribe failed\n",WEBCFG_EVENT_NAME);
		}
		else
		{
			WebcfgInfo("%s Unsubscribe with rbus successful\n",WEBCFG_EVENT_NAME);
		}
	}
	return ret;
}

WDMP_STATUS mapStatus(int ret)
{
	UNUSED(ret);
	return 0;
}

void setAttributes(param_t *attArr, const unsigned int paramCount, money_trace_spans *timeSpan, WDMP_STATUS *retStatus)
{
	UNUSED(attArr);
	UNUSED(paramCount);
	UNUSED(timeSpan);
	UNUSED(retStatus);
	return;
}
