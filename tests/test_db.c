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

bool get_global_shutdown()
{
	return false;
}
int akerwait__ (unsigned int secs)
{
	UNUSED(secs);
	return 0;

}
char *getAuthToken()
{
	return NULL;
}
char* get_deviceMAC()
{
	char *device_mac=strdup("b42xxxxxxxxx");
	return device_mac;
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

char * webcfg_appendeddoc(char * subdoc_name, uint32_t version, char * blob_data, size_t blob_size, uint16_t *trans_id)
{
	UNUSED(subdoc_name);
	UNUSED(version);
	UNUSED(blob_data);
	UNUSED(blob_size);
	UNUSED(trans_id);
	return NULL;
}

void initEventHandlingTask(){
	return;
}

void processWebcfgEvents(){
	return;
}

void webcfgStrncpy(char *destStr, const char *srcStr, size_t destSize)
{
    strncpy(destStr, srcStr, destSize-1);
    destStr[destSize-1] = '\0';
}

WEBCFG_STATUS checkAndUpdateTmpRetryCount(webconfig_tmp_data_t *temp, char *docname)
{
	UNUSED(temp);
	UNUSED(docname);
	return 0;
}
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


void isSubDocSupported(){
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

void processAkerSubdoc(){
return ;
}


char * getsupportedDocs()
{
	return NULL;
}
char * getsupportedVersion()
{
	return NULL;
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

char* get_global_auth_token(){
	char *tok = strdup("1234567890");
	return tok ;
}
void getCurrent_Time(struct timespec *timer){
	UNUSED(timer);
	return;
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

void set_global_supplementarySync(int value)
{
    UNUSED(value);
}

int get_global_supplementarySync()
{
	return 0;
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
char * getsupplementaryDocs()
{
      return NULL;
}
int generateRandomId()
{
	return 0;
}

/*----------------------------------------------------------------------------*/
/*                             Test Functions                             */
/*----------------------------------------------------------------------------*/




void test_blobPackUnpack(){
	size_t webcfgdbBlobPackSize = -1;
    	void * data = NULL;
	webconfig_db_data_t * webcfgdb = NULL;
	webconfig_tmp_data_t *webcfgtemp;
	webconfig_db_data_t *db_data;
    	webconfig_tmp_data_t *temp_data;
	//db data	
	webcfgdb = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
	webcfgdb->name = strdup("wan");
	webcfgdb->version = 410448631;
	webcfgdb->root_string = strdup("portmapping");
	webcfgdb->next=NULL;
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
	//webcfgdb_blob_pack function
	
 	webcfgdbBlobPackSize = webcfgdb_blob_pack(webcfgdb, webcfgtemp, &data);
	db_data = webcfgdb;
    	temp_data = webcfgtemp;
	CU_ASSERT_PTR_NOT_NULL(webcfgdbBlobPackSize);
	CU_ASSERT_PTR_NOT_NULL(db_data);
	CU_ASSERT_PTR_NOT_NULL(temp_data);
	
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
	
}

void test_dbPackUnpack(){
	size_t webcfgdbPackSize = -1;
     	void* dbData = NULL;
	size_t count=1;
	webconfig_db_data_t * webcfgdb = NULL;
	
	//db data	
	webcfgdb = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
	webcfgdb->name = strdup("wan");
	webcfgdb->version = 410448631;
	webcfgdb->root_string = strdup("portmapping");
	webcfgdb->next=NULL;
	
	//webcfgdb_pack function
	
 	webcfgdbPackSize = webcfgdb_pack(webcfgdb, &dbData, count);
	
	CU_ASSERT_PTR_NOT_NULL(webcfgdbPackSize);
	CU_ASSERT_PTR_NOT_NULL(dbData);
	//decodeData
	webconfig_db_data_t* sss =NULL;
	sss = decodeData((void *)dbData, webcfgdbPackSize );
	CU_ASSERT_PTR_NOT_NULL(sss);
	webcfgdb_destroy(sss);
	
}

void test_addToDBList(){
	webconfig_db_data_t *wd;
	wd = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
	CU_ASSERT_PTR_NOT_NULL(wd);
	wd->name = strdup("wan");
	wd->version = 410448631;
	wd->root_string = strdup("portmapping");
	wd->next=NULL;
        addToDBList(wd);
	WEBCFG_FREE(wd);
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "test blobPackUnpack", test_blobPackUnpack);
    CU_add_test( *suite, "test dbPackUnpack", test_dbPackUnpack);
    CU_add_test( *suite, "test addToDBList", test_addToDBList);
    
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
