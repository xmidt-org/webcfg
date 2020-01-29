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
#ifndef __DHCP_H__
#define __DHCP_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t mac[6];                     /* (R) V 1.0.0 */
    uint32_t ip;                        /* (R) V 1.0.0 */
} dhcp_static_t;

typedef struct {
    uint32_t       router_ip;           /* (R) V 1.0.0 */
    uint32_t       subnet_mask;         /* (R) V 1.0.0 */
    uint32_t       pool_range[2];       /* (R) V 1.0.0 */
    uint32_t       lease_length;        /* (R) V 1.0.0 */
    dhcp_static_t *fixed;               /* (O) V 1.0.0 */
    size_t         fixed_count;
} dhcp_t;

/**
 *  This function converts a msgpack buffer into an dhcp_t structure
 *  if possible.
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
dhcp_t* dhcp_convert( const void *buf, size_t len );

/**
 *  This function destroys an dhcp_t object.
 *
 *  @param e the dhcp to destroy
 */
void dhcp_destroy( dhcp_t *d );

/**
 *  This function returns a general reason why the conversion failed.
 *
 *  @param errnum the errno value to inspect
 *
 *  @return the constant string (do not alter or free) describing the error
 */
const char* dhcp_strerror( int errnum );

#endif
