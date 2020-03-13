/*
 * Copyright 2020 Comcast Cable Communications Management, LLC
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
#include <stdlib.h>
#include <msgpack.h>

#include "helpers.h"
#include "multipart.h"
#include "webcfgparam.h"
#include "webcfg_log.h"
#include "webcfg_db.h"
#include "webcfg_generic.h"
#include "webcfgpack.h"
#include "macbindingdoc.h"
#include "portmappingdoc.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    WD_OK                       = HELPERS_OK,
    WD_OUT_OF_MEMORY            = HELPERS_OUT_OF_MEMORY,
    WD_INVALID_FIRST_ELEMENT    = HELPERS_INVALID_FIRST_ELEMENT,
    WD_INVALID_DATATYPE,
    WD_INVALID_WD_OBJECT,
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_webcfgdbparams( webconfig_db_data_t *e, msgpack_object_map *map );
int process_webcfgdb( webconfig_db_t *pm, msgpack_object *obj );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

int initDB(char * db_file_path )
{
     FILE *fp;
     char *data;
     size_t len;
     int ch_count=0;
     printf("DB file path si %s\n", db_file_path);
     fp = fopen(db_file_path,"rb");

     if (fp == NULL)
     {
	WebConfigLog("Failed to open file %s\n", db_file_path);
	return 0;
     }
     
     fseek(fp, 0, SEEK_END);
     ch_count = ftell(fp);
     fseek(fp, 0, SEEK_SET);
     data = (char *) malloc(sizeof(char) * (ch_count + 1));
     fread(data, 1, ch_count-1,fp);
     len = ch_count;
     (data)[ch_count] ='\0';
     fclose(fp);

     wd = decodeData((void *)data, len);
     WebConfigLog("Size of wd %ld\n", (size_t)wd);
     return 1;
     
}

int addNewDocEntry(webconfig_db_t *subdoc)
{    
     size_t webcfgdbPackSize = -1;
     void* data = NULL;
 
     printf("size of subdoc %zu\n", (size_t)subdoc);
     webcfgdbPackSize = webcfgdb_pack(subdoc, &data);
     printf("size of webcfgdbPackSize %zu\n", webcfgdbPackSize);
     writeToDBFile("webconfig_db.bin",(char *)data);
  
     return 0;
}

int writeToDBFile(char *db_file_path, char *data)
{
	FILE *fp;
	fp = fopen(db_file_path , "w+");
	if (fp == NULL)
	{
		printf("Failed to open file in db %s\n", db_file_path );
		return 0;
	}
	if(data !=NULL)
	{
		fwrite(data, strlen(data), 1, fp);
		fclose(fp);
		return 1;
	}
	else
	{
		printf("WriteToJson failed, Data is NULL\n");
		return 0;
	}
}

webconfig_db_t* decodeData(const void * buf, size_t len)
{
     return helper_convert( buf, len, sizeof(webconfig_db_t),"webcfgdb",
                           MSGPACK_OBJECT_ARRAY, true,
                           (process_fn_t) process_webcfgdb,
                           (destroy_fn_t) webcfgdb_destroy );
}

void webcfgdb_destroy( webconfig_db_t *wd )
{
    if( NULL != wd ) {
        size_t i;
        for( i = 0; i < wd->entries_count; i++ ) {
            if( NULL != wd->entries[i].name ) {
                free( wd->entries[i].name );
            }
	    
        }
        if( NULL != wd->entries ) {
            free( wd->entries );
        }
        free( wd );
    }
}

const char* webcfgdbparam_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = WD_OK,                               .txt = "No errors." },
        { .v = WD_OUT_OF_MEMORY,                    .txt = "Out of memory." },
        { .v = WD_INVALID_FIRST_ELEMENT,            .txt = "Invalid first element." },
        { .v = WD_INVALID_DATATYPE,                 .txt = "Invalid 'datatype' value." },
        { .v = WD_INVALID_WD_OBJECT,                .txt = "Invalid 'parameters' array." },
        { .v = 0, .txt = NULL }
    };
    int i = 0;

    while( (map[i].v != errnum) && (NULL != map[i].txt) ) { i++; }

    if( NULL == map[i].txt ) {
        return "Unknown error.";
    }

    return map[i].txt;
}
/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Convert the msgpack map into the webconfig_db_data_t structure.
 *
 *  @param e    the entry pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_webcfgdbparams( webconfig_db_data_t *e, msgpack_object_map *map )
{
    int left = map->size;
    uint8_t objects_left = 0x02;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) )
    {
        if( MSGPACK_OBJECT_STR == p->key.type )
        {
            if( MSGPACK_OBJECT_POSITIVE_INTEGER == p->val.type )
            {
                if( 0 == match(p, "version") )
                {
                    if( UINT32_MAX < p->val.via.u64 )
                    {
			//printf("e->type is %d\n", e->type);
                        errno = WD_INVALID_DATATYPE;
                        return -1;
                    }
                    else
                    {
                        e->version = (uint32_t) p->val.via.u64;
			//printf("e->version is %d\n", e->version);
                    }
                    objects_left &= ~(1 << 1);
		    //printf("objects_left after datatype %d\n", objects_left);
                }
                
            }
            else if( MSGPACK_OBJECT_STR == p->val.type )
            {
                if( 0 == match(p, "name") )
                {
                    e->name = strndup( p->val.via.str.ptr, p->val.via.str.size );
		    //printf("e->name is %s\n", e->name);
                    objects_left &= ~(1 << 0);
		    //printf("objects_left after name %d\n", objects_left);
                }
            }
        }
        p++;
    }

    if( 1 & objects_left )
    {
    }
    else
    {
        errno = WD_OK;
    }

    return (0 == objects_left) ? 0 : -1;
}

int process_webcfgdb( webconfig_db_t *wd, msgpack_object *obj )
{
    //printf(" process_webcfgdbparam \n");
    msgpack_object_array *array = &obj->via.array;
    if( 0 < array->size )
    {
        size_t i;

        wd->entries_count = array->size;
        wd->entries = (webconfig_db_data_t *) malloc( sizeof(webconfig_db_data_t) * wd->entries_count );
        if( NULL == wd->entries )
        {
            wd->entries_count = 0;
            return -1;
        }
        
        //printf("wd->entries_count %lu\n",wd->entries_count);
        memset( wd->entries, 0, sizeof(webconfig_db_data_t) * wd->entries_count );
        for( i = 0; i < wd->entries_count; i++ )
        {
            if( MSGPACK_OBJECT_MAP != array->ptr[i].type )
            {
                errno = WD_INVALID_WD_OBJECT;
                return -1;
            }
            if( 0 != process_webcfgdbparams(&wd->entries[i], &array->ptr[i].via.map) )
            {
		//printf("process_webcfgdbparam failed\n");
                return -1;
            }
        }
    }

    return 0;
}
