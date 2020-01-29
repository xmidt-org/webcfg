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
#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>
#include <curl/curl.h>

typedef struct {
    /* Headers */
    const char *auth;           /* (optional) Authorization: Bearer %s */
    const char *cfg_ver;        /* IF-NONE-MATCH: %s */
    const char *schema_ver;     /* Schema-Version: %s */
    const char *fw;             /* X-System-Firmware-Version: %s */
    const char *status;         /* X-System-Status: %s */
    const char *trans_id;       /* Transaction-Id: %s */
    uint32_t boot_unixtime;     /* X-System-Boot-Time: %d */
    uint32_t ready_unixtime;    /* X-System-Ready-Time: %d */
    uint32_t current_unixtime;  /* X-System-Current-Time: %d */

    const char *url;            /* The URL to hit. */

    long timeout_s;             /* The HTTP timeout in seconds. */
    long ip_version;            /* The IP to use to connect via.
                                 * 4 = IPv4
                                 * 6 = IPv6
                                 * anything else = system default
                                 */

    const char *interface;      /* (optional) The specific interface to bind to.
                                 * If NULL is specified the system chooses for you. */
    const char *ca_cert_path;   /* (optional) The CA certificate path.
                                 * If NULL is specified the system chooses for you. */
} http_request_t;

typedef struct {
    CURLcode code;              /* The response code from the perform(). */
    long http_status;           /* The curl http status. */
    CURL *curl;                 /* The curl object for getting more information. */

    /* The response */
    size_t len;                 /* The response length. */
    void *data;                 /* The response data. */
} http_response_t;

/**
 *  Makes the HTTP request and provides the response.
 *
 *  The response MUST be destroyed with http_destroy() this will leak.
 *
 *  @param req  the request object
 *  @param resp the response object
 *
 *  @return 0 on success, error otherwise
 */
int http_request( http_request_t *req, http_response_t *resp );

/**
 *  Destroys the response object when you're done with it.
 */
void http_destroy( http_response_t *resp );

#endif
