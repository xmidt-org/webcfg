/*
 * Copyright 2023 Comcast Cable Communications Management, LLC
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
#ifdef _SKY_HUB_COMMON_PRODUCT_REQ_
#define WEBCFG_WANSTATUS_PARAM "Device.X_RDK_WanManager.CurrentStatus"
int subscribeTo_CurrentInterfaceStatus_Event();
#endif

void WanEventHandler();
#define LOGGING_INTERVAL_SECS    ( 60 * 60 )
