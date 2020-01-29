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

#include <CUnit/Basic.h>
#include "../src/http.h"

void test_simple()
{
    http_request_t req    = {
        .auth             = NULL,
        .cfg_ver          = "v1",
        .schema_ver       = "v1",
        .fw               = "fw",
        .status           = "amazing",
        .trans_id         = "1234",
        .boot_unixtime    = 1,
        .ready_unixtime   = 1,
        .current_unixtime = 1,
        .url              = "https://fabric.example.com:8080/api/v2",
        .timeout_s        = 5,
        .ip_version       = 0,
        .interface        = NULL,
        .ca_cert_path     = NULL,
    };

    http_response_t resp;
    int rv;

    rv = http_request( &req, &resp );
    printf( "rv: %d\ncode: %d\n", rv, (int) resp.code );
    printf( "http_status: %d\n", (int) resp.http_status );
    http_destroy( &resp );
}


void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Add header", test_simple);
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


