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
#include "firewall.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    FIREWALL_OK                      = HELPERS_OK,
    FIREWALL_OUT_OF_MEMORY           = HELPERS_OUT_OF_MEMORY,
    FIREWALL_INVALID_FIRST_ELEMENT   = HELPERS_INVALID_FIRST_ELEMENT,
    FIREWALL_MISSING_FIREWALL_ENTRY  = HELPERS_MISSING_WRAPPER,
    FIREWALL_INVALID_FILTERS,
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_firewall( firewall_t *firewall, msgpack_object *obj );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See firewall.h for details. */
firewall_t* firewall_convert( const void *buf, size_t len )
{
    return helper_convert( buf, len, sizeof(firewall_t), "firewall",
                           MSGPACK_OBJECT_MAP, true,
                           (process_fn_t) process_firewall,
                           (destroy_fn_t) firewall_destroy );
}

/* See firewall.h for details. */
void firewall_destroy( firewall_t *firewall )
{
    if( NULL != firewall ) {
        size_t i;

        if( NULL != firewall->level ) {
            free( firewall->level );
        }
        for( i = 0; i < firewall->filters_count; i++ ) {
            if( NULL != firewall->filters[i] ) {
                free( firewall->filters[i] );
            }
        }
        if( NULL != firewall->filters ) {
            free( firewall->filters );
        }
        free( firewall );
    }
}

/* See firewall.h for details. */
const char* firewall_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = FIREWALL_OK,                     .txt = "No errors." },
        { .v = FIREWALL_OUT_OF_MEMORY,          .txt = "Out of memory." },
        { .v = FIREWALL_INVALID_FIRST_ELEMENT,  .txt = "Invalid first element." },
        { .v = FIREWALL_MISSING_FIREWALL_ENTRY, .txt = "'firewall' element missing." },
        { .v = FIREWALL_INVALID_FILTERS,        .txt = "'filters' element is invalid." },
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
 *  Convert the msgpack map into the firewall_t structure.
 *
 *  @param firewall firewall pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_firewall( firewall_t *firewall, msgpack_object *obj )
{
    msgpack_object_map *map = &obj->via.map;
    int left = map->size;
    uint8_t objects_left = 0x03;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( MSGPACK_OBJECT_STR == p->val.type ) {
                if( 0 == match(p, "level") ) {
                    firewall->level = strndup( p->val.via.str.ptr, p->val.via.str.size );
                    if( NULL == firewall->level ) {
                        errno = FIREWALL_OUT_OF_MEMORY;
                        return -1;
                    }
                    objects_left &= ~(1 << 0);
                }
            } else if( MSGPACK_OBJECT_ARRAY == p->val.type ) {
                if( 0 == match(p, "filters") ) {
                    uint32_t i;
                    msgpack_object_array *array = &p->val.via.array;
                    for( i = 0; i < array->size; i++ ) {
                        if( MSGPACK_OBJECT_STR != array->ptr[i].type ) {
                            errno = FIREWALL_INVALID_FILTERS;
                            return -1;
                        }
                    }
                    firewall->filters = (char**) malloc( array->size * sizeof(char*) );
                    if( NULL == firewall->filters ) {
                        errno = FIREWALL_OUT_OF_MEMORY;
                        return -1;
                    }
                    memset( firewall->filters, 0, array->size * sizeof(char*) );
                    firewall->filters_count = array->size;
                    for( i = 0; i < array->size; i++ ) {
                        firewall->filters[i] = strndup( array->ptr[i].via.str.ptr, array->ptr[i].via.str.size );
                        if( NULL == firewall->filters[i] ) {
                            errno = FIREWALL_OUT_OF_MEMORY;
                            return -1;
                        }
                    }
                    objects_left &= ~(1 << 1);
                }
            }
        }
        p++;
    }

    errno = FIREWALL_OK;

    return 0;
}
