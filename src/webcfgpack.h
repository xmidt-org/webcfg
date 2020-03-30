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
#ifndef __WEBCFG_H__
#define __WEBCFG_H__

#include <stdint.h>
#include "webcfg_db.h"


/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
struct data {
    char *name;
    char *value;
    int notify_attribute;
    uint16_t type;
};

typedef struct data_struct {
    size_t count;
    struct data *data_items;
} data_t;

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Packs webconfig root config doc.
 *
 *  @param 
 *
 *  @return 0 if the operation was a success, error otherwise
 */
//ssize_t webcfg_pack_rootdoc( char *blob, const data_t *packData, void **data );
ssize_t webcfg_pack_rootdoc(  const data_t *packData, void **data );
ssize_t webcfgdb_blob_pack(webconfig_db_data_t *webcfgdb_data, webconfig_tmp_data_t * g_head, void **data);
ssize_t webcfgdb_pack( webconfig_db_data_t *packData, void **data, size_t count );


#endif
