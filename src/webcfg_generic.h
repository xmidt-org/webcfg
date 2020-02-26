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
#ifndef __WEBCFGCOMMON_H__
#define __WEBCFGCOMMON_H__

#include <stdint.h>
#include <wdmp-c.h>

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#if defined(_COSA_BCM_MIPS_)
#define DEVICE_MAC                   "Device.DPoE.Mac_address"
#elif defined(PLATFORM_RASPBERRYPI)
#define DEVICE_MAC                   "Device.Ethernet.Interface.5.MACAddress"
#elif defined(RDKB_EMU)
#define DEVICE_MAC                   "Device.DeviceInfo.X_COMCAST-COM_WAN_MAC"
#else
#define DEVICE_MAC                   "Device.X_CISCO_COM_CableModem.MACAddress"
#endif

#define SERIAL_NUMBER                "Device.DeviceInfo.SerialNumber"
#define FIRMWARE_VERSION             "Device.DeviceInfo.X_CISCO_COM_FirmwareName"
#define DEVICE_BOOT_TIME             "Device.DeviceInfo.X_RDKCENTRAL-COM_BootTime"
#define MODEL_NAME		     "Device.DeviceInfo.ModelName"
#define PRODUCT_CLASS		     "Device.DeviceInfo.ProductClass"
/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/* return logger file name */
const char *fetch_generic_file(void);
void _get_webCfg_interface(char **interface);
int _getConfigVersion(int index, char **version);
bool _getConfigURL(int index, char **url);
bool _getRequestTimeStamp(int index,char **RequestTimeStamp);
int _setRequestTimeStamp(int index);
int _setSyncCheckOK(int index, bool status);
int _setConfigVersion(int index, char *version);
int _setSyncCheckOK(int index, bool status);

char *get_global_systemReadyTime();
char * getParameterValue(char *paramName);
void getDeviceMac();
char* get_global_deviceMAC();

void sendNotification(char *payload, char *source, char *destination);
/**
 * @brief setValues interface sets the parameter value.
 *
 * @param[in] paramVal List of Parameter name/value pairs.
 * @param[in] paramCount Number of parameters.
 * @param[in] setType Flag to specify the type of set operation.
 * @param[out] timeSpan timing_values for each component. 
 * @param[out] retStatus Returns status
 * @param[out] ccspStatus Returns ccsp set status
 */
void setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus);

#endif
