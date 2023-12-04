/*
 * Copyright 2023 Comcast Cable Communications Management, LLC
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

/* This is a stub file that will be overridden in a patch */

#include "webcfg_wanhandle.h"

void WanEventHandler()
{
}
#ifdef _SKY_HUB_COMMON_PRODUCT_REQ_
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "time.h"
#include "webcfg.h"
#include "webcfg_rbus.h"

rbusHandle_t rbus_handle;
static void eventReceiveHandler(
    rbusHandle_t rbus_handle,
    rbusEvent_t const* event,
    rbusEventSubscription_t* subscription);

static void subscribeAsyncHandler(
    rbusHandle_t rbus_handle,
    rbusEventSubscription_t* subscription,
    rbusError_t error);

static void eventReceiveHandler(
    rbusHandle_t rbus_handle,
    rbusEvent_t const* event,
    rbusEventSubscription_t* subscription)
{
    (void)rbus_handle;
    rbusValue_t newValue = rbusObject_GetValue(event->data, "value");
    rbusValue_t oldValue = rbusObject_GetValue(event->data, "oldValue");
    WebcfgInfo("Consumer received ValueChange event for param %s\n", event->name);

    if(newValue!=NULL && oldValue!=NULL) {
            WebcfgInfo("New Value: %s Old Value: %s\n", rbusValue_GetString(newValue, NULL), rbusValue_GetString(oldValue, NULL));
    }
    else {
            if(newValue == NULL) {
                    WebcfgError(("NewValue is NULL\n"));
            }
            if(oldValue == NULL) {
                    WebcfgError(("oldValue is NULL\n"));
            }
     }
     if(newValue) {
         char *CurrentWANStatus = NULL;
         CurrentWANStatus = (char *) rbusValue_GetString(newValue, NULL);

         if (CurrentWANStatus == NULL)
         {
	     WebcfgError(("CurrentWANStatus is NULL returning..\n"));
             return;
	 }
	
         if (strcmp(CurrentWANStatus, "Down")==0)
         {
              WebcfgInfo("Received wan Down event\n");
         }
         else if (strcmp(CurrentWANStatus, "Up")==0)
         {
             WebcfgInfo("Received wan Up event, trigger force sync with cloud.\n");
             if(get_webcfgReady())
             {
                 WebcfgInfo("Trigger force sync with cloud on wan status change\n");
                 trigger_webcfg_forcedsync();
             }
             else
             {
                 WebcfgInfo("wan status force sync is skipped as webcfg is not ready\n");
             }
         }
     }
}

static void subscribeAsyncHandler(
    rbusHandle_t rbus_handle,
    rbusEventSubscription_t* subscription,
    rbusError_t error)
{
  (void)rbus_handle;

  WebcfgError("subscribeAsyncHandler event %s, error %d - %s\n", subscription->eventName, error, rbusError_ToString(error));
}

//Subscribe for Device.X_RDK_WanManager.CurrentStatus
int subscribeTo_CurrentInterfaceStatus_Event()
{
      int rc = RBUS_ERROR_SUCCESS;
      WebcfgInfo("Subscribing to %s Event\n", WEBCFG_WANSTATUS_PARAM);
      rc = rbusEvent_SubscribeAsync (
        rbus_handle,
        WEBCFG_WANSTATUS_PARAM,
        eventReceiveHandler,
        subscribeAsyncHandler,
        "Webcfg_InterfaceStatus",
        10*20);
      if(rc != RBUS_ERROR_SUCCESS) {
              WebcfgError("%s subscribe failed : %d - %s\n", WEBCFG_WANSTATUS_PARAM, rc, rbusError_ToString(rc));
      }
         return rc;
}
#endif
