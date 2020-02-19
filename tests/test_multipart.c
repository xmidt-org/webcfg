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
#include "../src/webcfgdoc.h"
#include "../src/webcfgparam.h"
#include "../src/multipart.h"
#include "../src/helpers.h"
#include "../src/portmappingdoc.h"
#include "../src/portmappingparam.h"
#include <msgpack.h>
#include <curl/curl.h>
#include <base64.h>
#define FILE_URL "/tmp/webcfg_url"

char *url = NULL;
char *interface = NULL;

//To strip all spaces , new line & carriage return characters from header output
void stripLine(char *str, int len, char **final_str)
{
	int i=0, j=0;

	for(i=0;i <len;++i)
	{
		if(str[i]!='\n')
		{
			str[j++]=str[i];
		}
	}
	str[j]='\0';
	*final_str = str;
	printf("j is %d\n", j);
	printf("*final_str is %s\n", *final_str);
}

void test_multipart()
{
	int r_count=0;
	int configRet = -1;
        webcfgparam_t *pm;
	char *webConfigData = NULL;
	long res_code;
        int err;
        int len =0, i=0;
	void* subdbuff;
	char *subfileData = NULL;
	char* b64buffer =  NULL;
	size_t encodeSize = 0;
	size_t subLen=0;

	char * decodeMsg =NULL;
	size_t decodeMsgSize =0;
	size_t size =0;

	msgpack_zone mempool;
	msgpack_object deserialized;
	msgpack_unpack_return unpack_ret;

	if(url == NULL)
	{
		printf("\nProvide config URL as argument\n");
		return;
	}
	configRet = webcfg_http_request(url, &webConfigData, r_count, &res_code, interface, &subfileData, &len);
	if(configRet == 0)
	{
		printf("config ret success\n");
		subLen = (size_t) len;
		subdbuff = ( void*)subfileData;
		printf("subLen is %ld\n", subLen);

		/*********** base64 encode *****************/
		printf("-----------Start of Base64 Encode ------------\n");
		encodeSize = b64_get_encoded_buffer_size( subLen );
		printf("encodeSize is %ld\n", encodeSize);
		b64buffer = malloc(encodeSize+1);
		b64_encode((const uint8_t *)subfileData, subLen, (uint8_t *)b64buffer);
		b64buffer[encodeSize] = '\0' ;
		printf("---------- End of Base64 Encode -------------\n");

		//printf("Final Encoded data: %s\n",b64buffer);
		printf("Final Encoded data length: %ld\n",strlen(b64buffer));
		/*********** base64 encode *****************/

		//Start of b64 decoding
		printf("----Start of b64 decoding----\n");
		decodeMsgSize = b64_get_decoded_buffer_size(strlen(b64buffer));
		printf("expected b64 decoded msg size : %ld bytes\n",decodeMsgSize);

		decodeMsg = (char *) malloc(sizeof(char) * decodeMsgSize);

		size = b64_decode( (const uint8_t *)b64buffer, strlen(b64buffer), (uint8_t *)decodeMsg );
		printf("base64 decoded data containing %ld bytes is :%s\n",size, decodeMsg);

		printf("----End of b64 decoding----\n");
		//End of b64 decoding

		//Start of msgpack decoding just to verify
		printf("----Start of msgpack decoding----\n");
		msgpack_zone_init(&mempool, 2048);
		unpack_ret = msgpack_unpack(decodeMsg, size, NULL, &mempool, &deserialized);
		printf("unpack_ret is %d\n",unpack_ret);
		switch(unpack_ret)
		{
			case MSGPACK_UNPACK_SUCCESS:
				printf("MSGPACK_UNPACK_SUCCESS :%d\n",unpack_ret);
				printf("\nmsgpack decoded data is:");
				msgpack_object_print(stdout, deserialized);
			break;
			case MSGPACK_UNPACK_EXTRA_BYTES:
				printf("MSGPACK_UNPACK_EXTRA_BYTES :%d\n",unpack_ret);
			break;
			case MSGPACK_UNPACK_CONTINUE:
				printf("MSGPACK_UNPACK_CONTINUE :%d\n",unpack_ret);
			break;
			case MSGPACK_UNPACK_PARSE_ERROR:
				printf("MSGPACK_UNPACK_PARSE_ERROR :%d\n",unpack_ret);
			break;
			case MSGPACK_UNPACK_NOMEM_ERROR:
				printf("MSGPACK_UNPACK_NOMEM_ERROR :%d\n",unpack_ret);
			break;
			default:
				printf("Message Pack decode failed with error: %d\n", unpack_ret);
		}

		msgpack_zone_destroy(&mempool);
		printf("----End of msgpack decoding----\n");
		//End of msgpack decoding

		//decode root doc
		char* final_subdbuff = NULL;
                printf("subdbuff is %s\n", (char*)subdbuff);

		stripLine(subdbuff, (int)subLen, &final_subdbuff);
		printf("subLen is %d\n", (int)subLen);
		//subdbuff = ( void*)subfileData;
		char *binfileData = NULL;
		int length=0;
		//decode root doc
		int status = writeToFile("buff.bin", (char*)final_subdbuff, subLen-1);
		printf("status %d\n", status);
		int readstatus = readFromFile("buff.bin", &binfileData , &length);
		printf("readstatus %d\n", readstatus);
		printf("--------------decode root doc-------------\n");
		pm = webcfgparam_convert( final_subdbuff, length+1 );
                err = errno;
		printf( "errno: %s\n", portmapping_strerror(err) );
		CU_ASSERT_FATAL( NULL != pm );
		CU_ASSERT_FATAL( NULL != pm->entries );
		//CU_ASSERT_STRING_EQUAL( "Device.NAT.PortMapping.", pm->entries[0].name );
		for(i = 0; i < (int)pm->entries_count ; i++)
		{
			printf("pm->entries[%d].name %s\n", i, pm->entries[i].name);
			printf("pm->entries[%d].value %s\n" , i, pm->entries[i].value);
			printf("pm->entries[%d].type %d\n", i, pm->entries[i].type);
		}

		//decode inner blob
		/*(portmappingdoc_t *rpm;
		printf("--------------decode blob-------------\n");
		rpm = portmappingdoc_convert( pm->entries[0].value, 2623 );
		printf("blob len %lu\n", strlen(pm->entries[0].value));
		err = errno;
		printf( "errno: %s\n", portmappingdoc_strerror(err) );
		CU_ASSERT_FATAL( NULL != rpm );
		CU_ASSERT_FATAL( NULL != rpm->entries );

		portmappingdoc_destroy( rpm );*/
		webcfgparam_destroy( pm );
	}	
	else
	{
		printf("webcfg_http_request failed\n");
	}
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Full", test_multipart);
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( int argc, char *argv[] )
{
	unsigned rv = 1;
	CU_pSuite suite = NULL;
	// int len=0;
	printf("argc %d \n", argc );
	if(argv[1] !=NULL)
	{
		url = strdup(argv[1]);
	}
	// Read url from file
	//readFromFile(FILE_URL, &url, &len );
	if(strlen(url)==0)
	{
		printf("<url> is NULL.. add url in /tmp/webcfg_url file\n");
		return 0;
	}
	printf("url fetched %s\n", url);
	if(argv[2] !=NULL)
	{
		interface = strdup(argv[2]);
	}
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

