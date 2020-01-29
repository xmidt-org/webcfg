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
#include "../src/xdns.h"

void test_basic()
{
    const uint8_t basic[] = {
        0x81,
            0xa4, 'x', 'd', 'n', 's',
                0x82,
                    0xac, 'd', 'e', 'f', 'a', 'u', 'l', 't', '-', 'i', 'p', 'v', '4',
                        0xce, 0x4c, 0x4c, 0x4c, 0x4c,
                    0xac, 'd', 'e', 'f', 'a', 'u', 'l', 't', '-', 'i', 'p', 'v', '6',
                        0xc4, 0x10, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
    };
    xdns_t *xdns;
    int err;

    xdns = xdns_convert( basic, sizeof(basic) );
    err = errno;
    printf( "errno: %s\n", xdns_strerror(err) );

    CU_ASSERT_FATAL( NULL != xdns );
    CU_ASSERT( 0x4c4c4c4c == xdns->default_ipv4 );

    xdns_destroy( xdns );
}


void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Full", test_basic);
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
