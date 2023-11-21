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
#include <unistd.h>
#include <CUnit/Basic.h>
#include "../src/webcfg_param.h"
#include "../src/webcfg.h"
#include "../src/webcfg_multipart.h"
#include "../src/webcfg_helpers.h"
#include "../src/webcfg_db.h"
#include "../src/webcfg_notify.h"
#include <msgpack.h>
#include <curl/curl.h>
#include <base64.h>
#include "../src/webcfg_generic.h"
#include "../src/webcfg_event.h"
#define FILE_URL "/tmp/webcfg_url"

#define UNUSED(x) (void )(x)

char *url = NULL;
char *interface = NULL;
char device_mac[32] = {'\0'};
int numLoops;

char* get_deviceMAC()
{
	strcpy(device_mac, "b42xxxxxxxxx");
	return device_mac;
}
void setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus)
{
	UNUSED(paramVal);
	UNUSED(paramCount);
	UNUSED(setType);
	UNUSED(transactionId);
	UNUSED(timeSpan);
	UNUSED(ccspStatus);
	*retStatus = WDMP_SUCCESS;
	return;
}

void setAttributes(param_t *attArr, const unsigned int paramCount, money_trace_spans *timeSpan, WDMP_STATUS *retStatus)
{
	UNUSED(attArr);
	UNUSED(paramCount);
	UNUSED(timeSpan);
	*retStatus = WDMP_SUCCESS;
	return;
}

int getForceSync(char** pString, char **transactionId)
{
	UNUSED(pString);
	UNUSED(transactionId);
	return 0;
}
int setForceSync(char* pString, char *transactionId,int *session_status)
{
	UNUSED(pString);
	UNUSED(transactionId);
	UNUSED(session_status);
	return 0;
}

char * getParameterValue(char *paramName)
{
	UNUSED(paramName);
	return NULL;
}

char * getSerialNumber()
{
	char *sNum = strdup("1234");
	return sNum;
}

char * getDeviceBootTime()
{
	char *bTime = strdup("152200345");
	return bTime;
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

char * getConnClientParamName()
{
	char *pName = strdup("ConnClientParamName");
	return pName;
}
char * getFirmwareVersion()
{
	char *fName = strdup("Firmware.bin");
	return fName;
}

char * getRebootReason()
{
	char *reason = strdup("factory-reset");
	return reason;
}

void sendNotification(char *payload, char *source, char *destination)
{
	WEBCFG_FREE(payload);
	WEBCFG_FREE(source);
	UNUSED(destination);
	return;
}

char *get_global_systemReadyTime()
{
	char *sTime = strdup("158000123");
	return sTime;
}

int Get_Webconfig_URL( char *pString)
{
	char *webConfigURL =NULL;
	loadInitURLFromFile(&webConfigURL);
	pString = webConfigURL;
        printf("The value of pString is %s\n",pString);
	return 0;
}

int Set_Webconfig_URL( char *pString)
{
	printf("Set_Webconfig_URL pString %s\n", pString);
	return 0;
}

int registerWebcfgEvent(WebConfigEventCallback webcfgEventCB)
{
	UNUSED(webcfgEventCB);
	return 0;
}

int unregisterWebcfgEvent()
{
	return 0;
}
/*----------------------------------------------------------------------------*/
/*                             Test Functions                             */
/*----------------------------------------------------------------------------*/

void test_get_global_event_con()
{
    pthread_cond_t test_event_con = PTHREAD_COND_INITIALIZER;
    pthread_cond_t *result = get_global_event_con();
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_FATAL(pthread_cond_signal (get_global_event_con()) == 0);
    CU_ASSERT_FATAL(memcmp(result, &test_event_con, sizeof(pthread_cond_t)) == 0);
}

void test_get_global_event_mut()
{
    pthread_mutex_t test_event_mut = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t *result = get_global_event_mut();
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_FATAL(memcmp(result, &test_event_mut, sizeof(pthread_mutex_t)) == 0);
}

void test_get_global_process_threadid()
{
    pthread_t result = get_global_process_threadid();
    CU_ASSERT_EQUAL(result, 0);
}

void test_get_global_event_threadid()
{
    pthread_t result = get_global_event_threadid();
    CU_ASSERT_EQUAL(result, 0);
}
void test_getDocVersionFromTmpList()
{
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("moca");
    tmpData->version = 1234;
    tmpData->status = strdup("success");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    tmpData->next = NULL;
    CU_ASSERT_PTR_NOT_NULL(tmpData);
    CU_ASSERT_EQUAL(getDocVersionFromTmpList(tmpData,"moca"),1234);
    CU_ASSERT_EQUAL(getDocVersionFromTmpList(tmpData,"wan"),0);
    if(tmpData)
    {
        WEBCFG_FREE(tmpData->name);
        tmpData->version = 0;
        WEBCFG_FREE(tmpData->status);
        tmpData->trans_id = 0;
        WEBCFG_FREE(tmpData->error_details);
        WEBCFG_FREE(tmpData);
    }
    CU_ASSERT_PTR_NULL(tmpData);
}

void test_free_event_params_struct()
{
    event_params_t *param = (event_params_t *)malloc(sizeof(event_params_t));
    param->subdoc_name = strdup("moca");
    param->status = strdup("success");
    param->process_name = strdup("unknown");
    param->failure_reason = strdup("none");
    CU_ASSERT_PTR_NOT_NULL(param);
    free_event_params_struct(param);
    if(param)
    {
        param = NULL;
    }
    CU_ASSERT_PTR_NULL(param);
}

void test_checkDBversion()
{
    int m = get_successDocCount();
    CU_ASSERT_EQUAL(0,m);
    webconfig_db_data_t *wd;
    wd = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    CU_ASSERT_PTR_NOT_NULL(wd);
    wd->name = strdup("wan");
    wd->version = 410448631;
    wd->root_string = strdup("portmapping");
    wd->next=NULL;
    addToDBList(wd);
    int n = get_successDocCount();
    CU_ASSERT_EQUAL(1,n);
    CU_ASSERT_FATAL( NULL != get_global_db_node());
    int a = checkDBVersion("wan",410448631);
    CU_ASSERT_EQUAL(0,a);
    int b = checkDBVersion("moca",410448631);
    CU_ASSERT_EQUAL(1,b);
    if(wd)
    {
        WEBCFG_FREE(wd->name);
        wd->version = 0;
        WEBCFG_FREE(wd->root_string);
        WEBCFG_FREE(wd); 
    }
    reset_successDocCount();
    reset_db_node();
    int p = get_successDocCount();
    CU_ASSERT_EQUAL(0,p);
    CU_ASSERT_FATAL( NULL == get_global_db_node());
}

void test_updateTimerList()
{
    expire_timer_t *new_node = NULL;
    new_node=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(new_node);
    memset( new_node, 0, sizeof( expire_timer_t ) );
    
    new_node->running = true;
    new_node->subdoc_name = strdup("moca");
    new_node->txid = 1234;
    new_node->timeout = 120;
    new_node->next=NULL;
    
    int m = updateTimerList(new_node,true,"moca",1234,120);
    CU_ASSERT_EQUAL(0,m);
    
    int n = updateTimerList(new_node,true,"wan",1234,120);
    CU_ASSERT_EQUAL(1,n);
    
    if(new_node)
    {
        WEBCFG_FREE(new_node->subdoc_name);
        WEBCFG_FREE(new_node);
    }
}

void test_deleteFromTimerList()
{
    int count = 0;
    expire_timer_t *new_node = NULL;
    new_node=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(new_node);
    memset( new_node, 0, sizeof( expire_timer_t ) );
    
    new_node->running = true;
    new_node->subdoc_name = strdup("moca");
    new_node->txid = 1234;
    new_node->timeout = 120;
    new_node->next=NULL;
    count++;

    expire_timer_t *temp = NULL;
    temp=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(temp);
    memset( temp, 0, sizeof( expire_timer_t ) );
    
    temp->running = true;
    temp->subdoc_name = strdup("wan");
    temp->txid = 4567;
    temp->timeout = 60;
    temp->next=new_node;
    count++;
    
    set_global_timer_node(temp);
    set_numOfEvents(count);
    CU_ASSERT_FATAL( NULL != get_global_timer_node());
    
    int m = deleteFromTimerList("moca");
    CU_ASSERT_EQUAL(0,m);

    int n = deleteFromTimerList("wan");
    CU_ASSERT_EQUAL(0,n);
    
    int a = deleteFromTimerList("lan");
    CU_ASSERT_EQUAL(1,a);
   
    int b = deleteFromTimerList(NULL);
    CU_ASSERT_EQUAL(1,b);

    CU_ASSERT_FATAL( NULL == get_global_timer_node());
}

void test_stopWebcfgTimer()
{
    int count = 0;
    expire_timer_t *temp = NULL;
    temp=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(temp);
    memset( temp, 0, sizeof( expire_timer_t ) );
    
    temp->running = true;
    temp->subdoc_name = strdup("wan");
    temp->txid = 4567;
    temp->timeout = 60;
    temp->next=NULL;
    count++;
    
    set_global_timer_node(temp);
    set_numOfEvents(count);
    CU_ASSERT_FATAL( NULL != get_global_timer_node());
    
    int m = stopWebcfgTimer(temp,"moca",4567);
    CU_ASSERT_EQUAL(1,m);

    int n = stopWebcfgTimer(temp,"wan",4567);
    CU_ASSERT_EQUAL(0,n);

    int p = stopWebcfgTimer(NULL,"moca",1234);
    CU_ASSERT_EQUAL(1,p);

}

void test_get_global_timer_node()
{
    CU_ASSERT_FATAL( NULL == get_global_timer_node());
    expire_timer_t *new_node = NULL;
    new_node=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(new_node);
    memset( new_node, 0, sizeof( expire_timer_t ) );
    
    new_node->running = true;
    new_node->subdoc_name = strdup("moca");
    new_node->txid = 1234;
    new_node->timeout = 120;
    new_node->next=NULL;
    
    set_global_timer_node(new_node);
    CU_ASSERT_FATAL( NULL != get_global_timer_node());
    if(new_node)
    {
        WEBCFG_FREE(new_node->subdoc_name);
        WEBCFG_FREE(new_node);
    }
    set_global_timer_node(new_node);
    CU_ASSERT_FATAL( NULL == get_global_timer_node());
}

void test_getTimerNode()
{
    CU_ASSERT_FATAL( NULL == get_global_timer_node());
    int count = 0;
    expire_timer_t *new_node = NULL;
    new_node=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(new_node);
    memset( new_node, 0, sizeof( expire_timer_t ) );
    
    new_node->running = true;
    new_node->subdoc_name = strdup("moca");
    new_node->txid = 1234;
    new_node->timeout = 120;
    new_node->next=NULL;
    count++;

    expire_timer_t *temp = NULL;
    temp=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(temp);
    memset( temp, 0, sizeof( expire_timer_t ) );
    
    temp->running = true;
    temp->subdoc_name = strdup("wan");
    temp->txid = 4567;
    temp->timeout = 60;
    temp->next=new_node;
    count++;
    
    set_global_timer_node(temp);
    set_numOfEvents(count);
    CU_ASSERT_FATAL( NULL != get_global_timer_node());

    CU_ASSERT_FATAL( NULL != getTimerNode("moca"));
    CU_ASSERT_FATAL( NULL != getTimerNode("wan"));
    CU_ASSERT_FATAL( NULL == getTimerNode("lan"));
    
    int m = deleteFromTimerList("moca");
    CU_ASSERT_EQUAL(0,m);

    int n = deleteFromTimerList("wan");
    CU_ASSERT_EQUAL(0,n);
    
    CU_ASSERT_FATAL( NULL == get_global_timer_node());
}

void test_startWebcfgTimer()
{
    int count = 0;
    expire_timer_t *new_node = NULL;
    new_node=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(new_node);
    memset( new_node, 0, sizeof( expire_timer_t ) );
    
    new_node->running = false;
    new_node->subdoc_name = strdup("moca");
    new_node->txid = 1234;
    new_node->timeout = 120;
    new_node->next=NULL;
    count++;
    
    set_global_timer_node(new_node);
    set_numOfEvents(count);
    CU_ASSERT_FATAL( NULL != get_global_timer_node());

    int m = startWebcfgTimer(new_node,"moca",3456,60);
    CU_ASSERT_EQUAL(0,m);

    int n = startWebcfgTimer(new_node,"wan",6789,80);
    CU_ASSERT_EQUAL(0,n);
    
    int a = deleteFromTimerList("moca");
    CU_ASSERT_EQUAL(0,a);

    int b = deleteFromTimerList("wan");
    CU_ASSERT_EQUAL(0,b);
    
    CU_ASSERT_FATAL( NULL == get_global_timer_node());
    
}

void test_checkTimerExpired()
{
    int count = 0;
    char *expired_doc= NULL;
    expire_timer_t *new_node = NULL;
    new_node=(expire_timer_t *)malloc(sizeof(expire_timer_t));
    CU_ASSERT_PTR_NOT_NULL(new_node);
    memset( new_node, 0, sizeof( expire_timer_t ) );
    
    new_node->running = true;
    new_node->subdoc_name = strdup("moca");
    new_node->txid = 1234;
    new_node->timeout = 0;
    new_node->next=NULL;
    count++;
    
    set_global_timer_node(new_node);
    set_numOfEvents(count);
    CU_ASSERT_FATAL( NULL != get_global_timer_node());
    
    int m = checkTimerExpired(&expired_doc);
    CU_ASSERT_EQUAL(1,m);
     
    if(new_node)
    {
        WEBCFG_FREE(new_node->subdoc_name);
        WEBCFG_FREE(new_node);
        expired_doc= NULL;
    }
    set_global_timer_node(new_node);
    int n = checkTimerExpired(&expired_doc);
    CU_ASSERT_EQUAL(0,n);
    CU_ASSERT_FATAL( NULL == get_global_timer_node());
}

void test_validateEvent()
{
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("moca");
    tmpData->version = 1234;
    tmpData->status = strdup("success");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    tmpData->next = NULL;
    CU_ASSERT_PTR_NOT_NULL(tmpData);

    int m = validateEvent(tmpData,"moca",4104);
    CU_ASSERT_EQUAL(0,m);

    if(tmpData)
    {
        WEBCFG_FREE(tmpData->name);
        tmpData->version = 0;
        WEBCFG_FREE(tmpData->status);
        tmpData->trans_id = 0;
        WEBCFG_FREE(tmpData->error_details);
        WEBCFG_FREE(tmpData);
    }
    CU_ASSERT_PTR_NULL(tmpData);
    int n = validateEvent(tmpData,"moca",4104);
    CU_ASSERT_EQUAL(1,n);
}

void test_parseEventData()
{
    event_params_t *eventParam = NULL;
    char *data = (char *)malloc(128 * sizeof(char));
    CU_ASSERT_PTR_NOT_NULL(data);
    snprintf(data,128*sizeof(char),"%s,%hu,%u,EXPIRE,%u","moca",4104,0,0);
    
    int m = parseEventData(data,&eventParam);
    CU_ASSERT_EQUAL(0,m);
    if(data)
    {
        data = NULL;
    }
    int n = parseEventData(data,&eventParam);
    CU_ASSERT_EQUAL(1,n);
    free_event_params_struct(eventParam);
}
void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "test get_global_event_con",test_get_global_event_con);
    CU_add_test( *suite, "test get_global_event_mut",test_get_global_event_mut);
    CU_add_test( *suite, "test get_global_process_threadid",test_get_global_process_threadid);
    CU_add_test( *suite, "test get_global_event_threadid",test_get_global_event_threadid);
    CU_add_test( *suite, "test getDocVersionFromTmpList",test_getDocVersionFromTmpList);
    CU_add_test( *suite, "test free_event_params_struct",test_free_event_params_struct);
    CU_add_test( *suite, "test checkDBVersion",test_checkDBversion);
    CU_add_test( *suite, "test updateTimerList",test_updateTimerList);
    CU_add_test( *suite, "test deleteFromTimerList",test_deleteFromTimerList);
    CU_add_test( *suite, "test stopWebcfgTimer",test_stopWebcfgTimer);
    CU_add_test( *suite, "test get_global_timer_node",test_get_global_timer_node);
    CU_add_test( *suite, "test getTimerNode",test_getTimerNode);
    CU_add_test( *suite, "test startWebcfgTimer",test_startWebcfgTimer);
    CU_add_test( *suite, "test checkTimerExpired",test_checkTimerExpired);
    CU_add_test( *suite, "test validateEvent",test_validateEvent);
    CU_add_test( *suite, "test parseEventData",test_parseEventData);
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( )
{
	unsigned rv = 1;
	CU_pSuite suite = NULL;
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

