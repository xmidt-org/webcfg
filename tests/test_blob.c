#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <base64.h>
#include <msgpack.h>
#include <CUnit/Basic.h>
#include "../src/webcfg_blob.h"
#include "../src/webcfg_log.h"
#define UNUSED(x) (void )(x)
bool rbus_enable = true;
bool isRbusEnabled() 
{

	return rbus_enable;
}
bool isRbusListener(char *subDoc)
{
	UNUSED(subDoc);
	return true;
}
char * base64blobencoder(char * blob_data, size_t blob_size )
{
	char* b64buffer =  NULL;
	size_t encodeSize = -1;
   	WebcfgDebug("Data is %s\n", blob_data);
     	WebcfgDebug("-----------Start of Base64 Encode ------------\n");
        encodeSize = b64_get_encoded_buffer_size(blob_size);
        WebcfgDebug("encodeSize is %zu\n", encodeSize);
        b64buffer = malloc(encodeSize + 1);
        if(b64buffer != NULL)
        {
            memset( b64buffer, 0, sizeof( encodeSize )+1 );

            b64_encode((uint8_t *)blob_data, blob_size, (uint8_t *)b64buffer);
            b64buffer[encodeSize] = '\0' ;
        }
	return b64buffer;
}
void test_writeToFileData()
{
    char* db_file_path = "test.txt";
    char* data = NULL;
    size_t size = 0;
    // Failed to create/open file in root 
    CU_ASSERT_EQUAL(writeToFileData("/test.txt", data, size), 0);
    // NULL data pointer
    CU_ASSERT_EQUAL(writeToFileData(db_file_path, data, size), 0);
    // with data 
    CU_ASSERT_EQUAL(writeToFileData(db_file_path, "data", 5), 1);
    //Removing created test.txt file
    if (remove(db_file_path) != 0) {
    	    CU_FAIL("Failed to remove file");
    }
}    

void test_webcfg_appendeddoc()
{
    char *appended_doc = NULL;
    int embPackSize = 0;
    uint16_t trans_id = 0;
    appended_doc = webcfg_appendeddoc("subdoc1", 1234, "test", 5, &trans_id, &embPackSize);
    CU_ASSERT_NOT_EQUAL(embPackSize,0);
	CU_ASSERT_FATAL( 0 != appended_doc);  
    rbus_enable = false;
    appended_doc = webcfg_appendeddoc("subdoc2", 1234, "test", 5, &trans_id, &embPackSize);
    CU_ASSERT_NOT_EQUAL(embPackSize,0);
	CU_ASSERT_FATAL( 0 != appended_doc);     
    rbus_enable = true;     
}

void test_appendWebcfgEncodedData()
{
	size_t embeddeddocPackSize = -1;
	void *embeddeddocdata = NULL;
	//encode data
    embeddeddocPackSize = appendWebcfgEncodedData(&embeddeddocdata, (void *)"abcd", 4, (void *)"efgh", 5);
    CU_ASSERT_STRING_EQUAL(embeddeddocdata,"dbcdefgh");
    CU_ASSERT_EQUAL(embeddeddocPackSize,9);
    //test Msgpack Map (fixmap)  MAX size 
    unsigned char byte[5]="abcd";
    byte[0]=0xff;
    embeddeddocPackSize = appendWebcfgEncodedData(&embeddeddocdata, (void *)byte, 4, (void *)"efgh", 5);    
    CU_ASSERT_EQUAL(embeddeddocPackSize,-1);    
}

void test_webcfg_pack_appenddoc()
{
    size_t appenddocPackSize = -1;
    appenddoc_t *appenddata = NULL;    
    void *appenddocdata = NULL;    
    appenddocPackSize = webcfg_pack_appenddoc(appenddata, &appenddocdata);    
    CU_ASSERT_EQUAL(appenddocPackSize,-1);      
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
	CU_add_test( *suite, "test writeToFileData", test_writeToFileData);
	CU_add_test( *suite, "test webcfg_appendeddoc", test_webcfg_appendeddoc);    
	CU_add_test( *suite, "test appendWebcfgEncodedData", test_appendWebcfgEncodedData);
	CU_add_test( *suite, "test webcfg_pack_appenddoc", test_webcfg_pack_appenddoc);      
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
