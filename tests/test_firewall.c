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
#include "../src/firewall.h"

void test_basic()
{
    const uint8_t basic[] = {
        0x81,
            0xa8, 'f', 'i', 'r', 'e', 'w', 'a', 'l', 'l',
                0x82,
                    0xa5, 'l', 'e', 'v', 'e', 'l',
                        0xa7, 'a', 'm', 'a', 'z', 'i', 'n', 'g',
                    0xa7, 'f', 'i', 'l', 't', 'e', 'r', 's',
                        0x92,
                            0xa4, 'h', 't', 't', 'p',
                            0xa5, 'i', 'd', 'e', 'n', 't',
    };
    firewall_t *firewall;
    int err;

    firewall = firewall_convert( basic, sizeof(basic) );
    err = errno;
    printf( "errno: %s\n", firewall_strerror(err) );

    CU_ASSERT_FATAL( NULL != firewall );

    firewall_destroy( firewall );
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
