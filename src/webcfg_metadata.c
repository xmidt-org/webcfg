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
static char * supported_bits = NULL;
static char * supported_version = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/


void initWebcfgProperties(char * filename)
{
	FILE *fp = NULL;
	char str[MAXCHAR];
	char * temp_bit_token = NULL, * temp_version_token = NULL;
	char * supported_bits_temp = NULL, * supported_version_temp = NULL;

	WebcfgDebug("webcfg properties file path is %s\n", filename);
	fp = fopen(filename,"r");

	if (fp == NULL)
	{
		WebcfgError("Failed to open file %s\n", filename);
	}

	while (fgets(str, MAXCHAR, fp) != NULL)
	{
		if((strstr(str,"WEBCONFIG_SUPPORTED_DOCS_BIT")!=NULL))
		{
			WebcfgDebug("The value stored in temp_bit_token is %s\n", str);
			temp_bit_token = strdup(str);
		}

		if((strstr(str,"WEBCONFIG_DOC_SCHEMA_VERSION")!=NULL))
		{
			WebcfgDebug("The value stored in temp_version_token is %s\n", str);
			temp_version_token = strdup(str);
		}
	}
	fclose(fp);

	if(temp_bit_token != NULL)
	{
		supported_bits_temp = strtok(temp_bit_token,"=");
		supported_bits_temp = strtok(NULL,"=");
		supported_bits_temp = strtok(supported_bits_temp,"\n");
		setsupportedDocs(supported_bits_temp);
		WebcfgDebug("Supported bits final %s value\n",supported_bits);
	}

	if(temp_version_token != NULL)
	{ 
		supported_version_temp = strtok(temp_version_token,"=");
		supported_version_temp = strtok(NULL,"=");
		supported_version_temp = strtok(supported_version_temp,"\n");
		setsupportedVersion(supported_version_temp);
		WebcfgDebug("supported_version = %s value\n", supported_version);
	}
	WEBCFG_FREE(temp_bit_token);
	WEBCFG_FREE(temp_version_token);
}

void setsupportedDocs( char * value)
{
     supported_bits = strdup(value);
}

void setsupportedVersion( char * value)
{
     supported_version = strdup(value);
}


char * getsupportedDocs()
{
    char * token_temp = strdup(supported_bits);
    char * token = strtok(token_temp,"|");
    long long number = 0;
    char * endptr = NULL;
    char * finalvalue = NULL , *tempvalue = NULL;
    finalvalue = malloc(180);
    memset(finalvalue,0,180);
    tempvalue = malloc(20);
    while(token != NULL)
    {
        memset(tempvalue,0,20);
        number = strtoll(token,&endptr,2);
        sprintf(tempvalue,"%lld", number);
        token = strtok(NULL, "|");
        strcat(finalvalue, tempvalue);
        if(token!=NULL)
        {
            strcat(finalvalue, ",");
        }
    }
    WEBCFG_FREE(tempvalue);
    WEBCFG_FREE(token_temp);
    return finalvalue;
}

char * getsupportedVersion()
{
      return supported_version;
}
