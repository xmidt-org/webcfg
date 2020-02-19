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
#include "portmappingparam.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    PM_OK                       = HELPERS_OK,
    PM_OUT_OF_MEMORY            = HELPERS_OUT_OF_MEMORY,
    PM_INVALID_FIRST_ELEMENT    = HELPERS_INVALID_FIRST_ELEMENT,
    PM_MISSING_PM_ENTRY         = HELPERS_MISSING_WRAPPER,
    PM_INVALID_PORT_RANGE,
    PM_INVALID_PORT_NUMBER,
    PM_INVALID_INTERNAL_IPV4,
    PM_BOTH_IPV4_AND_IPV6_TARGETS_EXIST,
    PM_INVALID_IPV6,
    PM_MISSING_INTERNAL_PORT,
    PM_INVALID_DATATYPE,
    PM_MISSING_TARGET_IP,
    PM_MISSING_PORT_RANGE,
    PM_MISSING_PROTOCOL,
    PM_INVALID_PM_OBJECT,
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
//int process_portrange( pm_entry_t *e, msgpack_object_array *array );
int process_entry( pm_entry_t *e, msgpack_object_map *map );
int process_portmapping( portmapping_t *pm, msgpack_object *obj );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See portmapping.h for details. */
portmapping_t* portmapping_convert( const void *buf, size_t len )
{
    return helper_convert( buf, len, sizeof(portmapping_t), "parameters",
                           MSGPACK_OBJECT_ARRAY, true,
                           (process_fn_t) process_portmapping,
                           (destroy_fn_t) portmapping_destroy );
}

/* See portmapping.h for details. */
void portmapping_destroy( portmapping_t *pm )
{
     if( NULL != pm ) {
        size_t i;
        for( i = 0; i < pm->entries_count; i++ ) {
            if( NULL != pm->entries[i].name ) {
                free( pm->entries[i].name );
            }
	    if( NULL != pm->entries[i].value ) {
                free( pm->entries[i].value );
            }
        }
        if( NULL != pm->entries ) {
            free( pm->entries );
        }
        free( pm );
    }
}

/* See portmapping.h for details. */
const char* portmapping_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = PM_OK,                               .txt = "No errors." },
        { .v = PM_OUT_OF_MEMORY,                    .txt = "Out of memory." },
        { .v = PM_INVALID_FIRST_ELEMENT,            .txt = "Invalid first element." },
        { .v = PM_MISSING_PM_ENTRY,                 .txt = "'port-mapping' element missing." },
        { .v = PM_INVALID_PORT_RANGE,               .txt = "Invalid 'external-port-range' value." },
        { .v = PM_INVALID_PORT_NUMBER,              .txt = "Invalid 'target-port' value." },
        { .v = PM_INVALID_INTERNAL_IPV4,            .txt = "Invalid 'target-ipv4' value." },
        { .v = PM_BOTH_IPV4_AND_IPV6_TARGETS_EXIST, .txt = "Conflict: both 'target-ipv4' and 'target-ipv6' exist." },
        { .v = PM_INVALID_IPV6,                     .txt = "Invalid 'target-ipv6' value." },
        { .v = PM_MISSING_INTERNAL_PORT,            .txt = "'target-port' element missing." },
        { .v = PM_MISSING_TARGET_IP,                .txt = "'target-ipv4|6' element missing." },
        { .v = PM_MISSING_PORT_RANGE,               .txt = "'external-port-range' element missing." },
        { .v = PM_MISSING_PROTOCOL,                 .txt = "'protocol' element missing." },
        { .v = PM_INVALID_PM_OBJECT,                .txt = "Invalid 'port-mapping' array." },
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
 *  @param e     the entry pointer
 *  @param array the msgpack array pointer
 *
 *  @return 0 on success, error otherwise
 */
/*
int process_portrange( pm_entry_t *e, msgpack_object_array *array )
{
    if( (2 == array->size) &&
        (MSGPACK_OBJECT_POSITIVE_INTEGER == array->ptr[0].type) &&
        (MSGPACK_OBJECT_POSITIVE_INTEGER == array->ptr[1].type) &&
        (array->ptr[0].via.u64 <= UINT16_MAX) &&
        (array->ptr[1].via.u64 <= UINT16_MAX) )
    {
        e->port_range[0] = (uint16_t) array->ptr[0].via.u64;
        e->port_range[1] = (uint16_t) array->ptr[1].via.u64;
        return 0;
    }

    errno = PM_INVALID_PORT_RANGE;
    return -1;
} */


/**
 *  Convert the msgpack map into the pm_entry_t structure.
 *
 *  @param e    the entry pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_entry( pm_entry_t *e, msgpack_object_map *map )
/*{
    int left = map->size;
    uint8_t objects_left = 0x07;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( MSGPACK_OBJECT_POSITIVE_INTEGER == p->val.type ) {
                if( 0 == match(p, "Internal_client") ) {
                    if( UINT16_MAX < p->val.via.u64 ) {
                        errno = PM_INVALID_PORT_NUMBER;
                        return -1;
                    } else {
                        e->target_port = (uint16_t) p->val.via.u64;
                    }
                    objects_left &= ~(1 << 0);
                } else if( 0 == match(p, "target-ipv4") ) {
                    if( 0 != e->ip_version ) {
                        errno = PM_BOTH_IPV4_AND_IPV6_TARGETS_EXIST;
                        return -1;
                    }
                    if( UINT32_MAX < p->val.via.u64 ) {
                        errno = PM_INVALID_INTERNAL_IPV4;
                        return -1;
                    } else {
                        e->ip.v4 = (uint32_t) p->val.via.u64;
                        e->ip_version = 4;
                    }
                    objects_left &= ~(1 << 1);
                }
            } else if( MSGPACK_OBJECT_ARRAY == p->val.type ) {
                if( 0 == match(p, "external-port-range") ) {
                    if( 0 != process_portrange(e, &p->val.via.array) ) {
                        return -1;
                    }
                    objects_left &= ~(1 << 2);
                }
            } else if( MSGPACK_OBJECT_STR == p->val.type ) {
                if( 0 == match(p, "protocol") ) {
                    e->protocol = strndup( p->val.via.str.ptr, p->val.via.str.size );
                    objects_left &= ~(1 << 3);
                }
            } else if( MSGPACK_OBJECT_BIN == p->val.type ) {
                if( 0 == match(p, "target-ipv6") ) {
                    if( 0 != e->ip_version ) {
                        errno = PM_BOTH_IPV4_AND_IPV6_TARGETS_EXIST;
                        return -1;
                    }
                    if( 16 == p->val.via.bin.size ) {
                        memcpy( &e->ip.v6, p->val.via.bin.ptr, 16 );
                        e->ip_version = 6;
                        objects_left &= ~(1 << 1);
                    } else {
                        errno = PM_INVALID_IPV6;
                        return -1;
                    }
                }
            }
        }
        p++;
    }

    if( 1 & objects_left ) {
        errno = PM_MISSING_INTERNAL_PORT;
    } else if( (1 << 1) & objects_left ) {
        errno = PM_MISSING_TARGET_IP;
    } else if( (1 << 2) & objects_left ) {
        errno = PM_MISSING_PORT_RANGE;
    } else if( (1 << 3) & objects_left ) {
        errno = PM_MISSING_PROTOCOL;
    } else {
        errno = PM_OK;
    }

    return (0 == objects_left) ? 0 : -1;
}
*/
{
    int left = map->size;
    uint8_t objects_left = 0x03;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( MSGPACK_OBJECT_POSITIVE_INTEGER == p->val.type ) {
                if( 0 == match(p, "dataType") ) {
                    if( UINT16_MAX < p->val.via.u64 ) {
			printf("e->type is %d\n", e->type);
                        errno = PM_INVALID_DATATYPE;
                        return -1;
                    } else {
                        e->type = (uint16_t) p->val.via.u64;
			printf("e->type is %d\n", e->type);
                    }
                    objects_left &= ~(1 << 0);
		    printf("objects_left after datatype %d\n", objects_left);
                }
            } else if( MSGPACK_OBJECT_STR == p->val.type ) {
                if( 0 == match(p, "name") ) {
                    e->name = strndup( p->val.via.str.ptr, p->val.via.str.size );
		    printf("e->name is %s\n", e->name);
                    objects_left &= ~(1 << 1);
		    printf("objects_left after name %d\n", objects_left);
                }
		if( 0 == match(p, "value") ) {
                    e->value = strndup( p->val.via.str.ptr, p->val.via.str.size );
		    printf("e->value is %s\n", e->value);
                    objects_left &= ~(1 << 2);
		    printf("objects_left after value %d\n", objects_left);
                }
	
            }
        }
        p++;
    }

    if( 1 & objects_left ) {
    } else {
        errno = PM_OK;
    }

    return (0 == objects_left) ? 0 : -1;
}
int process_portmapping( portmapping_t *pm, msgpack_object *obj )
{ 

    printf("in process \n");
    msgpack_object_array *array = &obj->via.array;
    if( 0 < array->size ) {
        size_t i;

        pm->entries_count = array->size;
        pm->entries = (pm_entry_t *) malloc( sizeof(pm_entry_t) * pm->entries_count );

        printf("pm->entries_count is %ld\n",pm->entries_count);
        if( NULL == pm->entries ) {
            pm->entries_count = 0;
            return -1;
        }

        memset( pm->entries, 0, sizeof(pm_entry_t) * pm->entries_count );

        for( i = 0; i < pm->entries_count; i++ ) {
            if( MSGPACK_OBJECT_MAP != array->ptr[i].type ) {
                errno = PM_INVALID_PM_OBJECT;
                return -1;
            }
            if( 0 != process_entry(&pm->entries[i], &array->ptr[i].via.map) ) {
                return -1;
            }
        }
    }

    return 0;
}
