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
#include "../src/dhcp.h"

void test_basic()
{
    const uint8_t basic[] = {
        0x81,
            0xa4, 'd', 'h', 'c', 'p',
                0x85,
                    0xa9, 'r', 'o', 'u', 't', 'e', 'r', '-', 'i', 'p',
                        0xce, 0xc0, 0xa8, 0x00, 0x01,   // 192.168.0.1
                    0xab, 's', 'u', 'b', 'n', 'e', 't', '-', 'm', 'a', 's', 'k',
                        0xce, 0xff, 0xff, 0xff, 0x00,   // 255.255.255.0
                    0xac, 'l', 'e', 'a', 's', 'e', '-', 'l', 'e', 'n', 'g', 't', 'h',
                        0xcd, 0x0c, 0x80,               // 3200
                    0xaa, 'p', 'o', 'o', 'l', '-', 'r', 'a', 'n', 'g', 'e',
                        0x92,
                            0xce, 0xc0, 0xa8, 0x00, 0x02,   // 192.168.0.2
                            0xce, 0xc0, 0xa8, 0x00, 0x64,   // 192.168.0.100
                    0xa6, 's', 't', 'a', 't', 'i', 'c',
                        0x93,
                            0x82,
                                0xa3, 'm', 'a', 'c',
                                    0xc4, 0x06, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                                0xa2, 'i', 'p',
                                    0xce, 0xc0, 0xa8, 0x00, 0x22,
                            0x82,
                                0xa3, 'm', 'a', 'c',
                                    0xc4, 0x06, 0x11, 0x22, 0x33, 0x44, 0x55, 0x60,
                                0xa2, 'i', 'p',
                                    0xce, 0xc0, 0xa8, 0x00, 0x23,
                            0x82,
                                0xa3, 'm', 'a', 'c',
                                    0xc4, 0x06, 0x11, 0x22, 0x33, 0x44, 0x55, 0x00,
                                0xa2, 'i', 'p',
                                    0xce, 0xc0, 0xa8, 0x00, 0x20,
    };
    dhcp_t *dhcp;
    int err;
    uint8_t mac0[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t mac1[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x60 };
    uint8_t mac2[6] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x00 };

    dhcp = dhcp_convert( basic, sizeof(basic) );
    err = errno;
    printf( "errno: %s\n", dhcp_strerror(err) );

    CU_ASSERT_FATAL( NULL != dhcp );
    CU_ASSERT( 0xc0a80001 == dhcp->router_ip );
    CU_ASSERT( 0xffffff00 == dhcp->subnet_mask );
    CU_ASSERT( 3200 == dhcp->lease_length );
    CU_ASSERT( 0xc0a80002 == dhcp->pool_range[0] );
    CU_ASSERT( 0xc0a80064 == dhcp->pool_range[1] );
    CU_ASSERT( 3 == dhcp->fixed_count );
    CU_ASSERT( 0xc0a80022 == dhcp->fixed[0].ip );
    CU_ASSERT( 0 == memcmp(mac0, dhcp->fixed[0].mac, 6) );
    CU_ASSERT( 0xc0a80023 == dhcp->fixed[1].ip );
    CU_ASSERT( 0 == memcmp(mac1, dhcp->fixed[1].mac, 6) );
    CU_ASSERT( 0xc0a80020 == dhcp->fixed[2].ip );
    CU_ASSERT( 0 == memcmp(mac2, dhcp->fixed[2].mac, 6) );

    dhcp_destroy( dhcp );
}

void test_no_optional()
{
    const uint8_t basic[] = {
        0x81,
            0xa4, 'd', 'h', 'c', 'p',
                0x84,
                    0xa9, 'r', 'o', 'u', 't', 'e', 'r', '-', 'i', 'p',
                        0xce, 0xc0, 0xa8, 0x00, 0x01,   // 192.168.0.1
                    0xab, 's', 'u', 'b', 'n', 'e', 't', '-', 'm', 'a', 's', 'k',
                        0xce, 0xff, 0xff, 0xff, 0x00,   // 255.255.255.0
                    0xac, 'l', 'e', 'a', 's', 'e', '-', 'l', 'e', 'n', 'g', 't', 'h',
                        0xcd, 0x0c, 0x80,               // 3200
                    0xaa, 'p', 'o', 'o', 'l', '-', 'r', 'a', 'n', 'g', 'e',
                        0x92,
                            0xce, 0xc0, 0xa8, 0x00, 0x02,   // 192.168.0.2
                            0xce, 0xc0, 0xa8, 0x00, 0x64,   // 192.168.0.100
    };
    dhcp_t *dhcp;

    dhcp = dhcp_convert( basic, sizeof(basic) );
    CU_ASSERT_FATAL( NULL != dhcp );

    CU_ASSERT( 0xc0a80001 == dhcp->router_ip );
    CU_ASSERT( 0xffffff00 == dhcp->subnet_mask );
    CU_ASSERT( 3200 == dhcp->lease_length );
    CU_ASSERT( 0xc0a80002 == dhcp->pool_range[0] );
    CU_ASSERT( 0xc0a80064 == dhcp->pool_range[1] );
    CU_ASSERT( 0 == dhcp->fixed_count );
    CU_ASSERT( NULL == dhcp->fixed );

    dhcp_destroy( dhcp );
}

void test_extras()
{
    const uint8_t basic[] = {
        0x81,
            0xa4, 'd', 'h', 'c', 'p',
                0x85,
                    0xa6, 'i', 'g', 'n', 'o', 'r', 'e',
                        0xa2, 'm', 'e',
                    0xa9, 'r', 'o', 'u', 't', 'e', 'r', '-', 'i', 'p',
                        0xce, 0xc0, 0xa8, 0x00, 0x01,   // 192.168.0.1
                    0xab, 's', 'u', 'b', 'n', 'e', 't', '-', 'm', 'a', 's', 'k',
                        0xce, 0xff, 0xff, 0xff, 0x00,   // 255.255.255.0
                    0xac, 'l', 'e', 'a', 's', 'e', '-', 'l', 'e', 'n', 'g', 't', 'h',
                        0xcd, 0x0c, 0x80,               // 3200
                    0xaa, 'p', 'o', 'o', 'l', '-', 'r', 'a', 'n', 'g', 'e',
                        0x92,
                            0xce, 0xc0, 0xa8, 0x00, 0x02,   // 192.168.0.2
                            0xce, 0xc0, 0xa8, 0x00, 0x64,   // 192.168.0.100
    };
    dhcp_t *dhcp;

    dhcp = dhcp_convert( basic, sizeof(basic) );
    CU_ASSERT_FATAL( NULL != dhcp );

    CU_ASSERT( 0xc0a80001 == dhcp->router_ip );
    CU_ASSERT( 0xffffff00 == dhcp->subnet_mask );
    CU_ASSERT( 3200 == dhcp->lease_length );
    CU_ASSERT( 0xc0a80002 == dhcp->pool_range[0] );
    CU_ASSERT( 0xc0a80064 == dhcp->pool_range[1] );
    CU_ASSERT( 0 == dhcp->fixed_count );
    CU_ASSERT( NULL == dhcp->fixed );

    dhcp_destroy( dhcp );
}




void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Full", test_basic);
    CU_add_test( *suite, "No Optionals", test_no_optional);
    CU_add_test( *suite, "Extra Elements", test_extras);
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
