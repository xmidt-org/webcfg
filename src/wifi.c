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
#include "wifi.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    WIFI_OK                      = HELPERS_OK,
    WIFI_OUT_OF_MEMORY           = HELPERS_OUT_OF_MEMORY,
    WIFI_INVALID_FIRST_ELEMENT   = HELPERS_INVALID_FIRST_ELEMENT,
    WIFI_MISSING_WIFI_ENTRY      = HELPERS_MISSING_WRAPPER,
    WIFI_MISSING_5G_CFG,
    WIFI_MISSING_2G_CFG,
};

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int process_wifi_config( wifi_config_t *cfg, msgpack_object_map *map );
int process_wifi( wifi_t *wifi, msgpack_object *obj );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See wifi.h for details. */
wifi_t* wifi_convert( const void *buf, size_t len )
{
    return helper_convert( buf, len, sizeof(wifi_t), "wifi",
                           MSGPACK_OBJECT_MAP, true,
                           (process_fn_t) process_wifi,
                           (destroy_fn_t) wifi_destroy );
}

/* See wifi.h for details. */
void wifi_destroy( wifi_t *wifi )
{
    if( NULL != wifi ) {
        free( wifi );
    }
}

/* See wifi.h for details. */
const char* wifi_strerror( int errnum )
{
    struct error_map {
        int v;
        const char *txt;
    } map[] = {
        { .v = WIFI_OK,                     .txt = "No errors." },
        { .v = WIFI_OUT_OF_MEMORY,          .txt = "Out of memory." },
        { .v = WIFI_INVALID_FIRST_ELEMENT,  .txt = "Invalid first element." },
        { .v = WIFI_MISSING_WIFI_ENTRY,     .txt = "'wifi' element missing." },
        { .v = WIFI_MISSING_5G_CFG,         .txt = "'5GHz' element missing." },
        { .v = WIFI_MISSING_2G_CFG,         .txt = "'2.4GHz' element missing." },
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

int process_wifi_config( wifi_config_t *cfg, msgpack_object_map *map )
{
    (void) cfg;
    (void) map;

    return 0;
}

/**
 *  Convert the msgpack map into the wifi_t structure.
 *
 *  @param wifi wifi pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int process_wifi( wifi_t *wifi, msgpack_object *obj )
{
    msgpack_object_map *map = &obj->via.map;
    int left = map->size;
    uint8_t objects_left = 0x03;
    msgpack_object_kv *p;

    p = map->ptr;
    while( (0 < objects_left) && (0 < left--) ) {
        if( MSGPACK_OBJECT_STR == p->key.type ) {
            if( MSGPACK_OBJECT_MAP == p->val.type ) {
                if( 0 == match(p, "5GHz") ) {
                    if( 0 != process_wifi_config(&wifi->config_5g, &p->val.via.map) ) {
                        return -1;
                    }
                    objects_left &= ~(1 << 0);
                } else if( 0 == match(p, "2.4GHz") ) {
                    if( 0 != process_wifi_config(&wifi->config_2g, &p->val.via.map) ) {
                        return -1;
                    }
                    objects_left &= ~(1 << 1);
                }
            }
        }
        p++;
    }

    if( 1 & objects_left ) {
        errno = WIFI_MISSING_5G_CFG;
    } else if( (1 << 1) & objects_left ) {
        errno = WIFI_MISSING_2G_CFG;
    } else {
        errno = WIFI_OK;
    }

    return (0 == objects_left) ? 0 : -1;
}
