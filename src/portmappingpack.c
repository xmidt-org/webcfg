/*
 * Copyright 2019 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <errno.h>
#include <string.h>
#include <msgpack.h>

#include "helpers.h"
#include "portmappingpack.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
struct portmapping_token {
    const char *name;
    size_t length;
};
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static const struct portmapping_token PORTMAPPING_PARAMETERS   = { .name = "parameters", .length = sizeof( "parameters" ) - 1 };
static void __msgpack_pack_string( msgpack_packer *pk, const void *string, size_t n );
static void __msgpack_pack_string_nvp( msgpack_packer *pk,
                                       const struct portmapping_token *token,
                                       const char *val );

static void __msgpack_pack_string( msgpack_packer *pk, const void *string, size_t n )
{
    msgpack_pack_str( pk, n );
    msgpack_pack_str_body( pk, string, n );
}

static void __msgpack_pack_string_nvp( msgpack_packer *pk,
                                       const struct portmapping_token *token,
                                       const char *val )
{
    if( ( NULL != token ) && ( NULL != val ) ) {
        __msgpack_pack_string( pk, token->name, token->length );
        __msgpack_pack_string( pk, val, strlen( val ) );
    }
}

ssize_t portmap_pack_subdoc(const subdoc_t *subdocData,void **data)
{
    size_t rv = -1;
    int i = 0;

    msgpack_sbuffer sbuf;
    msgpack_packer pk;
    msgpack_sbuffer_init( &sbuf );
    msgpack_packer_init( &pk, &sbuf, msgpack_sbuffer_write );
    msgpack_zone mempool;
    msgpack_object deserialized;

    if( subdocData != NULL && subdocData->count != 0)
    {
        int count = subdocData->count;
        msgpack_pack_array( &pk, count );
                
        for( i = 0; i < count; i++ )
        {
             msgpack_pack_map( &pk, 6);

             struct portmapping_token SUBDOC_MAP_INTERNALCLIENT;
             
             SUBDOC_MAP_INTERNALCLIENT.name = "InternalClient";
             SUBDOC_MAP_INTERNALCLIENT.length = strlen( "InternalClient" );
             __msgpack_pack_string_nvp( &pk, &SUBDOC_MAP_INTERNALCLIENT, subdocData->subdoc_items[i].internal_client );
             
             struct portmapping_token SUBDOC_MAP_EXTERNALPORTENDRANGE;
             
             SUBDOC_MAP_EXTERNALPORTENDRANGE.name = "ExternalPortEndRange";
             SUBDOC_MAP_EXTERNALPORTENDRANGE.length = strlen( "ExternalPortEndRange" );
             __msgpack_pack_string( &pk, SUBDOC_MAP_EXTERNALPORTENDRANGE.name, SUBDOC_MAP_EXTERNALPORTENDRANGE.length );
	     msgpack_pack_int(&pk, subdocData->subdoc_items[i].external_port_end_range );

             struct portmapping_token SUBDOC_MAP_ENABLE;
             
             SUBDOC_MAP_ENABLE.name = "Enable";
             SUBDOC_MAP_ENABLE.length = strlen( "Enable" );
             __msgpack_pack_string( &pk, SUBDOC_MAP_ENABLE.name, SUBDOC_MAP_ENABLE.length );
             if( subdocData->subdoc_items[i].enable )
                msgpack_pack_true(&pk);
             else
                msgpack_pack_false(&pk);

             struct portmapping_token SUBDOC_MAP_PROTOCOL;
             
             SUBDOC_MAP_PROTOCOL.name = "Protocol";
             SUBDOC_MAP_PROTOCOL.length = strlen( "Protocol" );
             __msgpack_pack_string_nvp( &pk, &SUBDOC_MAP_PROTOCOL, subdocData->subdoc_items[i].protocol );

             struct portmapping_token SUBDOC_MAP_DESCRIPTION;
             
             SUBDOC_MAP_DESCRIPTION.name = "Description";
             SUBDOC_MAP_DESCRIPTION.length = strlen( "Description" );
             __msgpack_pack_string_nvp( &pk, &SUBDOC_MAP_DESCRIPTION, subdocData->subdoc_items[i].description );

             struct portmapping_token SUBDOC_MAP_EXTERNALPORT;
             
             SUBDOC_MAP_EXTERNALPORT.name = "ExternalPort";
             SUBDOC_MAP_EXTERNALPORT.length = strlen( "ExternalPort" );
             __msgpack_pack_string( &pk, SUBDOC_MAP_EXTERNALPORT.name, SUBDOC_MAP_EXTERNALPORT.length );
	     msgpack_pack_int(&pk, subdocData->subdoc_items[i].external_port);

       }
         
    }
    else 
    {
        printf("parameters is NULL\n" );
        return rv;
    }

    if( sbuf.data ) 
    {
        *data = ( char * ) malloc( sizeof( char ) * sbuf.size );

        if( NULL != *data ) 
        {
            memcpy( *data, sbuf.data, sbuf.size );
	    printf("sbuf.data of subdoc is %s sbuf.size %ld\n", sbuf.data, sbuf.size);
            rv = sbuf.size;
        }
    }

    msgpack_zone_init(&mempool, 2048);

    msgpack_unpack(sbuf.data, sbuf.size, NULL, &mempool, &deserialized);
    msgpack_object_print(stdout, deserialized);

    msgpack_zone_destroy(&mempool);

    msgpack_sbuffer_destroy( &sbuf );
    return rv;
    
}

ssize_t portmap_pack_rootdoc( char *blob, const data_t *packData, void **data )
{
    size_t rv = -1;
    msgpack_sbuffer sbuf;
    msgpack_packer pk;
    msgpack_sbuffer_init( &sbuf );
    msgpack_packer_init( &pk, &sbuf, msgpack_sbuffer_write );

    msgpack_zone mempool;
    msgpack_object deserialized;
    
    int i =0;

    if( packData != NULL && packData->count != 0 ) {
	int count = packData->count;

  	msgpack_pack_map( &pk, 2);
        __msgpack_pack_string( &pk, PORTMAPPING_PARAMETERS.name, PORTMAPPING_PARAMETERS.length );
	msgpack_pack_array( &pk, count );
        
	msgpack_pack_map( &pk, 3); //name, value, type

	for( i = 0; i < count; i++ ) //1 element
	{
	    struct portmapping_token PORTMAPPING_MAP_NAME;

            PORTMAPPING_MAP_NAME.name = "name";
            PORTMAPPING_MAP_NAME.length = strlen( "name" );
            __msgpack_pack_string_nvp( &pk, &PORTMAPPING_MAP_NAME, "Device.NAT.PortMapping." );

	    struct portmapping_token PORTMAPPING_MAP_VALUE;

            PORTMAPPING_MAP_VALUE.name = "value";
            PORTMAPPING_MAP_VALUE.length = strlen( "value" );
	    __msgpack_pack_string_nvp( &pk, &PORTMAPPING_MAP_VALUE, blob );

	    struct portmapping_token PORTMAPPING_MAP_TYPE;

            PORTMAPPING_MAP_TYPE.name = "datatype";
            PORTMAPPING_MAP_TYPE.length = strlen( "datatype" );
             __msgpack_pack_string( &pk, PORTMAPPING_MAP_TYPE.name, PORTMAPPING_MAP_TYPE.length );
	    msgpack_pack_int(&pk, 2 );
	}

	struct portmapping_token PORTMAPPING_MAP_VERSION;

        PORTMAPPING_MAP_VERSION.name = "version";
        PORTMAPPING_MAP_VERSION.length = strlen( "version" );
	__msgpack_pack_string_nvp( &pk, &PORTMAPPING_MAP_VERSION, "154363892090392891829182011" );
       
        

    } else {
        printf("parameters is NULL\n" );
        return rv;
    }

    if( sbuf.data ) {
        *data = ( char * ) malloc( sizeof( char ) * sbuf.size );

        if( NULL != *data ) {
            memcpy( *data, sbuf.data, sbuf.size );
	    printf("sbuf.data is %s sbuf.size %ld\n", sbuf.data, sbuf.size);
            rv = sbuf.size;
        }
    }

    msgpack_zone_init(&mempool, 2048);
    
    printf("the value is :");
    msgpack_unpack(sbuf.data, sbuf.size, NULL, &mempool, &deserialized);
    msgpack_object_print(stdout, deserialized);

    msgpack_zone_destroy(&mempool);
    msgpack_sbuffer_destroy( &sbuf );
    return rv;
}






