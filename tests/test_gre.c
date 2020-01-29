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
#include "../src/gre.h"

void test_basic()
{
    const uint8_t basic[] = {
        0x81,
            0xa3, 'g', 'r', 'e',
                0x82,
                    0xb7, 'p', 'r', 'i', 'm', 'a', 'r', 'y', '-', 'r', 'e', 'm', 'o', 't', 'e', '-', 'e', 'n', 'd', 'p', 'o', 'i', 'n', 't',
                        0xa4, 'u', 'r', 'l', '1',
                    0xb9, 's', 'e', 'c', 'o', 'n', 'd', 'a', 'r', 'y', '-', 'r', 'e', 'm', 'o', 't', 'e', '-', 'e', 'n', 'd', 'p', 'o', 'i', 'n', 't',
                        0xa4, 'u', 'r', 'l', '2',
    };
    gre_t *gre;
    int err;

    gre = gre_convert( basic, sizeof(basic) );
    err = errno;
    printf( "errno: %s\n", gre_strerror(err) );

    CU_ASSERT_FATAL( NULL != gre );
    CU_ASSERT_STRING_EQUAL( "url1", gre->primary_remote_endpoint );
    CU_ASSERT_STRING_EQUAL( "url2", gre->secondary_remote_endpoint );

    gre_destroy( gre );
}

void test_no_optional()
{
    const uint8_t basic[] = {
        0x81,
            0xa3, 'g', 'r', 'e',
                0x80,
    };
    gre_t *gre;

    gre = gre_convert( basic, sizeof(basic) );
    CU_ASSERT_FATAL( NULL != gre );

    CU_ASSERT( NULL == gre->primary_remote_endpoint );
    CU_ASSERT( NULL == gre->secondary_remote_endpoint );

    gre_destroy( gre );
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
