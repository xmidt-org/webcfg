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
#include "../src/webcfg_db.h"
#include "../src/webcfg_pack.h"
#include "../src/webcfg_helpers.h"
#include "../src/webcfg_param.h"


void test_blobPack(){
	size_t webcfgdbBlobPackSize = -1;
    	void * data = NULL;
	webconfig_db_data_t * webcfgdb = NULL;
	webconfig_tmp_data_t *webcfgtemp;
	webconfig_db_data_t *db_data;
    	webconfig_tmp_data_t *temp_data;
	//db data	
	webcfgdb = (webconfig_db_data_t *) malloc (sizeof(webconfig_db_data_t));
	webcfgdb->name = strdup("root");
	webcfgdb->version = 2.0;
	webcfgdb->root_string = strdup("rootstr");
	webcfgdb->next=NULL;
	//temp data
	webcfgtemp=(webconfig_tmp_data_t *)malloc(sizeof(webconfig_tmp_data_t));
	webcfgtemp->name = strdup("root");
	webcfgtemp->version = 2.0;
	webcfgtemp->status = strdup("pending");
	webcfgtemp->error_details = strdup("none");
	webcfgtemp->error_code = 0;
	webcfgtemp->trans_id = 0;
	webcfgtemp->retry_count = 0;
	webcfgtemp->next=NULL;
	//webcfgdb_blob_pack
	size_t len = 0;
 	webcfgdbBlobPackSize = webcfgdb_blob_pack(webcfgdb, webcfgtemp, &data);
	//inside the function

	db_data = webcfgdb;
    	temp_data = webcfgtemp;
	CU_ASSERT_PTR_NOT_NULL(db_data);
	CU_ASSERT_PTR_NOT_NULL(temp_data);
	blob_t * webcfgdb_blob = NULL;
	webcfgdb_blob = (blob_t *)malloc(sizeof(blob_t));
	webcfgdb_blob->data = (char *)data;
        webcfgdb_blob->len  = webcfgdbBlobPackSize;
	printf("The webcfgdbBlobPackSize is : %ld\n",webcfgdb_blob->len);
	CU_ASSERT_PTR_NOT_NULL(data);
	CU_ASSERT_PTR_NOT_NULL(webcfgdbBlobPackSize);
	//decode data
	webconfig_db_data_t* dm = NULL;
	dm = decodeData((void *)data, len);
	printf("\ndm->name %s\n",dm->name);
	printf("dm->version %d\n",dm->version);
	CU_ASSERT_PTR_NOT_NULL(dm);
	

}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "test_blobPack", test_blobPack);
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
