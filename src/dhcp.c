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
#include "dhcp.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    DHCP_OK                         = HELPERS_OK,
    DHCP_OUT_OF_MEMORY              = HELPERS_OUT_OF_MEMORY,
    DHCP_INVALID_FIRST_ELEMENT      = HELPERS_INVALID_FIRST_ELEMENT,
    DHCP_MISSING_DHCP_ENTRY         = HELPERS_MISSING_WRAPPER,
    DHCP_MISSING_POOL_RANGE,
    DHCP_INVALID_ROUTER_ADDRESS,
    DHCP_INVALID_SUBNET_MASK,
    DHCP_INVALID_LEASE_LENGTH,
    DHCP_MISSING_ROUTER_ADDRESS,
    DHCP_MISSING_SUBNET_MASK,
    DHCP_MISSING_LEASE_LENGTH,
    DHCP_INVALID_POOL_RANGE,
    DHCP_INVALID_STATIC_IP,
    DHCP_INVALID_STATIC_MAC,
    DHCP_INVALID_STATIC_INVALID,
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_pool( dhcp_t *dhcp, msgpack_object_array *array );
int process_static( dhcp_t *dhcp, msgpack_object_array *array );
int process_dhcp( dhcp_t *dhcp, msgpack_object *obj );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See dhcp.h for details. */
dhcp_t* dhcp_convert( const void *buf, size_t len )
{
    return helper_convert( buf, len, sizeof(dhcp_t), "dhcp",
                           MSGPACK_OBJECT_MAP, true,
                           (process_fn_t) process_dhcp,
                           (destroy_fn_t) dhcp_destroy );
}

/* See dhcp.h for details. */
void dhcp_destroy( dhcp_t *dhcp )
{
    if( NULL != dhcp ) {
        if( NULL != dhcp->fixed ) {
            free( dhcp->fixed );
        }
        free( dhcp );
    }
}

/* See dhcp.h for details. */
const char* dhcp_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = DHCP_OK,                         .txt = "No errors." },
        { .v = DHCP_OUT_OF_MEMORY,              .txt = "Out of memory." },
        { .v = DHCP_INVALID_FIRST_ELEMENT,      .txt = "Invalid first element." },
        { .v = DHCP_MISSING_DHCP_ENTRY,         .txt = "'dhcp' element missing." },
        { .v = DHCP_MISSING_POOL_RANGE,         .txt = "'pool-range' element missing." },
        { .v = DHCP_MISSING_ROUTER_ADDRESS,     .txt = "'router-ip' element missing." },
        { .v = DHCP_MISSING_SUBNET_MASK,        .txt = "'subnet-mask' element missing." },
        { .v = DHCP_MISSING_LEASE_LENGTH,       .txt = "'lease-length' element missing." },
        { .v = DHCP_INVALID_ROUTER_ADDRESS,     .txt = "Invalid 'router-ip'." },
        { .v = DHCP_INVALID_SUBNET_MASK,        .txt = "Invalid 'subnet-mask'." },
        { .v = DHCP_INVALID_LEASE_LENGTH,       .txt = "Invalid 'lease-length'." },
        { .v = DHCP_INVALID_POOL_RANGE,         .txt = "Invalid 'pool-range'." },
        { .v = DHCP_INVALID_STATIC_IP,          .txt = "Invalid 'static.ip'." },
        { .v = DHCP_INVALID_STATIC_MAC,         .txt = "Invalid 'static.mac'." },
        { .v = DHCP_INVALID_STATIC_INVALID,     .txt = "Invalid 'static' array." },
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
 *  Converts the msgpack array into the pool_range values.
 *
 *  @param dhcp  dhcp pointer
 *  @param array the msgpack array pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_pool( dhcp_t *dhcp, msgpack_object_array *array )
{
    if( (2 == array->size) &&
        (MSGPACK_OBJECT_POSITIVE_INTEGER == array->ptr[0].type) &&
        (MSGPACK_OBJECT_POSITIVE_INTEGER == array->ptr[1].type) &&
        (array->ptr[0].via.u64 <= UINT32_MAX) &&
        (array->ptr[1].via.u64 <= UINT32_MAX) )
    {
        dhcp->pool_range[0] = array->ptr[0].via.u64;
        dhcp->pool_range[1] = array->ptr[1].via.u64;
        return 0;
    }

    errno = DHCP_INVALID_POOL_RANGE;
    return -1;
}

/**
 *  Converts the msgpack array into the static values.
 *
 *  @param dhcp  dhcp pointer
 *  @param array the msgpack array pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_static( dhcp_t *dhcp, msgpack_object_array *array )
{
    if( 0 < array->size ) {
        uint32_t i;

        dhcp->fixed_count = array->size;
        dhcp->fixed = (dhcp_static_t*) malloc( dhcp->fixed_count * sizeof(dhcp_static_t) );
        if( NULL == dhcp->fixed ) {
            errno = DHCP_OUT_OF_MEMORY;
            return -1;
        }

        memset( dhcp->fixed, 0, dhcp->fixed_count * sizeof(dhcp_static_t) );

        for( i = 0; i < array->size; i++ ) {
            if( MSGPACK_OBJECT_MAP == array->ptr[i].type ) {
                uint8_t objects_left = 0x03;
                msgpack_object_kv *p;
                int left;

                left = array->ptr[i].via.map.size;
                p = array->ptr[i].via.map.ptr;
                while( (0 < objects_left) && (0 < left--) ) {
                    if( MSGPACK_OBJECT_STR == p->key.type ) {
                        if( MSGPACK_OBJECT_POSITIVE_INTEGER == p->val.type ) {
                            if( 0 == match(p, "ip") ) {
                                if( UINT32_MAX < p->val.via.u64 ) {
                                    errno = DHCP_INVALID_STATIC_IP;
                                    return -1;
                                } else {
                                    dhcp->fixed[i].ip = (uint32_t) p->val.via.u64;
                                    objects_left &= ~(1 << 0);
                                }
                            }
                        } else if( MSGPACK_OBJECT_BIN == p->val.type ) {
                            if( 0 == match(p, "mac") ) {
                                if( 6 == p->val.via.bin.size ) {
                                    memcpy( &dhcp->fixed[i].mac, p->val.via.bin.ptr, 6 );
                                    objects_left &= ~(1 << 1);
                                } else {
                                    errno = DHCP_INVALID_STATIC_MAC;
                                    return -1;
                                }
                            }
                        }
                    }
                    p++;
                }
                if( 0 != objects_left ) {
                    errno = DHCP_INVALID_STATIC_INVALID;
                    return -1;
                }
            } else {
                errno = DHCP_INVALID_STATIC_INVALID;
                return -1;
            }
        }
    }

    return 0;
}

/**
 *  Convert the msgpack map into the dhcp_t structure.
 *
 *  @param dhcp dhcp pointer
 *  @param obj  the msgpack obj pointer that is a map
 *
 *  @return 0 on success, error otherwise
 */
int process_dhcp( dhcp_t *dhcp, msgpack_object *obj )
{
    msgpack_object_map *map = &obj->via.map;
    int left = map->size;
    uint8_t objects_left = 0x1f;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( MSGPACK_OBJECT_POSITIVE_INTEGER == p->val.type ) {
                if( 0 == match(p, "router-ip") ) {
                    if( UINT32_MAX < p->val.via.u64 ) {
                        errno = DHCP_INVALID_ROUTER_ADDRESS;
                        return -1;
                    } else {
                        dhcp->router_ip = (uint32_t) p->val.via.u64;
                    }
                    objects_left &= ~(1 << 0);
                } else if( 0 == match(p, "subnet-mask") ) {
                    if( UINT32_MAX < p->val.via.u64 ) {
                        errno = DHCP_INVALID_SUBNET_MASK;
                        return -1;
                    } else {
                        dhcp->subnet_mask = (uint32_t) p->val.via.u64;
                    }
                    objects_left &= ~(1 << 1);
                } else if( 0 == match(p, "lease-length") ) {
                    if( UINT32_MAX < p->val.via.u64 ) {
                        errno = DHCP_INVALID_LEASE_LENGTH;
                        return -1;
                    } else {
                        dhcp->lease_length = (uint32_t) p->val.via.u64;
                    }
                    objects_left &= ~(1 << 2);
                }
            } else if( MSGPACK_OBJECT_ARRAY == p->val.type ) {
                if( 0 == match(p, "static") ) {
                    if( 0 != process_static(dhcp, &p->val.via.array) ) {
                        return -1;
                    }
                    objects_left &= ~(1 << 3);
                } else if( 0 == match(p, "pool-range") ) {
                    if( 0 != process_pool(dhcp, &p->val.via.array) ) {
                        return -1;
                    }
                    objects_left &= ~(1 << 4);
                }
            }
        }
        p++;
    }

    if( 1 & objects_left ) {
        errno = DHCP_MISSING_ROUTER_ADDRESS;
    } else if( (1 << 1) & objects_left ) {
        errno = DHCP_MISSING_SUBNET_MASK;
    } else if( (1 << 2) & objects_left ) {
        errno = DHCP_MISSING_LEASE_LENGTH;
    } else if( (1 << 3) & objects_left ) {
        /* not required */
        objects_left &= ~(1 << 3);
    } else if( (1 << 4) & objects_left ) {
        errno = DHCP_MISSING_POOL_RANGE;
    } else {
        errno = DHCP_OK;
    }

    return (0 == objects_left) ? 0 : -1;
}
