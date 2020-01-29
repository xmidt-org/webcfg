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
#include "../src/portmapping.h"

void test_basic()
{
    const uint8_t basic[] = {
        0x81,
            0xac, 'p', 'o', 'r', 't', '-', 'm', 'a', 'p', 'p', 'i', 'n', 'g',
                0x92,
                    0x84,
                        0xa8, 'p', 'r', 'o', 't', 'o', 'c', 'o', 'l',
                            0xa3, 't', 'c', 'p',
                        0xb3, 'e', 'x', 't', 'e', 'r', 'n', 'a', 'l', '-', 'p', 'o', 'r', 't', '-', 'r', 'a', 'n', 'g', 'e',
                            0x92,
                                0xcc, 0x50,
                                0xcd, 0x04, 0xb0,
                        0xab, 't', 'a', 'r', 'g', 'e', 't', '-', 'p', 'o', 'r', 't',
                                0xcd, 0x26, 0xfc,
                        0xab, 't', 'a', 'r', 'g', 'e', 't', '-', 'i', 'p', 'v', '4',
                                0xce, 0xc0, 0xb4, 0x00, 0x22,
                    0x84,
                        0xa8, 'p', 'r', 'o', 't', 'o', 'c', 'o', 'l',
                            0xa3, 'u', 'd', 'p',
                        0xb3, 'e', 'x', 't', 'e', 'r', 'n', 'a', 'l', '-', 'p', 'o', 'r', 't', '-', 'r', 'a', 'n', 'g', 'e',
                            0x92,
                                0xcc, 53,
                                0xcc, 53,
                        0xab, 't', 'a', 'r', 'g', 'e', 't', '-', 'p', 'o', 'r', 't',
                                0xcc, 53,
                        0xab, 't', 'a', 'r', 'g', 'e', 't', '-', 'i', 'p', 'v', '6',
                                0xc4, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
    };
    portmapping_t *pm;
    int err;

    pm = portmapping_convert( basic, sizeof(basic) );
    err = errno;
    printf( "errno: %s\n", portmapping_strerror(err) );

    CU_ASSERT_FATAL( NULL != pm );
    CU_ASSERT_FATAL( NULL != pm->entries );
    CU_ASSERT_FATAL( 2 == pm->entries_count );
    CU_ASSERT_STRING_EQUAL( "tcp", pm->entries[0].protocol );

    portmapping_destroy( pm );
}

void test_no_optional()
{
    const uint8_t basic[] = {
        0x81,
            0xac, 'p', 'o', 'r', 't', '-', 'm', 'a', 'p', 'p', 'i', 'n', 'g',
                0x90,
    };
    portmapping_t *pm;

    pm = portmapping_convert( basic, sizeof(basic) );
    CU_ASSERT_FATAL( NULL != pm );

    CU_ASSERT( 0 == pm->entries_count );
    CU_ASSERT( NULL == pm->entries );

    portmapping_destroy( pm );
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
