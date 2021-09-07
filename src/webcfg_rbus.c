/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2021 RDK Management
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

#include <stdbool.h>
#include <string.h>

#include <stdlib.h>
#include <wdmp-c.h>
#include <cimplog.h>
#include "webcfg_rbus.h"

static rbusHandle_t rbus_handle;

static bool  RfcVal = false ;
static char* URLVal = NULL ;
static char* forceSyncVal = NULL ;

static bool isRbus = false ;

typedef struct
{
    char *paramName;
    char *paramValue;
    rbusValueType_t type;
} rbusParamVal_t;

static char *PSMPrefix  = "eRT.com.cisco.spvtg.ccsp.webpa.";

bool get_global_isRbus(void)
{
    return isRbus;
}

rbusHandle_t get_global_rbus_handle(void)
{
     return rbus_handle;
}

bool isRbusEnabled() 
{
	if(RBUS_ENABLED == rbus_checkStatus()) 
	{
		isRbus = true;
	}
	else
	{
		isRbus = false;
	}
	WebcfgInfo("Webconfig RBUS mode active status = %s\n", isRbus ? "true":"false");
	return isRbus;
}

bool isRbusInitialized( ) 
{
    return rbus_handle != NULL ? true : false;
}

WEBCFG_STATUS webconfigRbusInit(const char *pComponentName) 
{
	int ret = RBUS_ERROR_SUCCESS;   

	WebcfgInfo("rbus_open for component %s\n", pComponentName);
	ret = rbus_open(&rbus_handle, pComponentName);
	if(ret != RBUS_ERROR_SUCCESS)
	{
		WebcfgError("webconfigRbusInit failed with error code %d\n", ret);
		return WEBCFG_FAILURE;
	}
	WebcfgInfo("webconfigRbusInit is success. ret is %d\n", ret);
	return WEBCFG_SUCCESS;
}

void webpaRbus_Uninit()
{
    rbus_close(rbus_handle);
}

/**
 * Data set handler for Webpa parameters
 */
rbusError_t webcfgDataSetHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts) {

    WebcfgInfo("Inside webcfgDataSetHandler\n");
    (void) handle;
    (void) opts;
    char const* paramName = rbusProperty_GetName(prop);
    if((strncmp(paramName,  WEBCFG_RFC_PARAM, maxParamLen) != 0) &&
	(strncmp(paramName, WEBCFG_URL_PARAM, maxParamLen) != 0) &&
	(strncmp(paramName, WEBCFG_FORCESYNC_PARAM, maxParamLen) != 0)
	) {
        WebcfgError("Unexpected parameter = %s\n", paramName); //free paramName req.?
        return RBUS_ERROR_ELEMENT_DOES_NOT_EXIST;
    }

    rbusError_t retPsmSet = RBUS_ERROR_BUS_ERROR;
    WebcfgInfo("Parameter name is %s \n", paramName);
    rbusValueType_t type_t;
    rbusValue_t paramValue_t = rbusProperty_GetValue(prop);
    if(paramValue_t) {
        type_t = rbusValue_GetType(paramValue_t);
    } else {
	WebcfgError("Invalid input to set\n");
        return RBUS_ERROR_INVALID_INPUT;
    }

    if(strncmp(paramName, WEBCFG_RFC_PARAM, maxParamLen) == 0) {
        WebcfgInfo("Inside RFC datamodel handler \n");
        if(type_t == RBUS_BOOLEAN) {
	    char tmpchar[8] = {'\0'};

	    bool paramval = rbusValue_GetBoolean(paramValue_t);
	    WebcfgInfo("paramval is %d\n", paramval);
	    if(paramval == true || paramval == false)
            {
		    if(paramval == true)
		    {
			strcpy(tmpchar, "true");
		    }
		    else
		    {
			strcpy(tmpchar, "false");
                    }
		    WebcfgInfo("tmpchar is %s\n", tmpchar);
		    retPsmSet = rbus_StoreValueIntoDB( WEBCFG_RFC_PARAM, tmpchar );
		    if (retPsmSet != RBUS_ERROR_SUCCESS)
		    {
			WebcfgError("psm_set failed ret %d for parameter %s and value %s\n", retPsmSet, paramName, tmpchar);
			return retPsmSet;
		    }
		    else
		    {
			WebcfgInfo("psm_set success ret %d for parameter %s and value %s\n", retPsmSet, paramName, tmpchar);
			RfcVal = paramval;
		    }
		    WebcfgInfo("RfcVal after processing %s\n", (1==RfcVal)?"true":"false");
	    }
	    else
	    {
		WebcfgError("set value type is invalid\n");
                return RBUS_ERROR_INVALID_INPUT;
	    }
        } else {
            WebcfgError("Unexpected value type for property %s \n", paramName);
	    return RBUS_ERROR_INVALID_INPUT;
        }

    }else if(strncmp(paramName, WEBCFG_URL_PARAM, maxParamLen) == 0) {
        WebcfgInfo("Inside datamodel handler for URL\n");

        if(type_t == RBUS_STRING) {
            char* data = rbusValue_ToString(paramValue_t, NULL, 0);
            if(data) {
                WebcfgInfo("Call datamodel function  with data %s\n", data);

                if(URLVal) {
                    free(URLVal);
                    URLVal = NULL;
                }
                URLVal = strdup(data);
                free(data);
		WebcfgInfo("URLVal after processing %s\n", URLVal);
		WebcfgInfo("URLVal bus_StoreValueIntoDB\n");
		retPsmSet = rbus_StoreValueIntoDB( WEBCFG_URL_PARAM, URLVal );
		if (retPsmSet != RBUS_ERROR_SUCCESS)
		{
			WebcfgError("psm_set failed ret %d for parameter %s and value %s\n", retPsmSet, paramName, URLVal);
			return retPsmSet;
		}
		else
		{
			WebcfgInfo("psm_set success ret %d for parameter %s and value %s\n", retPsmSet, paramName, URLVal);
		}
            }
        } else {
            WebcfgError("Unexpected value type for property %s\n", paramName);
	    return RBUS_ERROR_INVALID_INPUT;
        }
    }else if(strncmp(paramName, WEBCFG_FORCESYNC_PARAM, maxParamLen) == 0) {
        WebcfgInfo("Inside Force Sync datamodel handler \n");
        if(type_t == RBUS_STRING) {
            char* data = rbusValue_ToString(paramValue_t, NULL, 0);
            if(data) {
                WebcfgInfo("Call datamodel function  with data %s \n", data);

                if (forceSyncVal){
                    free(forceSyncVal);
                    forceSyncVal = NULL;
                }
                forceSyncVal = strdup(data);
                free(data);
		WebcfgInfo("forceSyncVal after processing %s\n", forceSyncVal);
		WebcfgInfo("forceSyncVal bus_StoreValueIntoDB\n");
		retPsmSet = rbus_StoreValueIntoDB( WEBCFG_FORCESYNC_PARAM, forceSyncVal );
		if (retPsmSet != RBUS_ERROR_SUCCESS)
		{
			WebcfgError("psm_set failed ret %d for parameter %s and value %s\n", retPsmSet, paramName, forceSyncVal);
			return retPsmSet;
		}
		else
		{
			WebcfgInfo("psm_set success ret %d for parameter %s and value %s\n", retPsmSet, paramName, forceSyncVal);
		}
            }
        } else {
            WebcfgError("Unexpected value type for property %s \n", paramName);
            return RBUS_ERROR_INVALID_INPUT;
        }

    }
    WebcfgInfo("webcfgDataSetHandler End\n");
    return RBUS_ERROR_SUCCESS;
}

/**
 * Common data get handler for all parameters owned by Webconfig
 */
rbusError_t webcfgDataGetHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts) {

    WebcfgInfo("In webcfgDataGetHandler\n");
    (void) handle;
    (void) opts;
    char const* propertyName;
    rbusError_t retPsmGet = RBUS_ERROR_BUS_ERROR;

    propertyName = strdup(rbusProperty_GetName(property));
    if(propertyName) {
        WebcfgInfo("Property Name is %s \n", propertyName);
    } else {
        WebcfgError("Unable to handle get request for property \n");
        return RBUS_ERROR_INVALID_INPUT;
    }

    if(strncmp(propertyName, WEBCFG_RFC_PARAM, maxParamLen) == 0) {
        rbusValue_t value;
        rbusValue_Init(&value);
	char *tmpchar = NULL;
	retPsmGet = rbus_GetValueFromDB( WEBCFG_RFC_PARAM, &tmpchar );
	if (retPsmGet != RBUS_ERROR_SUCCESS){
		WebcfgError("psm_get failed ret %d for parameter %s and value %s\n", retPsmGet, propertyName, tmpchar);
		return retPsmGet;
	}
	else{
		WebcfgInfo("psm_get success ret %d for parameter %s and value %s\n", retPsmGet, propertyName, tmpchar);
		if(tmpchar != NULL)
		{
			WebcfgInfo("B4 char to bool conversion\n");
			if(((strcmp (tmpchar, "true") == 0)) || (strcmp (tmpchar, "TRUE") == 0))
			{
				RfcVal = true;
			}
			free(tmpchar);
		}
		WebcfgInfo("RfcVal fetched %d\n", RfcVal);
	}

	rbusValue_SetBoolean(value, RfcVal); 
	WebcfgInfo("After RfcVal set to value\n");
        rbusProperty_SetValue(property, value);
	WebcfgInfo("Rfc value fetched is %s\n", (rbusValue_GetBoolean(value)==true)?"true":"false");
        rbusValue_Release(value);

    }else if(strncmp(propertyName, WEBCFG_URL_PARAM, maxParamLen) == 0) {
        rbusValue_t value;
        rbusValue_Init(&value);

        if(URLVal){
            rbusValue_SetString(value, URLVal);
	}
        else{
		retPsmGet = rbus_GetValueFromDB( WEBCFG_URL_PARAM, &URLVal );
		if (retPsmGet != RBUS_ERROR_SUCCESS){
			WebcfgError("psm_get failed ret %d for parameter %s and value %s\n", retPsmGet, propertyName, URLVal);
			return retPsmGet;
		}
		else{
			WebcfgInfo("psm_get success ret %d for parameter %s and value %s\n", retPsmGet, propertyName, URLVal);
			rbusValue_SetString(value, URLVal);
			WebcfgInfo("After URLVal set to value\n");
		}
	}
        rbusProperty_SetValue(property, value);
	//WebcfgInfo("URL value fetched is %s\n", value);
        rbusValue_Release(value);

    }else if(strncmp(propertyName, WEBCFG_FORCESYNC_PARAM, maxParamLen) == 0) {
        rbusValue_t value;
        rbusValue_Init(&value);
        rbusValue_SetString(value, "");
        rbusProperty_SetValue(property, value);
	//WebcfgInfo("forceSyncVal value fetched is %s\n", value);
        rbusValue_Release(value);
    }

    if(propertyName) {
        free((char*)propertyName);
        propertyName = NULL;
    }

    WebcfgInfo("webcfgDataGetHandler End\n");
    return RBUS_ERROR_SUCCESS;
}

/**
 * To persist TR181 parameter values in PSM DB.
 */
rbusError_t rbus_StoreValueIntoDB(char *paramName, char *value)
{
	char recordName[ 256] = {'\0'};
	char psmName[256] = {'\0'};
	rbusParamVal_t val[1];
	bool commit = 1;
	rbusError_t errorcode = RBUS_ERROR_INVALID_INPUT;
	rbus_error_t err = RTMESSAGE_BUS_SUCCESS;

	sprintf(recordName, "%s%s",PSMPrefix, paramName);
	WebcfgInfo("rbus_StoreValueIntoDB recordName is %s\n", recordName);

	val[0].paramName  = recordName;
	val[0].paramValue = value;
	val[0].type = 0;

	sprintf(psmName, "%s%s", "eRT.", DEST_COMP_ID_PSM);
	WebcfgInfo("rbus_StoreValueIntoDB psmName is %s\n", psmName);

	rbusMessage request, response;

	rbusMessage_Init(&request);
	rbusMessage_SetInt32(request, 0); //sessionId
	rbusMessage_SetString(request, WEBCFG_COMPONENT_NAME); //component name that invokes the set
	rbusMessage_SetInt32(request, (int32_t)1); //size of params

	rbusMessage_SetString(request, val[0].paramName); //param details
	rbusMessage_SetInt32(request, val[0].type);
	rbusMessage_SetString(request, val[0].paramValue);
	rbusMessage_SetString(request, commit ? "1" : "0");


	if((err = rbus_invokeRemoteMethod(psmName, METHOD_SETPARAMETERVALUES, request, 6000, &response)) != RTMESSAGE_BUS_SUCCESS)
        {
            WebcfgError("rbus_invokeRemoteMethod failed with err %d", err);
            errorcode = RBUS_ERROR_BUS_ERROR;
        }
        else
        {
            int ret = -1;
            char const* pErrorReason = NULL;
            rbusMessage_GetInt32(response, &ret);

            WebcfgInfo("Response from the remote method is [%d]!", ret);
            errorcode = (rbusError_t) ret;

            if((errorcode == RBUS_ERROR_SUCCESS) || (errorcode == 100)) //legacy error codes returned from component PSM.
            {
                errorcode = RBUS_ERROR_SUCCESS;
                WebcfgInfo("Successfully Set the Value");
            }
            else
            {
                rbusMessage_GetString(response, &pErrorReason);
                WebcfgError("Failed to Set the Value for %s", pErrorReason);
            }

            rbusMessage_Release(response);
        }

	return errorcode;
}

/**
 * To fetch TR181 parameter values from PSM DB.
 */
rbusError_t rbus_GetValueFromDB( char* paramName, char** paramValue)
{
	char recordName[ 256] = {'\0'};
	char psmName[256] = {'\0'};
	char * parameterNames[1] = {NULL};
	int32_t type = 0;
	rbusError_t errorcode = RBUS_ERROR_INVALID_INPUT;
	rbus_error_t err = RTMESSAGE_BUS_SUCCESS;
	*paramValue = NULL;

	sprintf(recordName, "%s%s",PSMPrefix, paramName);
	WebcfgInfo("rbus_GetValueFromDB recordName is %s\n", recordName);

	parameterNames[0] = (char*)recordName;

	sprintf(psmName, "%s%s", "eRT.", DEST_COMP_ID_PSM);
	WebcfgInfo("rbus_GetValueFromDB psmName is %s\n", psmName);

	rbusMessage request, response;

	rbusMessage_Init(&request);
	rbusMessage_SetString(request, psmName); //component name that invokes the get
	rbusMessage_SetInt32(request, (int32_t)1); //size of params

	rbusMessage_SetString(request, parameterNames[0]); //param details

	if((err = rbus_invokeRemoteMethod(psmName, METHOD_GETPARAMETERVALUES, request, 6000, &response)) != RTMESSAGE_BUS_SUCCESS)
        {
            WebcfgError("rbus_invokeRemoteMethod GET failed with err %d\n", err);
            errorcode = RBUS_ERROR_BUS_ERROR;
        }
        else
        {
            int valSize=0;
	    int ret = -1;
	    rbusMessage_GetInt32(response, &ret);
	    WebcfgInfo("Response from the remote method is [%d]!\n",ret);
            errorcode = (rbusError_t) ret;

	    if((errorcode == RBUS_ERROR_SUCCESS) || (errorcode == 100)) //legacy error code from component.
            {
                WebcfgInfo("Successfully Get the Value\n");
		errorcode = RBUS_ERROR_SUCCESS;
		rbusMessage_GetInt32(response, &valSize);
		if(1/*valSize*/)
		{
			char const *buff = NULL;

			//Param Name
			rbusMessage_GetString(response, &buff);
			WebcfgInfo("Requested param buff [%s]\n", buff);
			if(buff && (strcmp(recordName, buff) == 0))
			{
				rbusMessage_GetInt32(response, &type);
				WebcfgInfo("Requested param type [%d]\n", type);
				buff = NULL;
				rbusMessage_GetString(response, &buff);
				*paramValue = strdup(buff); //free buff
				WebcfgInfo("Requested param DB value [%s]\n", *paramValue);
			}
			else
			{
			    WebcfgError("Requested param: [%s], Received Param: [%s]\n", recordName, buff);
			    errorcode = RBUS_ERROR_BUS_ERROR;
			}
		}
            }
            else
            {
		WebcfgError("Response from remote Get method failed!!\n");
		errorcode = RBUS_ERROR_BUS_ERROR;
            }

            rbusMessage_Release(response);
        }
	WebcfgInfo("rbus_GetValueFromDB End\n");
	return errorcode;
}

/**
 * Register data elements for dataModel implementation using rbus.
 * Data element over bus will be Device.X_RDK_WebConfig.RfcEnable, Device.X_RDK_WebConfig.ForceSync,
 * Device.X_RDK_WebConfig.URL
 */
WEBCFG_STATUS regWebConfigDataModel()
{
	rbusError_t ret = RBUS_ERROR_SUCCESS;
	WEBCFG_STATUS status = WEBCFG_SUCCESS;

	WebcfgInfo("Registering parameters deRfc %s, deForceSync %s, deURL %s\n", WEBCFG_RFC_PARAM, WEBCFG_FORCESYNC_PARAM, WEBCFG_URL_PARAM);
	if(!rbus_handle)
	{
		WebcfgError("regWebConfigDataModel Failed in getting bus handles\n");
		return WEBCFG_FAILURE;
	}

	rbusDataElement_t dataElements[NUM_WEBCFG_ELEMENTS] = {

		{WEBCFG_RFC_PARAM, RBUS_ELEMENT_TYPE_PROPERTY, {webcfgDataGetHandler, webcfgDataSetHandler, NULL, NULL, NULL, NULL}},
		{WEBCFG_URL_PARAM, RBUS_ELEMENT_TYPE_PROPERTY, {webcfgDataGetHandler, webcfgDataSetHandler, NULL, NULL, NULL, NULL}},
		{WEBCFG_FORCESYNC_PARAM, RBUS_ELEMENT_TYPE_PROPERTY, {webcfgDataGetHandler, webcfgDataSetHandler, NULL, NULL, NULL, NULL}},

	};
	ret = rbus_regDataElements(rbus_handle, NUM_WEBCFG_ELEMENTS, dataElements);
	if(ret == RBUS_ERROR_SUCCESS)
	{
		WebcfgInfo("Registered data element %s with rbus \n ", WEBCFG_RFC_PARAM);
	}
	else
	{
		WebcfgError("Failed in registering data element %s \n", WEBCFG_RFC_PARAM);
		status = WEBCFG_FAILURE;
	}

	WebcfgInfo("rbus reg status returned is %d\n", status);
	return status;
}


//maps rbus rbusValueType_t to WDMP datatype
DATA_TYPE mapRbusToWdmpDataType(rbusValueType_t rbusType)
{
	DATA_TYPE wdmp_type = WDMP_NONE;

	switch (rbusType)
	{
		case RBUS_INT16:
		case RBUS_INT32:
			wdmp_type = WDMP_INT;
			break;
		case RBUS_UINT16:
		case RBUS_UINT32:
			wdmp_type = WDMP_UINT;
			break;
		case RBUS_INT64:
			wdmp_type = WDMP_LONG;
			break;
		case RBUS_UINT64:
			wdmp_type = WDMP_ULONG;
			break;
		case RBUS_SINGLE:
			wdmp_type = WDMP_FLOAT;
			break;
		case RBUS_DOUBLE:
			wdmp_type = WDMP_DOUBLE;
			break;
		case RBUS_DATETIME:
			wdmp_type = WDMP_DATETIME;
			break;
		case RBUS_BOOLEAN:
			wdmp_type = WDMP_BOOLEAN;
			break;
		case RBUS_CHAR:
		case RBUS_INT8:
			wdmp_type = WDMP_INT;
			break;
		case RBUS_UINT8:
		case RBUS_BYTE:
			wdmp_type = WDMP_UINT;
			break;
		case RBUS_STRING:
			wdmp_type = WDMP_STRING;
			break;
		case RBUS_BYTES:
			wdmp_type = WDMP_BYTE;
			break;
		case RBUS_PROPERTY:
		case RBUS_OBJECT:
		case RBUS_NONE:
		default:
			wdmp_type = WDMP_NONE;
			break;
	}
	WebcfgInfo("mapRbusToWdmpDataType : wdmp_type is %d\n", wdmp_type);
	return wdmp_type;
}
