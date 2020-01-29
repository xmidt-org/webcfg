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

#include "http.h"
#include "http_headers.h"

#include <string.h>
#include <stdlib.h>

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

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
int to_headers( struct curl_slist **l, http_request_t *r );
size_t write_cb( void *buf, size_t size, size_t nmemb, http_response_t *resp );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int http_request( http_request_t *req, http_response_t *resp )
{
    int rv = -1;
    CURL *curl = NULL;
    struct curl_slist *headers = NULL;
    long ipvmode;

    if( NULL == req || NULL == resp ) {
        return -1;
    }
    memset( resp, 0, sizeof(http_response_t) );

    if( 0 != to_headers(&headers, req) ) {
        return -2;
    }

    /* Build the curl object. */
    curl = curl_easy_init();
    if( NULL != curl ) {

        curl_easy_setopt( curl, CURLOPT_URL, req->url );
        curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers );
        curl_easy_setopt( curl, CURLOPT_TIMEOUT, req->timeout_s );
        curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1L );
        if( req->interface ) {
            curl_easy_setopt( curl, CURLOPT_INTERFACE, req->interface );
        }

        /* figure out the IP version to use. */
        ipvmode = CURL_IPRESOLVE_WHATEVER;
        if( 4 == req->ip_version ) ipvmode = CURL_IPRESOLVE_V4;
        if( 6 == req->ip_version ) ipvmode = CURL_IPRESOLVE_V6;
        curl_easy_setopt( curl, CURLOPT_IPRESOLVE, ipvmode );

        /* Ensure TLS 1.2+, verify the hostname, cert, etc. */
        if( req->ca_cert_path ) {
            curl_easy_setopt( curl, CURLOPT_CAINFO, req->ca_cert_path );
        }
        curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 1L );
        curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 2L );
        curl_easy_setopt( curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2 );

        /* Don't perform an OCSP check as that can DDoS that endpoint. */
        curl_easy_setopt( curl, CURLOPT_SSL_VERIFYSTATUS, 0L );

        /* Setup response handling. */
        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_cb );
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, resp );

        resp->code = curl_easy_perform( curl );
        if( CURLE_OK == resp->code ) {
            curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &resp->http_status );
        }

        resp->curl = curl;

        rv = 0;
    }

    curl_slist_free_all( headers );

    return rv;
}

void http_destroy( http_response_t *resp )
{
    if( resp->data ) {
        free( resp->data );
    }
    curl_easy_cleanup( resp->curl );
}


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  Convert the http request's headers 
 *
 *  @param l the curl list to append headers to
 *  @param r the http request to convert
 *
 *  @return 0 on success, error otherwise
 */
int to_headers( struct curl_slist **l, http_request_t *r )
{
    int rv = 0;
    if( (NULL == l) || (NULL == r) || !(r->cfg_ver) || !(r->schema_ver) ||
        !(r->fw) || !(r->status) || !(r->trans_id) )
    {
        return -1;
    }

    if( r->auth ) {
        rv |= append_header( l, "Authorization: Bearer %s", r->auth );
    }

    rv |= append_header( l, "IF-NONE-MATCH: %s", r->cfg_ver );
    rv |= append_header( l, "Schema-Version: %s", r->schema_ver );
    rv |= append_header( l, "X-System-Firmware-Version: %s", r->fw );
    rv |= append_header( l, "X-System-Status: %s", r->status );
    rv |= append_header( l, "Transaction-Id: %s", r->trans_id );
    rv |= append_header( l, "X-System-Boot-Time: %d", r->boot_unixtime );
    rv |= append_header( l, "X-System-Ready-Time: %d", r->ready_unixtime );
    rv |= append_header( l, "X-System-Current-Time: %d", r->current_unixtime );

    return rv;
}

/**
 *  The write callback handler for pulling data from the response.
 */
size_t write_cb( void *buf, size_t size, size_t nmemb, http_response_t *resp )
{
    size_t n = size * nmemb;
    uint8_t *tmp;

    resp->data = realloc( resp->data, resp->len + n );
    if( NULL == resp->data ) {
        return 0;
    }

    tmp = (uint8_t*) resp->data;
    memcpy( &tmp[resp->len], buf, n );
    resp->len += n;

    return n;
}
