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
#ifndef __FULL_H__
#define __FULL_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    char *url;                      /* (R) V 1.0.0 */
    uint8_t *payload;               /* (R) V 1.0.0 */
    size_t payload_len;
} subsystem_t;

typedef struct {
    subsystem_t *subsystems;        /* (O) V 1.0.0 */
    size_t       subsystems_count;
} full_t;

/**
 *  This function converts a msgpack buffer into an full_t structure
 *  if possible.
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
full_t* full_convert( const void *buf, size_t len );

/**
 *  This function destroys an full_t object.
 *
 *  @param e the full to destroy
 */
void full_destroy( full_t *d );

/**
 *  This function returns a general reason why the conversion failed.
 *
 *  @param errnum the errno value to inspect
 *
 *  @return the constant string (do not alter or free) describing the error
 */
const char* full_strerror( int errnum );

/**
 *  This function returns the schema version supported by this component.
 *
 *  @return the constant string (do not alter or free) describing the schema
 *          accepted/supported.
 */
const char* full_get_schema_version( void );

#endif
