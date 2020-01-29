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
#ifndef HTTP_HEADERS_H
#define HTTP_HEADERS_H

#include <curl/curl.h>
#include <stdarg.h>

/**
 *  Convert a printf() formatted argument list into a curl header.
 *
 *  @return 0 on success or error otherwise
 */
int append_header( struct curl_slist **l, const char *format, ... );

/**
 *  The va_list version of append_header().
 *
 *  @return 0 on success or error otherwise
 */
int vappend_header( struct curl_slist **l, const char *format, va_list ap );

#endif
