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
#if defined(WEBCONFIG_BIN_SUPPORT)
#include "webcfg_rbus.h"
#include <rbus.h>
#endif
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define UNUSED(x) (void )(x)
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
#ifndef WEBCONFIG_BIN_SUPPORT
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
#endif
char *__attribute__((weak)) get_global_systemReadyTime(void);
int __attribute__((weak)) setForceSync(char* pString, char *transactionId,int *session_status);
int __attribute__((weak)) getForceSync(char** pString, char **transactionId);
#ifndef WEBCONFIG_BIN_SUPPORT
int __attribute__((weak)) Get_Webconfig_URL( char *pString);
int __attribute__((weak)) Set_Webconfig_URL( char *pString);
int __attribute__((weak)) Get_Supplementary_URL( char *name, char *pString);
int __attribute__((weak)) Set_Supplementary_URL( char *name, char *pString);
#endif
void __attribute__((weak)) setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus);
void __attribute__((weak)) sendNotification(char *payload, char *source, char *destination);
int __attribute__((weak)) registerWebcfgEvent(WebConfigEventCallback webcfgEventCB);
int __attribute__((weak)) unregisterWebcfgEvent();
WDMP_STATUS __attribute__((weak)) mapStatus(int ret);
void __attribute__((weak)) setAttributes(param_t *attArr, const unsigned int paramCount, money_trace_spans *timeSpan, WDMP_STATUS *retStatus);
#ifndef WEBCONFIG_BIN_SUPPORT
int __attribute__((weak)) rbus_GetValueFromDB( char* paramName, char** paramValue);
int __attribute__((weak)) rbus_StoreValueIntoDB(char *paramName, char *value);
#endif
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

#ifndef WEBCONFIG_BIN_SUPPORT
char *getDeviceBootTime(void)
{
    WebcfgInfo("Inside getDeviceBootTime weak function.\n");
    return NULL;
}

char *getSerialNumber(void)
{
    WebcfgInfo("Inside getSerialNumber weak function.\n");
    return NULL;
}

char *getProductClass(void)
{
    WebcfgInfo("Inside getProductClass weak function.\n");
    return NULL;
}

char *getModelName(void)
{
    WebcfgInfo("Inside getModelName weak function.\n");
    return NULL;
}

char *getPartnerID(void)
{
    WebcfgInfo("Inside getPartnerID weak function.\n");
    return NULL;
}

char *getAccountID(void)
{
    WebcfgInfo("Inside getAccountID weak function.\n");
    return NULL;
}

char *getRebootReason(void)
{
    WebcfgInfo("Inside getRebootReason weak function.\n");
    return NULL;
}

char *getConnClientParamName(void)
{
    return NULL;
}
char *getFirmwareVersion(void)
{
    WebcfgInfo("Inside getFirmwareVersion weak function.\n");
    return NULL;
}

char* get_deviceMAC(void)
{
	WebcfgInfo("Inside get_deviceMAC weak function.\n");
	return NULL;
}

char *getFirmwareUpgradeStartTime(void)
{
    WebcfgInfo("Inside getFirmwareUpgradeStartTime weak function.\n");
    return NULL;
}

char *getFirmwareUpgradeEndTime(void)
{
    WebcfgInfo("Inside getFirmwareUpgradeEndTime weak function.\n");
    return NULL;
}
#endif

char *get_global_systemReadyTime(void)
{
    WebcfgInfo("Inside get_global_systemReadyTime weak function.\n");
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

#ifndef WEBCONFIG_BIN_SUPPORT
int Get_Webconfig_URL( char *pString)
{
    WebcfgInfo("Inside Get_Webconfig_URL weak function.\n");
    UNUSED(pString);
    return 0;
}

int Set_Webconfig_URL( char *pString)
{
    WebcfgInfo("Inside Get_Webconfig_URL weak function.\n");
    UNUSED(pString);
    return 0;
}

int Get_Supplementary_URL( char *name, char *pString)
{
    WebcfgInfo("Inside Get_Supplementary_URL weak function.\n");
    UNUSED(name);
    UNUSED(pString);
    return 0;
}

int Set_Supplementary_URL( char *name, char *pString)
{
    WebcfgInfo("Inside Set_Supplementary_URL weak function.\n");
    UNUSED(name);
    UNUSED(pString);
    return 0;
}
#endif

void setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus)
{
#ifdef WEBCONFIG_BIN_SUPPORT
	if(isRbusEnabled())
	{
		int ccspRetStatus = 0;
		WDMP_STATUS ret = 0;
		WebcfgInfo("B4 setValues_rbus\n");
		setValues_rbus(paramVal, paramCount, setType, transactionId, timeSpan, &ret, &ccspRetStatus);
		WebcfgInfo("After setValues_rbus\n");
		WebcfgInfo("ret is %d\n", (int)ret);
		*retStatus = ret;
		*ccspStatus = ccspRetStatus;
		WebcfgInfo("ccspStatus %d retStatus is %d\n", *ccspStatus, (int)*retStatus);
	}
#else
        WebcfgInfo("B4 setValues weak fn.\n");
  	UNUSED(paramVal);
	UNUSED(paramCount);
	UNUSED(setType);
	UNUSED(transactionId);
	UNUSED(timeSpan);
	UNUSED(retStatus);
	UNUSED(ccspStatus);
#endif
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
#ifdef WEBCONFIG_BIN_SUPPORT
	int ret = 0;
	if(isRbusEnabled())
	{
		WebcfgInfo("In registerWebcfgEvent rbus\n");
		char user_data[64] = {0};
		//strncpy(user_data,WEBCFG_EVENT_NAME,sizeof(user_data)-1);

		rbusHandle_t rbus_handle = get_global_rbus_handle();
		//ret = rbusEvent_Subscribe(rbus_handle, WEBCFG_EVENT_NAME,(rbusEventHandler_t) webcfgEventCB, user_data, 0);
		ret = rbusEvent_Subscribe(rbus_handle, WEBCFG_EVENT_NAME, webcfgEventRbusHandler, user_data, 0);
		if(ret != RBUS_ERROR_SUCCESS)
		{
			WebcfgError("Unable to subscribe to event %s with rbus error code : %d\n", WEBCFG_EVENT_NAME, ret);
			ret = 0;
		}
		else
		{
			WebcfgInfo("registerWebcfgEvent : subscribe to %s ret value is %d\n",WEBCFG_EVENT_NAME,ret);
			ret = 1;
		}
	}
#else
        int ret = 0;
        WebcfgInfo("in registerWebcfgEvent else\n");
	UNUSED(webcfgEventCB);
#endif
	return ret;
}


int unregisterWebcfgEvent()
{
	int ret = 0 ;
#ifdef WEBCONFIG_BIN_SUPPORT
	if(isRbusEnabled())
	{
		rbusHandle_t rbus_handle = get_global_rbus_handle();
		ret = rbusEvent_Unsubscribe(rbus_handle, WEBCFG_EVENT_NAME);
		if ( ret != RBUS_ERROR_SUCCESS )
		{
			WebcfgError("%s Unsubscribe failed\n",WEBCFG_EVENT_NAME);
			ret = 0;
		}
		else
		{
			WebcfgInfo("%s Unsubscribe with rbus successful\n",WEBCFG_EVENT_NAME);
			ret = 1;
		}
	}
#endif
	return ret;
}

WDMP_STATUS mapStatus(int ret)
{
#ifdef WEBCONFIG_BIN_SUPPORT
	if(isRbusEnabled())
	{
		switch (ret)
		{
			case CCSP_Msg_Bus_OK:
				return WDMP_SUCCESS;
			case CCSP_Msg_BUS_TIMEOUT:
				return WDMP_ERR_TIMEOUT;
			case CCSP_ERR_INVALID_PARAMETER_VALUE:
				return WDMP_ERR_INVALID_PARAMETER_VALUE;
			default:
			return WDMP_FAILURE;
		}
	}
#else
	UNUSED(ret);
#endif
return ret;
}

void setAttributes(param_t *attArr, const unsigned int paramCount, money_trace_spans *timeSpan, WDMP_STATUS *retStatus)
{
	UNUSED(attArr);
	UNUSED(paramCount);
	UNUSED(timeSpan);
	UNUSED(retStatus);
	return;
}
#ifndef WEBCONFIG_BIN_SUPPORT
int rbus_GetValueFromDB( char* paramName, char** paramValue)
{
	WebcfgInfo("Inside rbus_GetValueFromDB weak fn\n");
	UNUSED(paramName);
	UNUSED(paramValue);
	return 0;
}

int rbus_StoreValueIntoDB(char *paramName, char *value)
{
	WebcfgInfo("Inside rbus_StoreValueIntoDB weak fn\n");
	UNUSED(paramName);
	UNUSED(value);
	return 0;
}
#endif
