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
#include <wdmp-c.h>
#include <wrp-c.h>
#include <CUnit/Basic.h>
#include "../src/webcfg_rbus.h"
#define FILE_URL "/tmp/webcfg_url"

#define UNUSED(x) (void )(x)

pthread_mutex_t sync_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sync_condition=PTHREAD_COND_INITIALIZER;

multipartdocs_t *get_global_mp(void)
{
    return NULL;
}

bool get_webcfgReady()
{
    return true;
}

bool get_maintenanceSync()
{
    return false;
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
pthread_mutex_t *get_global_sync_mutex(void)
{
    return &sync_mutex;
}
bool get_global_shutdown()
{
	return false;
}
void set_global_shutdown(bool shutdown)
{
	UNUSED(shutdown);
}
pthread_t *get_global_mpThreadId(void)
{
	return NULL;
}
int rbus_StoreValueIntoDB(char *paramName, char *value)
{
	WebcfgDebug("Inside rbus_StoreValueIntoDB weak fn\n");
	UNUSED(paramName);
	UNUSED(value);
	return 0;
}
int rbus_GetValueFromDB( char* paramName, char** paramValue)
{
	WebcfgDebug("Inside rbus_GetValueFromDB weak fn\n");
	UNUSED(paramName);
	*paramValue =strdup("true");
	return 0;
}
void webcfgStrncpy(char *destStr, const char *srcStr, size_t destSize)
{
   strncpy(destStr, srcStr, destSize-1);
   destStr[destSize-1] = '\0';
}
WDMP_STATUS mapStatus(int ret)
{
	if(isRbusEnabled())
	{
		switch (ret)
		{
			case CCSP_Msg_Bus_OK:
				return WDMP_SUCCESS;
			case CCSP_Msg_BUS_TIMEOUT:
				return WDMP_ERR_TIMEOUT;
			case CCSP_ERR_INVALID_PARAMETER_VALUE:
				return WDMP_ERR_INVALID_PARAMETER_VALUE;
			case CCSP_ERR_UNSUPPORTED_NAMESPACE:
				return WDMP_ERR_UNSUPPORTED_NAMESPACE;
			default:
			return WDMP_FAILURE;
		}
	}
return ret;
}
void webcfgCallback(char *Info, void* user_data)
{
	UNUSED(Info);
	UNUSED(user_data);
}
pthread_cond_t *get_global_sync_condition(void)
{
    return &sync_condition;
}
bool get_bootSync()
{
    return false;
}
void initWebConfigMultipartTask(unsigned long status)
{
	UNUSED(status);
}

char * get_DB_BLOB_base64()
{
	return NULL;
}

char * getsupportedDocs()
{
	return NULL;
}

char * getsupportedVersion()
{
	return NULL;
}


void test_setForceSync()
{
	int session_status = 0;
	int ret = set_rbus_ForceSync("root", &session_status);
	CU_ASSERT_EQUAL(0,session_status);
	CU_ASSERT_EQUAL(1,ret);
}

void test_setForceSync_failure()
{
	int session_status = 0;
	char *str = NULL;
	char* transID = NULL;
	int ret = set_rbus_ForceSync("", &session_status);
	CU_ASSERT_EQUAL(0,session_status);
	CU_ASSERT_EQUAL(1,ret);
	int retGet = get_rbus_ForceSync(&str, &transID);
	CU_ASSERT_EQUAL(0,retGet);
}

void test_setForceSync_json()
{
	char *data = "{ \"value\":\"root\", \"transaction_id\":\"e35c746c-bf15-43baXXXXXXXXXXXXXXXXXx____XXXXXXXXXXXXXX\"}";
	int session_status = 0;
	char *str = NULL;
	char* transID = NULL;
	int retSet = set_rbus_ForceSync(data, &session_status);
	CU_ASSERT_EQUAL(0,session_status);
	CU_ASSERT_EQUAL(1,retSet);
	int retGet = get_rbus_ForceSync(&str, &transID);
	CU_ASSERT_EQUAL(1,retGet);

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
     CU_add_test( *suite, "test rbus_forcesync", test_setForceSync);
     CU_add_test( *suite, "test rbus_forcesync_failure", test_setForceSync_failure);
     CU_add_test( *suite, "test rbus_forcesync_json", test_setForceSync_json);
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


