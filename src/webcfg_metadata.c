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
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "webcfg_log.h"
#include "webcfg_metadata.h"
#include "webcfg_multipart.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define MAXCHAR 1000

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
char *bitMap[MAX_GROUP_SIZE][MAX_SUBDOCS_SIZE] = {{"00000001","portforwarding","wan", "lan","macbinding","hotspot","bridge"},{"00000010","privatessid","homessid","radio"},{"00000011","moca"},{"00000100","xdns"},{"00000101","advsecurity"},{"00000110","mesh"},{"00000111","aker"},{"00001000","telemetry"},{"00001001","trafficreport","statusreport"},{"00001010","radioreport","interfacereport"}};
static char * supported_bits = NULL;
static char * supported_version = NULL;
static char * converted_supported_bits = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int getSubdocGroupId(char *subDoc, char **groupId);
void getSubdDocBitForGroupId(char *groupId, char **subDocBit);
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/


void initWebcfgProperties(char * filename)
{
	FILE *fp = NULL;
	char str[MAXCHAR] = {'\0'};
	char * temp_bit_token = NULL, * temp_version_token = NULL;
	char * supported_bits_temp = NULL, * supported_version_temp = NULL;

	WebcfgDebug("webcfg properties file path is %s\n", filename);
	fp = fopen(filename,"r");

	if (fp == NULL)
	{
		WebcfgError("Failed to open file %s\n", filename);
		return;
	}

	while (fgets(str, MAXCHAR, fp) != NULL)
	{
		char * value = NULL;

		if(NULL != (value = strstr(str,"WEBCONFIG_SUPPORTED_DOCS_BIT=")))
		{
			WebcfgDebug("The value stored in temp_bit_token is %s\n", str);
			value = value + strlen("WEBCONFIG_SUPPORTED_DOCS_BIT=");
			value[strlen(value)-1] = '\0';
			temp_bit_token = strdup(value);
                        value = NULL;
		}

		if(NULL != (value =strstr(str,"WEBCONFIG_DOC_SCHEMA_VERSION")))
		{
			WebcfgDebug("The value stored in temp_version_token is %s\n", str);
			value = value + strlen("WEBCONFIG_DOC_SCHEMA_VERSION=");
			value[strlen(value)-1] = '\0';
			temp_version_token = strdup(value);
			value = NULL;
		}
	}
	fclose(fp);

	if(temp_bit_token != NULL)
	{
		supported_bits_temp = strdup(temp_bit_token);
		setsupportedDocs(supported_bits_temp);
		WebcfgDebug("Supported bits final %s value\n",supported_bits);
		convertSupportedBit(supported_bits_temp);
		WEBCFG_FREE(temp_bit_token);
		WEBCFG_FREE(supported_bits_temp);
	}

	if(temp_version_token != NULL)
	{ 
		supported_version_temp = strdup(temp_version_token);
		setsupportedVersion(supported_version_temp);
		WebcfgDebug("supported_version = %s value\n", supported_version);
		WEBCFG_FREE(temp_version_token);
		WEBCFG_FREE(supported_version_temp);
	}
}

void convertSupportedBit(char * value)
{
	if(value != NULL)
	{
		char * token_temp = strdup(value);
		char * token = strtok(token_temp,"|");
		long long number = 0;
		char * endptr = NULL;
		char * finalvalue = NULL , *tempvalue = NULL;
		finalvalue = malloc(400);
		memset(finalvalue,0,400);
		tempvalue = malloc(40);
		while(token != NULL)
		{
		    memset(tempvalue,0,40);
		    number = strtoll(token,&endptr,2);
		    sprintf(tempvalue,"%lld", number);
		    token = strtok(NULL, "|");
		    strcat(finalvalue, tempvalue);
		    if(token!=NULL)
		    {
		        strcat(finalvalue, ",");
		    }
		}
		setConvertedSupportedDocs(finalvalue);
		WEBCFG_FREE(finalvalue);
		WEBCFG_FREE(tempvalue);
		WEBCFG_FREE(token_temp);
	}
}

void setConvertedSupportedDocs( char * value)
{
	if(value != NULL)
	{
		converted_supported_bits = strdup(value);
	}
	else
	{
		converted_supported_bits = NULL;
	}
}

void setsupportedDocs( char * value)
{
	if(value != NULL)
	{
		supported_bits = strdup(value);
	}
	else
	{
		supported_bits = NULL;
	}
}

void setsupportedVersion( char * value)
{
	if(value != NULL)
	{
		supported_version = strdup(value);
	}
	else
	{
		supported_version = NULL;
	}
}


char * getsupportedDocs()
{
	return supported_bits;
}

char * getconvertedsupportedDocs()
{
	return converted_supported_bits;
}

char * getsupportedVersion()
{
      return supported_version;
}

WEBCFG_STATUS isSubDocSupported(char *subDoc)
{
	int pos = 0;
	char *groupId = NULL, *subDocBit = NULL;
	long long docBit = 0;
	pos = getSubdocGroupId(subDoc, &groupId);
	if(pos > 0)
	{
		WebcfgDebug("%s is at %d position in %s group\n",subDoc,pos, groupId);
		getSubdDocBitForGroupId(groupId, &subDocBit);
		if(subDocBit != NULL)
		{
			WebcfgDebug("subDocBit: %s\n",subDocBit);
			sscanf(subDocBit,"%lld",&docBit);
			WebcfgDebug("docBit: %lld\n",docBit);
	
			if(docBit & (1 << (pos -1)))
			{
				WebcfgInfo("%s is supported\n",subDoc);
				WEBCFG_FREE(groupId);
				WEBCFG_FREE(subDocBit);
				return WEBCFG_SUCCESS;
			}
		}
		else
		{
			WebcfgError("Supported doc bit not found for %s\n",subDoc);
		}
		WEBCFG_FREE(groupId);
		if(subDocBit != NULL)
		{
			WEBCFG_FREE(subDocBit);
		}
		WebcfgInfo("%s is not supported\n",subDoc);
	}
	else
	{
		WebcfgError("Invalid subdoc %s\n",subDoc);
	}
	return WEBCFG_FAILURE;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
int getSubdocGroupId(char *subDoc, char **groupId)
{
	int position = 0, i = 0, j = 0;
	WebcfgDebug("subDoc: %s\n",subDoc);
	for(i=0; i<MAX_GROUP_SIZE; i++)
	{
		for(j=0;j<MAX_SUBDOCS_SIZE; j++)
		{
			if(bitMap[i][j] == NULL)
			{
				break;
			}
			if(strcmp(bitMap[i][j],subDoc) == 0)
			{
				WebcfgDebug("bitMap[%d][%d]: %s\n",i, j, bitMap[i][j]);
				*groupId = strdup(bitMap[i][0]);
				position = j;
				return position;
			}
		}
	}
	return -1;
}

void getSubdDocBitForGroupId(char *groupId, char **subDocBit)
{
	char *temp =  NULL, *numStr = NULL, *tmpStr;
	char subDoc[24] = {'\0'};
	WebcfgDebug("supported_bits: %s\n",supported_bits);
	if(supported_bits != NULL)
	{
		temp = strdup(supported_bits);
		tmpStr = temp;
		while(tmpStr != NULL)
		{
			numStr = strsep(&tmpStr, "|");
			WebcfgDebug("numStr: %s\n",numStr);
			WebcfgDebug("groupId: %s\n",groupId);
			if(strncmp(groupId, numStr, 8) == 0)
			{
				webcfgStrncpy(subDoc, numStr+8,sizeof(subDoc)+1);
				WebcfgDebug("subDoc: %s\n",subDoc);
				*subDocBit= strdup(subDoc);
				break;
			}
		}
		WEBCFG_FREE(temp);
	}
	else
	{
		WebcfgError("Failed to read supported bits from webconfig.properties\n");
	}
}
