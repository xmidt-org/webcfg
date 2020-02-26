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
#ifndef WEBCFGNOTIFY_H
#define WEBCFGNOTIFY_H

#include <stdint.h>
#include "cJSON.h"
#include "webcfg_log.h"
#include "webcfg.h"

void initWebConfigNotifyTask();
pthread_t get_global_notify_threadid();
void addWebConfigNotifyMsg(char *url, long status_code, char *application_status, int application_details, char *request_timestamp, char *version, char *transaction_uuid);
#endif
