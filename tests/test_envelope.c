 /**
  * Copyright 2020 Comcast Cable Communications Management, LLC
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

#include <CUnit/Basic.h>
#include "../src/envelope.h"

void test_simple()
{
    const uint8_t input[] = {
        0x83, 0xA6, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x84, 0xA4, 0x62, 0x61,
        0x73, 0x65, 0xA5, 0x74, 0x68, 0x69, 0x6E, 0x67, 0xA5, 0x6D, 0x61, 0x6A,
        0x6F, 0x72, 0x01, 0xA5, 0x6D, 0x69, 0x6E, 0x6F, 0x72, 0x02, 0xA5, 0x70,
        0x61, 0x74, 0x63, 0x68, 0x00,
        0xA6, 0x73, 0x68, 0x61, 0x32, 0x35, 0x36,
        0xC4, 0x20, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        0xA7, 0x70, 0x61, 0x79, 0x6C, 0x6F, 0x61, 0x64,
        0xC4, 0x0A, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    envelope_t *env;
    const uint8_t sha[32] = {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4};
    const uint8_t payload[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };


    env = envelope_convert( input, sizeof(input) );
    CU_ASSERT_FATAL( NULL != env );
    CU_ASSERT_STRING_EQUAL( "thing", env->schema.base );
    CU_ASSERT( 1 == env->schema.major );
    CU_ASSERT( 2 == env->schema.minor );
    CU_ASSERT( 0 == env->schema.patch );
    CU_ASSERT( 0 == memcmp(sha, env->sha256, 32) );
    CU_ASSERT( 10 == env->len );
    CU_ASSERT( 0 == memcmp(payload, env->payload, 10) );

    envelope_destroy( env );
}

void test_errors()
{
    const uint8_t missing_payload[] = {
        0x82, 0xA6, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x84, 0xA4, 0x62, 0x61,
        0x73, 0x65, 0xA5, 0x74, 0x68, 0x69, 0x6E, 0x67, 0xA5, 0x6D, 0x61, 0x6A,
        0x6F, 0x72, 0x01, 0xA5, 0x6D, 0x69, 0x6E, 0x6F, 0x72, 0x02, 0xA5, 0x70,
        0x61, 0x74, 0x63, 0x68, 0x00,
        0xA6, 0x73, 0x68, 0x61, 0x32, 0x35, 0x36,
        0xC4, 0x20, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
    const uint8_t missing_base[] = {
        0x83, 0xA6, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x83,
        0xA5, 0x6D, 0x61, 0x6A,
        0x6F, 0x72, 0x01, 0xA5, 0x6D, 0x69, 0x6E, 0x6F, 0x72, 0x02, 0xA5, 0x70,
        0x61, 0x74, 0x63, 0x68, 0x00,
        0xA6, 0x73, 0x68, 0x61, 0x32, 0x35, 0x36,
        0xC4, 0x20, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        0xA7, 0x70, 0x61, 0x79, 0x6C, 0x6F, 0x61, 0x64,
        0xC4, 0x0A, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    envelope_t *e;
    int err;

    e = envelope_convert( missing_payload, sizeof(missing_payload) );
    err = errno;
    CU_ASSERT( NULL == e );
    CU_ASSERT_STRING_EQUAL( "'payload' element missing.", envelope_strerror(err) );

    e = envelope_convert( missing_base, sizeof(missing_base) );
    err = errno;
    CU_ASSERT( NULL == e );
    CU_ASSERT_STRING_EQUAL( "'base' element missing.", envelope_strerror(err) );

    envelope_destroy( NULL );
}



void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Normal", test_simple);
    CU_add_test( *suite, "Errors", test_errors);
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


