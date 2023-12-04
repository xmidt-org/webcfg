/*
 * Copyright 2021 Comcast Cable Communications Management, LLC
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
#include "stdlib.h"
#include <string.h>
#include <unistd.h>
#include <wdmp-c.h>
#include "webcfg_generic.h"
#include "webcfg_log.h"
#if defined(WEBCONFIG_BIN_SUPPORT)
#include "webcfg_rbus.h"
#include <rbus/rbus.h>
#endif
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define UNUSED(x) (void )(x)
char *global_systemReadyTime = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
char *__attribute__((weak)) getDeviceBootTime(void);
char *__attribute__((weak)) getSerialNumber(void);
char *__attribute__((weak)) getProductClass(void);
char *__attribute__((weak)) getModelName(void);
#ifdef WAN_FAILOVER_SUPPORTED
char *__attribute__((weak)) getInterfaceName(void);
#endif
char *__attribute__((weak)) getPartnerID(void);
char *__attribute__((weak)) getAccountID(void);
char *__attribute__((weak)) getRebootReason(void);
char *__attribute__((weak)) getConnClientParamName(void);
char *__attribute__((weak)) getFirmwareVersion(void);
char *__attribute__((weak)) get_deviceMAC(void);
char *__attribute__((weak)) get_deviceWanMAC(void);
long __attribute__((weak)) getTimeOffset(void);
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
int __attribute__((weak)) rbus_GetValueFromDB( char* paramName, char** paramValue);
int __attribute__((weak)) rbus_StoreValueIntoDB(char *paramName, char *value);
int __attribute__((weak)) rbus_waitUntilSystemReady();
#ifdef FEATURE_SUPPORT_MQTTCM
int __attribute__((weak)) Get_Mqtt_LocationId( char *pString);
int __attribute__((weak)) Get_Mqtt_NodeId( char *pString);
int __attribute__((weak)) Get_Mqtt_Broker( char *pString);
int __attribute__((weak)) Get_Mqtt_Port( char *pString);
char *__attribute__((weak)) Get_Mqtt_ClientId();
#endif
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

long getTimeOffset(void)
{
    return 0;
}

char *getDeviceBootTime(void)
{
    char * BT= strdup("1634658266");
    return BT;
}

char *getSerialNumber(void)
{
    char *sn = strdup("123456789123456789");
    return sn;
}

char *getProductClass(void)
{
    char *PC= strdup("XB6");
    return PC;
}

char *getModelName(void)
{
    char *MN=strdup("CGM4140COM");
    return MN;
}

#ifdef WAN_FAILOVER_SUPPORTED
char *getInterfaceName(void)
{
    char *MN=strdup("erouter0");
    return MN;
}
#endif 

char *getPartnerID(void)
{
    char *PI=strdup("comcast");
    return PI;
}

char *getAccountID(void)
{
    char *AI=strdup("unkown");
    return AI;
}

char *getRebootReason(void)
{
    char *reboot=strdup("Forced_Software_upgrade");
    return reboot;
}

char *getConnClientParamName(void)
{
    return NULL;
}
char *getFirmwareVersion(void)
{
    char * FV= strdup("CGM4140COM_DEV_master_202100000000000sdy");
    return FV;
}

char *get_deviceMAC(void)
{
	return "123456789000";
}

char *get_deviceWanMAC(void)
{
	return "123456789000";
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
    return global_systemReadyTime;
}

void set_global_systemReadyTime(char* systemReadyTime)
{
    if(global_systemReadyTime != NULL)
    {
       free(global_systemReadyTime);
       global_systemReadyTime = NULL;
    }
    global_systemReadyTime = strdup(systemReadyTime);
}

int setForceSync(char* pString, char *transactionId,int *session_status)
{
#ifdef WEBCONFIG_BIN_SUPPORT
	if(isRbusEnabled())
	{
		//Set UNUSED as transaction id is fetched from force sync json schema on rbus mode.
		UNUSED(transactionId);
		set_rbus_ForceSync(pString, session_status);
	}
#else
	UNUSED(pString);
	UNUSED(transactionId);
	UNUSED(session_status);
#endif
	return 0;
}

int getForceSync(char** pString, char **transactionId)
{
#ifdef WEBCONFIG_BIN_SUPPORT
	if(isRbusEnabled())
	{
		char *str = NULL;
		char* transID = NULL;
		WebcfgDebug("B4 get_rbus_ForceSync\n");
		get_rbus_ForceSync(&str, &transID);
		*pString = str;
		*transactionId = transID;
		WebcfgDebug("get_rbus_ForceSync. *pString %s *transactionId %s\n", *pString, *transactionId);
	}
#else
	UNUSED(pString);
	UNUSED(transactionId);
#endif
	return 0;
}

int Get_Webconfig_URL( char *pString)
{
    WebcfgDebug("Inside Get_Webconfig_URL weak function.\n");
    UNUSED(pString);
    return 0;
}

int Set_Webconfig_URL( char *pString)
{
    WebcfgDebug("Inside Set_Webconfig_URL weak function.\n");
    UNUSED(pString);
    return 0;
}

int Get_Supplementary_URL( char *name, char *pString)
{
    WebcfgDebug("Inside Get_Supplementary_URL weak function.\n");
    UNUSED(name);
    UNUSED(pString);
    return 0;
}

int Set_Supplementary_URL( char *name, char *pString)
{
    WebcfgDebug("Inside Set_Supplementary_URL weak function.\n");
    UNUSED(name);
    UNUSED(pString);
    return 0;
}
#ifdef FEATURE_SUPPORT_MQTTCM
int Get_Mqtt_LocationId( char *pString)
{
    WebcfgDebug("Inside Get_Mqtt_LocationId weak function.\n");
    UNUSED(pString);
    return 0;
}
int Get_Mqtt_NodeId( char *pString)
{
    WebcfgDebug("Inside Get_Mqtt_NodeId weak function.\n");
    UNUSED(pString);
    return 0;
}
int Get_Mqtt_Broker( char *pString)
{
    WebcfgDebug("Inside Get_Mqtt_Broker weak function.\n");
    UNUSED(pString);
    return 0;
}
int Get_Mqtt_Port( char *pString)
{
    WebcfgDebug("Inside Get_Mqtt_Port weak function.\n");
    UNUSED(pString);
    return 0;
}
char* Get_Mqtt_ClientId()
{
    WebcfgDebug("Inside Get_Mqtt_ClientId weak function.\n");
    return NULL;
}
#endif
void setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus)
{
#ifdef WEBCONFIG_BIN_SUPPORT
	if(isRbusEnabled())
	{
		int ccspRetStatus = 0;
		WDMP_STATUS ret = 0;
		WebcfgDebug("B4 setValues_rbus\n");
		setValues_rbus(paramVal, paramCount, setType, transactionId, timeSpan, &ret, &ccspRetStatus);
		WebcfgDebug("ret is %d\n", (int)ret);
		*retStatus = ret;
		*ccspStatus = ccspRetStatus;
		WebcfgDebug("ccspStatus %d retStatus is %d\n", *ccspStatus, (int)*retStatus);
	}
#else
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
#ifdef WEBCONFIG_BIN_SUPPORT
	WebcfgDebug("Inside sendNotification weak impl\n");
	if(isRbusEnabled())
	{
		WebcfgDebug("B4 sendNotification_rbus\n");
		sendNotification_rbus(payload, source, destination);
	}
#else
	UNUSED(payload);
	UNUSED(source);
	UNUSED(destination);
#endif
	return;
}

int registerWebcfgEvent(WebConfigEventCallback webcfgEventCB)
{
	int ret = 0;
#ifdef WEBCONFIG_BIN_SUPPORT
	if(isRbusEnabled())
	{
		int rc = 0;
		WebcfgInfo("Registering RBUS event Element\n");
		rc = registerRBUSEventElement();
		WebcfgDebug("RBUSEventElement rc %d\n", rc);
		if(rc == RBUS_ERROR_SUCCESS)
		{
			ret = 1;
		}
	}
#else
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
		int rc = 0;
		rc = removeRBUSEventElement();
		WebcfgDebug("removeRBUSEventElement returns rc %d\n", rc);
		if ( rc != RBUS_ERROR_SUCCESS )
		{
			WebcfgError("Rbus UnRegistration with %s failed\n", WEBCFG_EVENT_NAME);
		}
		else
		{
			WebcfgDebug("Rbus UnRegistration with %s is success\n", WEBCFG_EVENT_NAME);
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
			case CCSP_ERR_UNSUPPORTED_NAMESPACE:
				return WDMP_ERR_UNSUPPORTED_NAMESPACE;
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

int rbus_GetValueFromDB( char* paramName, char** paramValue)
{
	WebcfgDebug("Inside rbus_GetValueFromDB weak fn\n");
	UNUSED(paramName);
	UNUSED(paramValue);
	return 0;
}

int rbus_StoreValueIntoDB(char *paramName, char *value)
{
	WebcfgDebug("Inside rbus_StoreValueIntoDB weak fn\n");
	UNUSED(paramName);
	UNUSED(value);
	return 0;
}

int rbus_waitUntilSystemReady()
{
	WebcfgDebug("Inside rbus_waitUntilSystemReady weak fn\n");
	return 0;
}
