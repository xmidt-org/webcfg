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
#ifndef __WEBCFG_TIMER_H__
#define __WEBCFG_TIMER_H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#ifdef BUILD_YOCTO
#define FW_START_FILE		"/nvram/.FirmwareUpgradeStartTime"
#define FW_END_FILE		    "/nvram/.FirmwareUpgradeEndTime"
#else
#define FW_START_FILE		"/tmp/.FirmwareUpgradeStartTime"
#define FW_END_FILE		    "/tmp/.FirmwareUpgradeEndTime"
#endif

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
void initMaintenanceTimer();
char* printTime(long long time);
void set_retry_timer(int value);
int get_retry_timer();
int checkMaintenanceTimer();
int readFWFiles(char* file_path, long *range);
int maintenanceSyncSeconds();
int retrySyncSeconds();
long getTimeInSeconds(long long time);
void set_global_maintenance_time(long value);
void set_global_fw_start_time(long value);
void set_global_fw_end_time(long value);
void set_global_retry_time(long value);
long get_global_retry_time();
#endif
