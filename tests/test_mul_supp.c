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
#include "../src/webcfg_param.h"
#include "../src/webcfg.h"
#include "../src/webcfg_multipart.h"
#include "../src/webcfg_helpers.h"
#include "../src/webcfg_db.h"
#include "../src/webcfg_notify.h"
#include "../src/webcfg_metadata.h"
#include <msgpack.h>
#include <curl/curl.h>
#include <base64.h>
#include "../src/webcfg_generic.h"
#include "../src/webcfg_event.h"
#include "../src/webcfg_timer.h"
#define FILE_URL "/tmp/webcfg_url"

#define UNUSED(x) (void )(x)

char *url = NULL;
char *interface = NULL;
char device_mac[32] = {'\0'};
int numLoops;

void printTest();

char* get_deviceMAC()
{
	char *device_mac = strdup("b42xxxxxxxxx");	
	return device_mac;
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

void test_multipart()
{
	unsigned long status = 0;

	if(url == NULL)
	{
		printf("\nProvide config URL as argument\n");
		return;
	}
	initWebcfgProperties(WEBCFG_PROPERTIES_FILE);
	initWebConfigNotifyTask();
	processWebcfgEvents();
	initEventHandlingTask();
#ifdef FEATURE_SUPPORT_AKER
	initWebConfigClient();
#endif
	initMaintenanceTimer();
	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status, "telemetry");


}
void test_Primary_supp()
{	
	unsigned long status = 0;

	if(url == NULL)
	{
		printf("\nProvide config URL as argument\n");
		return;
	}
	initMaintenanceTimer();
	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status, NULL);
	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status, "telemetry");
}

void test_Initdb_Primary_primary()
{
	
	unsigned long status = 0;

	if(url == NULL)
	{
		printf("\nProvide config URL as argument\n");
		return;
	}	
	initDB(WEBCFG_DB_FILE);

	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status, NULL);

	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status, NULL);

}
void test_Initdb_Primary_supp()
{
	
	unsigned long status = 0;

	if(url == NULL)
	{
		printf("\nProvide config URL as argument\n");
		return;
	}	
	initDB(WEBCFG_DB_FILE);

	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status, NULL);

	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status, "telemetry");

}
void test_supp_supp()
{
	
	unsigned long status = 0;

	if(url == NULL)
	{
		printf("\nProvide config URL as argument\n");
		return;
	}	
	

	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status, "telemetry");

	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status, "telemetry");

}
void test_prim_supp_prim()
{
	unsigned long status = 0;

	if(url == NULL)
	{
		printf("\nProvide config URL as argument\n");
		return;
	}	
	
	initMaintenanceTimer();
	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status, NULL);

	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status, "telemetry");

	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status, NULL);

}
void test_updateRetryTime()
{

	int time_diff = 0;
	struct timespec ct;
	long long present_time = 0;
	clock_gettime(CLOCK_REALTIME, &ct);
	present_time = ct.tv_sec;
	present_time = present_time+5;
	time_diff=updateRetryTimeDiff(present_time);
	CU_ASSERT_EQUAL(5,time_diff);
}

void test_updateRetryTimefailed()
{
	long long expiry_time = 0;
	int time_diff = 0;
	expiry_time = getRetryExpiryTimeout();
	time_diff=updateRetryTimeDiff(expiry_time);
	CU_ASSERT_EQUAL(900,time_diff);
}

void test_checkMaintenanceTimer()
{
	int time=0;	
	time =checkMaintenanceTimer();
	CU_ASSERT_EQUAL(1,time);
}

void test_checkRetryTimer()
{
	int time_diff = 0;
	struct timespec ct;
	long long present_time = 0;
	clock_gettime(CLOCK_REALTIME, &ct);
	present_time = ct.tv_sec;
	time_diff=checkRetryTimer(present_time);
	CU_ASSERT_EQUAL(1,time_diff);

}

void test_retrySyncSeconds()
{
	int sec=0;	
	sec =retrySyncSeconds();
	CU_ASSERT_EQUAL(5,sec)
}

void test_maintenanceSyncSeconds()
{
	int sec=0;	
	sec =getMaintenanceSyncSeconds(0);
	CU_ASSERT_FATAL( 0 != sec);
}
void printTest()
{
	int count = get_global_supplementarySync();
	printf("\n");
	printf("\n");
	printf("------------------------------------------------------------\n");
	printf("------------Testing with Supplementary Sync %s--------------\n",count?"ON":"OFF");
	printf("------------------------------------------------------------\n");
	printf("\n");
	printf("\n");
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Full", test_multipart);
    CU_add_test( *suite, "Full", test_Primary_supp);
    CU_add_test( *suite, "Full", test_Initdb_Primary_primary);
    CU_add_test( *suite, "Full", test_Initdb_Primary_supp);
    CU_add_test( *suite, "Full", test_supp_supp);
    CU_add_test( *suite, "Full", test_prim_supp_prim);
    CU_add_test( *suite, "Full", test_updateRetryTime);
    CU_add_test( *suite, "Full",test_updateRetryTimefailed);
    CU_add_test( *suite, "Full",test_checkMaintenanceTimer);
    CU_add_test( *suite, "Full",test_checkRetryTimer);
    CU_add_test( *suite, "Full",test_retrySyncSeconds);
    CU_add_test( *suite, "Full",test_maintenanceSyncSeconds);
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( int argc, char *argv[] )
{
	unsigned rv = 1;
	CU_pSuite suite = NULL;
	// int len=0;
	printf("argc %d \n", argc );
	if(argv[1] !=NULL)
	{
		url = strdup(argv[1]);
	}
	// Read url from file
	//readFromFile(FILE_URL, &url, &len );
	if(url !=NULL && strlen(url)==0)
	{
		printf("<url> is NULL.. add url in /tmp/webcfg_url file\n");
		return 0;
	}
	printf("url fetched %s\n", url);
	if(argv[2] !=NULL)
	{
		interface = strdup(argv[2]);
	}
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

