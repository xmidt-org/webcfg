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
#ifndef WEBCFGAUTH_H
#define WEBCFGAUTH_H

#include <stdint.h>
#if !defined FEATURE_SUPPORT_MQTTCM
#include <curl/curl.h>
#endif
#include "webcfg_log.h"
#include "webcfg.h"
#if ! defined(DEVICE_EXTENDER)
#define WEBPA_READ_HEADER             "/etc/parodus/parodus_read_file.sh"
#define WEBPA_CREATE_HEADER           "/etc/parodus/parodus_create_file.sh"
#else
#define WEBPA_READ_HEADER             "/lib/webcfg/read_auth_token.sh"
#define WEBPA_CREATE_HEADER           "/lib/webcfg/create_auth_token.sh"
#endif
#define TOKEN_SIZE                    4096

void getAuthToken();
void createNewAuthToken(char *newToken, size_t len, char *hw_mac, char* hw_serial_number);
char* get_global_auth_token();
char* get_global_serialNum();

#endif
