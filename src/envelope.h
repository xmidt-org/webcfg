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
#ifndef __ENVELOPE_H__
#define __ENVELOPE_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    char *base;         /* (R) V 1.0.0 */
    uint64_t major;     /* (R) V 1.0.0 */
    uint64_t minor;     /* (R) V 1.0.0 */
    uint64_t patch;     /* (R) V 1.0.0 */
} schema_t;

typedef struct {
    schema_t schema;    /* (R) V 1.0.0 */
    uint8_t sha256[32]; /* (R) V 1.0.0 */
    size_t len;         /* (R) V 1.0.0 */
    uint8_t *payload;   /* (R) V 1.0.0 */

    void *alt_payload;  /* Not managed by this library, but provides a nice
                         * spot to link the decoded payload to without the
                         * need for another wrapper. */
} envelope_t;

/**
 *  This function converts a msgpack buffer into an envelope_t structure
 *  if possible.
 *
 *  @note: errno is set with a custom error that can be made readable by
 *         envelope_strerror().
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
envelope_t* envelope_convert( const void *buf, size_t len );

/**
 *  This function destroys an envelope_t object.
 *
 *  @param e the envelope to destroy
 */
void envelope_destroy( envelope_t *e );

/**
 *  This function returns a general reason why the conversion failed.
 *
 *  @param errnum the errno value to inspect
 *
 *  @return the constant string (do not alter or free) describing the error
 */
const char* envelope_strerror( int errnum );

#endif
