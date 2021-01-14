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
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include "webcfg.h"
#include "webcfg_multipart.h"
#include "webcfg_notify.h"
#include "webcfg_generic.h"
#include "webcfg_param.h"
#include "webcfg_auth.h"
#include <msgpack.h>
#include <wdmp-c.h>
#include <base64.h>
#include "webcfg_db.h"
#include "webcfg_aker.h"
#include "webcfg_metadata.h"
#include "webcfg_event.h"
#include "webcfg_blob.h"
#include "webcfg_timer.h"
#include "webcfg_generic.h"
/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define MIN_MAINTENANCE_TIME		3600					//1hrs in seconds
#define MAX_MAINTENANCE_TIME		14400					//4hrs in seconds
/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static int g_retry_timer = 900;
static long g_retry_time = 0;
static long g_maintenance_time = 0;
static long g_fw_start_time = 0;
static long g_fw_end_time = 0;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
long get_global_maintenance_time()
{
    return g_maintenance_time;
}

void set_global_maintenance_time(long value)
{
    g_maintenance_time = value;
}

long get_global_fw_start_time()
{
    return g_fw_start_time;
}

void set_global_fw_start_time(long value)
{
    g_fw_start_time = value;
}

long get_global_fw_end_time()
{
    return g_fw_end_time;
}

void set_global_fw_end_time(long value)
{
    g_fw_end_time = value;
}

int get_retry_timer()
{
	return g_retry_timer;
}

void set_retry_timer(int value)
{
	g_retry_timer = value;
}

long get_global_retry_time()
{
    return g_retry_time;
}

void set_global_retry_time(long value)
{
    g_retry_time = value;
}

//To print the value in readable format
char* printTime(long long time)
{
	struct tm  ts;
	static char buf[80] = "";

	// Format time, "ddd yymmdd hh:mm:ss zzz"
	time_t rawtime = time;
	ts = *localtime(&rawtime);
	strftime(buf, sizeof(buf), "%a %y%m%d %H:%M:%S", &ts);
	return buf;
}

//To initialise maintenance random timer
void initMaintenanceTimer()
{
	long time_val = 0;
	long fw_start_time = 0;
	long fw_end_time = 0;
	uint16_t random_key = 0;
	char *upgrade_start_time = NULL;
	char *upgrade_end_time = NULL;

	upgrade_start_time = getFirmwareUpgradeStartTime();

	if( upgrade_start_time != NULL )
	{
		WebcfgInfo("Inside sucess case start_time %s\n",upgrade_start_time);
		fw_start_time = atoi(upgrade_start_time);
		WebcfgDebug("Inside sucess case fw_start_times %ld\n",fw_start_time);	
	}
	else
	{
		fw_start_time = MIN_MAINTENANCE_TIME;
		WebcfgInfo("Inside failure case start_time \n");
	}
	
	upgrade_end_time = getFirmwareUpgradeEndTime();
	WebcfgInfo("The firmware upgrade end_time %s\n",upgrade_end_time);
	if( upgrade_end_time != NULL )
	{
		WebcfgInfo("Inside sucess case end_time %s\n",upgrade_end_time);
		fw_end_time = atoi(upgrade_end_time);
		WebcfgInfo("Inside sucess case fw_end_times %ld\n",fw_end_time);	
	}
	else
	{
		fw_end_time = MAX_MAINTENANCE_TIME;
		WebcfgInfo("Inside failure case end_time\n");
	}

	if( fw_start_time == fw_end_time )
	{
		fw_start_time = MIN_MAINTENANCE_TIME;
		fw_end_time = MAX_MAINTENANCE_TIME;
		WebcfgDebug("Inside both values are equal\n");
	}

	if( fw_start_time > fw_end_time )
	{
		fw_start_time = fw_start_time - 86400;         //to get a time within the day
		WebcfgDebug("Inside start time is greater than end time\n");
	}

	set_global_fw_start_time( fw_start_time );
	set_global_fw_end_time( fw_end_time );

        random_key = generateRandomId();
        time_val = (random_key % (fw_end_time - fw_start_time+ 1)) + fw_start_time;

	WebcfgInfo("The fw_start_time is %ld\n",get_global_fw_start_time());
	WebcfgInfo("The fw_end_time is %ld\n",get_global_fw_end_time());

	if( time_val <= 0)
	{
		time_val = time_val + 86400;         //To set a time in next day
	}

	WebcfgDebug("The value of maintenance_time_val is %ld\n",time_val);
	set_global_maintenance_time(time_val);

}

//To Check whether the Maintenance time is at current time
int checkMaintenanceTimer()
{
	struct timespec rt;

	long long cur_time = 0;
	long cur_time_in_sec = 0;

	clock_gettime(CLOCK_REALTIME, &rt);
	cur_time = rt.tv_sec;
	cur_time_in_sec = getTimeInSeconds(cur_time);

	WebcfgDebug("The current time in checkMaintenanceTimer is %lld at %s\n",cur_time, printTime(cur_time));
	WebcfgDebug("The random timer in checkMaintenanceTimer is %ld\n",get_global_maintenance_time());

	if(cur_time_in_sec >= get_global_maintenance_time())
	{
		set_global_supplementarySync(1);
		WebcfgInfo("Rand time is equal to current time\n");
		return 1;
	}
	return 0;
}

//To read the start and end time from the file
/*int readFWFiles(char* file_path, long *range)
{
	FILE *fp = NULL;
	char *data = NULL;
	int ch_count=0;
	size_t sz = 0;

	fp = fopen(file_path,"r+");
	if (fp == NULL)
	{
		WebcfgError("Failed to open file %s\n", file_path);
		return WEBCFG_FAILURE;
	}

	fseek(fp, 0, SEEK_END);
	ch_count = ftell(fp);
	if (ch_count == -1) {
  		fclose(fp);
  		WebcfgError("ftell failed.\n");
  		return WEBCFG_FAILURE;
	}
	fseek(fp, 0, SEEK_SET);
	fseek(fp, 0, SEEK_SET);

	data = (char *) malloc(sizeof(char) * (ch_count + 1));
	if(NULL == data)
	{
		WebcfgError("Memory allocation for data failed.\n");
		fclose(fp);
		return WEBCFG_FAILURE;
	}

	WebcfgDebug("After data \n");
	memset(data,0,(ch_count + 1));
	sz = fread(data, 1, ch_count,fp);
     	if (sz == (size_t)-1) 
		{	
			fclose(fp);
			WebcfgError("fread failed.\n");
			WEBCFG_FREE(data);
			return WEBCFG_FAILURE;
		}

	WebcfgDebug("The data is %s\n", data);

	*range = atoi(data);

	WebcfgDebug("The range is %ld\n", *range);
	WEBCFG_FREE(data);
	fclose(fp);

	return WEBCFG_SUCCESS;
}*/

//To get the wait seconds for Maintenance Time
int maintenanceSyncSeconds()
{
	struct timespec ct;
	long maintenance_secs = 0;
	long current_time_in_sec = 0;
	long sec_to_12 = 0;
	long long current_time = 0;
	clock_gettime(CLOCK_REALTIME, &ct);

	current_time = ct.tv_sec;
	current_time_in_sec = getTimeInSeconds(current_time);

	maintenance_secs =  get_global_maintenance_time() - current_time_in_sec;

	WebcfgDebug("The current time in maintenanceSyncSeconds is %lld at %s\n",current_time, printTime(current_time));
	WebcfgDebug("The random timer in maintenanceSyncSeconds is %ld\n",get_global_maintenance_time());

	if (maintenance_secs < 0)
	{
		sec_to_12 = 86400 - current_time_in_sec;               //Getting the remaining time for midnight 12
		maintenance_secs = sec_to_12 + get_global_maintenance_time();//Adding with Maintenance wait time for nextday trigger
	}

	WebcfgDebug("The maintenance Seconds is %ld\n", maintenance_secs);

	return maintenance_secs;
}

//To get the wait seconds for Maintenance Time
int retrySyncSeconds()
{
	struct timespec ct;
	long retry_secs = 0;
	long current_time_in_sec = 0;
	long long current_time = 0;
	clock_gettime(CLOCK_REALTIME, &ct);

	current_time = ct.tv_sec;
	current_time_in_sec = getTimeInSeconds(current_time);

	retry_secs =  get_global_retry_time() - current_time_in_sec;

	WebcfgDebug("The current time in retrySyncSeconds is %lld at %s\n",current_time, printTime(current_time));
	WebcfgDebug("The random timer in retrySyncSeconds is %ld\n",get_global_retry_time());

	if (retry_secs < 0)
	{
		retry_secs = 0;//Adding with Maintenance wait time for nextday trigger
	}

	WebcfgDebug("The retry Seconds is %ld\n", retry_secs);

	return retry_secs;
}

//To get the Seconds from Epoch Time
long getTimeInSeconds(long long time)
{
	struct tm cts;
	time_t time_value = time;
	long cur_time_hrs_in_sec = 0;
	long cur_time_min_in_sec = 0;
	long cur_time_sec_in_sec = 0;
	long sec_of_cur_time = 0;

	cts = *localtime(&time_value);

	cur_time_hrs_in_sec = cts.tm_hour * 60 * 60;
	cur_time_min_in_sec = cts.tm_min * 60;
	cur_time_sec_in_sec = cts.tm_sec;

	sec_of_cur_time = cur_time_hrs_in_sec + cur_time_min_in_sec + cur_time_sec_in_sec;

	return sec_of_cur_time;
}

int updateRetryTimeDiff(long long expiry_time)
{
	struct timespec ct;
	int time_diff = 0;
	long long present_time = 0;

	clock_gettime(CLOCK_REALTIME, &ct);
	present_time = ct.tv_sec;

	time_diff = expiry_time - present_time;

	//To set the lowest retry timeout of all the docs
	if(get_retry_timer() > time_diff)
	{
		set_retry_timer(time_diff);
		set_global_retry_time(getTimeInSeconds(expiry_time));
		WebcfgDebug("The retry_timer is %d after set\n", get_retry_timer());
	}
	if(get_global_retry_time() == 0)
	{
		set_global_retry_time(getTimeInSeconds(present_time+900));
	}
	return time_diff;
}
