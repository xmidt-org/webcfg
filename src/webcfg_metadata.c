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
typedef struct
{
    char name[256];//portforwarding or wlan
    char support[10];//true or false;
}SubDocSupportMap_t;

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static char * supported_bits = NULL;
static char * supported_version = NULL;
static int g_webcont = 0;
SubDocSupportMap_t *sdInfo = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void displaystruct(SubDocSupportMap_t *sdInfo,int count);
void incrementWebCount();
int getWebCount();
SubDocSupportMap_t * get_global_sdInfo(void);
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/


void initWebcfgProperties(char * filename)
{
	FILE *fp = NULL;
	char str[MAXCHAR] = {'\0'};
	char * temp_bit_token = NULL, * temp_version_token = NULL;
	char * supported_bits_temp = NULL, * supported_version_temp = NULL;
	//For WEBCONFIG_SUBDOC_MAP parsing
	int i =0;
	int line_index =0;
	char *p;
	char *token;

	sdInfo = (SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t) * MAXCHAR);
	memset(sdInfo, 0, sizeof(SubDocSupportMap_t) * MAXCHAR);
	if( sdInfo==NULL )
	{
		WebcfgError("Unable to allocate memory");
		return;
	}

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

		if(strncmp(str,"WEBCONFIG_SUBDOC_MAP",strlen("WEBCONFIG_SUBDOC_MAP")) ==0)
		{
			
			p = str;
			char *tok = NULL;
			WebcfgDebug("The value of tok is %s\n", tok);
			tok = strtok_r(p, " =",&p);
			token = strtok_r(p,",",&p);
			while(token!= NULL)
			{

				char subdoc[100];
				char *subtoken;
				strcpy(subdoc,token);
				puts(subdoc);
				subtoken = strtok(subdoc,":");//portforwarding or lan

				if(subtoken == NULL)
				{
					return;
				}

				strcpy(sdInfo[i].name,subtoken);
				subtoken = strtok(NULL,":");//skip 1st value
				subtoken = strtok(NULL,":");//true or false
				strcpy(sdInfo[i].support,subtoken); 
				token =strtok_r(p,",",&p);
				i++;

				incrementWebCount();    
			}
			line_index++;

		}
	}
	fclose(fp);

	if(temp_bit_token != NULL)
	{
		supported_bits_temp = strdup(temp_bit_token);
		setsupportedDocs(supported_bits_temp);
		WebcfgDebug("Supported bits final %s value\n",supported_bits);
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

	if(sdInfo != NULL)
	{
		displaystruct(get_global_sdInfo(), getWebCount());
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
	WebcfgInfo("The value in supportedbits get is %s\n",supported_bits);
	return supported_bits;
}

char * getsupportedVersion()
{
      WebcfgInfo("The value in supportedversion get is %s\n",supported_version);
      return supported_version;
}

WEBCFG_STATUS isSubDocSupported(char *subDoc)
{
	int i = 0;

	SubDocSupportMap_t *sd = NULL;
	
	sd = get_global_sdInfo();

	for(i=0;i<getWebCount();i++)
	{
		if(strncmp(sd[i].name, subDoc, strlen(subDoc)) == 0)
		{
			WebcfgDebug("The subdoc %s is present\n",sd[i].name);
			if(strncmp(sd[i].support, "true", strlen("true")) == 0)
			{
				WebcfgInfo("%s is supported\n",subDoc);
				return WEBCFG_SUCCESS;
				
			}
			else
			{
				WebcfgInfo("%s is not supported\n",subDoc);
				return WEBCFG_FAILURE;
			}
		}
		
	}
	WebcfgError("Supported doc bit not found for %s\n",subDoc);
	return WEBCFG_FAILURE;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
SubDocSupportMap_t * get_global_sdInfo(void)
{
    SubDocSupportMap_t *tmp = NULL;
    tmp = sdInfo;
    return tmp;
}

void incrementWebCount()
{
	g_webcont = g_webcont + 1;
}

int getWebCount()
{
	return g_webcont;
}

void displaystruct(SubDocSupportMap_t *sd,int count)
{
	int i;
	WebcfgDebug("The struct values are:\n");
	for(i=0;i<count;i++)
	WebcfgDebug(" %s %s\n",sd[i].name,sd[i].support);
}
