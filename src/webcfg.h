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

#include "all.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/**
 *  update_config_fn is a pointer to the function to call when there is a new
 *  configuration to apply.
 *
 *  @note The memory given to the callback should be cleaned up via a call to
 *        webcfg_free().  Otherwise memory leaks will happen.
 *
 *  @param  new_cfg is the new configuration to apply
 *
 *  @return 0 if the operation was a success, error otherwise
 */
typedef int (*update_config_fn)( const all_t *new_cfg, void *user_data );

/**
 *  Called to get the authorization blob needed to connect.
 *
 *  @note The memory given to the caller shall be free()d when the caller is
 *        done with it.
 *
 *  @return the string if the operation was a success, NULL otherwise
 */
typedef char* (*get_auth_fn)( void *user_data );


struct webcfg_opts {
    const char *url;
    const char *interface;
    const char *ca_cert_path;
    const char *firmware;

    const char *tmp_path;
    const char *durable_path;

    uint32_t boot_unixtime;
    uint32_t ready_unixtime;

    void *user_data;

    update_config_fn update_config;
    get_auth_fn      get_auth;
};

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Initialize the library.
 *
 *  @param opts the configuration options to abide by
 *
 *  @return 0 if the operation was a success, error otherwise
 */
int webcfg_init( struct webcfg_opts *opts );


/**
 *  Shuts down and cleans up after the library.
 *
 *  @note the `user_data` is not freed.
 */
void webcfg_shutdown( void );


/**
 *  Called with an update for the actual configuration present.
 *
 *  @param cfg the configuration applied
 *
 *  @return 0 if the operation was a success, error otherwise
 */
int webcfg_update_actual( const all_t *cfg );


/**
 *  webcfg_free frees and releases all the resources associated with the
 *  all_t structure.
 *
 *  @param cfg the config structure to free
 */
void webcfg_free( all_t *cfg );

#endif
