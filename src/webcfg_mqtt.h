/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2023 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef _WEBCFG_MQTT_H_
#define _WEBCFG_MQTT_H_

#include <stdio.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include <time.h>
#include <math.h>
#include <wrp-c.h>
#include <rbus/rbus.h>
#include <rbus/rbus_object.h>
#include <rbus/rbus_property.h>
#include <rbus/rbus_value.h>
#include "webcfg.h"
#include "webcfg_log.h"
#include "webcfg_multipart.h"
#include "webcfg_metadata.h"
#include "webcfg_auth.h"
#include "webcfg_db.h"
#include "webcfg_generic.h"
#include "webcfg_auth.h"

#define MAX_MQTT_LEN         128
#define MQTT_PUBLISH_GET_TOPIC_PREFIX "x/fr/get/chi/"
#define MQTT_PUBLISH_NOTIFY_TOPIC_PREFIX "x/fr/poke/chi/"
#define MQTT_SUBSCRIBE_TOPIC "x/to/"

#define MQTTCM_COMPONENT_NAME             "mqttConnManager"

#define MQTT_CONNSTATUS_PARAM	     "Device.X_RDK_MQTT.ConnectionStatus"
#define MQTT_SUBSCRIBE_PARAM         "Device.X_RDK_MQTT.Subscribe"
#define WEBCFG_MQTT_PUBLISH_PARAM    "Device.X_RDK_MQTT.Publish"

#define WEBCFG_SUBSCRIBE_CALLBACK    "Device.X_RDK_MQTT.Webconfig.OnSubcribeCallback"
#define WEBCFG_ONMESSAGE_CALLBACK    "Device.X_RDK_MQTT.Webconfig.OnMessageCallback"
#define WEBCFG_ONPUBLISH_CALLBACK    "Device.X_RDK_MQTT.Webconfig.OnPublishCallback"

typedef struct
{
	void *data;
	int len;
}msg_t;

void publish_notify_mqtt(void *payload, ssize_t len, char * dest);
char * createMqttPubHeader(char * payload, char * dest, ssize_t * payload_len);
int createMqttHeader(char **header_list);
int triggerBootupSync();
int processPayload(char * data, int dataSize);
int sendNotification_mqtt(char *payload, char *destination, wrp_msg_t *notif_wrp_msg, void *msg_bytes);
void* WebconfigMqttTask(void *status);
void initWebconfigMqttTask(unsigned long status);
rbusError_t setBootupSyncHeader(char *publishGetVal);
rbusError_t mqttSubscribeInit();
int getMqttCMConnStatus();
void freeMqttHeaders(char *contentlen_header, char *contenttype_header, char *PartnerID_header, char *ModelName_header, char *productClass_header, char *uuid_header, char *systemReadyTime_header, char *currentTime_header, char *status_header, char *FwVersion_header, char *bootTime_header, char *schema_header, char *accept_header, char *version_header, char *doc_header, char *deviceId_header, char *transaction_uuid);
pthread_cond_t *get_global_mqtt_sync_condition(void);
#endif
