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

void (*rbusnotifyCbFnPtr)(NotifyData*) = NULL;

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

static void webpaRbus_Uninit()
{
    rbus_close(rbus_handle);
}

/**
 * Data set handler for Webpa parameters
 */
rbusError_t webcfgDataSetHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts) {

    WebcfgInfo("Inside webcfgDataSetHandler\n");

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
			tmpchar = strdup("true");
		    }
		    else
		    {
			tmpchar = strdup("false");
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
                retStatus = RBUS_ERROR_INVALID_INPUT;
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
    char* componentName = NULL;
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
				*RfcVal = true;
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
	WebcfgInfo("URL value fetched is %s\n", value);
        rbusValue_Release(value);

    }else if(strncmp(propertyName, WEBCFG_FORCESYNC_PARAM, maxParamLen) == 0) {
        rbusValue_t value;
        rbusValue_Init(&value);
        rbusValue_SetString(value, "");
        rbusProperty_SetValue(property, value);
	WebcfgInfo("forceSyncVal value fetched is %s\n", value);
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
	bool commit = TRUE;
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
	rbusMessage_SetString(request, commit ? "TRUE" : "FALSE");


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

            if((errorcode == RBUS_ERROR_SUCCESS) || (errorcode == CCSP_SUCCESS)) //legacy error codes returned from component PSM.
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

	    if((errorcode == RBUS_ERROR_SUCCESS) || (errorcode == CCSP_SUCCESS)) //legacy error code from component.
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

	WebcfgInfo("Registering parameters deRfc %s, deForceSync %s, deURL %s\n", WEBCFG_RFC_PARAM, WEBCFG_FORCESYNC_PARAM, WEBCFG_URL_PARAM;
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

void getValues_rbus(const char *paramName[], const unsigned int paramCount, int index, money_trace_spans *timeSpan, param_t ***paramArr, int *retValCount, int *retStatus)
{
	int cnt1=0;
	int resCount = 0;
	rbusError_t rc;
	rbusProperty_t props = NULL;
	char* paramValue = NULL;
	char *pName = NULL;
	int i =0;
	int val_size = 0;
	int startIndex = 0;
	rbusValue_t paramValue_t = NULL;
	rbusValueType_t type_t;
	int cnt=0;
	int retIndex=0;
	int error = 0;
	int ret = 0;
	int numComponents = 0;
	char **componentName = NULL;
	int compRet = 0;

	char parameterName[MAX_PARAMETERNAME_LEN] = {'\0'};

	for(cnt1 = 0; cnt1 < paramCount; cnt1++)
	{
		WebcfgInfo("rbus_getExt paramName[%d] : %s paramCount %d\n",cnt1,paramName[cnt1], paramCount);
		//walStrncpy(parameterName,paramName[cnt1],sizeof(parameterName));
		retIndex=IndexMpa_WEBPAtoCPE(paramName[cnt1]);
		if(retIndex == -1)
		{
			if(strstr(paramName[cnt1], PARAM_RADIO_OBJECT) != NULL)
			{
				ret = CCSP_ERR_INVALID_RADIO_INDEX; //need to return rbus error
				WebcfgError("%s has invalid Radio index, Valid indexes are 10000 and 10100. ret = %d\n", paramName[cnt1],ret);
			}
			else
			{
				ret = CCSP_ERR_INVALID_WIFI_INDEX;
				WebcfgError("%s has invalid WiFi index, Valid range is between 10001-10008 and 10101-10108. ret = %d\n",paramName[cnt1], ret);
			}
			error = 1;
			break;
		}

		WebcfgInfo("After mapping paramName[%d] : %s\n",cnt1,paramName[cnt1]);

	}

	//disc component
	for(cnt1 = 0; cnt1 < paramCount; cnt1++)
	{
		WebcfgInfo("paramName[%d] : %s\n",cnt1,paramName[cnt1]);

		rc = rbus_discoverComponentName(rbus_handle,paramCount,paramName,&numComponents,&componentName);
		WebcfgInfo("rc is %d\n", rc);
		if(RBUS_ERROR_SUCCESS == rc)
		{
			WebcfgInfo ("Discovered components are,\n");
			for(i=0;i<numComponents;i++)
			{
				WebcfgInfo("rbus_discoverComponentName %s: %s\n", paramName[i],componentName[i]);
				//free(componentName[i]);
			}
			//free(componentName);
		}
		else
		{
			WebcfgError ("Failed to discover component array. Error Code = %d\n", rc);
		}

		compRet = 1;
	}

	//testing.
	if(compRet == 1)
	{
		*retStatus = mapRbusStatus(rc);
		WebcfgError("disc comp *retStatus returning %d\n", *retStatus);
		return;
	}

	if(error == 1)
	{
		WebcfgError("error 1. returning ret %d\n", ret);
		/*for (cnt1 = 0; cnt1 < paramCount; cnt1++)
		{
			WEBCFG_FREE(paramName[cnt1]);
		}
		WebcfgError("Free paramName\n");
		WEBCFG_FREE(paramName);*/
		WebcfgError("*retstatus\n");
		*retStatus = ret;
		WebcfgError("*retStatus returning %d\n", *retStatus);
		return;
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
	}
	if(props)
	{
		rbusProperty_t next = props;
		val_size = resCount;
		WebcfgInfo("val_size : %d\n",val_size);
		if(val_size > 0)
		{
			if(paramCount == val_size)// && (parameterNamesLocal[0][strlen(parameterNamesLocal[0])-1] != '.'))
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

						IndexMpa_CPEtoWEBPA(&pName);
						IndexMpa_CPEtoWEBPA(&paramValue);

						WebcfgInfo("Framing paramArr\n");
						(*paramArr)[i][0].name = strdup(pName);
						(*paramArr)[i][0].value = strdup(paramValue);
						(*paramArr)[i][0].type = mapRbusToWdmpDataType(type_t);
						WebcfgInfo("success: %s %s %d \n",(*paramArr)[i][0].name,(*paramArr)[i][0].value, (*paramArr)[i][0].type);
						*retValCount = resCount;
						*retStatus = mapRbusStatus(rc);
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
			else
			{
				WebcfgInfo("Wildcard response\n");

				if(startIndex == 0)
				{
					WebcfgInfo("Single dest component wildcard\n");
					(*paramArr)[i] = (param_t *) malloc(sizeof(param_t)*val_size);
				}
				else
				{
					WebcfgInfo("Multi dest components wild card\n");
					(*paramArr)[i] = (param_t *) realloc((*paramArr)[i], sizeof(param_t)*(startIndex + val_size));
				}

				for (cnt = 0; cnt < val_size; cnt++)
				{
					//WebcfgInfo("Stack:> success: %s %s %d \n",parameterval[cnt][0].parameterName,parameterval[cnt][0].parameterValue, parameterval[cnt][0].type);
					//IndexMpa_CPEtoWEBPA(&parameterval[cnt][0].parameterName);
					//IndexMpa_CPEtoWEBPA(&parameterval[cnt][0].parameterValue);

					WebcfgInfo("Response Param is %s\n", rbusProperty_GetName(next));
					paramValue_t = rbusProperty_GetValue(next);

					if(paramValue_t)
					{
						type_t = rbusValue_GetType(paramValue_t);
						paramValue = rbusValue_ToString(paramValue_t, NULL, 0);
						WebcfgInfo("Response paramValue is %s\n", paramValue);
						pName = strdup(rbusProperty_GetName(next));

						IndexMpa_CPEtoWEBPA(&pName);
						IndexMpa_CPEtoWEBPA(&paramValue);

						WebcfgInfo("Framing paramArr\n");
						(*paramArr)[i][cnt+startIndex].name = strdup(pName);
						(*paramArr)[i][cnt+startIndex].value = strdup(paramValue);
						(*paramArr)[i][cnt+startIndex].type = mapRbusToWdmpDataType(type_t);
						WebcfgInfo("success: %s %s %d \n",(*paramArr)[i][cnt+startIndex].name,(*paramArr)[i][cnt+startIndex].value, (*paramArr)[i][cnt+startIndex].type);
						*retValCount = resCount;
						*retStatus = mapRbusStatus(rc);
						if(paramValue !=NULL)
						{
							WEBCFG_FREE(paramValue);
						}
						if(pName !=NULL)
						{
							WEBCFG_FREE(pName);
						}
					}
					next = rbusProperty_GetNext(next);
					//
					//WebcfgInfo("B4 assignment\n");
					//(*paramArr)[paramIndex][cnt+startIndex].name = parameterval[cnt][0].parameterName;
					//(*paramArr)[paramIndex][cnt+startIndex].value = parameterval[cnt][0].parameterValue;
					//(*paramArr)[paramIndex][cnt+startIndex].type = parameterval[cnt][0].type;
					//WebcfgInfo("success: %s %s %d \n",(*paramArr)[paramIndex][cnt+startIndex].name,(*paramArr)[paramIndex][cnt+startIndex].value, (*paramArr)[paramIndex][cnt+startIndex].type);
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

//To map Rbus error code to Ccsp error codes.
int mapRbusStatus(int Rbus_error_code)
{
    int CCSP_error_code = CCSP_Msg_Bus_ERROR;
    switch (Rbus_error_code)
    {
        case  0  : CCSP_error_code = CCSP_Msg_Bus_OK; break;
        case  1  : CCSP_error_code = CCSP_Msg_Bus_ERROR; break;
        case  2  : CCSP_error_code = 9007; break;// CCSP_ERR_INVALID_PARAMETER_VALUE
        case  3  : CCSP_error_code = CCSP_Msg_Bus_OOM; break;
        case  4  : CCSP_error_code = CCSP_Msg_Bus_ERROR; break;
        case  5  : CCSP_error_code = CCSP_Msg_Bus_ERROR; break;
        case  6  : CCSP_error_code = CCSP_Msg_BUS_TIMEOUT; break;
        case  7  : CCSP_error_code = CCSP_Msg_BUS_TIMEOUT; break;
        case  8  : CCSP_error_code = CCSP_ERR_UNSUPPORTED_PROTOCOL; break;
        case  9  : CCSP_error_code = CCSP_Msg_BUS_NOT_SUPPORT; break;
        case  10 : CCSP_error_code = CCSP_Msg_BUS_NOT_SUPPORT; break;
        case  11 : CCSP_error_code = CCSP_Msg_Bus_OOM; break;
        case  12 : CCSP_error_code = CCSP_Msg_BUS_CANNOT_CONNECT; break;
    }
    return CCSP_error_code;
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
