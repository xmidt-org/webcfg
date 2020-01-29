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
#include "../src/full.h"

void test_basic()
{
    const uint8_t basic[] = {
        0x81,
            0xa4, 'f', 'u', 'l', 'l',
                0x81,
                    0xaa, 's', 'u', 'b', 's', 'y', 's', 't', 'e', 'm', 's',
                        0x92,
                            0x82,
                                0xa3, 'u', 'r', 'l',
                                    0xa4, 'u', 'r', 'l', '1',
                                0xa7, 'p', 'a', 'y', 'l', 'o', 'a', 'd',
                                    0xc4, 0x00,
                            0x82,
                                0xa3, 'u', 'r', 'l',
                                    0xa4, 'u', 'r', 'l', '3',
                                0xa7, 'p', 'a', 'y', 'l', 'o', 'a', 'd',
                                    0xc4, 0x01, 0xff,
    };
    full_t *full;

    full = full_convert( basic, sizeof(basic) );

    CU_ASSERT_FATAL( NULL != full );
    CU_ASSERT( full->subsystems_count = 2 );
    CU_ASSERT_STRING_EQUAL( full->subsystems[0].url, "url1" );
    CU_ASSERT( 0 == full->subsystems[0].payload_len );
    CU_ASSERT( NULL == full->subsystems[0].payload );
    CU_ASSERT_STRING_EQUAL( full->subsystems[1].url, "url3" );
    CU_ASSERT( 1 == full->subsystems[1].payload_len );
    CU_ASSERT_FATAL( NULL != full->subsystems[1].payload );
    CU_ASSERT( 0xff == full->subsystems[1].payload[0] );

    full_destroy( full );
}

void test_no_optional()
{
    const uint8_t basic1[] = {
        0x81,
            0xa4, 'f', 'u', 'l', 'l',
                0x80,
    };
    const uint8_t basic2[] = {
        0x80,
    };
    full_t *full;

    full = full_convert( basic1, sizeof(basic1) );
    CU_ASSERT_FATAL( NULL != full );
    full_destroy( full );

    full = full_convert( basic2, sizeof(basic2) );
    CU_ASSERT_FATAL( NULL != full );
    full_destroy( full );
}



void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Full", test_basic);
    CU_add_test( *suite, "No Optionals", test_no_optional);
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
