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
#include <msgpack.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <cJSON.h>
#include "../src/webcfg_param.h"
#include "../src/webcfg_log.h"

int process_webcfgparam( webcfgparam_t *pm, msgpack_object *obj );
static void packJsonString( cJSON *item, msgpack_packer *pk );
static void packJsonNumber( cJSON *item, msgpack_packer *pk );
static void packJsonArray( cJSON *item, msgpack_packer *pk, int isBlob );
static void packJsonObject( cJSON *item, msgpack_packer *pk, int isBlob);
static void __msgpack_pack_string( msgpack_packer *pk, const void *string, size_t n );
static int convertJsonToMsgPack(char *data, char **encodedData, int isBlob);


static int getItemsCount(cJSON *object)
{
	int count = 0;
	while(object != NULL)
	{
		object = object->next;
		count++;
	}
	return count;
}
static void __msgpack_pack_string( msgpack_packer *pk, const void *string, size_t n )
{
	WebcfgDebug("%s:%s\n",__FUNCTION__,(char *)string);
    msgpack_pack_str( pk, n );
    msgpack_pack_str_body( pk, string, n );
}
static void packJsonString( cJSON *item, msgpack_packer *pk )
{
	if(item->string != NULL)
	{
		__msgpack_pack_string(pk, item->string, strlen(item->string));
	}
	__msgpack_pack_string(pk, item->valuestring, strlen(item->valuestring));
}
static void packJsonNumber( cJSON *item, msgpack_packer *pk )
{
	WebcfgDebug("%s:%d\n",__FUNCTION__,item->valueint);
	if(item->string != NULL)
	{
		__msgpack_pack_string(pk, item->string, strlen(item->string));
	}
	msgpack_pack_int(pk, item->valueint);
}
static void packJsonArray(cJSON *item, msgpack_packer *pk, int isBlob)
{
	int arraySize = cJSON_GetArraySize(item);
	WebcfgDebug("%s:%s\n",__FUNCTION__, item->string);
	if(item->string != NULL)
	{
		//printf("packing %s\n",item->string);
		__msgpack_pack_string(pk, item->string, strlen(item->string));
	}
	msgpack_pack_array( pk, arraySize );
	int i=0;
	for(i=0; i<arraySize; i++)
	{
		cJSON *arrItem = cJSON_GetArrayItem(item, i);
		switch((arrItem->type) & 0XFF)
		{
			case cJSON_Object:
				packJsonObject(arrItem, pk, isBlob);
				break;
		}
	}
}
int getEncodedBlob(char *data, char **encodedData)
{
	cJSON *jsonData=NULL;
	int encodedDataLen = 0;
	WebcfgDebug("------- %s -------\n",__FUNCTION__);
	jsonData=cJSON_Parse(data);
	if(jsonData != NULL)
	{
		msgpack_sbuffer sbuf1;
		msgpack_packer pk1;
		msgpack_sbuffer_init( &sbuf1 );
		msgpack_packer_init( &pk1, &sbuf1, msgpack_sbuffer_write );
		packJsonObject(jsonData, &pk1, 1);
		if( sbuf1.data )
		{
		    *encodedData = ( char * ) malloc( sizeof( char ) * sbuf1.size );
		    if( NULL != *encodedData )
			{
		        memcpy( *encodedData, sbuf1.data, sbuf1.size );
			}
			encodedDataLen = sbuf1.size;
		}
		msgpack_sbuffer_destroy(&sbuf1);
		cJSON_Delete(jsonData);
	}
	WebcfgDebug("------- %s -------\n",__FUNCTION__);
	return encodedDataLen;
}
static void packBlobData(cJSON *item, msgpack_packer *pk )
{
	char *blobData = NULL, *encodedBlob = NULL;
	int len = 0;
	WebcfgDebug("------ %s ------\n",__FUNCTION__);
	blobData = strdup(item->valuestring);
	if(strlen(blobData) > 0)
	{
		WebcfgDebug("%s\n",blobData);
		len = getEncodedBlob(blobData, &encodedBlob);
		WebcfgDebug("%s\n",encodedBlob);
	}
	__msgpack_pack_string(pk, item->string, strlen(item->string));
	__msgpack_pack_string(pk, encodedBlob, len);
	free(encodedBlob);
	free(blobData);
	WebcfgDebug("------ %s ------\n",__FUNCTION__);
}
static void packJsonObject( cJSON *item, msgpack_packer *pk, int isBlob )
{
	WebcfgDebug("%s\n",__FUNCTION__);
	cJSON *child = item->child;
	msgpack_pack_map( pk, getItemsCount(child));
	while(child != NULL)
	{
		switch((child->type) & 0XFF)
		{
			case cJSON_String:
				if(child->string != NULL && (strcmp(child->string, "value") == 0) && isBlob == 1)
				{
					packBlobData(child, pk);
				}
				else
				{
					packJsonString(child, pk);
				}
				break;
			case cJSON_Number:
				packJsonNumber(child, pk);
				break;
			case cJSON_Array:
				packJsonArray(child, pk, isBlob);
				break;
		}
		child = child->next;
	}
}
static int convertJsonToMsgPack(char *data, char **encodedData, int isBlob)
{
	cJSON *jsonData=NULL;
	int encodedDataLen = 0;
	jsonData=cJSON_Parse(data);
	if(jsonData != NULL)
	{
		msgpack_sbuffer sbuf;
		msgpack_packer pk;
		msgpack_sbuffer_init( &sbuf );
		msgpack_packer_init( &pk, &sbuf, msgpack_sbuffer_write );
		packJsonObject(jsonData, &pk, isBlob);
		if( sbuf.data )
		{
		    *encodedData = ( char * ) malloc( sizeof( char ) * sbuf.size );
		    if( NULL != *encodedData )
			{
		        memcpy( *encodedData, sbuf.data, sbuf.size );
			}
			encodedDataLen = sbuf.size;
		}
		msgpack_sbuffer_destroy(&sbuf);
		cJSON_Delete(jsonData);
	}
	return encodedDataLen;
}


void test_basic()
{

	webcfgparam_t *pm = NULL;
	int err, i=0;
	char *Data = "{\"parameters\": [{\"name\":\"Device.DeviceInfo.Test\",\"value\":\"false\",\"dataType\":3},{\"name\":\"Device.DeviceInfo.Test1\",\"value\":\"true\",\"dataType\":3}]}";

	char *encodedData = NULL;
	int encodedLen = 0;
	encodedLen = convertJsonToMsgPack(Data, &encodedData, 1);
	if(encodedLen)
	{
		void* basicV;

		basicV = ( void*)encodedData;

		pm = webcfgparam_convert( basicV, encodedLen+1 );
		err = errno;
		WebcfgInfo( "errno: %s\n", webcfgparam_strerror(err) );		

		CU_ASSERT_FATAL( NULL != pm );
		CU_ASSERT_FATAL( NULL != pm->entries );
		CU_ASSERT_FATAL( 2 == pm->entries_count );
		CU_ASSERT_STRING_EQUAL( "Device.DeviceInfo.Test", pm->entries[0].name );
		CU_ASSERT_FATAL( 3 == pm->entries[0].type );
		for(i = 0; i < (int)pm->entries_count ; i++)
		{
			WebcfgInfo("pm->entries[%d].name %s\n", i, pm->entries[i].name);
			WebcfgInfo("pm->entries[%d].value %s\n" , i, pm->entries[i].value);
			WebcfgInfo("pm->entries[%d].type %d\n", i, pm->entries[i].type);
		}

	    	webcfgparam_destroy( pm );
	}
}

void test_webcfgparam_strerror()
{
	const char *txt;
	txt = webcfgparam_strerror(0);
	CU_ASSERT_STRING_EQUAL(txt,"No errors.");
	txt = webcfgparam_strerror(1);
	CU_ASSERT_STRING_EQUAL(txt,"Out of memory.");	
	txt = webcfgparam_strerror(10);
	CU_ASSERT_STRING_EQUAL(txt,"Unknown error.");	
}

void test_process_webcfgparam()
{
	msgpack_object *inner;
	webcfgparam_t *pm;	
	int ret=0,err=0;
	inner = malloc( sizeof(msgpack_object));
	pm = malloc( sizeof(webcfgparam_t));	
	inner->via.array.size=0;
	ret = process_webcfgparam(pm,inner);
    CU_ASSERT_EQUAL(ret, 0);   
	 
	inner->via.array.size=1;
	msgpack_object_array *array = &inner->via.array;
	array->ptr = malloc( sizeof(msgpack_object));
	array->ptr->type = 0;
	ret = process_webcfgparam(pm,inner);
    CU_ASSERT_EQUAL(ret, -1);  
	err = errno;
	const char *txt;
	txt = webcfgparam_strerror(err);
	WebcfgInfo( "errno: %s\n", txt );		  	
	CU_ASSERT_STRING_EQUAL(txt,"Invalid 'parameters' array.");	
}

void test_process_params_invalid_datatype()
{
	int err;
	char *encodedData = NULL;
	int encodedLen = 0;
	encodedLen = convertJsonToMsgPack("{\"parameters\": [{\"name\":\"Device.DeviceInfo.CloudUIEnable1\",\"value\":\"false\",\"dataType\":65536}]} ", &encodedData, 1);
	if(encodedLen)
	{
		void* basicV;
		basicV = ( void*)encodedData;
		webcfgparam_convert( basicV, encodedLen+1 );
		err = errno;
		const char *txt;
		txt = webcfgparam_strerror(err);	
		CU_ASSERT_STRING_EQUAL(txt,"Invalid 'blob' object.");					
	}
}

void test_process_params_notify_attribute()
{
	int err;
	char *encodedData = NULL;
	int encodedLen = 0;
	webcfgparam_t *pm = NULL;	
	encodedLen = convertJsonToMsgPack("{\"parameters\": [{\"name\":\"Device.DeviceInfo.CloudUIEnable1\",\"value\":\"false\",\"notify_attribute\":3}]} ", &encodedData, 1);
	if(encodedLen)
	{
		void* basicV;
		basicV = ( void*)encodedData;
		pm = webcfgparam_convert( basicV, encodedLen+1 );
		CU_ASSERT_FATAL( NULL != pm );		
		err = errno;
		const char *txt;
		txt = webcfgparam_strerror(err);	
		CU_ASSERT_STRING_EQUAL(txt,"No errors.");					
	}
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
	CU_add_test( *suite, "test webcfgparam_strerror", test_webcfgparam_strerror);
	CU_add_test( *suite, "test process_webcfgparam", test_process_webcfgparam);	
    CU_add_test( *suite, "Full", test_basic);	
    CU_add_test( *suite, "test process_params", test_process_params_invalid_datatype);	
	CU_add_test( *suite, "test process_params", test_process_params_notify_attribute);	
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

