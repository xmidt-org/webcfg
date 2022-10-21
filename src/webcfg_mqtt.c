/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2022 RDK Management
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

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include "webcfg_generic.h"
#include "webcfg_multipart.h"
#include "webcfg_mqtt.h"

void on_connect(struct mosquitto *mosq, void *obj, int reason_code);
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
void on_publish(struct mosquitto *mosq, void *obj, int mid);

struct mosquitto *mosq = NULL;

//Initialize mqtt library and connect to mqtt broker
bool webcfg_mqtt_init()
{
	char *client_id , *username = NULL;
	char * topic, *hostname = NULL;
	char *PORT = NULL;
	int rc;

	res_init();
	WebcfgInfo("Initializing MQTT library\n");

	mosquitto_lib_init();

	int clean_session = true;

	//client_id = get_deviceMAC();
	get_from_file("CID=", &client_id);
	WebcfgInfo("client_id is %s\n", client_id);
	
	//if(client_id !=NULL)
	//{
		username = client_id;
		WebcfgInfo("client_id is %s username is %s\n", client_id, username);

		//fetch broker hostname ,topic from file
		get_from_file("TOPIC=", &topic);
		get_from_file("HOSTNAME=", &hostname);
		if(topic != NULL)
		{
			WebcfgInfo("The topic is %s\n", topic);
		}

		if(hostname != NULL)
		{
			WebcfgInfo("The hostname is %s\n", hostname);
		}
	
		if(!hostname || !topic)
		{
			WebcfgError("Invalid config hostname or topic is NULL\n");
			return MOSQ_ERR_INVAL;
		}

		//struct mosquitto *mosq;
		//struct userdata__callback cb_userdata;

		//cb_userdata.topic = topic;
		//cb_userdata.qos = qos;
		//cb_userdata.userdata = userdata;
		//cb_userdata.callback = callback;

		if(client_id !=NULL)
		{
			mosq = mosquitto_new(client_id, clean_session, NULL);
		}
		else
		{
			WebcfgInfo("client_id is NULL, init with clean_session true\n");
			mosq = mosquitto_new(NULL, true, NULL);
		}
		if(!mosq)
		{
			WebcfgError("Error initializing mosq instance\n");
			return MOSQ_ERR_NOMEM;
		}

		/*if(username !=NULL)
		{
			rc = mosquitto_username_pw_set(mosq, username, "password");
			if(rc)
			{
				WebcfgError("Error setting username %s\n", mosquitto_strerror(rc));
				mosquitto_destroy(mosq);
				return rc;
			}
		}*/

		get_from_file("PORT=", &PORT);
		WebcfgInfo("hostname is %s and PORT is %s\n", hostname, PORT);
		int port = PORT ? atoi(PORT) : MQTT_PORT;
		WebcfgInfo("port int %d\n", port);
		if(port == 80)
		{
			WebcfgInfo("connect to mqtt broker without tls\n");
			//connect to mqtt broker
			mosquitto_connect_callback_set(mosq, on_connect);
			mosquitto_subscribe_callback_set(mosq, on_subscribe);
			mosquitto_message_callback_set(mosq, on_message);
			mosquitto_publish_callback_set(mosq, on_publish);

			rc = mosquitto_connect(mosq, hostname, port, KEEPALIVE);
			WebcfgInfo("mosquitto_connect rc %d\n", rc);
			if(rc != MOSQ_ERR_SUCCESS)
			{
				WebcfgError("mqtt connect Error: %s\n", mosquitto_strerror(rc));
				mosquitto_destroy(mosq);
				return rc;
			}
			//WebcfgInfo("broker connect success. mosquitto_loop_forever\n");
			//rc = mosquitto_loop_forever(mosq, -1, 1);
			/* Run the network loop in a background thread, this call returns quickly. */
			//WebcfgInfo("broker connect success. mosquitto_loop\n");
			rc = mosquitto_loop_start(mosq);
			if(rc != MOSQ_ERR_SUCCESS)
			{
				mosquitto_destroy(mosq);
				WebcfgError( "mosquitto_loop_start Error: %s\n", mosquitto_strerror(rc));
				return 1;
			}
			WebcfgInfo("after loop rc is %d\n", rc);
		}
		else
		{
		struct libmosquitto_tls *tls;
		tls = malloc (sizeof (struct libmosquitto_tls));
		if(tls)
		{
			memset(tls, 0, sizeof(struct libmosquitto_tls));

			char * CAFILE, *CERTFILE , *KEYFILE = NULL;

			get_from_file("CA_FILE_PATH=", &CAFILE);
			get_from_file("CERT_FILE_PATH=", &CERTFILE);
			get_from_file("KEY_FILE_PATH=", &KEYFILE);

			if(CAFILE !=NULL && CERTFILE!=NULL && KEYFILE !=NULL)
			{
				WebcfgInfo("CAFILE %s, CERTFILE %s, KEYFILE %s MOSQ_TLS_VERSION %s\n", CAFILE, CERTFILE, KEYFILE, MOSQ_TLS_VERSION);

				tls->cafile = CAFILE;
				tls->certfile = CERTFILE;
				tls->keyfile = KEYFILE;
				tls->tls_version = MOSQ_TLS_VERSION;

				rc = mosquitto_tls_set(mosq, tls->cafile, tls->capath, tls->certfile, tls->keyfile, tls->pw_callback);
				WebcfgInfo("mosquitto_tls_set rc %d\n", rc);
				if(rc)
				{
					WebcfgError("Failed in mosquitto_tls_set %d %s\n", rc, mosquitto_strerror(rc));
					mosquitto_destroy(mosq);
					return rc;
				}
				rc = mosquitto_tls_opts_set(mosq, tls->cert_reqs, tls->tls_version, tls->ciphers);
				WebcfgInfo("mosquitto_tls_opts_set rc %d\n", rc);
				if(rc)
				{
					WebcfgError("Failed in mosquitto_tls_opts_set %d %s\n", rc, mosquitto_strerror(rc));
					mosquitto_destroy(mosq);
					return rc;
				}

				//connect to mqtt broker
				mosquitto_connect_callback_set(mosq, on_connect);
				mosquitto_subscribe_callback_set(mosq, on_subscribe);
				mosquitto_message_callback_set(mosq, on_message);
				mosquitto_publish_callback_set(mosq, on_publish);

				//get_from_file("PORT=", &PORT);
				//WebcfgInfo("hostname is %s and PORT is %s\n", hostname, PORT);
				//int port = PORT ? atoi(PORT) : MQTT_PORT;
				WebcfgInfo("port %d\n", port);
				rc = mosquitto_connect(mosq, hostname, port, KEEPALIVE);
				WebcfgInfo("mosquitto_connect rc %d\n", rc);
				if(rc != MOSQ_ERR_SUCCESS)
				{
					WebcfgError("mqtt connect Error: %s\n", mosquitto_strerror(rc));
					mosquitto_destroy(mosq);
					return rc;
				}
				else
				{
					WebcfgInfo("mqtt broker connect success %d\n", rc);
				}
				/*WebcfgInfo("mosquitto_loop_forever\n");
				rc = mosquitto_loop_forever(mosq, -1, 1);*/
				rc = mosquitto_loop_start(mosq);
				if(rc != MOSQ_ERR_SUCCESS)
				{
					mosquitto_destroy(mosq);
					WebcfgError("mosquitto_loop_start Error: %s\n", mosquitto_strerror(rc));
					return rc;
				}
				WebcfgInfo("after loop rc is %d\n", rc);
			}
			else
			{
				WebcfgError("Failed to get tls cert files\n");
				return 1;
			}
		}
		else
		{
			WebcfgError("Allocation failed\n");
			return MOSQ_ERR_NOMEM;
		}
		}
	/*}
	else
	{
		WebcfgError("Failed to get client_id\n");
		return 1;

	}*/
	return rc;
}

// callback called when the client receives a CONNACK message from the broker
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
        int rc;
	char * topic = NULL;
        WebcfgInfo("on_connect: reason_code %d %s\n", reason_code, mosquitto_connack_string(reason_code));
        if(reason_code != 0)
	{
		WebcfgError("on_connect received error\n");
                //reconnect
                mosquitto_disconnect(mosq);
		return;
        }

	get_from_file("TOPIC=", &topic);
	if(topic != NULL)
	{
		WebcfgInfo("subscribe to topic %s\n", topic);
	}
        rc = mosquitto_subscribe(mosq, NULL, topic, 1);
        if(rc != MOSQ_ERR_SUCCESS)
	{
                WebcfgError("Error subscribing: %s\n", mosquitto_strerror(rc));
                mosquitto_disconnect(mosq);
        }
	else
	{
		WebcfgInfo("subscribe to topic %s success\n", topic);
	}
}


// callback called when the broker sends a SUBACK in response to a SUBSCRIBE.
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
        int i;
        bool have_subscription = false;

	WebcfgInfo("on_subscribe callback: qos_count %d\n", qos_count);
        //SUBSCRIBE can contain many topics at once
        for(i=0; i<qos_count; i++)
	{
                WebcfgInfo("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
                if(granted_qos[i] <= 2)
		{
                        have_subscription = true;
                }
        }
        if(have_subscription == false)
	{
                WebcfgError("Error: All subscriptions rejected.\n");
                mosquitto_disconnect(mosq);
        }
}

/* callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	if(msg !=NULL)
	{
		if(msg->payload !=NULL)
		{
			WebcfgInfo("Received message from %s qos %d payloadlen %d payload %s\n", msg->topic, msg->qos, msg->payloadlen, (char *)msg->payload);

			int dataSize = msg->payloadlen;
			char * data = malloc(sizeof(char) * dataSize+1);
			memset(data, 0, sizeof(char) * dataSize+1);
			data = memcpy(data, (char *) msg->payload, dataSize+1);
			data[dataSize] = '\0';

			int status = 0;
			WebcfgInfo("Received dataSize is %d\n", dataSize);
			status = processPayload((char *)data, dataSize);
			WebcfgInfo("processPayload status %d\n", status);
		}
		else
		{
			WebcfgError("Received payload from mqtt is NULL\n");
		}
	}
	else
	{
		WebcfgError("Received message from mqtt is NULL\n");
	}
}

void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
        WebcfgInfo("Message with mid %d has been published.\n", mid);
}

/* This function pretends to read some data from a sensor and publish it.*/
void publish_notify_mqtt(void *payload,ssize_t len)
{
        int rc;
	char *pub_topic = NULL;
	get_from_file("PUB_TOPIC=", &pub_topic);
	WebcfgInfo("pub_topic is %s\n", pub_topic);
        rc = mosquitto_publish(mosq, NULL, pub_topic, len, payload, 2, false);
	WebcfgInfo("Publish rc %d\n", rc);
        if(rc != MOSQ_ERR_SUCCESS)
	{
                WebcfgError("Error publishing: %s\n", mosquitto_strerror(rc));
        }
	else
	{
		WebcfgInfo("Publish payload success %d\n", rc);
	}
	//WebcfgInfo("mosquitto_loop\n");
	//mosquitto_loop(mosq, 0, 1);
	//WebcfgInfo("mosquitto_loop done\n");
}

void get_from_file(char *key, char **val)
{
        FILE *fp = fopen(HOST_FILE_LOCATION, "r");

        if (NULL != fp)
        {
                char str[255] = {'\0'};
                while (fgets(str, sizeof(str), fp) != NULL)
                {
                    char *value = NULL;

                    if(NULL != (value = strstr(str, key)))
                    {
                        value = value + strlen(key);
                        value[strlen(value)-1] = '\0';
                        *val = strdup(value);
                        break;
                    }

                }
                fclose(fp);
        }

        if (NULL == *val)
        {
                WebcfgError("WebConfig val is not present in file\n");

        }
        else
        {
                WebcfgDebug("val fetched is %s\n", *val);
        }
}
