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

static rbusValueType_t mapWdmpToRbusDataType(DATA_TYPE wdmpType)
{
	DATA_TYPE rbusType = RBUS_NONE;

	switch (wdmpType)
	{
		case WDMP_INT:
			rbusType = RBUS_INT32;
			break;
		case WDMP_UINT:
			rbusType = RBUS_UINT32;
			break;
		case WDMP_LONG:
			rbusType = RBUS_INT64;
			break;
		case WDMP_ULONG:
			rbusType = RBUS_UINT64;
			break;
		case WDMP_FLOAT:
			rbusType = RBUS_SINGLE;
			break;
		case WDMP_DOUBLE:
			rbusType = RBUS_DOUBLE;
			break;
		case WDMP_DATETIME:
			rbusType = RBUS_DATETIME;
			break;
		case WDMP_BOOLEAN:
			rbusType = RBUS_BOOLEAN;
			break;
		case WDMP_STRING:
			rbusType = RBUS_STRING;
			break;
		case WDMP_BYTE:
			rbusType = RBUS_BYTES;
			break;
		case WDMP_NONE:
		default:
			rbusType = RBUS_NONE;
			break;
	}

	WebcfgInfo("mapWdmpToRbusDataType : rbusType is %d\n", rbusType);
	return rbusType;
}

//To map Rbus error code to Ccsp error codes.
int mapRbusToCcspStatus(int Rbus_error_code)
{
    int CCSP_error_code = CCSP_Msg_Bus_ERROR;
    switch (Rbus_error_code)
    {
        case  RBUS_ERROR_SUCCESS  : CCSP_error_code = CCSP_Msg_Bus_OK; break;
        case  RBUS_ERROR_BUS_ERROR  : CCSP_error_code = CCSP_Msg_Bus_ERROR; break;
        case  RBUS_ERROR_INVALID_INPUT  : CCSP_error_code = CCSP_ERR_INVALID_PARAMETER_VALUE; break;
        case  RBUS_ERROR_NOT_INITIALIZED  : CCSP_error_code = CCSP_Msg_Bus_ERROR; break;
        case  RBUS_ERROR_OUT_OF_RESOURCES  : CCSP_error_code = CCSP_Msg_Bus_OOM; break;
        case  RBUS_ERROR_DESTINATION_NOT_FOUND  : CCSP_error_code = CCSP_Msg_BUS_CANNOT_CONNECT; break;
        case  RBUS_ERROR_DESTINATION_NOT_REACHABLE  : CCSP_error_code = CCSP_Msg_BUS_CANNOT_CONNECT; break;
        case  RBUS_ERROR_DESTINATION_RESPONSE_FAILURE  : CCSP_error_code = CCSP_Msg_BUS_TIMEOUT; break;
        case  RBUS_ERROR_INVALID_RESPONSE_FROM_DESTINATION  : CCSP_error_code = CCSP_ERR_UNSUPPORTED_PROTOCOL; break;
        case  RBUS_ERROR_INVALID_OPERATION  : CCSP_error_code = CCSP_Msg_BUS_NOT_SUPPORT; break;
        case  RBUS_ERROR_INVALID_EVENT : CCSP_error_code = CCSP_Msg_BUS_NOT_SUPPORT; break;
        case  RBUS_ERROR_INVALID_HANDLE : CCSP_error_code = CCSP_Msg_BUS_NOT_SUPPORT; break;
        case  RBUS_ERROR_SESSION_ALREADY_EXIST : CCSP_error_code = CCSP_Msg_BUS_NOT_SUPPORT; break;
    }
    return CCSP_error_code;
}

//To map Rbus error code to Ccsp error codes.
/*int mapRbusStatus(int Rbus_error_code)
{
    int CCSP_error_code = WDMP_BUS_ERROR;
    switch (Rbus_error_code)
    {
        case  RBUS_ERROR_SUCCESS  : CCSP_error_code = WDMP_SUCCESS; break;
        case  RBUS_ERROR_BUS_ERROR  : CCSP_error_code = WDMP_FAILURE; break;
        case  RBUS_ERROR_INVALID_INPUT  : CCSP_error_code = WDMP_ERR_INVALID_PARAMETER_VALUE; break;
        case  RBUS_ERROR_NOT_INITIALIZED  : CCSP_error_code = WDMP_FAILURE; break;
        case  RBUS_ERROR_OUT_OF_RESOURCES  : CCSP_error_code = WDMP_BUS_OOM; break;
        case  RBUS_ERROR_DESTINATION_NOT_FOUND  : CCSP_error_code = WDMP_BUS_CANNOT_CONNECT; break;
        case  RBUS_ERROR_DESTINATION_NOT_REACHABLE  : CCSP_error_code = WDMP_BUS_CANNOT_CONNECT; break;
        case  RBUS_ERROR_DESTINATION_RESPONSE_FAILURE  : CCSP_error_code = WDMP_ERR_TIMEOUT; break;
        case  RBUS_ERROR_INVALID_RESPONSE_FROM_DESTINATION  : CCSP_error_code = WDMP_ERR_UNSUPPORTED_PROTOCOL; break;
        case  RBUS_ERROR_INVALID_OPERATION  : CCSP_error_code = WDMP_BUS_NOT_SUPPORT; break;
        case  RBUS_ERROR_INVALID_EVENT : CCSP_error_code = WDMP_BUS_NOT_SUPPORT; break;
        case  RBUS_ERROR_INVALID_HANDLE : CCSP_error_code = WDMP_BUS_NOT_SUPPORT; break;
        case  RBUS_ERROR_SESSION_ALREADY_EXIST : CCSP_error_code = WDMP_BUS_NOT_SUPPORT; break;
    }
    return CCSP_error_code;
}*/


void setValues_rbus(const param_t paramVal[], const unsigned int paramCount, const int setType,char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspRetStatus)
{
	int cnt = 0;
	int isInvalid = 0;
	bool isCommit = true;
	int sessionId = 0;
	rbusError_t ret = RBUS_ERROR_BUS_ERROR;
	rbusProperty_t properties = NULL, last = NULL;
	rbusValue_t setVal[paramCount];
	char const* setNames[paramCount];
	*retStatus = WDMP_FAILURE;

	if(!rbus_handle)
	{
		WebcfgError("setValues_rbus Failed as rbus_handle is not initialized\n");
		return;
	}

	for(cnt=0; cnt<paramCount; cnt++)
	{
		rbusValue_Init(&setVal[cnt]);

		setNames[cnt] = paramVal[cnt].name;
		WebcfgInfo("paramName to be set is %s paramCount %d\n", paramVal[cnt].name, paramCount);
		WebcfgInfo("paramVal is %s\n", paramVal[cnt].value);

		rbusValueType_t type = mapWdmpToRbusDataType(paramVal[cnt].type);

		if (type == RBUS_NONE)
		{
			WebcfgError("Invalid data type\n");
			isInvalid = 1;
			break;
		}

		rbusValue_SetFromString(setVal[cnt], type, paramVal[cnt].value);

		rbusProperty_t next;
		rbusProperty_Init(&next, setNames[cnt], setVal[cnt]);

		WebcfgInfo("Property Name[%d] is %s\n", cnt, rbusProperty_GetName(next));
		WebcfgInfo("Value type[%d] is %d\n", cnt, rbusValue_GetType(setVal[cnt]));

		if(properties == NULL)
		{
			properties = last = next;
		}
		else
		{
			rbusProperty_SetNext(last, next);
			last=next;
		}
	}

	if(!isInvalid)
	{
		isCommit = true;
		sessionId = 0;

		rbusSetOptions_t opts = {isCommit,sessionId};

		ret = rbus_setMulti(rbus_handle, paramCount, properties, &opts);
		WebcfgInfo("The ret status for rbus_setMulti is %d\n", ret);
	}
	else
	{
		ret = RBUS_ERROR_INVALID_INPUT;
		WebcfgError("Invalid input. ret %d\n", ret);
	}

	*ccspRetStatus = mapRbusToCcspStatus((int)ret);
	WebcfgInfo("ccspRetStatus is %d\n", *ccspRetStatus);

        *retStatus = mapStatus(*ccspRetStatus);

	WebcfgInfo("paramCount is %d\n", paramCount);
        for (cnt = 0; cnt < paramCount; cnt++)
        {
            rbusValue_Release(setVal[cnt]);
        }
	WebcfgInfo("After Value free\n");
        rbusProperty_Release(properties);
	WebcfgInfo("After properties free\n");
}

void getValues_rbus(const char *paramName[], const unsigned int paramCount, int index, money_trace_spans *timeSpan, param_t ***paramArr, int *retValCount, int *retStatus)
{
	int resCount = 0;
	rbusError_t rc;
	rbusProperty_t props = NULL;
	char* paramValue = NULL;
	char *pName = NULL;
	int i =0;
	int val_size = 0;
	rbusValue_t paramValue_t = NULL;
	rbusValueType_t type_t;
	int cnt=0;
	*retStatus = WDMP_FAILURE;

	for(cnt = 0; cnt < paramCount; cnt++)
	{
		WebcfgInfo("rbus_getExt paramName[%d] : %s paramCount %d\n",cnt,paramName[cnt], paramCount);
	}

	if(!rbus_handle)
	{
		WebcfgError("getValues_rbus Failed as rbus_handle is not initialized\n");
		return;
	}
	rc = rbus_getExt(rbus_handle, paramCount, paramName, &resCount, &props);
	WebcfgInfo("After rbus_getExt\n");

	WebcfgInfo("rbus_getExt rc=%d resCount=%d\n", rc, resCount);

	if(RBUS_ERROR_SUCCESS != rc)
	{
		WebcfgError("Failed to get value\n");
		rbusProperty_Release(props);
		WebcfgError("getValues_rbus failed\n");
		return;
	}
	if(props)
	{
		rbusProperty_t next = props;
		val_size = resCount;
		WebcfgInfo("val_size : %d\n",val_size);
		if(val_size > 0)
		{
			if(paramCount == val_size)
			{
				for (i = 0; i < resCount; i++)
				{
					WebcfgInfo("Response Param is %s\n", rbusProperty_GetName(next));
					paramValue_t = rbusProperty_GetValue(next);

					if(paramValue_t)
					{
						type_t = rbusValue_GetType(paramValue_t);
						paramValue = rbusValue_ToString(paramValue_t, NULL, 0);
						WebcfgInfo("Response paramValue is %s\n", paramValue);
						pName = strdup(rbusProperty_GetName(next));
						(*paramArr)[i] = (param_t *) malloc(sizeof(param_t));

						WebcfgInfo("Framing paramArr\n");
						(*paramArr)[i][0].name = strdup(pName);
						(*paramArr)[i][0].value = strdup(paramValue);
						(*paramArr)[i][0].type = mapRbusToWdmpDataType(type_t);
						WebcfgInfo("success: %s %s %d \n",(*paramArr)[i][0].name,(*paramArr)[i][0].value, (*paramArr)[i][0].type);
						*retValCount = resCount;
						//*retStatus = mapRbusStatus(rc);
						*retStatus = WDMP_SUCCESS;
						if(paramValue !=NULL)
						{
							WEBCFG_FREE(paramValue);
						}
						if(pName !=NULL)
						{
							WEBCFG_FREE(pName);
						}
					}
					else
					{
						WebcfgError("Parameter value from rbus_getExt is empty\n");
					}
					next = rbusProperty_GetNext(next);
				}
			}
		}
		else if(val_size == 0 && rc == RBUS_ERROR_SUCCESS)
		{
			WebcfgInfo("No child elements found\n");
		}
		WebcfgInfo("rbusProperty_Release\n");
		rbusProperty_Release(props);
	}
	WebcfgInfo("getValues_rbus End\n");
}

