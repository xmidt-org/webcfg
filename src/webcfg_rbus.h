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
#include <rbus/rbus.h>
#include <rbus/rbus_object.h>
#include <rbus/rbus_property.h>
#include <rbus/rbus_value.h>
#include <rbus-core/rbus_core.h>
#include <rbus-core/rbus_session_mgr.h>

#include <wdmp-c.h>
#include <cimplog.h>
#include "webcfg.h"
#include "webcfg_log.h"
#include "webcfg_generic.h"

#define buffLen 1024
#define maxParamLen 128

#define NUM_WEBCFG_ELEMENTS 3

#define WEBCFG_COMPONENT_NAME "webconfig"
#define MAX_PARAMETERNAME_LEN			4096
// Data elements provided by webconfig

#define WEBCFG_RFC_PARAM "Device.X_RDK_WebConfig.RfcEnable"
#define WEBCFG_URL_PARAM "Device.X_RDK_WebConfig.URL"
#define WEBCFG_FORCESYNC_PARAM "Device.X_RDK_WebConfig.ForceSync"

#define CCSP_Msg_Bus_OK             100
#define CCSP_Msg_Bus_OOM            101
#define CCSP_Msg_Bus_ERROR          102

#define CCSP_Msg_BUS_CANNOT_CONNECT 190
#define CCSP_Msg_BUS_TIMEOUT        191
#define CCSP_Msg_BUS_NOT_EXIST      192
#define CCSP_Msg_BUS_NOT_SUPPORT    193
#define CCSP_ERR_UNSUPPORTED_PROTOCOL 9013
#define CCSP_ERR_INVALID_PARAMETER_VALUE 9007

bool isRbusEnabled();

WEBCFG_STATUS webconfigRbusInit(const char *pComponentName);
WEBCFG_STATUS regWebConfigDataModel();

rbusHandle_t get_global_rbus_handle(void);

void getValues_rbus(const char *paramName[], const unsigned int paramCount, int index, money_trace_spans *timeSpan, param_t ***paramArr, int *retValCount, int *retStatus);
void setValues_rbus(const param_t paramVal[], const unsigned int paramCount, const int setType,char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspRetStatus);

DATA_TYPE mapRbusToWdmpDataType(rbusValueType_t rbusType);
int mapRbusStatus(int Rbus_error_code);
#endif