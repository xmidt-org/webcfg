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

#include "envelope.h"
#include "helpers.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    ENV_OK                              = HELPERS_OK,
    ENV_OUT_OF_MEMORY                   = HELPERS_OUT_OF_MEMORY,
    ENV_INVALID_FIRST_ELEMENT           = HELPERS_INVALID_FIRST_ELEMENT,
    ENV_MISSING_SCHEMA_ELEMENT,
    ENV_MISSING_SCHEMA_BASE_ELEMENT,
    ENV_MISSING_SCHEMA_MAJOR_ELEMENT,
    ENV_MISSING_SCHEMA_MINOR_ELEMENT,
    ENV_MISSING_SCHEMA_PATCH_ELEMENT,
    ENV_MISSING_SHA256_ELEMENT,
    ENV_MISSING_PAYLOAD_ELEMENT
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_schema( schema_t *s, msgpack_object_map *map );
int process_env( envelope_t *e, msgpack_object *obj );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See envelope.h for details. */
envelope_t* envelope_convert( const void *buf, size_t len )
{
    return helper_convert( buf, len, sizeof(envelope_t), NULL,
                           MSGPACK_OBJECT_MAP, false,
                           (process_fn_t) process_env,
                           (destroy_fn_t) envelope_destroy );
}

/* See envelope.h for details. */
void envelope_destroy( envelope_t *env )
{
    if( NULL != env ) {
        if( NULL != env->schema.base ) {
            free( env->schema.base );
        }
        if( NULL != env->payload ) {
            free( env->payload );
        }
        free( env );
    }
}

/* See envelope.h for details. */
const char* envelope_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = ENV_OK,                              .txt = "No errors." },
        { .v = ENV_OUT_OF_MEMORY,                   .txt = "Out of memory." },
        { .v = ENV_INVALID_FIRST_ELEMENT,           .txt = "Invalid first element." },
        { .v = ENV_MISSING_SCHEMA_ELEMENT,          .txt = "'schema' element missing." },
        { .v = ENV_MISSING_SCHEMA_BASE_ELEMENT,     .txt = "'base' element missing." },
        { .v = ENV_MISSING_SCHEMA_MAJOR_ELEMENT,    .txt = "'major' element missing." },
        { .v = ENV_MISSING_SCHEMA_MINOR_ELEMENT,    .txt = "'minor' element missing." },
        { .v = ENV_MISSING_SCHEMA_PATCH_ELEMENT,    .txt = "'patch' element missing." },
        { .v = ENV_MISSING_SHA256_ELEMENT,          .txt = "'sha256' element missing." },
        { .v = ENV_MISSING_PAYLOAD_ELEMENT,         .txt = "'payload' element missing." },
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
 *  Convert the msgpack map into the schema_t structure.
 *
 *  @param s    schema pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_schema( schema_t *s, msgpack_object_map *map )
{
    int size = map->size;
    uint8_t objects_left = 0x0f;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < size--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( (MSGPACK_OBJECT_STR == p->val.type) && (0 == match(p, "base")) ) {
                objects_left &= ~(1 << 0);
                s->base = strndup( p->val.via.str.ptr, p->val.via.str.size );
                if( NULL == s->base ) {
                    errno = ENV_OUT_OF_MEMORY;
                    return -1;
                }
            } else if( MSGPACK_OBJECT_POSITIVE_INTEGER == p->val.type ) {
                if( 0 == match(p, "major") ) {
                    s->major = p->val.via.u64;
                    objects_left &= ~(1 << 1);
                } else if( 0 == match(p, "minor") ) {
                    s->minor = p->val.via.u64;
                    objects_left &= ~(1 << 2);
                } else if( 0 == match(p, "patch") ) {
                    s->patch = p->val.via.u64;
                    objects_left &= ~(1 << 3);
                }
            }
        }

        p++;
    }

    if( 1 & objects_left ) {
        errno = ENV_MISSING_SCHEMA_BASE_ELEMENT;
    } else if( (1 << 1) & objects_left ) {
        errno = ENV_MISSING_SCHEMA_MAJOR_ELEMENT;
    } else if( (1 << 2) & objects_left ) {
        errno = ENV_MISSING_SCHEMA_MINOR_ELEMENT;
    } else if( (1 << 3) & objects_left ) {
        errno = ENV_MISSING_SCHEMA_PATCH_ELEMENT;
    } else {
        errno = ENV_OK;
    }

    return (0 == objects_left) ? 0 : -1;
}

/**
 *  Convert the msgpack map into the envelope_t structure.
 *
 *  @param e    envelope pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_env( envelope_t *e, msgpack_object *obj )
{
    msgpack_object_map *map = &obj->via.map;
    int size = map->size;
    uint8_t objects_left = 0x07;
    msgpack_object_kv *p;
    size_t sha256_size = member_size(envelope_t, sha256);

    p = map->ptr;
    while( (0 < objects_left) && (0 < size--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( (MSGPACK_OBJECT_MAP == p->val.type) && (0 == match(p, "schema")) ) {
                if( 0 != process_schema( &e->schema, &p->val.via.map) ) {
                    return -1;
                }
                objects_left &= ~(1 << 0);
            } else if( MSGPACK_OBJECT_BIN == p->val.type ) {
                if( (sha256_size == p->val.via.bin.size) && (0 == match(p, "sha256")) ) {
                    memcpy( e->sha256, p->val.via.bin.ptr, sha256_size );
                    objects_left &= ~(1 << 1);
                } else if( 0 == match(p, "payload") ) {
                    e->len = p->val.via.bin.size;
                    e->payload = malloc( e->len );
                    if( NULL == e->payload ) {
                        errno = ENV_OUT_OF_MEMORY;
                        return -1;
                    }
                    memcpy( e->payload, p->val.via.bin.ptr, e->len );
                    objects_left &= ~(1 << 2);
                }
            }
        }
        p++;
    }

    if( 1 & objects_left ) {
        errno = ENV_MISSING_SCHEMA_ELEMENT;
    } else if( (1 << 1) & objects_left ) {
        errno = ENV_MISSING_SHA256_ELEMENT;
    } else if( (1 << 2) & objects_left ) {
        errno = ENV_MISSING_PAYLOAD_ELEMENT;
    } else {
        errno = ENV_OK;
    }

    return (0 == objects_left) ? 0 : -1;
}
