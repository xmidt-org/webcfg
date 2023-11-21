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
#include "../src/webcfg_db.h"
#include "../src/webcfg_pack.h"
#include "../src/webcfg_helpers.h"
#include "../src/webcfg_param.h"
#include "../src/webcfg_multipart.h"
#include "../src/webcfg_timer.h"
#include "../src/webcfg_notify.h"
#define UNUSED(x) (void )(x)
//mock functions
/*
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
*/

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
}/*
void setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus)
{
	UNUSED(paramVal);
	UNUSED(paramCount);
	UNUSED(setType);
	UNUSED(transactionId);
	UNUSED(timeSpan);
	UNUSED(retStatus);
	UNUSED(ccspStatus);
	return;
}


void sendNotification(char *payload, char *source, char *destination)
{
	WEBCFG_FREE(payload);
	WEBCFG_FREE(source);
	UNUSED(destination);
	return;
}

WDMP_STATUS mapStatus(int ret)
{
	UNUSED(ret);
	return 0;
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

void processAkerSubdoc(){
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
}*/


void retryMultipartSubdoc(){
	return ;
}/*
char *get_global_systemReadyTime()
{
	char *sTime = strdup("158000123");
	return sTime;
}

char * getRebootReason()
{
	char *reason = strdup("factory-reset");
	return reason;
}*/

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

/*
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
}*/

/*----------------------------------------------------------------------------*/
/*                             Test Functions                             */
/*----------------------------------------------------------------------------*/

void test_reset_numOfMpDocs()
{
    multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
    multipartdocs->name_space = strdup("moca");
    multipartdocs->data = (char* )malloc(64);
    multipartdocs->isSupplementarySync = 1;
    multipartdocs->next = NULL;
    set_global_mp(multipartdocs);
    CU_ASSERT_FATAL( NULL != get_global_mp());
    int m = addToTmpList();
    CU_ASSERT_EQUAL(0,m);
    int n = get_numOfMpDocs();
    CU_ASSERT_EQUAL(1,n);
    reset_numOfMpDocs();
    int t = get_numOfMpDocs();
    CU_ASSERT_EQUAL(0,t);
    if(multipartdocs)
    {
        WEBCFG_FREE(multipartdocs->name_space);
        WEBCFG_FREE(multipartdocs->data);
        multipartdocs->data_size = 0;
        WEBCFG_FREE(multipartdocs);
    }
    delete_tmp_list();
    //set_global_tmp_node(NULL); //optional
    set_global_mp(NULL);
}

void test_delete_tmp_list()
{
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("moca");
    tmpData->version = 1234;
    tmpData->status = strdup("success");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    //tmpData->next = NULL;
    webconfig_tmp_data_t *tmp = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp->name = strdup("wan");
    tmp->version = 5678;
    tmp->status = strdup("success");
    tmp->trans_id = 4204;
    tmp->retry_count = 0;
    tmp->error_code = 0;
    tmp->error_details = strdup("none");
    tmp->next = NULL;
    tmpData->next = tmp;
    set_global_tmp_node(tmpData);
    CU_ASSERT_FATAL( NULL != get_global_tmp_node());
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
    //set_global_tmp_node(NULL);
}

void test_get_numOfMpDocs()
{
    multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
    multipartdocs->name_space = strdup("moca");
    multipartdocs->data = (char* )malloc(64);
    multipartdocs->isSupplementarySync = 1;
    multipartdocs->next = NULL;
    set_global_mp(multipartdocs);
    CU_ASSERT_FATAL( NULL !=get_global_mp());    
    int m = addToTmpList();
    CU_ASSERT_EQUAL(0,m);
    int n = get_numOfMpDocs(); 
    CU_ASSERT_EQUAL(1,n);
    if(multipartdocs)
    {
        WEBCFG_FREE(multipartdocs->name_space);
        WEBCFG_FREE(multipartdocs->data);
        multipartdocs->data_size = 0;
        WEBCFG_FREE(multipartdocs);
    }
    delete_tmp_list();
    //set_global_tmp_node(NULL);
    set_global_mp(NULL);
}

void test_addToTmpList() 
{
    multipartdocs_t *multipartdocs = (multipartdocs_t *)malloc(sizeof(multipartdocs_t));
    multipartdocs->name_space = strdup("moca");
    multipartdocs->data = (char* )malloc(64);
    multipartdocs->isSupplementarySync = 1;
    multipartdocs->next = NULL;
    set_global_mp(multipartdocs);
    CU_ASSERT_FATAL( NULL !=get_global_mp());
    int m = addToTmpList();
    CU_ASSERT_EQUAL(0,m);
    CU_ASSERT_FATAL( NULL != get_global_tmp_node());
    if(multipartdocs)
    {
        WEBCFG_FREE(multipartdocs->name_space);
        WEBCFG_FREE(multipartdocs->data);
        multipartdocs->data_size = 0;
        WEBCFG_FREE(multipartdocs);
    }
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
    //set_global_tmp_node(NULL);
    set_global_mp(NULL);
}

void test_set_global_tmp_node()
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
    set_global_tmp_node(tmpData);
    CU_ASSERT_FATAL( NULL != get_global_tmp_node());
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}

void test_get_global_tmp_node()
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
    set_global_tmp_node(tmpData);
    CU_ASSERT_FATAL( NULL != get_global_tmp_node());
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}

void test_getTmpNode()
{
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("root");
    tmpData->version = 232323;
    tmpData->status = strdup("pending");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    tmpData->next = NULL;
    webconfig_tmp_data_t *tmp = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp->name = strdup("privatessid");
    tmp->version = 1234;
    tmp->status = strdup("pending");
    tmp->trans_id = 4204;
    tmp->retry_count = 0;
    tmp->error_code = 0;
    tmp->error_details = strdup("none");
    tmp->next = NULL;
    tmpData->next = tmp;
    webconfig_tmp_data_t *tmp1 = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp1->name = strdup("moca");
    tmp1->version = 5678;
    tmp1->status = strdup("success");
    tmp1->trans_id = 4304;
    tmp1->retry_count = 0;
    tmp1->error_code = 0;
    tmp1->error_details = strdup("none");
    tmp1->next = NULL;
    tmp->next = tmp1;
    set_global_tmp_node(tmpData);
    CU_ASSERT_FATAL( NULL != getTmpNode("moca"));
    CU_ASSERT_FATAL( NULL != getTmpNode("privatessid"));
    CU_ASSERT_FATAL( NULL == getTmpNode("wan"));
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}

void test_updateTmpList()
{
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("root");
    tmpData->version = 232323;
    tmpData->status = strdup("pending");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    tmpData->next = NULL;
    webconfig_tmp_data_t *tmp = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp->name = strdup("privatessid");
    tmp->version = 1234;
    tmp->status = strdup("pending");
    tmp->trans_id = 4204;
    tmp->retry_count = 0;
    tmp->error_code = 0;
    tmp->error_details = strdup("none");
    tmp->next = NULL;
    tmpData->next = tmp;
    webconfig_tmp_data_t *tmp1 = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp1->name = strdup("moca");
    tmp1->version = 5678;
    tmp1->status = strdup("failed");
    tmp1->trans_id = 4304;
    tmp1->retry_count = 0;
    tmp1->error_code = 0;
    tmp1->error_details = strdup("none");
    tmp1->next = NULL;
    tmp->next = tmp1;
    set_global_tmp_node(tmpData);
    webconfig_tmp_data_t * root_node = NULL;
    if(( root_node = getTmpNode("moca") ))
    {
        int m = updateTmpList(root_node, "moca", 6789, "success", "none", 0, 0, 0); //update version & status
        CU_ASSERT_EQUAL(0,m);
    }
    if(( root_node = getTmpNode("privatessid") ))
    {
        int m = updateTmpList(root_node, "privatessid", 2345, "success", "none", 0, 0, 0); //update version & status
        CU_ASSERT_EQUAL(0,m);
    }
    CU_ASSERT_FATAL( NULL == getTmpNode("wan"));
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}

void test_deleteFromTmpList()
{
    int count = 0;
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("root");
    tmpData->version = 232323;
    tmpData->status = strdup("pending");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    tmpData->next = NULL;
    count++;
    webconfig_tmp_data_t *tmp = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp->name = strdup("privatessid");
    tmp->version = 1234;
    tmp->status = strdup("pending");
    tmp->trans_id = 4204;
    tmp->retry_count = 0;
    tmp->error_code = 0;
    tmp->error_details = strdup("none");
    tmp->next = NULL;
    tmpData->next = tmp;
    count++;
    webconfig_tmp_data_t *tmp1 = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp1->name = strdup("moca");
    tmp1->version = 5678;
    tmp1->status = strdup("success");
    tmp1->trans_id = 4304;
    tmp1->retry_count = 0;
    tmp1->error_code = 0;
    tmp1->error_details = strdup("none");
    tmp1->next = NULL;
    tmp->next = tmp1;
    count++;
    set_global_tmp_node(tmpData);
    set_numOfMpDocs(count);
    webconfig_tmp_data_t * root_node = NULL, *next_node = NULL;
    int m = deleteFromTmpList(NULL, &next_node);
    CU_ASSERT_EQUAL(1,m);
    if(( root_node = getTmpNode("privatessid") ))
    {
        int m = deleteFromTmpList(root_node->name, &next_node);
        CU_ASSERT_EQUAL(0,m);
        next_node = NULL;   
    }
    int n = deleteFromTmpList("wan", &next_node);
    CU_ASSERT_EQUAL(1,n);
    reset_numOfMpDocs();
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}

void test_delete_tmp_docs_list()
{
    int count = 0;
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("root");
    tmpData->version = 232323;
    tmpData->status = strdup("pending");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    tmpData->next = NULL;
    count++;
    webconfig_tmp_data_t *tmp = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp->name = strdup("privatessid");
    tmp->version = 1234;
    tmp->status = strdup("pending");
    tmp->trans_id = 4204;
    tmp->retry_count = 0;
    tmp->error_code = 0;
    tmp->error_details = strdup("none");
    tmp->next = NULL;
    tmpData->next = tmp;
    count++;
    webconfig_tmp_data_t *tmp1 = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp1->name = strdup("moca");
    tmp1->version = 5678;
    tmp1->status = strdup("success");
    tmp1->trans_id = 4304;
    tmp1->retry_count = 0;
    tmp1->error_code = 0;
    tmp1->error_details = strdup("none");
    tmp1->next = NULL;
    tmp->next = tmp1;
    count++;
    set_global_tmp_node(tmpData);
    set_numOfMpDocs(count);
    CU_ASSERT_FATAL( NULL != getTmpNode("moca"));
    CU_ASSERT_FATAL( NULL != getTmpNode("privatessid"));
    delete_tmp_docs_list();
    CU_ASSERT_FATAL( NULL == getTmpNode("moca"));
    CU_ASSERT_FATAL( NULL == getTmpNode("privatessid"));
    reset_numOfMpDocs();
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}


void test_release_success_docs_tmplist()
{
    int count = 0;
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("root");
    tmpData->version = 232323;
    tmpData->status = strdup("pending");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    tmpData->next = NULL;
    count++;
    webconfig_tmp_data_t *tmp = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp->name = strdup("privatessid");
    tmp->version = 1234;
    tmp->status = strdup("success");
    tmp->trans_id = 4204;
    tmp->retry_count = 0;
    tmp->error_code = 0;
    tmp->error_details = strdup("none");
    tmp->next = NULL;
    tmpData->next = tmp;
    count++;
    webconfig_tmp_data_t *tmp1 = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp1->name = strdup("moca");
    tmp1->version = 5678;
    tmp1->status = strdup("success");
    tmp1->trans_id = 4304;
    tmp1->retry_count = 0;
    tmp1->error_code = 0;
    tmp1->error_details = strdup("none");
    tmp1->next = NULL;
    tmp->next = tmp1;
    count++;
    set_global_tmp_node(tmpData);
    set_numOfMpDocs(count);
    release_success_docs_tmplist(); //to release success docs
    CU_ASSERT_FATAL( NULL == getTmpNode("moca"));
    CU_ASSERT_FATAL( NULL == getTmpNode("privatessid"));
    CU_ASSERT_FATAL( NULL != getTmpNode("root"));
    reset_numOfMpDocs();
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}

void test_get_successDocCount()
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
    //webcfgdb_destroy(wd);
}

void test_get_global_db_node()
{
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
    //webcfgdb_destroy(wd);
    if(wd)
    {
        WEBCFG_FREE(wd->name);
        wd->version = 0;
        WEBCFG_FREE(wd->root_string);
        WEBCFG_FREE(wd);
    }
    reset_successDocCount();
    reset_db_node();
    CU_ASSERT_FATAL( NULL == get_global_db_node());
}

void test_reset_db_node()
{
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
    //webcfgdb_destroy(wd);
    if(wd)
    {
        WEBCFG_FREE(wd->name);
        wd->version = 0;
        WEBCFG_FREE(wd->root_string);
        WEBCFG_FREE(wd);
    }
    reset_successDocCount();
    reset_db_node();
    CU_ASSERT_FATAL( NULL == get_global_db_node());
}

void test_reset_successDocCount()
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
    if(wd)
    {
        WEBCFG_FREE(wd->name);
        wd->version = 0;
        WEBCFG_FREE(wd->root_string);
        WEBCFG_FREE(wd);
    }
    //webcfgdb_destroy(wd);
    reset_successDocCount();
    reset_db_node();
    int p = get_successDocCount();
    CU_ASSERT_EQUAL(0,p);
}

void test_checkTmpRootUpdate()
{
    int count = 0;
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("root");
    tmpData->version = 232323;
    tmpData->status = strdup("failed");
    tmpData->trans_id = 4104;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("invalid");
    tmpData->next = NULL;
    count++;
    webconfig_tmp_data_t *tmp = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp->name = strdup("privatessid");
    tmp->version = 1234;
    tmp->status = strdup("success");
    tmp->trans_id = 4204;
    tmp->retry_count = 0;
    tmp->error_code = 0;
    tmp->error_details = strdup("none");
    tmp->next = NULL;
    tmpData->next = tmp;
    count++;
    webconfig_tmp_data_t *tmp1 = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmp1->name = strdup("moca");
    tmp1->version = 5678;
    tmp1->status = strdup("success");
    tmp1->trans_id = 4304;
    tmp1->retry_count = 0;
    tmp1->error_code = 0;
    tmp1->error_details = strdup("none");
    tmp1->next = NULL;
    tmp->next = tmp1;
    count++;
    set_global_tmp_node(tmpData);
    set_global_supplementarySync(0);
    set_numOfMpDocs(count);
    CU_ASSERT_FATAL( NULL != getTmpNode("root"));
    checkTmpRootUpdate();
    CU_ASSERT_FATAL( NULL != getTmpNode("root"));
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}

void test_set_doc_fail()
{
    int m = get_doc_fail();
    CU_ASSERT_EQUAL(0,m);
    set_doc_fail(1);
    int n = get_doc_fail();
    CU_ASSERT_EQUAL(1,n);
    set_doc_fail(0);
    int p = get_doc_fail();
    CU_ASSERT_EQUAL(0,p);
}

void test_get_doc_fail()
{
    int m = get_doc_fail();
    CU_ASSERT_EQUAL(0,m);
    set_doc_fail(1);
    int n = get_doc_fail();
    CU_ASSERT_EQUAL(1,n);
    set_doc_fail(0);
    int p = get_doc_fail();
    CU_ASSERT_EQUAL(0,p);
}

void test_webcfgdbparam_strerror() 
{
    CU_ASSERT_STRING_EQUAL(webcfgdbparam_strerror(0),"No errors.");
    CU_ASSERT_STRING_EQUAL(webcfgdbparam_strerror(1),"Out of memory.");
    CU_ASSERT_STRING_EQUAL(webcfgdbparam_strerror(2),"Invalid first element.");
    CU_ASSERT_STRING_EQUAL(webcfgdbparam_strerror(3),"Invalid 'datatype' value.");
    CU_ASSERT_STRING_EQUAL(webcfgdbparam_strerror(4),"Invalid 'parameters' array.");
    CU_ASSERT_STRING_EQUAL(webcfgdbparam_strerror(5),"Unknown error.");
}

void test_updateFailureTimeStamp()
{
    long long expiry_time = 0;
    webconfig_tmp_data_t *tmpData = (webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    tmpData->name = strdup("privatessid");
    tmpData->version = 1234;
    tmpData->status = strdup("pending");
    tmpData->trans_id = 4204;
    tmpData->retry_count = 0;
    tmpData->error_code = 0;
    tmpData->error_details = strdup("none");
    tmpData->next = NULL;
    set_global_tmp_node(tmpData);
    expiry_time = getRetryExpiryTimeout();
    int m = updateFailureTimeStamp(tmpData,"privatessid", expiry_time);
    CU_ASSERT_EQUAL(0,m);
    int n = updateFailureTimeStamp(tmpData,"moca", expiry_time);
    CU_ASSERT_EQUAL(1,n);
    //CU_ASSERT_FATAL( NULL != getTmpNode("moca"));
    //CU_ASSERT_FATAL( NULL != getTmpNode("privatessid"));
    //CU_ASSERT_FATAL( NULL == getTmpNode("wan"));
    delete_tmp_list();
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());
}

void test_writeToDBFile()
{
    size_t webcfgdbPackSize = -1;
    void* dbData = NULL;
    size_t count=1;
    webconfig_db_data_t * webcfgdb = NULL;
    webcfgdb = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    webcfgdb->name = strdup("wan");
    webcfgdb->version = 410448631;
    webcfgdb->root_string = strdup("portmapping");
    webcfgdb->next=NULL;
    webcfgdbPackSize = webcfgdb_pack(webcfgdb, &dbData, count);
    //CU_ASSERT_PTR_NOT_NULL(webcfgdbPackSize);
    WebcfgDebug("webcfgdbPackSize %lu \n",webcfgdbPackSize);
    CU_ASSERT_NOT_EQUAL(-1,webcfgdbPackSize);
    CU_ASSERT_PTR_NOT_NULL(dbData);
    int m = writeToDBFile("/tmp/webcfgdb.txt",(char *)dbData,webcfgdbPackSize);
    CU_ASSERT_EQUAL(1,m);
    int n = writeToDBFile("/tmp/webcfgdb.txt",NULL,webcfgdbPackSize);
    CU_ASSERT_EQUAL(0,n);
    if(dbData)
    {
        WEBCFG_FREE(dbData);
    }
    if(webcfgdb)
    {
        WEBCFG_FREE(webcfgdb->name);
        webcfgdb->version = 0;
        WEBCFG_FREE(webcfgdb->root_string);
        WEBCFG_FREE(webcfgdb);
    }
    remove("/tmp/webcfgdb.txt");
}


void test_webcfgdb_destroy() //doubt
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
    WEBCFG_FREE(wd->root_string);
    webcfgdb_destroy(wd);
    reset_successDocCount();
    reset_db_node();
    /*if(wd == NULL)
	WebcfgInfo("\nvenkat setting to freee %s!!!!!!!!", (char *)wd); */
    CU_ASSERT_FATAL( NULL == get_global_db_node());
}

void test_webcfgdbblob_destroy()
{
    size_t webcfgdbBlobPackSize = -1;
    void * data = NULL;
    webconfig_db_data_t * webcfgdb = NULL;
    webconfig_tmp_data_t *webcfgtemp;
    //db data
    webcfgdb = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    webcfgdb->name = strdup("wan");
    webcfgdb->version = 410448631;
    webcfgdb->root_string = strdup("portmapping");
    webcfgdb->next=NULL;
    CU_ASSERT_PTR_NOT_NULL(webcfgdb);
    //temp data
    webcfgtemp=(webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    webcfgtemp->name = strdup("wan");
    webcfgtemp->version = 410448631;
    webcfgtemp->status = strdup("pending");
    webcfgtemp->error_details = strdup("none");
    webcfgtemp->error_code = 0;
    webcfgtemp->trans_id = 0;
    webcfgtemp->retry_count = 0;
    webcfgtemp->next=NULL;
    CU_ASSERT_PTR_NOT_NULL(webcfgtemp);
    //webcfgdb_blob_pack function
    webcfgdbBlobPackSize = webcfgdb_blob_pack(webcfgdb, webcfgtemp, &data);
    CU_ASSERT_NOT_EQUAL(-1,webcfgdbBlobPackSize);

    //decodeBlobData
    blob_struct_t *blobdata = NULL;
    blobdata = decodeBlobData((void *)data, webcfgdbBlobPackSize );
    CU_ASSERT_PTR_NOT_NULL(blobdata);
    CU_ASSERT_STRING_EQUAL("wan",blobdata->entries[0].name);
    CU_ASSERT_EQUAL(410448631,blobdata->entries[0].version);
    CU_ASSERT_STRING_EQUAL("success",blobdata->entries[0].status);
    CU_ASSERT_STRING_EQUAL("none",blobdata->entries[0].error_details);
    CU_ASSERT_EQUAL(0,blobdata->entries[0].error_code);
    CU_ASSERT_STRING_EQUAL("portmapping",blobdata->entries[0].root_string);
    webcfgdbblob_destroy(blobdata);
    WEBCFG_FREE(data);
    if(webcfgdb)
    {
        WEBCFG_FREE(webcfgdb->name);
        webcfgdb->version = 0;
        WEBCFG_FREE(webcfgdb->root_string);
        WEBCFG_FREE(webcfgdb);
    }
    CU_ASSERT_PTR_NULL(webcfgdb);
    if(webcfgtemp)
    {
        WEBCFG_FREE(webcfgtemp->name);
        webcfgtemp->version = 0;
        WEBCFG_FREE(webcfgtemp->status);
        WEBCFG_FREE(webcfgtemp->error_details);
        WEBCFG_FREE(webcfgtemp);
    }
    CU_ASSERT_PTR_NULL(webcfgtemp);
}

void test_webcfgdbblob_strerror()
{
    CU_ASSERT_STRING_EQUAL(webcfgdbblob_strerror(0),"No errors.");
    CU_ASSERT_STRING_EQUAL(webcfgdbblob_strerror(1),"Out of memory.");
    CU_ASSERT_STRING_EQUAL(webcfgdbblob_strerror(2),"Invalid first element.");
    CU_ASSERT_STRING_EQUAL(webcfgdbblob_strerror(3),"Invalid 'datatype' value.");
    CU_ASSERT_STRING_EQUAL(webcfgdbblob_strerror(4),"Invalid 'parameters' array.");
    CU_ASSERT_STRING_EQUAL(webcfgdbblob_strerror(5),"Unknown error.");
}

void test_writebase64ToDBFile()
{
    size_t webcfgdbPackSize = -1;
    void* dbData = NULL;
    size_t count=1;
    webconfig_db_data_t * webcfgdb = NULL;
    webcfgdb = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    webcfgdb->name = strdup("wan");
    webcfgdb->version = 410448631;
    webcfgdb->root_string = strdup("portmapping");
    webcfgdb->next=NULL;
    webcfgdbPackSize = webcfgdb_pack(webcfgdb, &dbData, count);
    //CU_ASSERT_PTR_NOT_NULL(webcfgdbPackSize);
    WebcfgDebug("webcfgdbPackSize %lu \n",webcfgdbPackSize);
    CU_ASSERT_NOT_EQUAL(-1,webcfgdbPackSize);
    CU_ASSERT_PTR_NOT_NULL(dbData);
    int m = writebase64ToDBFile("/tmp/webcfgdb.txt",(char *)dbData);
    CU_ASSERT_EQUAL(1,m);
    if(dbData)
    {
        WEBCFG_FREE(dbData);
    }
    if(webcfgdb)
    {
        WEBCFG_FREE(webcfgdb->name);
        webcfgdb->version = 0;
        WEBCFG_FREE(webcfgdb->root_string);
        WEBCFG_FREE(webcfgdb);
    }
    int n = writebase64ToDBFile("/tmp/webcfgdb.txt",NULL);
    CU_ASSERT_EQUAL(0,n);
    remove("/tmp/webcfgdb.txt");
}

void test_get_DB_BLOB()
{
    //size_t webcfgdbBlobPackSize = -1;
   // void * data = NULL;
    webconfig_tmp_data_t * webcfgtemp = NULL;
    webconfig_db_data_t * webcfgdb = NULL;

    //temp data
    webcfgtemp=(webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
    webcfgtemp->name = strdup("wan");
    webcfgtemp->version = 5678;
    webcfgtemp->status = strdup("success");
    webcfgtemp->trans_id = 4204;
    webcfgtemp->retry_count = 0;
    webcfgtemp->error_code = 0;
    webcfgtemp->error_details = strdup("none");
    webcfgtemp->next=NULL;
    CU_ASSERT_PTR_NOT_NULL(webcfgtemp);
    set_global_tmp_node(webcfgtemp);
    CU_ASSERT_FATAL( NULL != get_global_tmp_node());
    //db data
    webcfgdb = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
    webcfgdb->name = strdup("wan");
    webcfgdb->version = 410448631;
    webcfgdb->root_string = strdup("portmapping");
    webcfgdb->next=NULL;
    CU_ASSERT_PTR_NOT_NULL(webcfgdb);
    addToDBList(webcfgdb);
    int m = get_successDocCount();
    CU_ASSERT_EQUAL(1,m);
    CU_ASSERT_FATAL( NULL != get_global_db_node());

    CU_ASSERT_FATAL( NULL != get_DB_BLOB());
    if(webcfgtemp)
    {
        WEBCFG_FREE(webcfgtemp->name);
        webcfgtemp->version = 0;
        WEBCFG_FREE(webcfgtemp->status);
        WEBCFG_FREE(webcfgtemp->error_details);
        WEBCFG_FREE(webcfgtemp);
    }
    CU_ASSERT_PTR_NULL(webcfgtemp);
    set_global_tmp_node(webcfgtemp);
    CU_ASSERT_FATAL( NULL == get_global_tmp_node());

    if(webcfgdb)
    {
        WEBCFG_FREE(webcfgdb->name);
        webcfgdb->version = 0;
        WEBCFG_FREE(webcfgdb->root_string);
        WEBCFG_FREE(webcfgdb);
    }
    reset_db_node();
    CU_ASSERT_PTR_NULL(webcfgdb);
    reset_successDocCount();
    int n = get_successDocCount();
    CU_ASSERT_EQUAL(0,n);
    CU_ASSERT_FATAL( NULL == get_global_db_node());

    CU_ASSERT_FATAL( NULL == get_DB_BLOB());
}
void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "test set_global_tmp_node", test_set_global_tmp_node);
    CU_add_test( *suite, "test addToTmpList", test_addToTmpList);
    CU_add_test( *suite, "test get_numOfMpDocs", test_get_numOfMpDocs);
    CU_add_test( *suite, "test delete_tmp_list", test_delete_tmp_list);
    CU_add_test( *suite, "test reset_numOfMpDocs", test_reset_numOfMpDocs);
    CU_add_test( *suite, "test get_global_tmp_node", test_get_global_tmp_node);
    CU_add_test( *suite, "test getTmpNode", test_getTmpNode);
    CU_add_test( *suite, "test updateTmpList", test_updateTmpList);
    CU_add_test( *suite, "test deleteFromTmpList", test_deleteFromTmpList);
    CU_add_test( *suite, "test delete_tmp_docs_list", test_delete_tmp_docs_list);
    CU_add_test( *suite, "test release_success_docs_tmplist", test_release_success_docs_tmplist);
    CU_add_test( *suite, "test get_successDocCount", test_get_successDocCount);
    CU_add_test( *suite, "test get_global_db_node", test_get_global_db_node);
    CU_add_test( *suite, "test reset_db_node", test_reset_db_node);
    CU_add_test( *suite, "test reset_successDocCount", test_reset_successDocCount);
    CU_add_test( *suite, "test checkTmpRootUpdate", test_checkTmpRootUpdate);
    CU_add_test( *suite, "test set_doc_fail", test_set_doc_fail);
    CU_add_test( *suite, "test get_doc_fail", test_get_doc_fail);
    CU_add_test( *suite, "test webcfgdbparam_strerror", test_webcfgdbparam_strerror);
    CU_add_test( *suite, "test updateFailureTimeStamp", test_updateFailureTimeStamp); 
    CU_add_test( *suite, "test writeToDBFile", test_writeToDBFile);
    CU_add_test( *suite, "test webcfgdb_destroy", test_webcfgdb_destroy);
    CU_add_test( *suite, "test webcfgdbblob_destroy", test_webcfgdbblob_destroy);
    CU_add_test( *suite, "test webcfgdbblob_strerror", test_webcfgdbblob_strerror);
    CU_add_test( *suite, "test writebase64ToDBFile", test_writebase64ToDBFile);
    CU_add_test( *suite, "test get_DB_BLOB", test_get_DB_BLOB);
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
