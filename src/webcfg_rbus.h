/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2021 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef _WEBCFG_RBUS_H_
#define _WEBCFG_RBUS_H_

#include <stdio.h>
#include <stdbool.h>
#include <rbus/rbus.h>
#include <rbus/rbus_object.h>
#include <rbus/rbus_property.h>
#include <rbus/rbus_value.h>
#include <rbus-core/rbus_core.h>
#include <rbus-core/rbus_session_mgr.h>

#include <wdmp-c.h>
#include <cimplog.h>
#include <wrp-c.h>
#include <cJSON.h>
#include "webcfg.h"
#include "webcfg_log.h"
#include "webcfg_generic.h"
#include "webcfg_event.h"

#define buffLen 1024
#define maxParamLen 128

#define NUM_WEBCFG_ELEMENTS 4

#define WEBCFG_COMPONENT_NAME "webconfig"
#define MAX_PARAMETERNAME_LEN			4096
#define WEBCFG_EVENT_NAME "webconfigSignal"

// Data elements provided by webconfig
#define WEBCFG_RFC_PARAM "Device.X_RDK_WebConfig.RfcEnable"
#define WEBCFG_URL_PARAM "Device.X_RDK_WebConfig.URL"
#define WEBCFG_FORCESYNC_PARAM "Device.X_RDK_WebConfig.ForceSync"
#define WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM  "Device.X_RDK_WebConfig.SupplementaryServiceUrls.Telemetry"
#define PARAM_RFC_ENABLE "eRT.com.cisco.spvtg.ccsp.webpa.WebConfigRfcEnable"

#define CCSP_Msg_Bus_OK             100
#define CCSP_Msg_Bus_OOM            101
#define CCSP_Msg_Bus_ERROR          102

#define CCSP_Msg_BUS_CANNOT_CONNECT 190
#define CCSP_Msg_BUS_TIMEOUT        191
#define CCSP_Msg_BUS_NOT_EXIST      192
#define CCSP_Msg_BUS_NOT_SUPPORT    193
#define CCSP_ERR_UNSUPPORTED_PROTOCOL 9013
#define CCSP_ERR_INVALID_PARAMETER_VALUE 9007
#define CCSP_ERR_UNSUPPORTED_NAMESPACE 204

bool isRbusEnabled();

WEBCFG_STATUS webconfigRbusInit(const char *pComponentName);
WEBCFG_STATUS regWebConfigDataModel();

rbusHandle_t get_global_rbus_handle(void);

void getValues_rbus(const char *paramName[], const unsigned int paramCount, int index, money_trace_spans *timeSpan, param_t ***paramArr, int *retValCount, int *retStatus);
void setValues_rbus(const param_t paramVal[], const unsigned int paramCount, const int setType,char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspRetStatus);

DATA_TYPE mapRbusToWdmpDataType(rbusValueType_t rbusType);
int mapRbusStatus(int Rbus_error_code);
void webcfgEventRbusHandler(rbusHandle_t handle, rbusEvent_t const* event, rbusEventSubscription_t* subscription);
rbusError_t registerRBUSEventlistener();
rbusError_t removeRBUSEventlistener();
void rbusWebcfgEventHandler(rbusHandle_t handle, rbusMessage_t* msg, void * userData);
int set_rbus_RfcEnable(bool bValue);
int set_rbus_ForceSync(char* pString, int *pStatus);
int parseForceSyncJson(char *jsonpayload, char **forceSyncVal, char **forceSynctransID);
int get_rbus_ForceSync(char** pString, char **transactionId );
bool get_rbus_RfcEnable();
void sendNotification_rbus(char *payload, char *source, char *destination);
#endif
