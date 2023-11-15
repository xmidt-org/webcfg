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
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <CUnit/Basic.h>

#include "../src/webcfg_log.h"
#include "../src/webcfg_auth.h"

#define MAX_TOKEN_SIZE 256

static char *deviceMac = NULL;
static char *serialNumber = NULL;


char *get_deviceMAC(void)
{
    char *deviceMac_get = deviceMac;
	return deviceMac_get;
}

char *getSerialNumber(void)
{
    char *serialNum_get = serialNumber;
    return serialNum_get;
}

void webcfgStrncpy(char *destStr, const char *srcStr, size_t destSize)
{
    strncpy(destStr, srcStr, destSize-1);
    destStr[destSize-1] = '\0';
}

void test_execute_token_script(void) {
    char token[MAX_TOKEN_SIZE] = {'\0'};
    size_t len = MAX_TOKEN_SIZE;
    char mac[] = "12345abcdef";
    char serNum[] = "123456789";

    FILE *testFile = fopen(WEBPA_READ_HEADER, "w");
    if (testFile!=NULL) {
        fprintf(testFile, "#!/bin/bash\n");
        fprintf(testFile, "echo %s %s %s", WEBPA_READ_HEADER, serNum, mac);
        fclose(testFile);

        chmod(WEBPA_READ_HEADER, S_IRWXU);

        execute_token_script(token, WEBPA_READ_HEADER, len, mac, serNum);
        CU_ASSERT_TRUE(strlen(token)>0);
    } 
    else 
    {
        CU_FAIL("Unable to create test script file");
    }
    remove(WEBPA_READ_HEADER);
}

void test_CreateNewAuthToken() {
    char token[MAX_TOKEN_SIZE] = {'\0'};
    char mac[] = "12345abcdef";
    char serNum[] = "123456789";

    // Create and write to the test file for WEBPA_READ_HEADER
    FILE *testFile = fopen(WEBPA_CREATE_HEADER, "w");

    CU_ASSERT_PTR_NOT_NULL(testFile);

    if (testFile != NULL) {
        //file content will return "SUCCESS"
        fprintf(testFile, "#!/bin/bash\n");
        fprintf(testFile, "echo -n SUCCESS");
        fclose(testFile);

        chmod(WEBPA_CREATE_HEADER, S_IRWXU);
    }

     // Create and write to the test file for WEBPA_READ_HEADER
    FILE *testFile1 = fopen(WEBPA_READ_HEADER, "w");

    CU_ASSERT_PTR_NOT_NULL(testFile1);

    if (testFile != NULL) {
        // Ensure that the file content will return "SUCCESS"
        fprintf(testFile1, "#!/bin/bash\n");
        fprintf(testFile1, "echo -n SUCCESS");
        fclose(testFile1);

        chmod(WEBPA_READ_HEADER, S_IRWXU);
    }

    createNewAuthToken(token, sizeof(token), mac, serNum);

    CU_ASSERT_STRING_EQUAL(token, "SUCCESS");

    remove(WEBPA_CREATE_HEADER);
    remove(WEBPA_READ_HEADER);
}

void test_getAuthToken_no_file()
{
    //Both files return NULL
    getAuthToken();
    
    CU_ASSERT_PTR_NULL(get_deviceMAC());
    CU_ASSERT_PTR_NULL(getSerialNumber());
}

void test_getAuthToken_failure()
{
    //test file not NULL and get_deviceMAC equal to NULL
    deviceMac = NULL;
    FILE *testFile = fopen(WEBPA_CREATE_HEADER, "w");

    CU_ASSERT_PTR_NOT_NULL(testFile);

    if (testFile != NULL) {
        //file content will return "SUCCESS"
        fprintf(testFile, "#!/bin/bash\n");
        fprintf(testFile, "echo -n SUCCESS");
        fclose(testFile);

        chmod(WEBPA_CREATE_HEADER, S_IRWXU);
    }

    FILE *testFile1 = fopen(WEBPA_READ_HEADER, "w");

    CU_ASSERT_PTR_NOT_NULL(testFile1);

    if (testFile != NULL) {
        fprintf(testFile1, "#!/bin/bash\n");
        fprintf(testFile1, "echo -n SUCCESS");
        fclose(testFile1);

        chmod(WEBPA_READ_HEADER, S_IRWXU);
    }

    getAuthToken();
   
    CU_ASSERT_PTR_NULL(get_deviceMAC());

    //get_deviceMAC not NULL and getSerialNumber equals NULL
    deviceMac = strdup("b42xxxxxxxxx");
    serialNumber = NULL;

    getAuthToken();
    CU_ASSERT_PTR_NOT_NULL(get_deviceMAC());
    CU_ASSERT_PTR_NULL(getSerialNumber());
}

void test_getAuthToken_error()
{
    serialNumber = strdup("1234");
    deviceMac = strdup("b42xxxxxxxxx");

    //Create and write to the test file for WEBPA_READ_HEADER
    FILE *testFile = fopen(WEBPA_CREATE_HEADER, "w");

    CU_ASSERT_PTR_NOT_NULL(testFile);

    if (testFile != NULL) {
        //file content will return "SUCCESS"
        fprintf(testFile, "#!/bin/bash\n");
        fprintf(testFile, "echo -n SUCCESS");
        fclose(testFile);

        chmod(WEBPA_CREATE_HEADER, S_IRWXU);
    }

     // Create and write to the test file for WEBPA_READ_HEADER
    FILE *testFile1 = fopen(WEBPA_READ_HEADER, "w");

    CU_ASSERT_PTR_NOT_NULL(testFile1);

    if (testFile != NULL) {
        // Ensure that the file content will return "SUCCESS"
        fprintf(testFile1, "#!/bin/bash\n");
        fprintf(testFile1, "echo -n ERROR");
        fclose(testFile1);

        chmod(WEBPA_READ_HEADER, S_IRWXU);
    }

    getAuthToken();
    
    CU_ASSERT_PTR_NOT_NULL(get_deviceMAC());
    CU_ASSERT_PTR_NOT_NULL(getSerialNumber());
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "test execute_token_script", test_execute_token_script);
    CU_add_test( *suite, "test CreateNewAuthToken", test_CreateNewAuthToken);
    CU_add_test( *suite, "test getAuthToken_no_file", test_getAuthToken_no_file);
    CU_add_test( *suite, "test getAuthToken_failue", test_getAuthToken_failure);
    CU_add_test( *suite, "test getAuthToken_error", test_getAuthToken_error);
}

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
            WebcfgInfo( "\n" );
            CU_basic_show_failures( CU_get_failure_list() );
            WebcfgInfo( "\n\n" );
            rv = CU_get_number_of_tests_failed();
        }
        CU_cleanup_registry();
    }
    return rv;
}


