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
#include <msgpack.h>

#include "helpers.h"
#include "full.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    FULL_OK                      = HELPERS_OK,
    FULL_OUT_OF_MEMORY           = HELPERS_OUT_OF_MEMORY,
    FULL_INVALID_FIRST_ELEMENT   = HELPERS_INVALID_FIRST_ELEMENT,
    FULL_MISSING_FULL_ENTRY      = HELPERS_MISSING_WRAPPER,
    FULL_INVALID_SUBSYSTEMS,
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_full( full_t *full, msgpack_object *obj );
int process_subsystems( full_t *full, msgpack_object_array *array );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See full.h for details. */
full_t* full_convert( const void *buf, size_t len )
{
    return helper_convert( buf, len, sizeof(full_t), "full",
                           MSGPACK_OBJECT_MAP, true,
                           (process_fn_t) process_full,
                           (destroy_fn_t) full_destroy );
}

/* See full.h for details. */
void full_destroy( full_t *full )
{
    if( NULL != full ) {
        if( NULL != full->subsystems ) {
            size_t i;

            for( i = 0; i < full->subsystems_count; i++ ) {
                if( NULL != full->subsystems[i].url ) {
                    free( full->subsystems[i].url );
                }
                if( NULL != full->subsystems[i].payload ) {
                    free( full->subsystems[i].payload );
                }
            }

            free( full->subsystems );
        }
        free( full );
    }
}

/* See full.h for details. */
const char* full_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = FULL_OK,                      .txt = "No errors." },
        { .v = FULL_OUT_OF_MEMORY,           .txt = "Out of memory." },
        { .v = FULL_INVALID_FIRST_ELEMENT,   .txt = "Invalid first element." },
        { .v = FULL_MISSING_FULL_ENTRY,      .txt = "'full' element missing." },
        { .v = 0, .txt = NULL }
    };
    int i = 0;

    while( (map[i].v != errnum) && (NULL != map[i].txt) ) { i++; }

    if( NULL == map[i].txt ) {
        return "Unknown error.";
    }

    return map[i].txt;
}

/* See full.h for details. */
const char* full_get_schema_version( void ) {
    return "full-1.0.0";
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Convert the msgpack map into the full_t structure.
 *
 *  @param full full pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_full( full_t *full, msgpack_object *obj )
{
    msgpack_object_map *map = &obj->via.map;
    int left = map->size;
    uint8_t objects_left = 0x01;
    msgpack_object_kv *p;

    (void) full;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( MSGPACK_OBJECT_ARRAY == p->val.type ) {
                if( 0 == match(p, "subsystems") ) {
                    if( 0 != process_subsystems(full, &p->val.via.array) ) {
                        return -1;
                    }
                    objects_left &= ~(1 << 0);
                }
            }
        }
        p++;
    }

    errno = FULL_OK;

    return 0;
}

int process_subsystems( full_t *full, msgpack_object_array *array )
{
    if( 0 < array->size ) {
        uint32_t i;

        full->subsystems_count = array->size;
        full->subsystems = (subsystem_t*) malloc( full->subsystems_count * sizeof(subsystem_t) );
        if( NULL == full->subsystems ) {
            errno = FULL_OUT_OF_MEMORY;
            return -1;
        }

        memset( full->subsystems, 0, full->subsystems_count * sizeof(subsystem_t) );

        for( i = 0; i < array->size; i++ ) {
            if( MSGPACK_OBJECT_MAP == array->ptr[i].type ) {
                uint8_t objects_left = 0x03;
                msgpack_object_kv *p;
                int left;

                left = array->ptr[i].via.map.size;
                p = array->ptr[i].via.map.ptr;
                while( (0 < objects_left) && (0 < left--) ) {
                    if( MSGPACK_OBJECT_STR == p->key.type ) {
                        if( MSGPACK_OBJECT_STR == p->val.type ) {
                            if( 0 == match(p, "url") ) {
                                full->subsystems[i].url = strndup( p->val.via.str.ptr, p->val.via.str.size );
                                if( NULL == full->subsystems[i].url ) {
                                    errno = FULL_OUT_OF_MEMORY;
                                    return -1;
                                }
                                objects_left &= ~(1 << 0);
                            }
                        } else if( MSGPACK_OBJECT_BIN == p->val.type ) {
                            if( 0 == match(p, "payload") ) {
                                full->subsystems[i].payload_len = p->val.via.bin.size;
                                if( 0 < p->val.via.bin.size ) {
                                    full->subsystems[i].payload = (uint8_t*) malloc( p->val.via.bin.size );
                                    if( NULL == full->subsystems[i].payload ) {
                                        errno = FULL_OUT_OF_MEMORY;
                                        return -1;
                                    }
                                    memcpy( full->subsystems[i].payload, p->val.via.bin.ptr, p->val.via.bin.size );
                                }
                                objects_left &= ~(1 << 1);
                            }
                        }
                    }
                    p++;
                }

                if( 0 < objects_left ) {
                    errno = FULL_INVALID_SUBSYSTEMS;
                    return -1;
                }
            } else {
                errno = FULL_INVALID_SUBSYSTEMS;
                return -1;
            }
        }
    }

    return 0;
}
