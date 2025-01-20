 /**
  * Copyright 2021 Comcast Cable Communications Management, LLC
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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <cJSON.h>
#include <wdmp-c.h>
#include <wrp-c.h>
#include <msgpack.h>
#include <rbus/rbus.h>
#include <rbus/rbus_object.h>
#include <rbus/rbus_property.h>
#include <rbus/rbus_value.h>
#include <CUnit/Basic.h>
#include "../src/webcfg_rbus.h"
#include "../src/webcfg_db.h"
#include "../src/webcfg_helpers.h"
#include "../src/webcfg_multipart.h"
#include "../src/webcfg_param.h"
#include "../src/webcfg_generic.h"
#define FILE_URL "/tmp/webcfg_url"

#define UNUSED(x) (void )(x)
#define WEBCFG_BLOB_PARAM "Device.X_RDK_WebConfig.blobElement"
int numLoops;
rbusHandle_t handle;
extern rbusHandle_t rbus_handle;
pthread_mutex_t sync_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sync_condition=PTHREAD_COND_INITIALIZER;

bool get_webcfgReady_flag =true;
bool get_webcfgReady()
{
	return get_webcfgReady_flag;
}
bool get_maintenanceSync_flag = false;
bool get_maintenanceSync()
{
	return get_maintenanceSync_flag;
}

void set_global_webcfg_forcedsync_needed(int value)
{
	UNUSED(value);
}

void set_cloud_forcesync_retry_needed(int value)
{
	UNUSED(value);
}

int get_global_webcfg_forcedsync_needed()
{
	return false;
}

int get_cloud_forcesync_retry_needed()
{
	return true;
}

bool  get_global_webcfg_forcedsync_started_flag = false;
int get_global_webcfg_forcedsync_started()
{
	return get_global_webcfg_forcedsync_started_flag;
}

bool get_cloud_forcesync_retry_started_flag = false;
int get_cloud_forcesync_retry_started()
{
 	return get_cloud_forcesync_retry_started_flag;
}

int get_global_supplementarySync()
{
	return 0;
}

char Telemetry_string[128] = {0}; 
int Get_Supplementary_URL( char *name, char *pString)
{
	UNUSED(name);
	if(strcmp(name,"Telemetry") == 0)
 		strcpy(pString,Telemetry_string);
	else
		pString[0] = '\0';	
 	return 0;
}

pthread_mutex_t *get_global_sync_mutex(void)
{
	return &sync_mutex;
}

void set_global_shutdown(bool shutdown)
{
	UNUSED(shutdown);
}

pthread_t *get_global_mpThreadId(void)
{
	return NULL;
}

int rbus_StoreValueIntoDB_ret = 0;
int rbus_StoreValueIntoDB(char *paramName, char *value)
{
	WebcfgDebug("Inside rbus_StoreValueIntoDB weak fn\n");
	UNUSED(paramName);
	UNUSED(value);
	return rbus_StoreValueIntoDB_ret;
}
int rbus_GetValueFromDB_ret = 0;
char rbus_GetValueFromDB_paramValue[6]="true";
int rbus_GetValueFromDB( char* paramName, char** paramValue)
{
	WebcfgDebug("Inside rbus_GetValueFromDB weak fn\n");
	UNUSED(paramName);
	*paramValue =strdup(rbus_GetValueFromDB_paramValue);
	return rbus_GetValueFromDB_ret;
}

void webcfgStrncpy(char *destStr, const char *srcStr, size_t destSize)
{
	strncpy(destStr, srcStr, destSize-1);
	destStr[destSize-1] = '\0';
}

pthread_cond_t *get_global_sync_condition(void)
{
	return &sync_condition;
}

bool get_bootSync_flag = false;
bool get_bootSync()
{
	return get_bootSync_flag;
}


void initWebConfigMultipartTask(unsigned long status)
{
	UNUSED(status);
}

bool shutdown_flag = false;
bool get_global_shutdown()
{
	return shutdown_flag;
} 

void getCurrent_Time(struct timespec *timer)
{
	UNUSED(timer);
}

void set_global_supplementarySync(int value)
{
    	UNUSED(value);
}

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/

void test_addForceSyncMsgToQueue(void) {

    addForceSyncMsgToQueue("root", "TID1");
    CU_ASSERT_EQUAL(addForceSyncMsgToQueue("root", "TransID1"), 0);
	deleteForceSyncMsgQueue();
}
void test_DisplayQueue()
{
	 DisplayQueue();
}
void test_deleteForceSyncMsgQueue() {
    WebcfgDebug("Starting test_deleteForceSyncMsgQueue_simple...\n");


    deleteForceSyncMsgQueue();
    CU_ASSERT_PTR_NULL(getForceSyncMsgQueue());

    CU_ASSERT_EQUAL(addForceSyncMsgToQueue("ForceSync1", "TransID1"), 0);
    CU_ASSERT_PTR_NOT_NULL(getForceSyncMsgQueue());

    deleteForceSyncMsgQueue();
    CU_ASSERT_PTR_NULL(getForceSyncMsgQueue());
    WebcfgDebug("Completed test_deleteForceSyncMsgQueue_simple.\n");
}
void test_updateForceSyncMsgQueue_Failure()
{

    int result = updateForceSyncMsgQueue("new_trans_id");
    CU_ASSERT_EQUAL(result, 0);
    WebcfgDebug("failed update inside\n");
	deleteForceSyncMsgQueue();
}
void test_updateForceSyncMsgQueue_found(void) {
    // Initialize the ForceSyncMsg queue with an entry
	char* pString ="root";
	int session_status = 0;
	set_rbus_ForceSync(pString,&session_status);
    // Retrieve the queue for checking purposes and print the initial queue
	ForceSyncMsg *temp = getForceSyncMsgQueue();
	int res=0;
	while (temp != NULL) {
        if (strcmp(temp->ForceSyncVal,"root") == 0) {
           res = updateForceSyncMsgQueue("TID2");
        }
        temp = temp->next;
    }
	CU_ASSERT_NOT_EQUAL(0,res);
	while (temp != NULL) {
        free(temp->ForceSyncVal);
        free(temp->ForceSyncTransID);
        free(temp);
    }
}

// Test cases for set_rbus_ForceSync & get_rbus_ForceSync
void test_setForceSync()
{
	int session_status = 0;
	int ret = set_rbus_ForceSync("root", &session_status);
	CU_ASSERT_EQUAL(0,session_status);
	CU_ASSERT_EQUAL(1,ret);

	get_webcfgReady_flag = false;
	ret = set_rbus_ForceSync("root", &session_status);
	CU_ASSERT_EQUAL(2,session_status);
	CU_ASSERT_EQUAL(0,ret);
	get_webcfgReady_flag = true;

	get_bootSync_flag = true;
	ret = set_rbus_ForceSync("root", &session_status);
	CU_ASSERT_EQUAL(1,session_status);
	CU_ASSERT_EQUAL(0,ret);
	get_bootSync_flag = false;	

	get_maintenanceSync_flag = true;
	ret = set_rbus_ForceSync("root", &session_status);
	CU_ASSERT_EQUAL(1,session_status);
	CU_ASSERT_EQUAL(0,ret);
	get_maintenanceSync_flag = false;	

	get_global_webcfg_forcedsync_started_flag = true;
	ret = set_rbus_ForceSync("root", &session_status);
	CU_ASSERT_EQUAL(1,session_status);
	CU_ASSERT_EQUAL(0,ret);
	get_global_webcfg_forcedsync_started_flag = false;		

	get_cloud_forcesync_retry_started_flag = true;
	ret = set_rbus_ForceSync("root", &session_status);
	CU_ASSERT_EQUAL(1,session_status);
	CU_ASSERT_EQUAL(0,ret);
	get_cloud_forcesync_retry_started_flag = false;

	ret = set_rbus_ForceSync("root,telemetry", &session_status);
	CU_ASSERT_EQUAL(1,session_status);
	CU_ASSERT_EQUAL(0,ret);
	ret = set_rbus_ForceSync("telemetry,root", &session_status);
	CU_ASSERT_EQUAL(1,session_status);
	CU_ASSERT_EQUAL(0,ret);

    set_rbus_ForceSync("root", &session_status);
    ret = set_rbus_ForceSync("telemetry,root", &session_status);
    CU_ASSERT_EQUAL(1,session_status);
    CU_ASSERT_EQUAL(0,ret);
    ForceSyncMsg* head = getForceSyncMsgQueue();
    CU_ASSERT_PTR_NOT_NULL(head); // Ensure queue is not null
    // Check the first node contains "telemetry"
    CU_ASSERT_PTR_NOT_NULL(head->ForceSyncVal);
    CU_ASSERT_STRING_EQUAL(head->ForceSyncVal, "telemetry");
    // Move to the next node and check it contains "root"
    ForceSyncMsg* second = head->next;
    CU_ASSERT_PTR_NOT_NULL(second); // Ensure the second node exists
    CU_ASSERT_PTR_NOT_NULL(second->ForceSyncVal);
    CU_ASSERT_STRING_EQUAL(second->ForceSyncVal, "root");
    // Ensure there are no more nodes in the queue
    CU_ASSERT_PTR_NULL(second->next);

	ForceSyncMsg* current = head;
	while (current != NULL) {
		ForceSyncMsg* next = current->next;
		free(current->ForceSyncVal);
		free(current->ForceSyncTransID);
		free(current);
		current = next;
	}
}

void test_setForceSync_failure()
{
	int session_status = 0;
	int ret = set_rbus_ForceSync("", &session_status);
	CU_ASSERT_EQUAL(0,session_status);
	CU_ASSERT_EQUAL(1,ret);
}

// Test case for isRbusEnabled
void test_isRbusEnabled_success()
{
	bool result = isRbusEnabled();
	CU_ASSERT_TRUE(result);
}

// Test case for get_global_isRbus
void test_get_global_isRbus_success()
{
	bool result = get_global_isRbus();
	CU_ASSERT_TRUE(result);
}

// Test case for isRfcEnabled
void test_isRfcEnabled_success()
{
	set_rbus_RfcEnable(true);
	bool result = isRfcEnabled();
	CU_ASSERT_TRUE(result);
}

void test_isRfcEnabled_failure()
{
	set_rbus_RfcEnable(false);
	bool result = isRfcEnabled();
	CU_ASSERT_FALSE(result);
}

// Test case for webconfigRbusInit
void test_webconfigRbusInit_success()
{
	int result = webconfigRbusInit("componentName");
    	CU_ASSERT_EQUAL(result, 0);
	webpaRbus_Uninit();  
}

// Test case for isRbusInitialized
void test_isRbusInitialized_success()
{
	webconfigRbusInit("componentName");
	bool result = isRbusInitialized();
	CU_ASSERT_TRUE(result);
	webpaRbus_Uninit();
}

void test_isRbusInitialized_failure()
{
	bool result = isRbusInitialized();
	CU_ASSERT_FALSE(result);
}

// Test case for get_global_rbus_handle
void test_get_global_rbus_handle()
{
	CU_ASSERT_EQUAL(rbus_handle, get_global_rbus_handle());
}

// Test case for webpaRbus_Uninit
void test_webpaRbus_Uninit()
{
	webconfigRbusInit("componentName");
	webpaRbus_Uninit();
}

// Test case for webcfgRfcSetHandler & webcfgRfcGetHandler
void test_webcfgRfcSet_GetHandler_success(void)
{
	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();
    
    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}
		//RFC set true
    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetBoolean(value, true);

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_RFC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SUCCESS);

    	rbusError_t rc_get = rbus_get(handle, WEBCFG_RFC_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	bool retrievedValue = false;
    	retrievedValue = rbusValue_GetBoolean(value);
    	CU_ASSERT_TRUE(retrievedValue);

		strncpy(rbus_GetValueFromDB_paramValue,"TRUE",strlen("TRUE")+1);	
    	rc_get = rbus_get(handle, WEBCFG_RFC_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	retrievedValue = false;
    	retrievedValue = rbusValue_GetBoolean(value);
    	CU_ASSERT_TRUE(retrievedValue);

    	rbusValue_Release(value);
		
		//RFC set false
    	rbusValue_t value1;
    	rbusValue_Init(&value1);
    	rbusValue_SetBoolean(value1, false);
    	rbusSetOptions_t opts1;
    	opts1.commit = true;
		strncpy(rbus_GetValueFromDB_paramValue,"false",strlen("false")+1);
    	rbusError_t rc1 = rbus_set(handle, WEBCFG_RFC_PARAM, value1, &opts1);
    	CU_ASSERT_EQUAL(rc1, RBUS_ERROR_SUCCESS);
    	rbusError_t rc_get1 = rbus_get(handle, WEBCFG_RFC_PARAM, &value1);
    	CU_ASSERT_EQUAL(rc_get1, RBUS_ERROR_SUCCESS);
    	bool retrievedValue1 = false;
    	retrievedValue1 = rbusValue_GetBoolean(value1);
    	CU_ASSERT_FALSE(retrievedValue1);
    	rbusValue_Release(value1);
		strncpy(rbus_GetValueFromDB_paramValue,"true",strlen("true")+1);		
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

// Test case for webcfgRfcSetHandler & webcfgRfcGetHandler
void test_webcfgRfcSet_GetHandler_failure(void)
{
	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();
    
    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetBoolean(value, true);

    	rbusSetOptions_t opts;
    	opts.commit = true;
		rbus_StoreValueIntoDB_ret = 1; //DB store fail
    	rbusError_t rc = rbus_set(handle, WEBCFG_RFC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, 1);
		rbus_StoreValueIntoDB_ret = 0;	

		rbus_GetValueFromDB_ret = 1; //DB store fail	
    	rbusError_t rc_get = rbus_get(handle, WEBCFG_RFC_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, 1);	
		rbus_GetValueFromDB_ret = 0;
    	rbusValue_Release(value);

		//Test with invalid data type
    	rbusValue_t value1;
    	rbusValue_Init(&value1);
    	rbusValue_SetInt32(value1, 1);

    	rbusSetOptions_t opts1;
    	opts1.commit = true;

    	rbusError_t rc1 = rbus_set(handle, WEBCFG_RFC_PARAM, value1, &opts1);
    	CU_ASSERT_EQUAL(rc1, RBUS_ERROR_INVALID_INPUT);
    	rbusValue_Release(value1);	

    	rbus_close(handle);
    	webpaRbus_Uninit();			
}

// Test case for webcfgDataSetHandler & webcfgDataGetHandler
void test_webcfgDataSet_GetHandler_success(void)
{
	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "Data123");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_DATA_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_ACCESS_NOT_ALLOWED);

    	rbusError_t rc_get = rbus_get(handle, WEBCFG_DATA_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	const char* retrievedValue = NULL;
    	retrievedValue = rbusValue_GetString(value, NULL);
    	CU_ASSERT_STRING_EQUAL(retrievedValue, "");
    	rbusValue_Release(value);
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

// Test case for webcfgDataSetHandler & webcfgDataGetHandler
void test_webcfgDataSet_GetHandler_failure(void)
{
	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "Data123");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_DATA_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_ACCESS_NOT_ALLOWED);
		set_rbus_RfcEnable(false);
    	rbusError_t rc_get = rbus_get(handle, WEBCFG_DATA_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	const char* retrievedValue = NULL;
    	retrievedValue = rbusValue_GetString(value, NULL);
    	CU_ASSERT_STRING_EQUAL(retrievedValue, "");
    	rbusValue_Release(value);
		set_rbus_RfcEnable(true);		
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

// Test case for webcfgSupportedDocsSetHandler & webcfgSupportedDocsGetHandler
void test_webcfgSupportedDocsSet_GetHandler(void)
{
	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "Doc1");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_SUPPORTED_DOCS_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_ACCESS_NOT_ALLOWED);

    	rbusError_t rc_get = rbus_get(handle, WEBCFG_SUPPORTED_DOCS_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	const char* retrievedValue = NULL;
    	retrievedValue = rbusValue_GetString(value, NULL);
    	CU_ASSERT_STRING_EQUAL(retrievedValue, "");
    	rbusValue_Release(value);
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

//Test case for webcfgSupportedVersionSetHandler & webcfgSupportedVersionGetHandler
void test_webcfgSupportedVersionSet_GetHandler(void)
{
	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "version123");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_SUPPORTED_VERSION_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_ACCESS_NOT_ALLOWED);

    	rbusError_t rc_get = rbus_get(handle, WEBCFG_SUPPORTED_VERSION_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	const char* retrievedValue = NULL;
    	retrievedValue = rbusValue_GetString(value, NULL);
    	CU_ASSERT_STRING_EQUAL(retrievedValue, "");
    	rbusValue_Release(value);
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

// Test case for webcfgUrlSetHandler & webcfgUrlGetHandler
void test_webcfgUrlSet_GetHandler_success(void)
{
	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "webcfgurl");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_URL_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SUCCESS);

    	rbusError_t rc_get = rbus_get(handle, WEBCFG_URL_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	const char* retrievedValue = NULL;
    	retrievedValue = rbusValue_GetString(value, NULL);
    	CU_ASSERT_STRING_EQUAL(retrievedValue, "webcfgurl");
    	rbusValue_Release(value);
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

void test_webcfgUrlSet_GetHandler_failure(void)
{
		webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "webcfgurl");

    	rbusSetOptions_t opts;
    	opts.commit = true;
		set_rbus_RfcEnable(false); //RFC False
    	rbusError_t rc = rbus_set(handle, WEBCFG_URL_PARAM, value, &opts);
    	CU_ASSERT_NOT_EQUAL(rc, RBUS_ERROR_SUCCESS);
    	rbusError_t rc_get = rbus_get(handle, WEBCFG_URL_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
		set_rbus_RfcEnable(true);
    	rbusValue_Release(value);

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "webcfgurl");
    	opts.commit = true;		
		rbus_StoreValueIntoDB_ret = 1; //rbus_StoreValueIntoDB Fail
    	rc = rbus_set(handle, WEBCFG_URL_PARAM, value, &opts);
    	CU_ASSERT_NOT_EQUAL(rc, RBUS_ERROR_SUCCESS);
		rbus_StoreValueIntoDB_ret = 0;
    	rbusValue_Release(value);
		
    	rbusValue_Init(&value);
    	rbusValue_SetInt32(value, 32);//Invalid datatype
    	opts.commit = true;		
    	rc = rbus_set(handle, WEBCFG_URL_PARAM, value, &opts);
    	CU_ASSERT_NOT_EQUAL(rc, RBUS_ERROR_SUCCESS);
    	rbusValue_Release(value);

    	rbus_close(handle);
    	webpaRbus_Uninit();		
}

// Test case for webcfgTelemetrySetHandler & webcfgTelemetryGetHandler
void test_webcfgTelemetrySet_GetHandler_success(void)
{
    	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "telemetry");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SUCCESS);

    	rbusError_t rc_get = rbus_get(handle, WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	const char* retrievedValue = NULL;
    	retrievedValue = rbusValue_GetString(value, NULL);
    	CU_ASSERT_STRING_EQUAL(retrievedValue, "telemetry");
    	rbusValue_Release(value);
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

void test_webcfgTelemetrySet_GetHandler_failure(void)
{
    	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "telemetry");
    	rbusSetOptions_t opts;
    	opts.commit = true;
		set_rbus_RfcEnable(false); //RFC False
    	rbusError_t rc = rbus_set(handle, WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_ACCESS_NOT_ALLOWED);
    	rbusError_t rc_get = rbus_get(handle, WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
		set_rbus_RfcEnable(true); 		
    	rbusValue_Release(value);

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "telemetry");
    	opts.commit = true;
		rbus_StoreValueIntoDB_ret = 1; //rbus_StoreValueIntoDB Fail
    	rc = rbus_set(handle, WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM, value, &opts);
    	CU_ASSERT_NOT_EQUAL(rc, RBUS_ERROR_SUCCESS);
		rbus_StoreValueIntoDB_ret = 0; 		
    	rbusValue_Release(value);

    	rbusValue_Init(&value);
    	rbusValue_SetInt32(value, 23); //Ivalid date type
    	opts.commit = true;
    	rc = rbus_set(handle, WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_INVALID_INPUT);	
    	rbusValue_Release(value);

    	rbus_close(handle);
    	webpaRbus_Uninit();
}

// Test case for webcfgFrSetHandler & webcfgFrGetHandler
void test_webcfgFrSet_GetHandler_success(void)
{
    	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "telemetry");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_FORCESYNC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SUCCESS);

    	rbusError_t rc_get = rbus_get(handle, WEBCFG_FORCESYNC_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	const char* retrievedValue = NULL;
    	retrievedValue = rbusValue_GetString(value, NULL);
    	CU_ASSERT_STRING_EQUAL(retrievedValue, "");
    	rbusValue_Release(value);
		
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

void test_webcfgFrSet_GetHandler_failure(void)
{
    	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "telemetry");
    	rbusSetOptions_t opts;
    	opts.commit = true;
		set_rbus_RfcEnable(false); //RFC False
    	rbusError_t rc = rbus_set(handle, WEBCFG_FORCESYNC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_ACCESS_NOT_ALLOWED);
    	rbusError_t rc_get = rbus_get(handle, WEBCFG_FORCESYNC_PARAM, &value);
    	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
		set_rbus_RfcEnable(true); 		
    	rbusValue_Release(value);

    	rbusValue_Init(&value);
    	rbusValue_SetInt32(value, 32);//Invalid Datatype
    	opts.commit = true;
    	rc = rbus_set(handle, WEBCFG_FORCESYNC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_INVALID_INPUT);
    	rbusValue_Release(value);

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "telemetry");
    	opts.commit = true;
		strcpy(Telemetry_string,"NULL"); //telemetry string NULL
    	rc = rbus_set(handle, WEBCFG_FORCESYNC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_BUS_ERROR);	
		strcpy(Telemetry_string,"");			
    	rbusValue_Release(value);	

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "telemetry");
    	opts.commit = true;
		get_webcfgReady_flag = false;	//Webcgf Ready Flag False
    	rc = rbus_set(handle, WEBCFG_FORCESYNC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_NOT_INITIALIZED);			
    	rbusValue_Release(value);	
		get_webcfgReady_flag = true;

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "telemetry");
    	opts.commit = true;
		get_bootSync_flag = true;	//bootSync Flag true
    	rc = rbus_set(handle, WEBCFG_FORCESYNC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SESSION_ALREADY_EXIST);			
    	rbusValue_Release(value);	
		get_bootSync_flag = false;


    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "{ \"value\":\"telemetry\", \"transaction_id\":\"e35c746c-bf15-43baXXXXXXXXXXXXXXXXXx____XXXXXXXXXXXXXX\"}");
    	opts.commit = true;
		strcpy(Telemetry_string,"NULL");		
    	rc = rbus_set(handle, WEBCFG_FORCESYNC_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_BUS_ERROR);			
    	rbusValue_Release(value);	
		strcpy(Telemetry_string,"");			

    	rbus_close(handle);
    	webpaRbus_Uninit();
}

// Test case for webcfgError_ToString
void test_webcfgError_ToString()
{
	char *value = NULL;
	value = webcfgError_ToString(ERROR_SUCCESS);
	CU_ASSERT_STRING_EQUAL(value, "method success");
	value = webcfgError_ToString(ERROR_FAILURE);
	CU_ASSERT_STRING_EQUAL(value, "method failure");
	value = webcfgError_ToString(ERROR_INVALID_INPUT);
	CU_ASSERT_STRING_EQUAL(value, "Invalid Input");
	value = webcfgError_ToString(ERROR_ELEMENT_DOES_NOT_EXIST);
	CU_ASSERT_STRING_EQUAL(value, "subdoc name is not found in cache");
	value = webcfgError_ToString(4);
	CU_ASSERT_STRING_EQUAL(value, "unknown error");
}

// Test case for mapRbusToWdmpDataType
void test_mapRbusToWdmpDataType()
{
	DATA_TYPE wdmp_type = WDMP_NONE;
	wdmp_type = mapRbusToWdmpDataType(RBUS_INT16);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_INT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_INT32);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_INT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_UINT16);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_UINT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_UINT32);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_UINT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_INT64);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_LONG);
	wdmp_type = mapRbusToWdmpDataType(RBUS_UINT64);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_ULONG);
	wdmp_type = mapRbusToWdmpDataType(RBUS_SINGLE);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_FLOAT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_DOUBLE);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_DOUBLE);
	wdmp_type = mapRbusToWdmpDataType(RBUS_DATETIME);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_DATETIME);
	wdmp_type = mapRbusToWdmpDataType(RBUS_BOOLEAN);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_BOOLEAN);
	wdmp_type = mapRbusToWdmpDataType(RBUS_CHAR);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_INT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_INT8);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_INT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_UINT8);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_UINT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_BYTE);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_UINT);
	wdmp_type = mapRbusToWdmpDataType(RBUS_STRING);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_STRING);
	wdmp_type = mapRbusToWdmpDataType(RBUS_BYTES);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_BYTE);
	wdmp_type = mapRbusToWdmpDataType(RBUS_PROPERTY);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_NONE);
	wdmp_type = mapRbusToWdmpDataType(RBUS_OBJECT);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_NONE);
	wdmp_type = mapRbusToWdmpDataType(RBUS_NONE);
	CU_ASSERT_EQUAL(wdmp_type, WDMP_NONE);
}

// Test case for mapWdmpToRbusDataType
void test_mapWdmpToRbusDataType()
{
	rbusValueType_t rbusType = RBUS_NONE;
	rbusType = mapWdmpToRbusDataType(WDMP_INT);
	CU_ASSERT_EQUAL(rbusType, RBUS_INT32);
	rbusType = mapWdmpToRbusDataType(WDMP_UINT);
	CU_ASSERT_EQUAL(rbusType, RBUS_UINT32);
	rbusType = mapWdmpToRbusDataType(WDMP_LONG);
	CU_ASSERT_EQUAL(rbusType, RBUS_INT64);
	rbusType = mapWdmpToRbusDataType(WDMP_ULONG);
	CU_ASSERT_EQUAL(rbusType, RBUS_UINT64);
	rbusType = mapWdmpToRbusDataType(WDMP_FLOAT);
	CU_ASSERT_EQUAL(rbusType, RBUS_SINGLE);
	rbusType = mapWdmpToRbusDataType(WDMP_DOUBLE);
	CU_ASSERT_EQUAL(rbusType, RBUS_DOUBLE);
	rbusType = mapWdmpToRbusDataType(WDMP_DATETIME);
	CU_ASSERT_EQUAL(rbusType, RBUS_DATETIME);
	rbusType = mapWdmpToRbusDataType(WDMP_BOOLEAN);
	CU_ASSERT_EQUAL(rbusType, RBUS_BOOLEAN);
	rbusType = mapWdmpToRbusDataType(WDMP_STRING);
	CU_ASSERT_EQUAL(rbusType, RBUS_STRING);
	rbusType = mapWdmpToRbusDataType(WDMP_BASE64);
	CU_ASSERT_EQUAL(rbusType, RBUS_STRING);
	rbusType = mapWdmpToRbusDataType(WDMP_BYTE);
	CU_ASSERT_EQUAL(rbusType, RBUS_BYTES);
	rbusType = mapWdmpToRbusDataType(WDMP_NONE);
	CU_ASSERT_EQUAL(rbusType, RBUS_NONE);
}

// Test case for mapRbusToCcspStatus
void test_mapRbusToCcspStatus()
{
	int CCSP_error_code = CCSP_Msg_Bus_ERROR;
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_SUCCESS);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_Bus_OK);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_BUS_ERROR);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_Bus_ERROR);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_INVALID_INPUT);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_ERR_INVALID_PARAMETER_VALUE);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_NOT_INITIALIZED);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_Bus_ERROR);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_OUT_OF_RESOURCES);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_Bus_OOM);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_DESTINATION_NOT_FOUND);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_BUS_CANNOT_CONNECT);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_DESTINATION_NOT_REACHABLE);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_ERR_UNSUPPORTED_NAMESPACE);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_DESTINATION_RESPONSE_FAILURE);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_BUS_TIMEOUT);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_INVALID_RESPONSE_FROM_DESTINATION);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_ERR_UNSUPPORTED_PROTOCOL);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_INVALID_OPERATION);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_BUS_NOT_SUPPORT);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_INVALID_EVENT);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_BUS_NOT_SUPPORT);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_INVALID_HANDLE);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_BUS_NOT_SUPPORT);
	CCSP_error_code = mapRbusToCcspStatus(RBUS_ERROR_SESSION_ALREADY_EXIST);
	CU_ASSERT_EQUAL(CCSP_error_code, CCSP_Msg_BUS_NOT_SUPPORT);
}

// Test case for regWebConfigDataModel
void test_regWebConfigDataModel_success()
{
	webconfigRbusInit("providerComponent");
	WEBCFG_STATUS ret = regWebConfigDataModel();
	CU_ASSERT_EQUAL(ret, WEBCFG_SUCCESS);
}

void test_regWebConfigDataModel_failure()
{
	WEBCFG_STATUS ret = regWebConfigDataModel();
	CU_ASSERT_EQUAL(ret, WEBCFG_FAILURE);
	webpaRbus_Uninit();
}

// Test case for registerRBUSEventElement
void test_registerRBUSEventElement_success()
{
	webconfigRbusInit("providerComponent");
	rbusError_t ret = registerRBUSEventElement();
	CU_ASSERT_EQUAL(ret, RBUS_ERROR_SUCCESS);	
	webpaRbus_Uninit();		
}

void test_registerRBUSEventElement_failure()
{
	rbusError_t ret = registerRBUSEventElement();
	CU_ASSERT_NOT_EQUAL(ret, RBUS_ERROR_SUCCESS);
	
	rbus_handle = NULL;	
	ret = registerRBUSEventElement();
	CU_ASSERT_NOT_EQUAL(ret, RBUS_ERROR_SUCCESS);		
}

//Test case for removeRBUSEventElement
void test_removeRBUSEventElement_success()
{
	webconfigRbusInit("providerComponent");
	rbusError_t ret = removeRBUSEventElement();
	CU_ASSERT_EQUAL(ret, RBUS_ERROR_SUCCESS);
	webpaRbus_Uninit();
}

//Test case for removeRBUSEventElement
void test_removeRBUSEventElement_failure()
{
	rbusError_t ret = removeRBUSEventElement();
	CU_ASSERT_NOT_EQUAL(ret, RBUS_ERROR_SUCCESS);

	rbus_handle = NULL;
	ret = removeRBUSEventElement();
	CU_ASSERT_NOT_EQUAL(ret, RBUS_ERROR_SUCCESS);		
}

// Test case for waitForUpstreamEventSubscribe
void test_waitForUpstreamEventSubscribe()
{
	waitForUpstreamEventSubscribe(5);
}

// Test case for eventSubHandler
void test_eventSubHandler()
{
	rbusError_t ret = RBUS_ERROR_BUS_ERROR;
	ret = eventSubHandler(handle, RBUS_EVENT_ACTION_SUBSCRIBE, WEBCFG_UPSTREAM_EVENT, NULL, 0, false);
	CU_ASSERT_EQUAL(ret, 0);

	ret = eventSubHandler(handle, RBUS_EVENT_ACTION_SUBSCRIBE, WEBCFG_SUBDOC_FORCERESET_PARAM, NULL, 0, false);
	CU_ASSERT_EQUAL(ret, 0);
}

// Test case for resetEventSubHandler
void test_resetEventSubHandler()
{
	bool autopublish = false;
	rbusError_t ret = RBUS_ERROR_BUS_ERROR;
	ret = resetEventSubHandler(handle, RBUS_EVENT_ACTION_SUBSCRIBE, WEBCFG_SUBDOC_FORCERESET_PARAM, NULL, 0, &autopublish);
	CU_ASSERT_EQUAL(ret, 0);

	ret = resetEventSubHandler(handle, RBUS_EVENT_ACTION_SUBSCRIBE, WEBCFG_UPSTREAM_EVENT, NULL, 0, &autopublish);
	CU_ASSERT_EQUAL(ret, 0);

	ret = resetEventSubHandler(handle, RBUS_EVENT_ACTION_UNSUBSCRIBE, WEBCFG_SUBDOC_FORCERESET_PARAM, NULL, 0, &autopublish);
	CU_ASSERT_EQUAL(ret, 0);
}

// Test case for set_rbus_RfcEnable & get_rbus_RfcEnable
void test_set_get_rbus_RfcEnable()
{
	int result = 0;
	bool val = true;
	result = set_rbus_RfcEnable(false);
	CU_ASSERT_EQUAL(result, 0);
	val = get_rbus_RfcEnable();
	CU_ASSERT_FALSE(val);
	result = set_rbus_RfcEnable(true);
	CU_ASSERT_EQUAL(result, 0);
	val = get_rbus_RfcEnable();
	CU_ASSERT_TRUE(val);	
	rbus_StoreValueIntoDB_ret = 1;
	result = set_rbus_RfcEnable(true);
	CU_ASSERT_EQUAL(result, 1);	
	rbus_StoreValueIntoDB_ret = 0;		
}

// Test case for rbus_log_handler
void test_rbus_log_handler()
{
    	rbus_log_handler(0, "file1", 1, 0, "message1");
    	rbus_log_handler(1, "file2", 2, 0, "message2");
    	rbus_log_handler(2, "file3", 3, 0, "message3");
    	rbus_log_handler(3, "file4", 4, 0, "message4");
    	rbus_log_handler(4, "file5", 5, 0, "message5");
}

// Test case for registerRbusLogger 
void test_registerRbusLogger()
{
    	registerRbusLogger();
}
 
// Test case for set_global_webconfig_url
void test_set_global_webconfig_url()
{
	set_global_webconfig_url("webcfgurl");
}

// Test case for set_global_supplementary_url
void test_set_global_supplementary_url()
{
	set_global_supplementary_url("supplementaryurl");
}

// Test case for checkSubdocInDb
void test_checkSubdocInDb()
{
	webcfgError_t result = ERROR_FAILURE;
	//DB is NULL
	result = checkSubdocInDb("doc1");
	CU_ASSERT_EQUAL(result, ERROR_FAILURE);

	webconfig_db_data_t *wd1, *wd2;
	wd1 = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    	CU_ASSERT_PTR_NOT_NULL(wd1);
    	wd1->name = strdup("doc1");
    	wd1->version = 567843562;
    	wd1->root_string = strdup("portmapping");
    	wd1->next=NULL;
    	addToDBList(wd1);
    	int n1 = get_successDocCount();
    	CU_ASSERT_EQUAL(1,n1);
	wd2 = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    	CU_ASSERT_PTR_NOT_NULL(wd1);
    	wd2->name = strdup("doc2");
    	wd2->version = 567843534;
    	wd2->root_string = strdup("wan");
    	wd2->next=NULL;
    	addToDBList(wd2);
    	int n2 = get_successDocCount();
    	CU_ASSERT_EQUAL(2,n2);
	CU_ASSERT_FATAL( NULL != get_global_db_node());

	//subdoc present
	result = checkSubdocInDb("doc2");
	CU_ASSERT_EQUAL(result, ERROR_SUCCESS);

	//subdoc not found
	result = checkSubdocInDb("doc3");
	CU_ASSERT_EQUAL(result, ERROR_ELEMENT_DOES_NOT_EXIST);

    	webcfgdb_destroy(wd1);
	webcfgdb_destroy(wd2);
    	reset_successDocCount();
    	reset_db_node();
    	CU_ASSERT_FATAL( NULL == get_global_db_node());
}

//Test case for resetSubdocVersion
void test_resetSubdocVersion()
{
	webcfgError_t result = ERROR_FAILURE;
	//subdoc not found
	result = resetSubdocVersion("doc1");
	CU_ASSERT_EQUAL(result, ERROR_ELEMENT_DOES_NOT_EXIST);

	webconfig_db_data_t *wd;
	wd = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    	CU_ASSERT_PTR_NOT_NULL(wd);
    	wd->name = strdup("doc1");
    	wd->version = 567843562;
    	wd->root_string = strdup("portmapping");
    	wd->next=NULL;
    	addToDBList(wd);
    	int n = get_successDocCount();
    	CU_ASSERT_EQUAL(1,n);
	
	//Tmp list is NULL
	result = resetSubdocVersion("doc1");
	CU_ASSERT_EQUAL(result, ERROR_SUCCESS);

    	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    	tmpData->name = strdup("doc1");
    	tmpData->version = 567843562;
    	tmpData->status = strdup("pending");
    	tmpData->trans_id = 4104;
    	tmpData->retry_count = 0;
    	tmpData->error_code = 0;
    	tmpData->error_details = strdup("none");
    	tmpData->next = NULL;
	set_global_tmp_node(tmpData);
	
	//subdoc reseted in DB & TmpList	
	result = resetSubdocVersion("doc1");
	CU_ASSERT_EQUAL(result, ERROR_SUCCESS);
	
	webcfgdb_destroy(wd);
	reset_successDocCount();
    	reset_db_node();
	delete_tmp_list();	
}

// Test case for trigger_webcfg_forcedsync
void test_trigger_webcfg_forcedsync()
{
	char *str = NULL;
	char* transID = NULL;
	trigger_webcfg_forcedsync();
	int result = get_rbus_ForceSync(&str, &transID);
	CU_ASSERT_EQUAL(result, 1);
	CU_ASSERT_STRING_EQUAL(str, "root");
}

// Test case for parseForceSyncJson
void test_parseForceSyncJson()
{
	char *transactionId = NULL;
    	char *value = NULL;
        int result = 0;
	char *data1 = "root";
	char *data2 = "{ \"transaction_id\":\"e35c746c-bf15-43baXXXXXXXXXXXXXXXXXx____XXXXXXXXXXXXXX\"}";
	char *data3 = "{ \"value\":\"\", \"transaction_id\":\"e35c746c-bf15-43baXXXXXXXXXXXXXXXXXx____XXXXXXXXXXXXXX\"}";
	char *data4 = "{ \"value\":\"root\", \"transaction_id\":\"e67c746c-bf15-43baXXXXXXXXXXXXXXXXXx____XXXXXXXXXXXXXX\"}";
	char *data5 = "{ \"value\":\"root\", \"transaction_id\":\"\"}";
	char *data6= "{ \"value\":\"telemetry\", \"transaction_id\":\"e58c746c-bf15-43baXXXXXXXXXXXXXXXXXx____XXXXXXXXXXXXXX\"}";

	result = parseForceSyncJson(data1, &value, &transactionId);
	CU_ASSERT_EQUAL(result, 0);
	result = parseForceSyncJson(data2, &value, &transactionId);
	CU_ASSERT_EQUAL(result, 0);
	result = parseForceSyncJson(data3, &value, &transactionId);
	CU_ASSERT_EQUAL(result, 0);
	result = parseForceSyncJson(data4, &value, &transactionId);
	CU_ASSERT_EQUAL(result, 0);
	result = parseForceSyncJson(data5, &value, &transactionId);
	CU_ASSERT_EQUAL(result, 0);
	result = parseForceSyncJson(data6, &value, &transactionId);
	CU_ASSERT_EQUAL(result, 0);
}

// Test case for rbusWebcfgEventHandler
void test_rbusWebcfgEventHandler()
{	
    	webconfigRbusInit("providerComponent");
    	registerRBUSEventElement();
 
    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
    	{
		CU_FAIL("rbus_open failed for consumerComponent");
    	}   
	
    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "eventhandler");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_EVENT_NAME, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SUCCESS);
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

// Test case for fetchMpBlobData
void test_fetchMpBlobData()
{
	webcfgError_t result = ERROR_SUCCESS;
	int bloblen = 0;
	void *blobData = NULL;
	uint32_t etag = 0;
	//Multipart Cache is NULL
	result = fetchMpBlobData("moca", &blobData, &bloblen, &etag);
	CU_ASSERT_EQUAL(result, ERROR_FAILURE);
       
	multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
   	multipartdocs->name_space = strdup("moca");
    	multipartdocs->data = strdup("mocaBLOB");
	multipartdocs->data_size = sizeof(multipartdocs->data);
    	multipartdocs->isSupplementarySync = 1;
	multipartdocs->etag = 11573827;
    	multipartdocs->next = NULL;
    	set_global_mp(multipartdocs);
    	CU_ASSERT_FATAL(NULL != get_global_mp());

	//Subdoc not found
	result = fetchMpBlobData("wan", &blobData, &bloblen, &etag);
	CU_ASSERT_EQUAL(result, ERROR_ELEMENT_DOES_NOT_EXIST);

	//Subdoc found and fetch blob data success
	result = fetchMpBlobData("moca", &blobData, &bloblen, &etag);
	CU_ASSERT_EQUAL(result, ERROR_SUCCESS);
	CU_ASSERT_STRING_EQUAL(blobData, "mocaBLOB");
	CU_ASSERT_EQUAL(bloblen, 8);
	CU_ASSERT_EQUAL(etag, 11573827);

	set_global_mp(NULL);
	CU_ASSERT_FATAL(NULL == get_global_mp());
}

//Test case for setFetchCachedBlobErrCode & fetchCachedBlobHandler
void test_webcfg_util_method()
{
	webconfigRbusInit("providerComponent");
	regWebConfigDataModel();
	int rc = 0;
	int res = rbus_open(&handle, "consumerComponent");
        if(res != RBUS_ERROR_SUCCESS)
        {
		CU_FAIL("rbus_open failed for consumerComponent");
        } 

        rbusObject_t inParams;
    	rbusObject_t outParams;
	rbusProperty_t checkParams;
    	rbusValue_t value, propValue;

    	rbusObject_Init(&inParams, NULL);

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "");
    	rbusObject_SetValue(inParams, "subdocName", value);
    	rbusValue_Release(value);

	//Subdoc value is empty
    	rc = rbusMethod_Invoke(handle, WEBCFG_UTIL_METHOD, inParams, &outParams);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_BUS_ERROR);
	checkParams = rbusObject_GetProperties(outParams);
	while(checkParams)
	{
		char const* propName = rbusProperty_GetName(checkParams);
		propValue = rbusProperty_GetValue(checkParams);
		if(strcmp(propName, "error_code") == 0)
		{
			uint8_t propGetValue = rbusValue_GetUInt8(propValue);
			CU_ASSERT_STRING_EQUAL(propName, "error_code");
			CU_ASSERT_EQUAL(propGetValue, 2);
		}
		else if(strcmp(propName, "error_string") == 0)
		{
			const char* propGetValue = rbusValue_GetString(propValue, NULL);
			CU_ASSERT_STRING_EQUAL(propName, "error_string");
			CU_ASSERT_STRING_EQUAL(propGetValue, "Invalid Input");
		}
		checkParams = rbusProperty_GetNext(checkParams);	
	}
    	rbusObject_Release(inParams);
	rbusObject_Release(outParams);
	rbusProperty_Release(checkParams);

    	rbusObject_Init(&inParams, NULL);

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "moca");
    	rbusObject_SetValue(inParams, "subdocName", value);
    	rbusValue_Release(value);

	//Multipart Cache is NULL
    	rc = rbusMethod_Invoke(handle, WEBCFG_UTIL_METHOD, inParams, &outParams);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_BUS_ERROR);
	checkParams = rbusObject_GetProperties(outParams);
	while(checkParams)
	{
		char const* propName = rbusProperty_GetName(checkParams);
		propValue = rbusProperty_GetValue(checkParams);
		if(strcmp(propName, "error_code") == 0)
		{
			uint8_t propGetValue = rbusValue_GetUInt8(propValue);
			CU_ASSERT_STRING_EQUAL(propName, "error_code");
			CU_ASSERT_EQUAL(propGetValue, 1);
		}
		else if(strcmp(propName, "error_string") == 0)
		{
			const char* propGetValue = rbusValue_GetString(propValue, NULL);
			CU_ASSERT_STRING_EQUAL(propName, "error_string");
			CU_ASSERT_STRING_EQUAL(propGetValue, "method failure");
		}
		checkParams = rbusProperty_GetNext(checkParams);	
	}
    	rbusObject_Release(inParams);
	rbusObject_Release(outParams);
	rbusProperty_Release(checkParams);

	multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
   	multipartdocs->name_space = strdup("moca");
    	multipartdocs->data = strdup("mocaBLOB");
	multipartdocs->data_size = sizeof(multipartdocs->data);
    	multipartdocs->isSupplementarySync = 1;
	multipartdocs->etag = 11573827;
    	multipartdocs->next = NULL;
    	set_global_mp(multipartdocs);
    	CU_ASSERT_FATAL(NULL != get_global_mp());

    	rbusObject_Init(&inParams, NULL);

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "wan");
    	rbusObject_SetValue(inParams, "subdocName", value);
    	rbusValue_Release(value);
	//subdoc is not found
    	rc = rbusMethod_Invoke(handle, WEBCFG_UTIL_METHOD, inParams, &outParams);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_BUS_ERROR);
	checkParams = rbusObject_GetProperties(outParams);
	while(checkParams)
	{
		char const* propName = rbusProperty_GetName(checkParams);
		propValue = rbusProperty_GetValue(checkParams);
		if(strcmp(propName, "error_code") == 0)
		{
			uint8_t propGetValue = rbusValue_GetUInt8(propValue);
			CU_ASSERT_STRING_EQUAL(propName, "error_code");
			CU_ASSERT_EQUAL(propGetValue, 3);
		}
		else if(strcmp(propName, "error_string") == 0)
		{
			const char* propGetValue = rbusValue_GetString(propValue, NULL);
			CU_ASSERT_STRING_EQUAL(propName, "error_string");
			CU_ASSERT_STRING_EQUAL(propGetValue, "subdoc name is not found in cache");
		}
		checkParams = rbusProperty_GetNext(checkParams);	
	}
    	rbusObject_Release(inParams);
	rbusObject_Release(outParams);
	rbusProperty_Release(checkParams);
	
	rbusObject_Init(&inParams, NULL);

    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "moca");
    	rbusObject_SetValue(inParams, "subdocName", value);
    	rbusValue_Release(value);
	//successcase
    	rc = rbusMethod_Invoke(handle, WEBCFG_UTIL_METHOD, inParams, &outParams);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SUCCESS);
	checkParams = rbusObject_GetProperties(outParams);
	while(checkParams)
	{
		char const* propName = rbusProperty_GetName(checkParams);
		propValue = rbusProperty_GetValue(checkParams);
		if(strcmp(propName, "etag") == 0)
		{
			uint32_t propGetValue = rbusValue_GetUInt32(propValue);
			CU_ASSERT_STRING_EQUAL(propName, "etag");
			CU_ASSERT_EQUAL(propGetValue, 11573827);
		}
		else if(strcmp(propName, "data") == 0)
		{
			int len;
			uint8_t const* propGetValue = rbusValue_GetBytes(propValue, &len);
			CU_ASSERT_STRING_EQUAL(propName, "data");
			CU_ASSERT_FATAL(NULL != propGetValue);
		}
		checkParams = rbusProperty_GetNext(checkParams);	
	}
    	rbusObject_Release(inParams);
	rbusObject_Release(outParams);
	rbusProperty_Release(checkParams);

	set_global_mp(NULL);
	CU_ASSERT_FATAL(NULL == get_global_mp());
	rbus_close(handle);
	webpaRbus_Uninit();
}

static void publishEventSuccessCallbackHandler(
    rbusHandle_t handle,
    rbusEvent_t const* event,
    rbusEventSubscription_t* subscription)
{
    	rbusValue_t incoming_value;

    	incoming_value = rbusObject_GetValue(event->data, "value");

    	if(incoming_value)
    	{
        	char * inVal = (char *)rbusValue_GetString(incoming_value, NULL);
		WebcfgInfo("inVal is %s\n", inVal);
		WebcfgInfo("rbusEvent_OnPublish webcfgSubdocForceReset success callback received successfully with message - %s\n", inVal);
		CU_ASSERT(1);
    	}
    	(void)handle;
}

// Test case for webcfgSubdocForceResetSetHandler, webcfgSubdocForceResetGetHandler & publishSubdocResetEvent
void test_webcfgSubdocForceResetSet_GetHandler(void)
{
    	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
	{
		CU_FAIL("rbus_open failed for consumerComponent");
	}

    	webconfig_db_data_t *wd1, *wd2, *wd3;
    	wd1 = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    	CU_ASSERT_PTR_NOT_NULL(wd1);
    	wd1->name = strdup("root");
    	wd1->version = 567843562;
    	wd1->root_string = strdup("rootstring");
    	wd1->next=NULL;
    	addToDBList(wd1);
    	int n1 = get_successDocCount();
    	CU_ASSERT_EQUAL(1,n1);
    	wd2 = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    	CU_ASSERT_PTR_NOT_NULL(wd2);
    	wd2->name = strdup("doc1");
    	wd2->version = 43843534;
    	wd2->root_string = strdup("portmapping");
    	wd2->next=NULL;
    	addToDBList(wd2);
    	int n2 = get_successDocCount();
    	CU_ASSERT_EQUAL(2,n2);
    	wd3 = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    	CU_ASSERT_PTR_NOT_NULL(wd3);
    	wd3->name = strdup("doc2");
    	wd3->version = 264343534;
    	wd3->root_string = strdup("wan");
    	wd3->next=NULL;
    	addToDBList(wd3);
    	int n3 = get_successDocCount();
    	CU_ASSERT_EQUAL(3,n3);
    	CU_ASSERT_FATAL( NULL != get_global_db_node());

    	int ret = rbusEvent_Subscribe(handle, WEBCFG_SUBDOC_FORCERESET_PARAM, publishEventSuccessCallbackHandler, NULL, 0);
    	if(ret != RBUS_ERROR_SUCCESS)
	{
		CU_FAIL("subscribe_to_event onsubscribe event failed");
	}

    	rbusValue_t value;
    	rbusValue_Init(&value);
    	rbusValue_SetString(value, "doc1,doc2");

    	rbusSetOptions_t opts;
    	opts.commit = true;

    	rbusError_t rc = rbus_set(handle, WEBCFG_SUBDOC_FORCERESET_PARAM, value, &opts);
    	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SUCCESS);

    	rbusError_t rc_get = rbus_get(handle, WEBCFG_SUBDOC_FORCERESET_PARAM, &value);
   	CU_ASSERT_EQUAL(rc_get, RBUS_ERROR_SUCCESS);
    	const char* retrievedValue = NULL;
    	retrievedValue = rbusValue_GetString(value, NULL);
    	CU_ASSERT_STRING_EQUAL(retrievedValue, "");
    	rbusValue_Release(value);
    	webcfgdb_destroy(wd1);
    	webcfgdb_destroy(wd2);
    	webcfgdb_destroy(wd3);
    	reset_successDocCount();
    	reset_db_node();
    	CU_ASSERT_FATAL( NULL == get_global_db_node());
    	rbusEvent_Unsubscribe(handle, WEBCFG_SUBDOC_FORCERESET_PARAM);
    	rbus_close(handle);
    	webpaRbus_Uninit();
}

#ifdef WAN_FAILOVER_SUPPORTED
rbusError_t webcfgInterfaceSubscribeHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish)
{
    	(void)handle;
    	(void)filter;
    	(void)interval;
	
	*autoPublish = false;

    	WebcfgInfo(
        	"webcfgInterfaceSubscribeHandler called:\n" \
        	"\taction=%s\n" \
        	"\teventName=%s\n",
        	action == RBUS_EVENT_ACTION_SUBSCRIBE ? "subscribe" : "unsubscribe",
        	eventName);
    
    	if(!strcmp(WEBCFG_INTERFACE_PARAM, eventName))
    	{
        	int subscribe = action == RBUS_EVENT_ACTION_SUBSCRIBE ? 1 : 0;
		if(subscribe) {
			WebcfgInfo("provider: webcfgInterfaceSubscribeHandler subscribe\n");
		}
		else{
			WebcfgInfo("provider: webcfgInterfaceSubscribeHandler unsubscribe\n");
		}
    	}
    	else
    	{
        	WebcfgInfo("provider: webcfgInterfaceSubscribeHandler unexpected eventName %s\n", eventName);
    	}

    	return RBUS_ERROR_SUCCESS;
}

// Test for subscribeTo_CurrentActiveInterface_Event
void test_subscribeTo_CurrentActiveInterface_Event()
{
	int rc = subscribeTo_CurrentActiveInterface_Event();//Failure Case
	CU_ASSERT_NOT_EQUAL(rc, RBUS_ERROR_SUCCESS);

	webconfigRbusInit("consumerComponent");
	int res = rbus_open(&handle, "providerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
	{
		CU_FAIL("rbus_open failed for providerComponent");
	}

	rbusDataElement_t webcfgInterfaceElement[1] = {
		{WEBCFG_INTERFACE_PARAM, RBUS_ELEMENT_TYPE_PROPERTY, {NULL, NULL, NULL, NULL, webcfgInterfaceSubscribeHandler, NULL}}
	};
	rbusError_t ret = rbus_regDataElements(handle, 1, webcfgInterfaceElement);
	CU_ASSERT_EQUAL(ret, RBUS_ERROR_SUCCESS);

	rc = subscribeTo_CurrentActiveInterface_Event();
	CU_ASSERT_EQUAL(rc, RBUS_ERROR_SUCCESS);
    sleep(1);
	rbusEvent_Unsubscribe(rbus_handle,WEBCFG_INTERFACE_PARAM);	
	webpaRbus_Uninit();
	rbus_unregDataElements(handle, 1, webcfgInterfaceElement);
	rbus_close(handle);
}

//WanMgr_Rbus_String_EventPublish_OnValueChange(): publish rbus events on value change
rbusError_t WanMgr_Rbus_String_EventPublish_OnValueChange(rbusHandle_t rbusHandle,char *dm_event, void *prev_dm_value, void *dm_value)
{
    rbusEvent_t event;
    rbusObject_t rdata;
    rbusValue_t Value, preValue, byVal;
	rbusError_t ret;
    int rc = RBUS_ERROR_BUS_ERROR;

    if(dm_event == NULL || dm_value == NULL)
    {
        WebcfgError("%s %d - Failed publishing\n", __FUNCTION__, __LINE__);
        return rc;
    }

    rbusValue_Init(&Value);
    rbusValue_SetString(Value, (char*)dm_value);

    rbusValue_Init(&preValue);
    rbusValue_SetString(preValue, (char*)prev_dm_value);

    rbusValue_Init(&byVal);
    rbusValue_SetString(byVal, "webcfg_unitest");

    rbusObject_Init(&rdata, NULL);
    rbusObject_SetValue(rdata, "value", Value);
    rbusObject_SetValue(rdata, "oldValue", preValue);
    rbusObject_SetValue(rdata, "by", byVal);

    event.name = dm_event;
    event.data = rdata;
    event.type = RBUS_EVENT_VALUE_CHANGED;

    WebcfgInfo("%s %d - dm_event[%s],prev_dm_value[%s],dm_value[%s]\n", __FUNCTION__, __LINE__, dm_event, (char*)prev_dm_value, (char*)dm_value);

    if((ret = rbusEvent_Publish(rbusHandle, &event)) != RBUS_ERROR_SUCCESS)
    {
        WebcfgError("%s %d - Event [%s] published failed %d\n", __FUNCTION__, __LINE__,dm_event,ret);
    }
    else
    {
        WebcfgInfo("%s %d - Event [%s] published successfully\n", __FUNCTION__, __LINE__, dm_event);
        rc = RBUS_ERROR_SUCCESS;
    }

    rbusValue_Release(Value);
    rbusValue_Release(preValue);
    rbusValue_Release(byVal);
    rbusObject_Release(rdata);
    return rc;
}
 //Event subscribe handler API for objects:
rbusError_t WanMgr_Rbus_SubscribeHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char *eventName, rbusFilter_t filter, int32_t interval, bool *autoPublish)
{
    (void)handle;
    (void)filter;
    (void)interval;

    *autoPublish = false;

    WebcfgInfo(("WanMgr_Rbus_SubscribeHandler called.\n"));

    if (action == RBUS_EVENT_ACTION_SUBSCRIBE)
    {
        WebcfgInfo(" %s %d - %s Subscribed \n", __FUNCTION__, __LINE__, eventName);
    }
    else
    {
        WebcfgError(" %s %d - %s Unsubscribed \n", __FUNCTION__, __LINE__, eventName);
    }
    return RBUS_ERROR_SUCCESS;
}

void test_eventReceiveHandler()
{
	rbusDataElement_t wanMgrRbusDataElements[1] = {
		{WEBCFG_INTERFACE_PARAM,  RBUS_ELEMENT_TYPE_PROPERTY, {NULL, NULL, NULL, NULL, WanMgr_Rbus_SubscribeHandler, NULL}}};		

	int res = rbus_open(&handle, "providerComponent");
    if(res != RBUS_ERROR_SUCCESS)
	{
		CU_FAIL("rbus_open failed for providerComponent");
	}
	rbusError_t ret = rbus_regDataElements(handle, 1, wanMgrRbusDataElements);
	CU_ASSERT_EQUAL(ret, RBUS_ERROR_SUCCESS);
	webconfigRbusInit(WEBCFG_COMPONENT_NAME);
	int rc = RBUS_ERROR_SUCCESS;	
	rc = subscribeTo_CurrentActiveInterface_Event();
 	WebcfgInfo(" subscribeTo_CurrentActiveInterface_Event : %d\n",rc);
	sleep(1);
	get_webcfgReady_flag = false;
	WanMgr_Rbus_String_EventPublish_OnValueChange(handle,WEBCFG_INTERFACE_PARAM,"eth1","eth2");
	sleep(2);
	get_webcfgReady_flag = true;
	WanMgr_Rbus_String_EventPublish_OnValueChange(handle,WEBCFG_INTERFACE_PARAM,"eth1","eth2");
	sleep(2);
	printf("get_global_interface(): %s\n",get_global_interface());
	CU_ASSERT_STRING_EQUAL(get_global_interface(),"eth2");
	rbusEvent_Unsubscribe(rbus_handle,WEBCFG_INTERFACE_PARAM);	
	webpaRbus_Uninit();
	rbus_unregDataElements(handle, 1, wanMgrRbusDataElements);
	rbus_close(handle);		
}
#endif

void test_setValues_rbus()
{
	webconfigRbusInit("consumerComponent");
	regWebConfigDataModel();
	
	WDMP_STATUS ret = WDMP_FAILURE;
	int ccspStatus = 0;
	int paramCount = 2;
	param_t *reqParam = NULL;
	reqParam = (param_t *) malloc(sizeof(param_t) * paramCount);
	memset(reqParam,0,(sizeof(param_t) * paramCount));
	reqParam[0].name = WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM;
	reqParam[0].value = strdup("telemetry");
	reqParam[0].type = WDMP_BASE64;
	reqParam[1].name = WEBCFG_URL_PARAM;
	reqParam[1].value = strdup("webcfgurl");
	reqParam[1].type = WDMP_BASE64;

	setValues_rbus(reqParam, paramCount, 3, NULL, NULL, &ret, &ccspStatus);
	CU_ASSERT_EQUAL(CCSP_Msg_Bus_OK, ccspStatus);
	CU_ASSERT_EQUAL(WDMP_SUCCESS, ret);
	free(reqParam);
}

void test_getVlues_rbus()
{
	int paramCount=0;
	int retStatus = WDMP_FAILURE;
	int count=0;
	const char *getParamList[2];
	getParamList[0] = WEBCFG_SUPPLEMENTARY_TELEMETRY_PARAM;
	getParamList[1] = WEBCFG_URL_PARAM;

	paramCount = sizeof(getParamList)/sizeof(getParamList[0]);
	param_t **parametervalArr = (param_t **) malloc(sizeof(param_t *) * paramCount);

	getValues_rbus(getParamList, paramCount, 0, NULL, &parametervalArr, &count, &retStatus);
	CU_ASSERT_EQUAL(count, 2);
	CU_ASSERT_EQUAL(retStatus, WDMP_SUCCESS);
	CU_ASSERT_STRING_EQUAL(parametervalArr[0]->value, "telemetry");
	CU_ASSERT_STRING_EQUAL(parametervalArr[1]->value, "webcfgurl");
	
	free(parametervalArr);
	webpaRbus_Uninit(); 
}

static void publishUpstreamEventSuccessCallbackHandler(
    rbusHandle_t handle,
    rbusEvent_t const* event,
    rbusEventSubscription_t* subscription)
{
	WebcfgInfo("OnPublish webcfgUtil success callback received successfully\n");
}

void test_sendNotification_rbus()
{
	webconfigRbusInit("providerComponent");
    	regWebConfigDataModel();

	char *payload = strdup("payloadData");
	char *source = strdup("src");
	char *destination = strdup("dest");

    	int res = rbus_open(&handle, "consumerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
	{
		CU_FAIL("rbus_open failed for consumerComponent");
	}
	int ret = rbusEvent_Subscribe(handle, WEBCFG_UPSTREAM_EVENT, publishUpstreamEventSuccessCallbackHandler, NULL, 0);
    	if(ret != RBUS_ERROR_SUCCESS)
	{
		CU_FAIL("subscribe_to_event onsubscribe event failed");
	}
	
	sendNotification_rbus(payload, source, destination);
    rbusEvent_Unsubscribe(handle,WEBCFG_UPSTREAM_EVENT);
	rbus_close(handle);
	webpaRbus_Uninit();
}

rbusError_t webcfgBlobElementSetHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts) {

	(void) handle;
    	(void) opts;
    	char const* paramName = rbusProperty_GetName(prop);

    	WebcfgInfo("Parameter name is %s \n", paramName);
    	rbusValueType_t type_t;
    	rbusValue_t paramValue_t = rbusProperty_GetValue(prop);
    	if(paramValue_t) {
        	type_t = rbusValue_GetType(paramValue_t);
    	} else {
		WebcfgError("Invalid input to set\n");
        	return RBUS_ERROR_INVALID_INPUT;
    	}
   
    	if(strncmp(paramName, WEBCFG_BLOB_PARAM, maxParamLen) == 0){
        	if(type_t == RBUS_BYTES) {
            		int len;
	    		uint8_t const* data = rbusValue_GetBytes(paramValue_t, &len);
	    		CU_ASSERT_FATAL(NULL != data);
        	}
         	else {
            		WebcfgError("Unexpected value type for property %s\n", paramName);
	    		return RBUS_ERROR_INVALID_INPUT;
        	}
    	}
	else {
		WebcfgError("Unexpected parameter = %s\n", paramName);
		return RBUS_ERROR_ELEMENT_DOES_NOT_EXIST;
	}   
     	return RBUS_ERROR_SUCCESS;
}

void test_blobSet_rbus()
{
	char *buff = strdup("blobdata");
	int sendMsgSize = sizeof(buff);
	WDMP_STATUS retStatus = WDMP_FAILURE;
	int ccspStatus = 0;

	int res = rbus_open(&handle, "providerComponent");
    	if(res != RBUS_ERROR_SUCCESS)
	{
		CU_FAIL("rbus_open failed for providerComponent");
	}
	rbusDataElement_t webcfgBlobElement[1] = {
		{WEBCFG_BLOB_PARAM, RBUS_ELEMENT_TYPE_PROPERTY, {NULL, webcfgBlobElementSetHandler, NULL, NULL, NULL, NULL}}
	};
	rbusError_t ret = rbus_regDataElements(handle, 1, webcfgBlobElement);
	CU_ASSERT_EQUAL(ret, RBUS_ERROR_SUCCESS);

	webconfigRbusInit("consumerComponent");	
	blobSet_rbus(WEBCFG_BLOB_PARAM, buff, sendMsgSize, &retStatus, &ccspStatus);
	CU_ASSERT_EQUAL(CCSP_Msg_Bus_OK, ccspStatus);
	CU_ASSERT_EQUAL(WDMP_SUCCESS, retStatus);

	rbus_close(handle);
	webpaRbus_Uninit(); 
}

void add_suites( CU_pSuite *suite )
{
	*suite = CU_add_suite( "tests", NULL, NULL );
     	CU_add_test( *suite, "test rbus_forcesync", test_setForceSync);
     	CU_add_test( *suite, "test rbus_forcesync_failure", test_setForceSync_failure);
     	CU_add_test( *suite, "test isRbusEnabled_success", test_isRbusEnabled_success);
     	CU_add_test( *suite, "test get_global_isRbus_success", test_get_global_isRbus_success);
     	CU_add_test( *suite, "test isRfcEnabled_success", test_isRfcEnabled_success);
     	CU_add_test( *suite, "test isRfcEnabled_failure", test_isRfcEnabled_failure);
     	CU_add_test( *suite, "test isRbusInitialized_failure", test_isRbusInitialized_failure);
     	CU_add_test( *suite, "test mqttCMRbusInit_success", test_webconfigRbusInit_success);
     	CU_add_test( *suite, "test isRbusInitialized_success", test_isRbusInitialized_success);
     	CU_add_test( *suite, "test get_global_rbus_handle", test_get_global_rbus_handle);
     	CU_add_test( *suite, "test webpaRbus_Uninit", test_webpaRbus_Uninit);
     	CU_add_test( *suite, "test webcfgRfcSet_GetHandler_success", test_webcfgRfcSet_GetHandler_success);
     	CU_add_test( *suite, "test webcfgRfcSet_GetHandler_failure", test_webcfgRfcSet_GetHandler_failure);		
     	CU_add_test( *suite, "test webcfgDataSet_GetHandler_success",test_webcfgDataSet_GetHandler_success);
		CU_add_test( *suite, "test webcfgDataSet_GetHandler_failure",test_webcfgDataSet_GetHandler_failure);
     	CU_add_test( *suite, "test webcfgSupportedDocsSet_GetHandler", test_webcfgSupportedDocsSet_GetHandler);
     	CU_add_test( *suite, "test webcfgSupportedVersionSet_GetHandler", test_webcfgSupportedVersionSet_GetHandler);
     	CU_add_test( *suite, "test webcfgUrlSet_GetHandler_success", test_webcfgUrlSet_GetHandler_success);
     	CU_add_test( *suite, "test webcfgUrlSet_GetHandler_failure", test_webcfgUrlSet_GetHandler_failure);		
     	CU_add_test( *suite, "test webcfgTelemetrySet_GetHandler_success", test_webcfgTelemetrySet_GetHandler_success);
     	CU_add_test( *suite, "test webcfgTelemetrySet_GetHandler_failure", test_webcfgTelemetrySet_GetHandler_failure);		
     	CU_add_test( *suite, "test webcfgFrSet_GetHandler_success", test_webcfgFrSet_GetHandler_success);
     	CU_add_test( *suite, "test webcfgFrSet_GetHandler_failure", test_webcfgFrSet_GetHandler_failure);		
     	CU_add_test( *suite, "test webcfgError_ToString", test_webcfgError_ToString);
     	CU_add_test( *suite, "test mapRbusToWdmpDataType", test_mapRbusToWdmpDataType);
     	CU_add_test( *suite, "test mapWdmpToRbusDataType", test_mapWdmpToRbusDataType);
     	CU_add_test( *suite, "test mapRbusToCcspStatus", test_mapRbusToCcspStatus);
     	CU_add_test( *suite, "test regWebConfigDataModel_success", test_regWebConfigDataModel_success);
     	CU_add_test( *suite, "test regWebConfigDataModel_failure", test_regWebConfigDataModel_failure);
     	CU_add_test( *suite, "test registerRBUSEventElement_success", test_registerRBUSEventElement_success);
     	CU_add_test( *suite, "test registerRBUSEventElement_failure", test_registerRBUSEventElement_failure);
     	CU_add_test( *suite, "test removeRBUSEventElement_success", test_removeRBUSEventElement_success);
     	CU_add_test( *suite, "test removeRBUSEventElement_failure", test_removeRBUSEventElement_failure);     	
     	CU_add_test( *suite, "test waitForUpstreamEventSubscribe", test_waitForUpstreamEventSubscribe);
     	CU_add_test( *suite, "test eventSubHandler", test_eventSubHandler);
     	CU_add_test( *suite, "test resetEventSubHandler", test_resetEventSubHandler);
     	CU_add_test( *suite, "test set_get_rbus_RfcEnable", test_set_get_rbus_RfcEnable);
     	CU_add_test( *suite, "test set_global_webconfig_url", test_set_global_webconfig_url);
     	CU_add_test( *suite, "test set_global_supplementary_url", test_set_global_supplementary_url);
     	CU_add_test( *suite, "test checkSubdocInDb", test_checkSubdocInDb);
     	CU_add_test( *suite, "test resetSubdocVersion", test_resetSubdocVersion);
     	CU_add_test( *suite, "test trigger_webcfg_forcedsync", test_trigger_webcfg_forcedsync);
     	CU_add_test( *suite, "test parseForceSyncJson", test_parseForceSyncJson);
     	CU_add_test( *suite, "test rbusWebcfgEventHandler", test_rbusWebcfgEventHandler);
     	CU_add_test( *suite, "test fetchMpBlobData", test_fetchMpBlobData);
     	CU_add_test( *suite, "test webcfg_util_method", test_webcfg_util_method);
		CU_add_test( *suite, "test addForceSyncMsgToQueue", test_addForceSyncMsgToQueue);
		CU_add_test( *suite, "test DisplayQueue", test_DisplayQueue);
		CU_add_test( *suite, "test deleteForceSyncMsgQueue_simple", test_deleteForceSyncMsgQueue);
		CU_add_test( *suite, "test updateForceSyncMsgQueue", test_updateForceSyncMsgQueue_Failure);
		CU_add_test( *suite, "test DisplayQueue", test_updateForceSyncMsgQueue_found);
	#ifdef WAN_FAILOVER_SUPPORTED
     	CU_add_test( *suite, "test eventReceiveHandler", test_eventReceiveHandler);			
     	CU_add_test( *suite, "test subscribeTo_CurrentActiveInterface_Event", test_subscribeTo_CurrentActiveInterface_Event);
	#endif
     	CU_add_test( *suite, "test webcfgSubdocForceResetSet_GetHandler", test_webcfgSubdocForceResetSet_GetHandler);	
	CU_add_test( *suite, "test setValues_rbus", test_setValues_rbus);
	CU_add_test( *suite, "test getVlues_rbus", test_getVlues_rbus);
	CU_add_test( *suite, "test sendNotification_rbus", test_sendNotification_rbus);
	CU_add_test( *suite, "test blobSet_rbus", test_blobSet_rbus); 
    	CU_add_test( *suite, "test registerRbusLogger", test_registerRbusLogger);
    	CU_add_test( *suite, "test rbus_log_handler", test_rbus_log_handler);		
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( int argc, char *argv[] )
{
	  unsigned rv = 1;
    CU_pSuite suite = NULL;
 
    (void ) argc;
    (void ) argv;
    
    if( CUE_SUCCESS == CU_initialize_registry() ) {
        add_suites( &suite );

        if( NULL != suite ) {
            CU_basic_set_mode( CU_BRM_VERBOSE );
            CU_basic_run_tests();
            printf( "\n" );
            CU_basic_show_failures( CU_get_failure_list() );
            printf( "\n\n" );
            rv = CU_get_number_of_tests_failed();
        }

        CU_cleanup_registry();

    }

    return rv;
}


