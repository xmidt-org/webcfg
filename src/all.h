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
#ifndef __ALL_H__
#define __ALL_H__

#include <stdint.h>
#include <stdlib.h>
#include "dhcp.h"
#include "envelope.h"
#include "firewall.h"
#include "gre.h"
#include "portmapping.h"
#include "wifi.h"
#include "xdns.h"

typedef struct {
    envelope_t *full_envelope;

    envelope_t *dhcp_envelope;
    dhcp_t *dhcp;

    envelope_t *firewall_envelope;
    firewall_t *firewall;

    envelope_t *gre_envelope;
    gre_t *gre;

    envelope_t *portmapping_envelope;
    portmapping_t *portmapping;

    envelope_t *wifi_envelope;
    wifi_t *wifi;

    envelope_t *xdns_envelope;
    xdns_t *xdns;
} all_t;

#endif
