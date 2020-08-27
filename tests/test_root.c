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
char *reason = NULL;
extern char g_RebootReason;

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

void setRebootReason(char *r_reason)
{
	if (r_reason)
	reason = strdup(r_reason);
	//return reason;
}

char * getRebootReason()
{
	//char *reason = strdup("factory-reset");
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

void test_FRNONE()
{
	char *root_str = NULL;
	uint32_t root_version = 0;
	int http_status = 0;
	FILE *fp = NULL;

	fp = fopen("/tmp/webconfig_db.bin","rb");
	if(fp !=NULL)
	{
		system("rm -rf /tmp/webconfig_db.bin");
		fclose(fp);
	}
	setRebootReason("factory-reset");

	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL != root_str );
	CU_ASSERT_STRING_EQUAL( "NONE", root_str );
}

void test_MIGRATION()
{
	char *root_str = NULL;
	uint32_t root_version = 0;
	int http_status = 0;
	g_RebootReason ='\0';

	setRebootReason("Software_upgrade");
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL != root_str );
	CU_ASSERT_STRING_EQUAL( "NONE-MIGRATION", root_str );
}

void test_NONE_REBOOT()
{
	char *root_str = NULL;
	uint32_t root_version = 0;
	int http_status = 0;
	g_RebootReason ='\0';

	setRebootReason("unknown");
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL != root_str );
	CU_ASSERT_STRING_EQUAL( "NONE-REBOOT", root_str );
}

void test_ROOTNULL()
{
	char *root_str = NULL;
	uint32_t root_version = 0;
	int http_status = 0;
	g_RebootReason ='\0';
	reason = NULL;

	setRebootReason(NULL);
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL == root_str );
}

void test_POSTNONE()
{
	char *root_str = NULL;
	uint32_t root_version = 0;
	int http_status = 0;
	FILE *fp = NULL;

	root_str = strdup("POST-NONE");
	setRebootReason("unknown");
	checkDBList("root", root_version, root_str);
	addNewDocEntry(1);
	
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL != root_str );
	CU_ASSERT_STRING_EQUAL( "NONE-REBOOT", root_str );
	fp = fopen("/tmp/webconfig_db.bin","rb");
	if(fp !=NULL)
	{
		system("rm -rf /tmp/webconfig_db.bin");
		fclose(fp);
	}
}

void test_FR_404()
{
	char *root_str = NULL;
	uint32_t root_version = 0;
	int http_status = 404;
	FILE *fp = NULL;

	root_str = strdup("NONE");
	setRebootReason("factory-reset");
	checkDBList("root", root_version, root_str);
	addNewDocEntry(1);
	
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL != root_str );
	CU_ASSERT_STRING_EQUAL( "POST-NONE", root_str );
	fp = fopen("/tmp/webconfig_db.bin","rb");
	if(fp !=NULL)
	{
		system("rm -rf /tmp/webconfig_db.bin");
		fclose(fp);
	}
}

void test_FR_200()
{
	char *root_str = NULL;
	uint32_t root_version = 0;
	int http_status = 200;
	FILE *fp = NULL;

	root_str = strdup("NONE");
	setRebootReason("factory-reset");
	checkDBList("root", root_version, root_str);
	addNewDocEntry(1);
	
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL != root_str );
	CU_ASSERT_STRING_EQUAL( "POST-NONE", root_str );
	fp = fopen("/tmp/webconfig_db.bin","rb");
	if(fp !=NULL)
	{
		system("rm -rf /tmp/webconfig_db.bin");
		fclose(fp);
	}
}

void test_FW_ROLLBACK()
{
	char *root_str = NULL;
	uint32_t root_version = 1234;
	int http_status = 0;
	g_RebootReason ='\0';
	FILE *fp = NULL;

	setRebootReason("Software_upgrade");
	checkDBList("root", root_version, root_str);
	checkDBList("test", 123, NULL);
	addNewDocEntry(2);
	
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL == root_str );
	CU_ASSERT_EQUAL( 0, root_version );
	fp = fopen("/tmp/webconfig_db.bin","rb");
	if(fp !=NULL)
	{
		system("rm -rf /tmp/webconfig_db.bin");
		fclose(fp);
	}
}

void test_zerorootVersion()
{
	char *root_str = "POST-NONE";
	uint32_t root_version = 0;
	int http_status = 0;
	g_RebootReason ='\0';
	FILE *fp = NULL;

	setRebootReason("Software_upgrade");
	checkDBList("root", root_version, root_str);
	checkDBList("test", 1234, NULL);
	addNewDocEntry(2);
	
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL != root_str );
	CU_ASSERT_STRING_EQUAL( "POST-NONE", root_str );
	fp = fopen("/tmp/webconfig_db.bin","rb");
	if(fp !=NULL)
	{
		system("rm -rf /tmp/webconfig_db.bin");
		fclose(fp);
	}
}

void test_rootVersion()
{
	char *root_str = NULL;
	uint32_t root_version = 44444;
	int http_status = 0;
	g_RebootReason ='\0';
	FILE *fp = NULL;

	setRebootReason("unknown");
	checkDBList("root", root_version, root_str);
	checkDBList("test", 1234, NULL);
	addNewDocEntry(2);
	
	get_root_version_string(&root_str, &root_version, http_status);
	CU_ASSERT_FATAL( NULL == root_str );
	CU_ASSERT_EQUAL( 44444, root_version );
	fp = fopen("/tmp/webconfig_db.bin","rb");
	if(fp !=NULL)
	{
		system("rm -rf /tmp/webconfig_db.bin");
		fclose(fp);
	}
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Factory reset NONE\n", test_FRNONE);
    CU_add_test( *suite, "Software upgrade\n", test_MIGRATION);
    CU_add_test( *suite, "Unknown reboot\n", test_NONE_REBOOT);
    CU_add_test( *suite, "No reboot reason\n", test_ROOTNULL);
    CU_add_test( *suite, "Unknown Reboot with DB\n", test_POSTNONE);
    CU_add_test( *suite, "Factory reset 404\n", test_FR_404);
    CU_add_test( *suite, "Factory reset 200\n", test_FR_200);
    CU_add_test( *suite, "Migration/rollback\n", test_FW_ROLLBACK);
    CU_add_test( *suite, "zero root DB\n", test_zerorootVersion);
    CU_add_test( *suite, "Non zero root DB\n", test_rootVersion);
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

