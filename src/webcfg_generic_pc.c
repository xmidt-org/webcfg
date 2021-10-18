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
#include "stdlib.h"
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
//#ifndef WEBCONFIG_BIN_SUPPORT
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
//#endif
char *__attribute__((weak)) get_global_systemReadyTime(void);
int __attribute__((weak)) setForceSync(char* pString, char *transactionId,int *session_status);
int __attribute__((weak)) getForceSync(char** pString, char **transactionId);
//#ifndef WEBCONFIG_BIN_SUPPORT
int __attribute__((weak)) Get_Webconfig_URL( char *pString);
int __attribute__((weak)) Set_Webconfig_URL( char *pString);
int __attribute__((weak)) Get_Supplementary_URL( char *name, char *pString);
int __attribute__((weak)) Set_Supplementary_URL( char *name, char *pString);
//#endif
void __attribute__((weak)) setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus);
void __attribute__((weak)) sendNotification(char *payload, char *source, char *destination);
int __attribute__((weak)) registerWebcfgEvent(WebConfigEventCallback webcfgEventCB);
int __attribute__((weak)) unregisterWebcfgEvent();
WDMP_STATUS __attribute__((weak)) mapStatus(int ret);
void __attribute__((weak)) setAttributes(param_t *attArr, const unsigned int paramCount, money_trace_spans *timeSpan, WDMP_STATUS *retStatus);
//#ifndef WEBCONFIG_BIN_SUPPORT
int __attribute__((weak)) rbus_GetValueFromDB( char* paramName, char** paramValue);
int __attribute__((weak)) rbus_StoreValueIntoDB(char *paramName, char *value);
//#endif
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

//#ifndef WEBCONFIG_BIN_SUPPORT
char *getDeviceBootTime(void)
{
    char * BT= strdup("163XXXX");
    return BT;
}

char *getSerialNumber(void)
{
    char *sn = strdup("28093006017710XXXX");
    return sn;
}

char *getProductClass(void)
{
    char *PC= strdup("XXX");
    return PC;
}

char *getModelName(void)
{
    char *MN=strdup("CGMXXXX");
    return MN;
}

char *getPartnerID(void)
{
    char *PI=strdup("comXXX");
    return PI;
}

char *getAccountID(void)
{
    char *AI=strdup("8842515XXXXXXXX");
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
    char * FV= strdup("CGMXXXXXXXXXXXXXXXXXXXXXX");
    return FV;
}

char *get_deviceMAC(void)
{
	//char * mac = strdup("b2XXXXXXXXXX");
	return "b2XXXXXXXXXX";
}

char *getFirmwareUpgradeStartTime(void)
{
    return NULL;
}

char *getFirmwareUpgradeEndTime(void)
{
    return NULL;
}
//#endif

char *get_global_systemReadyTime(void)
{
    return NULL;
}

int setForceSync(char* pString, char *transactionId,int *session_status)
{
#ifdef WEBCONFIG_BIN_SUPPORT
	if(isRbusEnabled())
	{
		WebcfgDebug("B4 set_rbus_ForceSync\n");
		set_rbus_ForceSync(pString, transactionId, session_status);
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
		*pString = strdup("root");
		*transactionId = transID;
		*transactionId = strdup("378dd11f-a56f-45ac-9a28-08d86f562a58");
		WebcfgDebug("get_rbus_ForceSync. *pString %s *transactionId %s\n", *pString, *transactionId);
	}
#else
	UNUSED(pString);
	UNUSED(transactionId);
#endif
	return 0;
}

//#ifndef WEBCONFIG_BIN_SUPPORT
int Get_Webconfig_URL( char *pString)
{
    WebcfgDebug("Inside Get_Webconfig_URL weak function.\n");
    pString= strdup("https://webconfig-red.staging.xdp.comcast.net/api/v1/device/44aaf59b1f18/config");
    //UNUSED(pString);
    return 0;
}

int Set_Webconfig_URL( char *pString)
{
    WebcfgDebug("Inside Get_Webconfig_URL weak function.\n");
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
//#endif

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
		WebcfgInfo("Registering RBUS event listener!\n");
		rc = registerRBUSEventlistener();
		WebcfgDebug("RBUSEventlistener rc %d\n", rc);
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
		rc = removeRBUSEventlistener();
		WebcfgDebug("removeRBUSEventlistener returns rc %d\n", rc);
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
//#ifndef WEBCONFIG_BIN_SUPPORT
int rbus_GetValueFromDB( char* paramName, char** paramValue)
{
	WebcfgDebug("Inside rbus_GetValueFromDB weak fn\n");
	UNUSED(paramName);
	//UNUSED(paramValue);
	*paramValue =strdup("true");
	return 0;
}

int rbus_StoreValueIntoDB(char *paramName, char *value)
{
	WebcfgDebug("Inside rbus_StoreValueIntoDB weak fn\n");
	UNUSED(paramName);
	UNUSED(value);
	return 0;
}
//#endif
