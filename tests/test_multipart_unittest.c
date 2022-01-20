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


#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <msgpack.h>
#include <curl/curl.h>
#include <base64.h>
#include <uuid/uuid.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <CUnit/Basic.h>
#include <wdmp-c.h>

#include "../src/webcfg_log.h"
#include "../src/webcfg_param.h"
#include "../src/webcfg.h"
#include "../src/webcfg_multipart.h"
#include "../src/webcfg_helpers.h"
#include "../src/webcfg_db.h"
#include "../src/webcfg_notify.h"
#include "../src/webcfg_metadata.h"
#include "../src/webcfg_generic.h"
#include "../src/webcfg_event.h"
#include "../src/webcfg_auth.h"
#include "../src/webcfg_blob.h"

#ifdef FEATURE_SUPPORT_AKER
#include "webcfg_aker.h"
#endif

int numLoops;

#define MAX_HEADER_LEN	4096
#define UNUSED(x) (void )(x)

char device_mac[32] = {'\0'};

char* get_deviceMAC()
{
	char *device_mac = strdup("b42xxxxxxxxx");	
	return device_mac;
}
char * getRebootReason()
{
	char *reason = strdup("factory-reset");
	return reason;
}

char *get_global_systemReadyTime()
{
	char *sTime = strdup("158000123");
	return sTime;
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
long long getRetryExpiryTimeout()
{
return 0;
}
int get_retry_timer()
{
	return 0;
}

void set_retry_timer(int value)
{
	UNUSED(value);	
	return;
}
char* printTime(long long time)
{
	UNUSED(time);
	return NULL;
}

long get_global_retry_timestamp()
{
    return 0;
}
void set_global_retry_timestamp(long value)
{
    	UNUSED(value);	
        return;
}
int updateRetryTimeDiff(long long expiry_time)
{
	UNUSED(expiry_time);
	return 0;
}
int checkRetryTimer( long long timestamp)
{
	UNUSED(timestamp);
	return 0;
}
void initMaintenanceTimer()
{	
	return;
}
int checkMaintenanceTimer()
{
	return 0;
}
int getMaintenanceSyncSeconds(int maintenance_count)
{
	UNUSED(maintenance_count);
	return 0;
}
int retrySyncSeconds()
{
	return 0;
}
void set_global_maintenance_time(long value)
{
	UNUSED(value);    
	return;
}
	


void test_generate_trans_uuid(){

	char *transaction_uuid = NULL;
	transaction_uuid = generate_trans_uuid();
	CU_ASSERT_FATAL( NULL != transaction_uuid);
}


void test_replaceMac(){
	
	char *configURL= "https://config/{sss}/com";
	char c[] = "{sss}";
	char *webConfigURL = replaceMacWord(configURL, c, get_deviceMAC());
	CU_ASSERT_STRING_EQUAL(webConfigURL,"https://config/b42xxxxxxxxx/com");
	
	
}

void test_createHeader(){
	
	CURL *curl;
	struct curl_slist *list = NULL;
	struct curl_slist *headers_list = NULL;
	char *transID = NULL;
	int status=0;
	curl = curl_easy_init();
	CU_ASSERT_PTR_NOT_NULL(curl);
	createCurlHeader(list, &headers_list, status, &transID);
	CU_ASSERT_PTR_NOT_NULL(transID);
	CU_ASSERT_PTR_NOT_NULL(headers_list);		
}


void test_validateParam()
{	param_t *reqParam = NULL;
	int paramCount = 1;
 	reqParam = (param_t *) malloc(sizeof(param_t) * paramCount);
	reqParam[0].name = strdup("Device.X_RDK_WebConfig.RootConfig.Data");
	reqParam[0].value = strdup("true");
	reqParam[0].type = 2;
	int n=validate_request_param(reqParam, paramCount);
	CU_ASSERT_EQUAL(0,n);
	reqParam_destroy(paramCount, reqParam);
	
}

void test_checkRootUpdate(){
	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	tmpData->name = strdup("wan");
	tmpData->version = 410448631;
	tmpData->status = strdup("ACK");
	tmpData->trans_id = 14464;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("success");
	tmpData->next = NULL;
	set_global_tmp_node(tmpData);
	int m=checkRootUpdate();
	CU_ASSERT_EQUAL(0,m);
	if(tmpData)
	{
		WEBCFG_FREE(tmpData->name);
		WEBCFG_FREE(tmpData->status);
		WEBCFG_FREE(tmpData->error_details);
		WEBCFG_FREE(tmpData);
	}

}


void test_checkDBList(){
	char *root_str = strdup("factory-reset");
	uint32_t root_version = 1234;
	webconfig_db_data_t * webcfgdb = NULL;
  	webcfgdb = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
	checkDBList("root", root_version, root_str);
	memset( webcfgdb, 0, sizeof( webconfig_db_data_t ) );
	CU_ASSERT_PTR_NOT_NULL(webcfgdb );
	WEBCFG_FREE(webcfgdb);
}
	
void test_updateDBlist(){
	char *rootstr = strdup("factory-reset");
	uint32_t version = 1234;
	int sts = updateDBlist("root", version, rootstr);
	CU_ASSERT_EQUAL(0,sts);
}

void test_appendedDoc(){
	size_t appenddocPackSize = -1;
	size_t embeddeddocPackSize = -1;
	void *appenddocdata = NULL;
	void *embeddeddocdata = NULL;
	appenddoc_t *appenddata = NULL;
        char * finaldocdata = NULL;

	//blob data
	webconfig_db_data_t *blob_data;
    	blob_data = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
	blob_data->name = strdup("wan");
	blob_data->version = 410448631;
	blob_data->root_string = strdup("portmapping");
	blob_data->next=NULL;
  	size_t blob_size=175;

	//append data
	appenddata = (appenddoc_t *) malloc(sizeof(appenddoc_t ));
	CU_ASSERT_PTR_NOT_NULL(appenddata);
	memset(appenddata, 0, sizeof(appenddoc_t));
        appenddata->subdoc_name = strdup("Device.Data");
        appenddata->version = 410448631;
	appenddata->transaction_id =22334;

	//pack append doc
	appenddocPackSize = webcfg_pack_appenddoc(appenddata, &appenddocdata);
	CU_ASSERT_FATAL( 0 != appenddocPackSize);
	WEBCFG_FREE(appenddata->subdoc_name);
    	WEBCFG_FREE(appenddata);
	
	//encode data
        embeddeddocPackSize = appendWebcfgEncodedData(&embeddeddocdata, (void *)blob_data, blob_size, appenddocdata, appenddocPackSize);
	CU_ASSERT_FATAL( 0 != embeddeddocPackSize);
	WEBCFG_FREE(appenddocdata);

	//base64 encode
        finaldocdata = base64blobencoder((char *)embeddeddocdata, embeddeddocPackSize);
	CU_ASSERT_FATAL( NULL != finaldocdata);
	WEBCFG_FREE(embeddeddocdata);
        
}

#ifdef FEATURE_SUPPORT_AKER

void test_UpdateErrorsendAkerblob(){
	WDMP_STATUS ret = WDMP_FAILURE;
	char *paramName= strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_Aker.Update");
	char *blob= strdup("true");;
	uint32_t blobSize =2;
	uint16_t docTransId=12345; 
	int version=2.0;
	ret = send_aker_blob(paramName,blob,blobSize,docTransId,version);
	CU_ASSERT_EQUAL(1, ret);

}
void test_DeleteErrorsendAkerblob(){
	WDMP_STATUS ret = WDMP_FAILURE;
	char *paramName= strdup("Device.DeviceInfo.X_RDKCENTRAL-COM_Aker.Delete");
	char *blob= strdup("true");;
	uint32_t blobSize =2;
	uint16_t docTransId=12345; 
	int version=2.0;
	ret = send_aker_blob(paramName,blob,blobSize,docTransId,version);
	CU_ASSERT_EQUAL(1, ret);

}
void test_akerWait(){
	
	int backoffRetryTime = 0;
	int c=2;
	backoffRetryTime = (int) pow(2, c) -1;
	int wait=akerwait__ (backoffRetryTime);
	CU_ASSERT_EQUAL(0,wait);
}
#endif
void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
      CU_add_test( *suite, "test  generate_trans_uuid", test_generate_trans_uuid); 
      CU_add_test( *suite, "test  replaceMacWord", test_replaceMac);  
      CU_add_test( *suite, "test  createCurlHeader", test_createHeader);
      CU_add_test( *suite, "test  validateParam", test_validateParam);
      CU_add_test( *suite, "test  checkRootUpdate", test_checkRootUpdate);
      CU_add_test( *suite, "test  checkDBList", test_checkDBList);
      CU_add_test( *suite, "test  updateDBlist", test_updateDBlist);
      CU_add_test( *suite, "test  pack append doc", test_appendedDoc);
#ifdef FEATURE_SUPPORT_AKER
      CU_add_test( *suite, "test  Aker update blob send", test_UpdateErrorsendAkerblob);
      CU_add_test( *suite, "test  Aker delete blob send", test_DeleteErrorsendAkerblob);
      CU_add_test( *suite, "test  Aker wait", test_akerWait);
#endif
      
     
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
