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
#ifndef __WIFI_H__
#define __WIFI_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    char *name;             /* (R) V 1.0.0 */
    char *ssid;             /* (R) V 1.0.0 */
    char *password;         /* (R) V 1.0.0 */
    char *advertisement;    /* (R) V 1.0.0 */
    char *security_mode;    /* (R) V 1.0.0 */
    char *method;           /* (R) V 1.0.0 */
} wifi_ap_t;

typedef struct {
    /* 'a' = auto, '-' = below, '+' = above */
    char        extension_channel;  /* (R) V 1.0.0 */
    int16_t     channel;            /* (R) V 1.0.0 */
    uint64_t    bandwith;           /* (R) V 1.0.0 */
    char       *standards;          /* (R) V 1.0.0 */
    size_t      standards_count;    /* (R) V 1.0.0 */
    wifi_ap_t  *aps;                /* (O) V 1.0.0 */
    size_t      aps_count;
    bool        dfs_enabled;        /* (O) V 1.0.0 (default=false) */
    char       *basic_rate;         /* (R) V 1.0.0 */
    uint64_t    tx_power;           /* (R) V 1.0.0 */
} wifi_config_t;

typedef struct {
    wifi_config_t config_5g;        /* (R) V 1.0.0 */
    wifi_config_t config_2g;        /* (R) V 1.0.0 */
} wifi_t;

/**
 *  This function converts a msgpack buffer into an wifi_t structure
 *  if possible.
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
wifi_t* wifi_convert( const void *buf, size_t len );

/**
 *  This function destroys an wifi_t object.
 *
 *  @param w the wifi to destroy
 */
void wifi_destroy( wifi_t *wifi );

/**
 *  This function returns a general reason why the conversion failed.
 *
 *  @param errnum the errno value to inspect
 *
 *  @return the constant string (do not alter or free) describing the error
 */
const char* wifi_strerror( int errnum );

#endif
