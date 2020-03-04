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
#ifndef __PORTMAPPINGPARAM_H__
#define __PORTMAPPINGPARAM_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
   char *name;
   char *value;
   uint16_t type;                       
} pm_entry_t;

typedef struct {
    pm_entry_t *entries;        
    size_t      entries_count;
    char * version;
} portmapping_t;

/**
 *  This function converts a msgpack buffer into an portmapping_t structure
 *  if possible.
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
portmapping_t* portmapping_convert( const void *buf, size_t len );

/**
 *  This function destroys an portmapping_t object.
 *
 *  @param e the portmapping to destroy
 */
void portmapping_destroy( portmapping_t *d );

/**
 *  This function returns a general reason why the conversion failed.
 *
 *  @param errnum the errno value to inspect
 *
 *  @return the constant string (do not alter or free) describing the error
 */
const char* portmapping_strerror( int errnum );

#endif
