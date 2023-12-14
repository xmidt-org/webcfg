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
#define FILE_URL "/tmp/webcfg_url"

#define UNUSED(x) (void )(x)

char *url = NULL;
char *interface = NULL;
char device_mac[32] = {'\0'};
int numLoops;

void printTest();

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
	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status, NULL);
	
/*	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status);
	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status);
	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status);
	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status);
	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status);
	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status);
	set_global_supplementarySync(0);
	printTest();
	processWebconfgSync((int)status);
	set_global_supplementarySync(1);
	printTest();
	processWebconfgSync((int)status);*/

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

