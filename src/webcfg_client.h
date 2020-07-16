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
#ifndef __WEBCFG_CLIENT_H__
#define __WEBCFG_CLIENT_H__

#include "webcfg.h"

#define AKER_UPDATE_PARAM "Device.DeviceInfo.X_RDKCENTRAL-COM_Aker.Update"
#define AKER_DELETE_PARAM "Device.DeviceInfo.X_RDKCENTRAL-COM_Aker.Delete"
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
void initWebConfigClient();
int send_aker_blob(char *paramName, char *blob, uint32_t blobSize, uint16_t docTransId, int version);
WEBCFG_STATUS checkAkerStatus();
#endif
