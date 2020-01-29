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
#include "xdns.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    XDNS_OK                      = HELPERS_OK,
    XDNS_OUT_OF_MEMORY           = HELPERS_OUT_OF_MEMORY,
    XDNS_INVALID_FIRST_ELEMENT   = HELPERS_INVALID_FIRST_ELEMENT,
    XDNS_MISSING_XDNS_ENTRY      = HELPERS_MISSING_WRAPPER,
    XDNS_INVALID_DEFAULT_IPV6,
    XDNS_INVALID_DEFAULT_IPV4,
    XDNS_MISSING_DEFAULT_IPV6,
    XDNS_MISSING_DEFAULT_IPV4,
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_xdns( xdns_t *xdns, msgpack_object *obj );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See xdns.h for details. */
xdns_t* xdns_convert( const void *buf, size_t len )
{
    return helper_convert( buf, len, sizeof(xdns_t), "xdns",
                           MSGPACK_OBJECT_MAP, true,
                           (process_fn_t) process_xdns,
                           (destroy_fn_t) xdns_destroy );
}

/* See xdns.h for details. */
void xdns_destroy( xdns_t *xdns )
{
    if( NULL != xdns ) {
        free( xdns );
    }
}

/* See xdns.h for details. */
const char* xdns_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = XDNS_OK,                     .txt = "No errors." },
        { .v = XDNS_OUT_OF_MEMORY,          .txt = "Out of memory." },
        { .v = XDNS_INVALID_FIRST_ELEMENT,  .txt = "Invalid first element." },
        { .v = XDNS_MISSING_XDNS_ENTRY,     .txt = "'xdns' element missing." },
        { .v = XDNS_INVALID_DEFAULT_IPV6,   .txt = "Invalid 'default-ipv6' value." },
        { .v = XDNS_INVALID_DEFAULT_IPV4,   .txt = "Invalid 'default-ipv4' value." },
        { .v = XDNS_MISSING_DEFAULT_IPV6,   .txt = "'default-ipv6' element missing." },
        { .v = XDNS_MISSING_DEFAULT_IPV4,   .txt = "'default-ipv4' element missing." },
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
 *  Convert the msgpack map into the xdns_t structure.
 *
 *  @param xdns xdns pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_xdns( xdns_t *xdns, msgpack_object *obj )
{
    msgpack_object_map *map = &obj->via.map;
    int left = map->size;
    uint8_t objects_left = 0x03;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( MSGPACK_OBJECT_BIN == p->val.type ) {
                if( 0 == match(p, "default-ipv6") ) {
                    if( 16 == p->val.via.bin.size ) {
                        memcpy( &xdns->default_ipv6, p->val.via.bin.ptr, 16 );
                        objects_left &= ~(1 << 0);
                    } else {
                        errno = XDNS_INVALID_DEFAULT_IPV6;
                        return -1;
                    }
                }
            } else if( MSGPACK_OBJECT_POSITIVE_INTEGER == p->val.type ) {
                if( 0 == match(p, "default-ipv4") ) {
                    if( UINT32_MAX < p->val.via.u64 ) {
                        errno = XDNS_INVALID_DEFAULT_IPV4;
                        return -1;
                    }
                    xdns->default_ipv4 = (uint32_t) p->val.via.u64;
                    objects_left &= ~(1 << 1);
                }
            }
        }
        p++;
    }

    if( 1 & objects_left ) {
        errno = XDNS_MISSING_DEFAULT_IPV6;
    } else if( (1 << 1) & objects_left ) {
        errno = XDNS_MISSING_DEFAULT_IPV4;
    } else {
        errno = XDNS_OK;
    }

    return 0;
}
