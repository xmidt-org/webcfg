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

#include "http_headers.h"

#include <stdlib.h>

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/**
 *  Returns the array size of a variable.
 *
 *  @note Only works for variables that the compiler can inspect at compile time.
 *
 *  @param x the variable (that is an array) to inspect
 *
 *  @return the count of elements in the array
 */
#define sizeof_array(x) (sizeof(x) / sizeof((x)[0]))

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int append_header( struct curl_slist **l, const char *format, ... )
{
    int rv;
    va_list args;

    va_start( args, format );
    rv = vappend_header( l, format, args );
    va_end( args );

    return rv;
}

int vappend_header( struct curl_slist **l, const char *format, va_list args )
{
    char buffer[256];
    char *buf;
    int len;
    va_list args2;

    va_copy( args2, args );

    buf = &buffer[0];
    buffer[sizeof_array(buffer) - 1] = '\0';

    len = vsnprintf( buf, sizeof_array(buffer), format, args );
    if( sizeof_array(buffer) <= (size_t) len ) {
        buf = (char*) malloc( (len + 1) * sizeof(char) );
        if( NULL == buf ) {
            return -1;
        }
        vsnprintf( buf, (len + 1), format, args2 );
        buf[len] = '\0';
    }

    *l = curl_slist_append( *l, buf );

    if( &buffer[0] != buf ) {
        free( buf );
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/* none */
