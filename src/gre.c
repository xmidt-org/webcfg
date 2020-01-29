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
#include "gre.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    GRE_OK                      = HELPERS_OK,
    GRE_OUT_OF_MEMORY           = HELPERS_OUT_OF_MEMORY,
    GRE_INVALID_FIRST_ELEMENT   = HELPERS_INVALID_FIRST_ELEMENT,
    GRE_MISSING_GRE_ENTRY       = HELPERS_MISSING_WRAPPER,
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_gre( gre_t *gre, msgpack_object *obj );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See gre.h for details. */
gre_t* gre_convert( const void *buf, size_t len )
{
    return helper_convert( buf, len, sizeof(gre_t), "gre",
                           MSGPACK_OBJECT_MAP, true,
                           (process_fn_t) process_gre,
                           (destroy_fn_t) gre_destroy );
}

/* See gre.h for details. */
void gre_destroy( gre_t *gre )
{
    if( NULL != gre ) {
        if( NULL != gre->primary_remote_endpoint ) {
            free( gre->primary_remote_endpoint );
        }
        if( NULL != gre->secondary_remote_endpoint ) {
            free( gre->secondary_remote_endpoint );
        }
        free( gre );
    }
}

/* See gre.h for details. */
const char* gre_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = GRE_OK,                      .txt = "No errors." },
        { .v = GRE_OUT_OF_MEMORY,           .txt = "Out of memory." },
        { .v = GRE_INVALID_FIRST_ELEMENT,   .txt = "Invalid first element." },
        { .v = GRE_MISSING_GRE_ENTRY,       .txt = "'gre' element missing." },
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
 *  Convert the msgpack map into the gre_t structure.
 *
 *  @param gre gre pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_gre( gre_t *gre, msgpack_object *obj )
{
    msgpack_object_map *map = &obj->via.map;
    int left = map->size;
    uint8_t objects_left = 0x03;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( MSGPACK_OBJECT_STR == p->val.type ) {
                if( 0 == match(p, "primary-remote-endpoint") ) {
                    gre->primary_remote_endpoint = strndup( p->val.via.str.ptr, p->val.via.str.size );
                    objects_left &= ~(1 << 0);
                } else if( 0 == match(p, "secondary-remote-endpoint") ) {
                    gre->secondary_remote_endpoint = strndup( p->val.via.str.ptr, p->val.via.str.size );
                    objects_left &= ~(1 << 1);
                }
            }
        }
        p++;
    }

    errno = GRE_OK;

    return 0;
}
