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
#ifndef __PORTMAPPINGPACK_H__
#define __PORTMAPPINGPACK_H__

#include <stdint.h>
#include <stdbool.h>


/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
struct rootdata {
    char *name;
    char *value;
    uint16_t type;
};

typedef struct data_struct {
    size_t count;
    struct rootdata *data_items;
    char * version;
} data_t;

//for subdoc

struct subdoc {
    char      *internal_client; 
    bool      enable;       
    uint16_t  external_port_end_range;    
    char      *protocol;
    char      *description;
    uint16_t  external_port; 
};

typedef struct subdoc_struct {
    size_t count;
    struct subdoc *subdoc_items;
} subdoc_t;

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Packs portmapping sub config doc.
 *
 *  @param 
 *
 *  @return 0 if the operation was a success, error otherwise
 */

ssize_t portmap_pack_subdoc(const subdoc_t *subdocData,void **data);

/**
 *  Packs portmapping root config doc.
 *
 *  @param 
 *
 *  @return 0 if the operation was a success, error otherwise
 */

ssize_t portmap_pack_rootdoc( char *blob, const data_t *packData, void **data );




#endif
