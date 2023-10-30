 /**
  * Copyright 2019 Comcast Cable Communications Management, LLC
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
 */
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include <unistd.h>
#include <cmocka.h>
#include <setjmp.h>
#include <cimplog.h>
#include "../src/webcfg_db.h"
#include "../src/webcfg_log.h"
//#include "../src/webcfg_pack.h"
//#include "../src/webcfg_helpers.h"
//#include "../src/webcfg_param.h"
#include "../src/webcfg_multipart.h"
#define UNUSED(x) (void )(x)
//mock functions

void* test_thread_function()
{
	WebcfgInfo("Thread created for test\n");
	sleep(2);
	pthread_exit(0);
	return NULL;
}

long getTimeOffset()
{
	return 0;
}

int akerwait__ (unsigned int secs)
{
	UNUSED(secs);
	return 0;

}

char* get_deviceMAC()
{
	char *device_mac=strdup("b42xxxxxxxxx");
	return device_mac;
}
char* get_deviceWanMAC()
{
	char *device_wan_mac=strdup("b42xxxxxxxxx");
	return device_wan_mac;
}
int Get_Webconfig_URL( char *pString)
{
	UNUSED(pString);
	return 0;
}
int Set_Webconfig_URL( char *pString)
{
	printf("Set_Webconfig_URL pString %s\n", pString);
	return 0;
}

void initEventHandlingTask(){
	return;
}
void webcfgCallback(char *Info, void* user_data)
{
	UNUSED(Info);
	UNUSED(user_data);
	return;
}

void processWebcfgEvents(){
	return;
}


WEBCFG_STATUS checkAndUpdateTmpRetryCount(webconfig_tmp_data_t *temp, char *docname)
{
	UNUSED(temp);
	UNUSED(docname);
	return 0;
}

void sendNotification(char *payload, char *source, char *destination)
{
	WEBCFG_FREE(payload);
	WEBCFG_FREE(source);
	UNUSED(destination);
	return;
}


void checkAkerStatus(){
	
	return;
}
void updateAkerMaxRetry(webconfig_tmp_data_t *temp, char *docname)
{
	UNUSED(temp);
	UNUSED(docname);
	return;
}

void processAkerSubdoc()
{
	return ;
}



char * getDeviceBootTime()
{
	char *bTime = strdup("152200345");
	return bTime;
}

char * getFirmwareVersion()
{
	char *fName = strdup("Firmware.bin");
	return fName;
}

int getForceSync(char** pString, char **transactionId)
{
	UNUSED(pString);
	UNUSED(transactionId);
	return 0;
}

char * getProductClass()
{
	char *pClass = strdup("Product");
	return pClass;
}

char * getModelName()
{
	char *mName = strdup("Model");
	return mName;
}


void retryMultipartSubdoc(){
	return ;
}
char *get_global_systemReadyTime()
{
	char *sTime = strdup("158000123");
	return sTime;
}

char * getRebootReason()
{
	char *reason = strdup("factory-reset");
	return reason;
}

pthread_t get_global_event_threadid()
{
	return 0;
}

pthread_t get_global_process_threadid()
{
	return 0;
}

pthread_cond_t *get_global_event_con(void)
{
	return 0;
}

pthread_mutex_t *get_global_event_mut(void)
{
	return 0;
}


char * getPartnerID()
{
	char *pID = strdup("partnerID");
	return pID;
}

char * getAccountID()
{
	char *aID = strdup("accountID");
	return aID;
}



int Get_Supplementary_URL( char *name, char *pString)
{
	UNUSED(name);
	UNUSED(pString);
	return 0;
}

int Set_Supplementary_URL( char *name, char *pString)
{
	UNUSED(name);
	UNUSED(pString);
	return 0;
}
char *getFirmwareUpgradeStartTime(void)
{
	return NULL;
}

char *getFirmwareUpgradeEndTime(void)
{
	return NULL;
}

/*----------------------------------------------------------------------------*/
/*                             Test Functions                             */
/*----------------------------------------------------------------------------*/

void test_timeVal_Diff()
{
	struct timespec startTime, finishTime;
	startTime.tv_sec = 1635187500;
	startTime.tv_nsec = 0;

	finishTime.tv_sec = 1635187800; //300sec diff
	finishTime.tv_nsec = 0;

	long diff =timeVal_Diff(&startTime, &finishTime);
	assert_int_equal(300000, diff); //300000msec = 300sec
}

void test_successJoinThread()
{
	int err = 0;
	pthread_t threadId;

	err = pthread_create(&threadId, NULL, test_thread_function, NULL);
	if (err != 0) 
	{
		WebcfgError("Error creating test_thread_function thread :[%s]\n", strerror(err));
	}
	else
	{
		WebcfgInfo("test_thread_function Thread created Successfully.\n");
	}
	printf("After check\n");
	JoinThread(threadId);
	assert_non_null(threadId);
}

void test_failureJoinThread()
{
	pthread_t threadId = pthread_self();
	assert_non_null(threadId);
	JoinThread(threadId);
}

void test_handlehttpResponse_304()
{
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(304, "test", 0, transid, "example_ct", 100);
}

void test_handlehttpResponse_200_Post_None()
{
	webconfig_db_data_t *tmp = NULL;
	tmp = (webconfig_db_data_t *) malloc(sizeof(webconfig_db_data_t));
	if(tmp)
	{
		memset(tmp, 0, sizeof(webconfig_db_data_t));

		tmp->name = strdup("root");
		tmp->version = 0;
		tmp->root_string = strdup("0");
		tmp->next = NULL;

		set_global_db_node(tmp);
	}
	char * content_length = strdup("0");
	set_global_contentLen(content_length);
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(200, NULL, 0, transid, "example_ct", 100);
}

void test_handlehttpResponse_204()
{
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(204, NULL, 0, transid, "example_ct", 100);
}

void test_handlehttpResponse_403()
{
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(403, NULL, 0, transid, "example_ct", 100);
}

void test_handlehttpResponse_429()
{
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(429, NULL, 0, transid, "example_ct", 100);
}

void test_handlehttpResponse_5xx()
{
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(503, NULL, 0, transid, "example_ct", 100);
}

void test_initWebConfigMultipartTask_Thread()
{
    assert_null(get_global_mpThreadId());
    unsigned long status = 0;
    initWebConfigMultipartTask(status);
    sleep(8);
    assert_non_null(get_global_mpThreadId());
}


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_timeVal_Diff),
		cmocka_unit_test(test_successJoinThread),
		cmocka_unit_test(test_failureJoinThread),
		cmocka_unit_test(test_initWebConfigMultipartTask_Thread),
		cmocka_unit_test(test_handlehttpResponse_304),
		cmocka_unit_test(test_handlehttpResponse_200_Post_None),
		cmocka_unit_test(test_handlehttpResponse_204),
		cmocka_unit_test(test_handlehttpResponse_403),
		cmocka_unit_test(test_handlehttpResponse_429),
		cmocka_unit_test(test_handlehttpResponse_5xx)
	};
	return cmocka_run_group_tests(tests, NULL, 0);
}
