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
#ifndef __XDNS_H__
#define __XDNS_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t default_ipv4;      /* (R) V 1.0.0 */
    uint8_t default_ipv6[16];   /* (R) V 1.0.0 */
} xdns_t;

/**
 *  This function converts a msgpack buffer into an xdns_t structure
 *  if possible.
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
xdns_t* xdns_convert( const void *buf, size_t len );

/**
 *  This function destroys an xdns_t object.
 *
 *  @param e the xdns to destroy
 */
void xdns_destroy( xdns_t *d );

/**
 *  This function returns a general reason why the conversion failed.
 *
 *  @param errnum the errno value to inspect
 *
 *  @return the constant string (do not alter or free) describing the error
 */
const char* xdns_strerror( int errnum );

#endif
