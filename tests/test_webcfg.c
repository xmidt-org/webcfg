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
#ifdef WEBCONFIG_BIN_SUPPORT	
#include "../src/webcfg_rbus.h"
#endif	
#include "../src/webcfg_metadata.h"
#include "../src/webcfg.h"
#include "../src/webcfg_event.h"
#include "../src/webcfg_multipart.h"
#define UNUSED(x) (void )(x)
//mock functions
int numLoops;
static int maintenance_sync_lock = true;

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

#ifdef FEATURE_SUPPORT_AKER
int akerwait__ (unsigned int secs)
{
	UNUSED(secs);
	return 0;

}
#endif

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

void sendNotification(char *payload, char *source, char *destination)
{
	WEBCFG_FREE(payload);
	WEBCFG_FREE(source);
	UNUSED(destination);
	return;
}

#ifdef FEATURE_SUPPORT_AKER
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
#endif

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
	if(get_doc_fail())
	{
		UNUSED(*pString);
		UNUSED(*transactionId);
	}
	else if(get_global_supplementarySync())
	{
		*pString = strdup("telemetry");
		*transactionId = strdup("42215325617131212");
	}
	else if(!get_maintenanceSync())
	{
		*pString = strdup("root");
		*transactionId = strdup("42215325617131212");
	}
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
	if(!maintenance_sync_lock)
	{
		time_t rawtime;
		struct tm *timeval;

		time(&rawtime);
		timeval = localtime(&rawtime);

		int hrs = timeval->tm_hour;
		int min = timeval->tm_min;
		int sec = timeval->tm_sec;

		int total_secs = hrs * 3600 + min * 60 + sec;
		total_secs += 6;

		char *total_secs_str = (char *)malloc(sizeof(char) * 6);
		snprintf(total_secs_str, sizeof(total_secs_str), "%d", total_secs);

		return total_secs_str;
	}
	else
	{
		return NULL;
	}
}

char *getFirmwareUpgradeEndTime(void)
{

	if(!maintenance_sync_lock)
	{
		time_t rawtime;
		struct tm *timeval;

		time(&rawtime);
		timeval = localtime(&rawtime);

		int hrs = timeval->tm_hour;
		int min = timeval->tm_min;
		int sec = timeval->tm_sec;

		int total_secs = hrs * 3600 + min * 60 + sec;
		total_secs += 10;

		char *total_secs_str = (char *)malloc(sizeof(char) * 6);
		snprintf(total_secs_str, sizeof(total_secs_str), "%d", total_secs);

		return total_secs_str;
	}
	else
	{
		return NULL;
	}
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

void test_handlehttpResponse_200_Success()
{
	char * webConfigData = strdup("HTTP 200 OK\r\nContent-Type: multipart/mixed; boundary=+CeB5yCWds7LeVP4oibmKefQ091Vpt2x4g99cJfDCmXpFxt5d\r\nEtag: 345431215\r\n\n--+CeB5yCWds7LeVP4oibmKefQ091Vpt2x4g99cJfDCmXpFxt5d\r\nContent-type: application/msgpack\r\nEtag: 2132354\r\nNamespace: value\r\n\r\nparameters: somedata\r\n--+CeB5yCWds7LeVP4oibmKefQ091Vpt2x4g99cJfDCmXpFxt5d--\r\n");
	size_t data_size = strlen(webConfigData);
	char *ct = strdup("Content-Type: multipart/mixed; boundary=+CeB5yCWds7LeVP4oibmKefQ091Vpt2x4g99cJfDCmXpFxt5d");
	char * transid = strdup("23133213edqq");
	handlehttpResponse(200, webConfigData, 0, transid, ct, data_size);
}

void test_webconfigData_empty_retry3()
{
	/*webconfig_db_data_t *tmp = NULL;
	tmp = (webconfig_db_data_t *) malloc(sizeof(webconfig_db_data_t));
	if(tmp)
	{
		memset(tmp, 0, sizeof(webconfig_db_data_t));

		tmp->name = strdup("root");
		tmp->version = 0;
		tmp->root_string = strdup("0");
		tmp->next = NULL;

		set_global_db_node(tmp);
	}*/
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(200, NULL, 3, transid, "example_ct", 100);
}

void test_handlehttpResponse_204()
{
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(204, NULL, 0, transid, "example_ct", 100);
}

void test_handlehttpResponse_404()
{
	char * transid = strdup("23133213131edqq");
	handlehttpResponse(404, NULL, 0, transid, "example_ct", 100);
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

void test_doc_retry()
{
    maintenance_sync_lock = false;
    unsigned long status = 0;
    initWebConfigMultipartTask(status);
    sleep(6);
    set_maintenanceSync(true);
    pthread_cond_signal(get_global_sync_condition());
    sleep(2);
    set_doc_fail(1);
    sleep(9);
    assert_false(get_maintenanceSync());
}

void test_global_shutdown()
{
    unsigned long status = 0;
    initWebConfigMultipartTask(status);
    sleep(6);
    //Aseerting webcfgReady should be true
    assert_true(get_webcfgReady());
    //Asserting bootsync
    assert_false(get_bootSync());
    set_global_shutdown(true);
    pthread_cond_signal(get_global_sync_condition());
    sleep(6);
    set_global_shutdown(false);
    //Aseerting webcfgReady should be false in global Shutdown
    assert_false(get_webcfgReady());
    assert_null(get_global_mpThreadId());
}

void test_maintenanceSync_check()
{
    unsigned long status = 0;
    initWebConfigMultipartTask(status);
    sleep(6);
    set_maintenanceSync(true);
    pthread_cond_signal(get_global_sync_condition());
    sleep(4);
    assert_false(get_maintenanceSync());
}

void test_forcedsync()
{
    unsigned long status = 0;
    initWebConfigMultipartTask(status);
    sleep(6);
    set_global_webcfg_forcedsync_needed(1);
    pthread_cond_signal(get_global_sync_condition());
    sleep(6);
    assert_int_equal(get_global_webcfg_forcedsync_needed(), 0);
}

void test_maintenanceSync_trigger()
{
    maintenance_sync_lock = false;
    set_global_supplementarySync(0);
    unsigned long status = 0;
    initWebConfigMultipartTask(status);
    sleep(6);
    set_maintenanceSync(true);
    pthread_cond_signal(get_global_sync_condition());
    sleep(9);
    assert_false(get_maintenanceSync());
}

void test_supplementarySync_and_ForceSync()
{
    SupplementaryDocs_t *spInfo = NULL;
    spInfo = (SupplementaryDocs_t *)malloc(sizeof(SupplementaryDocs_t));

    if(spInfo == NULL)
    {
        WebcfgError("Unable to allocate memory for supplementary docs\n");
    }

    memset(spInfo, 0, sizeof(SupplementaryDocs_t));

    spInfo->name = strdup("telemetry");
    spInfo->next = NULL;

    WebcfgInfo("Test print\n");
    set_global_spInfoHead(spInfo);
    set_global_spInfoTail(spInfo);

    unsigned long status = 0;
    initWebConfigMultipartTask(status);
    sleep(10);
    set_global_webcfg_forcedsync_needed(1);
    pthread_cond_signal(get_global_sync_condition());
    set_global_supplementarySync(1);
    sleep(6);
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
		cmocka_unit_test(test_global_shutdown),
		cmocka_unit_test(test_initWebConfigMultipartTask_Thread),
		cmocka_unit_test(test_handlehttpResponse_304),
		cmocka_unit_test(test_handlehttpResponse_200_Post_None),
		cmocka_unit_test(test_handlehttpResponse_200_Success),
		cmocka_unit_test(test_webconfigData_empty_retry3),
		cmocka_unit_test(test_handlehttpResponse_204),
		cmocka_unit_test(test_handlehttpResponse_404),
		cmocka_unit_test(test_handlehttpResponse_403),
		cmocka_unit_test(test_handlehttpResponse_429),
		cmocka_unit_test(test_handlehttpResponse_5xx),
		cmocka_unit_test(test_forcedsync),
		cmocka_unit_test(test_supplementarySync_and_ForceSync),
		cmocka_unit_test(test_maintenanceSync_check),
		cmocka_unit_test(test_maintenanceSync_trigger),
		cmocka_unit_test(test_doc_retry)
	};
	return cmocka_run_group_tests(tests, NULL, 0);
}
