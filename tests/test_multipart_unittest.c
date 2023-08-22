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
	char *subdoclist = NULL;
	int status=0;
	curl = curl_easy_init();
	CU_ASSERT_PTR_NOT_NULL(curl);
	createCurlHeader(list, &headers_list, status, &transID, &subdoclist);
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
	tmpData->status = strdup("success");
	tmpData->trans_id = 14464;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("none");
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
	CU_ASSERT_EQUAL(2,sts);
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

void test_checkRootDelete(){
	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	tmpData->name = strdup("moca");
	tmpData->version = 1234;
	tmpData->status = strdup("success");
	tmpData->trans_id = 4104;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("none");
	tmpData->next = NULL;
	set_global_tmp_node(tmpData);
	int m=checkRootDelete();
	CU_ASSERT_EQUAL(0,m);
	if(tmpData)
	{
		WEBCFG_FREE(tmpData->name);
		WEBCFG_FREE(tmpData->status);
		WEBCFG_FREE(tmpData->error_details);
		WEBCFG_FREE(tmpData);
	}

}

void test_checkRootDeleteFailure(){
	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	tmpData->name = strdup("privatessid");
	tmpData->version = 1234;
	tmpData->status = strdup("pending");
	tmpData->trans_id = 4104;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("failed");
	tmpData->next = NULL;
	set_global_tmp_node(tmpData);
	int m=checkRootDelete();
	CU_ASSERT_EQUAL(1,m);
	if(tmpData)
	{
		WEBCFG_FREE(tmpData->name);
		WEBCFG_FREE(tmpData->status);
		WEBCFG_FREE(tmpData->error_details);
		WEBCFG_FREE(tmpData);
	}
}
void test_updateRootVersionToDB(){
	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	tmpData->name = strdup("root");
	tmpData->version = 232323;
	tmpData->status = strdup("pending");
	tmpData->trans_id = 4104;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("none");
	tmpData->next = NULL;
	set_global_tmp_node(tmpData);
	set_global_ETAG("123");
	int m=updateRootVersionToDB();
	CU_ASSERT_EQUAL(0,m);
	if(tmpData)
	{
		WEBCFG_FREE(tmpData->name);
		WEBCFG_FREE(tmpData->status);
		WEBCFG_FREE(tmpData->error_details);
		WEBCFG_FREE(tmpData);
	}

}

void test_updateRootVersionToDBNoroot(){
	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	tmpData->name = strdup("wan");
	tmpData->version = 232323;
	tmpData->status = strdup("pending");
	tmpData->trans_id = 4104;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("none");
	tmpData->next = NULL;
	set_global_tmp_node(tmpData);
	set_global_ETAG("123");
	int m=updateRootVersionToDB();
	CU_ASSERT_EQUAL(0,m);
	if(tmpData)
	{
		WEBCFG_FREE(tmpData->name);
		WEBCFG_FREE(tmpData->status);
		WEBCFG_FREE(tmpData->error_details);
		WEBCFG_FREE(tmpData);
	}

}
void test_set_global_ETAG(){
	set_global_ETAG("1234");
	CU_ASSERT_STRING_EQUAL(get_global_ETAG(),"1234");
}

void test_get_global_ETAG(){
	printf("etag is %s\n", get_global_ETAG());
	CU_ASSERT_FATAL( NULL != get_global_ETAG() );
}

void test_set_global_transID(){
	set_global_transID("3344411a");
	CU_ASSERT_STRING_EQUAL(get_global_transID(),"3344411a");
}

void test_get_global_transID(){
	printf("transID is %s\n", get_global_transID());
	CU_ASSERT_FATAL( NULL != get_global_transID() );
}
void test_set_global_contentLen(){
	set_global_contentLen("1001");
	CU_ASSERT_STRING_EQUAL(get_global_contentLen(),"1001");
}

void test_get_global_contentLen(){
	printf("contentLen is %s\n", get_global_contentLen());
	CU_ASSERT_FATAL( NULL != get_global_contentLen() );
}
void test_set_global_eventFlag(){
	set_global_eventFlag(1);
	CU_ASSERT_EQUAL(get_global_eventFlag(),1);
}

void test_get_global_eventFlag(){
	printf("eventFlag is %d\n", get_global_eventFlag());
	CU_ASSERT_FATAL( 0 != get_global_eventFlag() );
}

void test_reset_global_eventFlag(){
	reset_global_eventFlag();
	CU_ASSERT_EQUAL(get_global_eventFlag(),0);
}

#ifdef WAN_FAILOVER_SUPPORTED
void test_set_global_interface(){
	set_global_interface("eth0");
	CU_ASSERT_STRING_EQUAL(get_global_interface(),"eth0");
}

void test_get_global_interface(){
	printf("interface is %s\n", get_global_interface());
	CU_ASSERT_FATAL( NULL != get_global_interface() );
}
#endif
void test_print_tmp_doc_list(){
	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	tmpData->name = strdup("wan");
	tmpData->version = 232323;
	tmpData->status = strdup("pending");
	tmpData->trans_id = 4104;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("none");
	tmpData->next = NULL;
	int m=print_tmp_doc_list(1);
	CU_ASSERT_EQUAL(0,m);
	if(tmpData)
	{
		WEBCFG_FREE(tmpData->name);
		WEBCFG_FREE(tmpData->status);
		WEBCFG_FREE(tmpData->error_details);
		WEBCFG_FREE(tmpData);
	}

}
void test_get_global_mp_null(){
	CU_ASSERT_FATAL( NULL == get_global_mp());
}

void test_get_global_mp(){
	multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
	multipartdocs->name_space = strdup("portforwarding");
	multipartdocs->data = (char* )malloc(64);
	multipartdocs->isSupplementarySync = 0;
	multipartdocs->next = NULL;
	set_global_mp(multipartdocs);
	CU_ASSERT_FATAL( NULL !=get_global_mp());
	if(multipartdocs)
	{
		WEBCFG_FREE(multipartdocs->name_space);
		WEBCFG_FREE(multipartdocs->data);
		multipartdocs->data_size = 0;
		WEBCFG_FREE(multipartdocs);
		set_global_mp(NULL);
	}
}

void test_deleteRootAndMultipartDocs(){
	multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
	multipartdocs->name_space = strdup("moca");
	multipartdocs->data = (char* )malloc(64);
	multipartdocs->isSupplementarySync = 0;
	multipartdocs->next = NULL;
	set_global_mp(multipartdocs);
	CU_ASSERT_FATAL( NULL !=get_global_mp());
	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	tmpData->name = strdup("moca");
	tmpData->version = 1234;
	tmpData->status = strdup("success");
	tmpData->trans_id = 4104;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("none");
	tmpData->next = NULL;
	set_global_tmp_node(tmpData);
	int m=deleteRootAndMultipartDocs();
	CU_ASSERT_EQUAL(0,m);
	set_global_tmp_node(NULL);
	set_global_mp(NULL);
}

void test_deleteRootAndMultipartDocs_fail(){
	multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
	multipartdocs->name_space = strdup("wan");
	multipartdocs->data = (char* )malloc(64);
	multipartdocs->isSupplementarySync = 0;
	multipartdocs->next = NULL;
	set_global_mp(multipartdocs);
	CU_ASSERT_FATAL( NULL !=get_global_mp());
	webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	tmpData->name = strdup("wan");
	tmpData->version = 3456;
	tmpData->status = strdup("pending");
	tmpData->trans_id = 1231;
	tmpData->retry_count = 0;
	tmpData->error_code = 0;
	tmpData->error_details = strdup("failed");
	tmpData->next = NULL;
	set_global_tmp_node(tmpData);
	int m=deleteRootAndMultipartDocs();
	CU_ASSERT_EQUAL(1,m);
	set_global_tmp_node(NULL);
	set_global_mp(NULL);
}

void test_deleteFromMpList(){
	multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
	multipartdocs->name_space = strdup("wan");
	multipartdocs->data = (char* )malloc(64);
	multipartdocs->isSupplementarySync = 0;
	multipartdocs->next = NULL;
	set_global_mp(multipartdocs);
	int m= deleteFromMpList("wan");
	CU_ASSERT_EQUAL(0,m);
	set_global_mp(NULL);
}

void test_deleteFromMpListFailure(){
	multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
	multipartdocs->name_space = strdup("wan");
	multipartdocs->data = (char* )malloc(64);
	multipartdocs->isSupplementarySync = 0;
	multipartdocs->next = NULL;
	set_global_mp(multipartdocs);
	int m= deleteFromMpList("moca");
	CU_ASSERT_EQUAL(1,m);
	set_global_mp(NULL);
}

void test_deleteFromMpListInvalidDoc(){
	multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
	multipartdocs->name_space = strdup("wan");
	multipartdocs->data = (char* )malloc(64);
	multipartdocs->isSupplementarySync = 0;
	multipartdocs->next = NULL;
	set_global_mp(multipartdocs);
	int m= deleteFromMpList(NULL);
	CU_ASSERT_EQUAL(1,m);
	set_global_mp(NULL);
}

void test_deleteFromMpList_2docs(){
	addToMpList(123, "wan", "data1", 10);
	addToMpList(1234, "moca", "data2", 20);
	int m= deleteFromMpList("moca");
	CU_ASSERT_EQUAL(0,m);
	set_global_mp(NULL);
}

void test_addToMpList(){
	addToMpList(123, "wan", "data1", 10);
        CU_ASSERT_STRING_EQUAL(get_global_mp()->name_space, "wan");
	CU_ASSERT_EQUAL(get_global_mp()->etag,123);
	addToMpList(1234, "moca", "data2", 20);
	CU_ASSERT_STRING_EQUAL(get_global_mp()->next->name_space, "moca");
	CU_ASSERT_EQUAL(get_global_mp()->next->etag,1234);
	delete_multipart();
	set_global_mp(NULL);
}

void test_delete_mp_doc(){
	addToMpList(123, "wan", "data1", 10);
	addToMpList(1234, "moca", "data2", 20);
	delete_mp_doc("moca");
	delete_mp_doc("wan");
	CU_ASSERT_FATAL( NULL == get_global_mp() );
}

void test_get_multipartdoc_count(){
	addToMpList(44, "wan", "data1", 10);
	addToMpList(555, "moca", "data2", 20);
	addToMpList(666, "privatessid", "data3", 20);
	addToMpList(7777, "lan", "data4", 30);
	addToMpList(1111, "mesh", "data5", 20);
	CU_ASSERT_EQUAL(5,get_multipartdoc_count());
	delete_multipart();
	CU_ASSERT_FATAL( NULL == get_global_mp() );
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
      CU_add_test( *suite, "test  generate_trans_uuid", test_generate_trans_uuid);
      CU_add_test( *suite, "test  replaceMacWord", test_replaceMac);  
      CU_add_test( *suite, "test  createCurlHeader", test_createHeader);
      CU_add_test( *suite, "test  validateParam", test_validateParam);
      CU_add_test( *suite, "test  checkRootUpdate", test_checkRootUpdate);
      CU_add_test( *suite, "test  checkRootDelete", test_checkRootDelete);
      CU_add_test( *suite, "test  checkRootDeleteFailure", test_checkRootDeleteFailure);
      CU_add_test( *suite, "test  updateRootVersionToDB", test_updateRootVersionToDB);
      CU_add_test( *suite, "test  updateRootVersionToDBNoroot", test_updateRootVersionToDBNoroot);
      CU_add_test( *suite, "test  set_global_ETAG", test_set_global_ETAG);
      CU_add_test( *suite, "test  get_global_ETAG", test_get_global_ETAG);
      CU_add_test( *suite, "test  set_global_transID", test_set_global_transID);
      CU_add_test( *suite, "test  get_global_transID", test_get_global_transID);
      CU_add_test( *suite, "test  set_global_contentLen", test_set_global_contentLen);
      CU_add_test( *suite, "test  get_global_contentLen", test_get_global_contentLen);
      CU_add_test( *suite, "test  set_global_eventFlag", test_set_global_eventFlag);
      CU_add_test( *suite, "test  get_global_eventFlag", test_get_global_eventFlag);
      CU_add_test( *suite, "test  reset_global_eventFlag", test_reset_global_eventFlag);
#ifdef WAN_FAILOVER_SUPPORTED
      CU_add_test( *suite, "test  set_global_interface", test_set_global_interface);
      CU_add_test( *suite, "test  get_global_interface", test_get_global_interface);
#endif
      CU_add_test( *suite, "test  print_tmp_doc_list", test_print_tmp_doc_list);
      CU_add_test( *suite, "test  get_global_mp_null", test_get_global_mp_null);
      CU_add_test( *suite, "test  get_global_mp", test_get_global_mp);
      CU_add_test( *suite, "test  deleteRootAndMultipartDocs", test_deleteRootAndMultipartDocs);
      CU_add_test( *suite, "test  deleteRootAndMultipartDocs_fail", test_deleteRootAndMultipartDocs_fail);
      CU_add_test( *suite, "test  checkDBList", test_checkDBList);
      CU_add_test( *suite, "test  updateDBlist", test_updateDBlist);
      CU_add_test( *suite, "test  pack append doc", test_appendedDoc);
#ifdef FEATURE_SUPPORT_AKER
      CU_add_test( *suite, "test  Aker update blob send", test_UpdateErrorsendAkerblob);
      CU_add_test( *suite, "test  Aker delete blob send", test_DeleteErrorsendAkerblob);
      CU_add_test( *suite, "test  Aker wait", test_akerWait);
#endif
      CU_add_test( *suite, "test  deleteFromMpList", test_deleteFromMpList);
      CU_add_test( *suite, "test  deleteFromMpListFailure", test_deleteFromMpListFailure);
      CU_add_test( *suite, "test  deleteFromMpListInvalidDoc", test_deleteFromMpListInvalidDoc);
      CU_add_test( *suite, "test  deleteFromMpList_2docs", test_deleteFromMpList_2docs);
      CU_add_test( *suite, "test  addToMpList", test_addToMpList);
      CU_add_test( *suite, "test  delete_mp_doc", test_delete_mp_doc);
      CU_add_test( *suite, "test  get_multipartdoc_count", test_get_multipartdoc_count);
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
