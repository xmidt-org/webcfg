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
#include <ctype.h>
#include "webcfg_log.h"
#include "webcfg_metadata.h"
#include "webcfg_multipart.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define MAXCHAR 1024
/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct SubDocSupportMap
{
    char name[256];//portforwarding or wlan
    char support[8];//true or false;
    #ifdef WEBCONFIG_BIN_SUPPORT
    char rbus_listener[8];//true or false
    char dest[64]; //comp destination eg. webconfig.pam.portforwarding
    #endif
    struct SubDocSupportMap *next;
}SubDocSupportMap_t;

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static char * supported_bits = NULL;
static char * supported_version = NULL;
static char * supplementary_docs = NULL;
SubDocSupportMap_t *g_sdInfoHead = NULL;
SubDocSupportMap_t *g_sdInfoTail = NULL;
SupplementaryDocs_t *g_spInfoHead = NULL;
SupplementaryDocs_t *g_spInfoTail = NULL;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
void displaystruct();
SubDocSupportMap_t * get_global_sdInfoHead(void);
SubDocSupportMap_t * get_global_sdInfoTail(void);
SupplementaryDocs_t * get_global_spInfoTail(void);
/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/


void initWebcfgProperties(char * filename)
{
	FILE *fp = NULL;
	char str[MAXCHAR] = {'\0'};
	//For WEBCONFIG_SUBDOC_MAP parsing
	char *p;
	char *token;

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
			WebcfgDebug("The value stored is %s\n", str);
			value = value + strlen("WEBCONFIG_SUPPORTED_DOCS_BIT=");
			value[strlen(value)-1] = '\0';
			setsupportedDocs(value);
                        value = NULL;
		}

		if(NULL != (value =strstr(str,"WEBCONFIG_DOC_SCHEMA_VERSION")))
		{
			WebcfgDebug("The value stored is %s\n", str);
			value = value + strlen("WEBCONFIG_DOC_SCHEMA_VERSION=");
			value[strlen(value)-1] = '\0';
			setsupportedVersion(value);
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
				SubDocSupportMap_t *sdInfo = NULL;
				strncpy(subdoc,token,(sizeof(subdoc)-1));
				puts(subdoc);
				sdInfo = (SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
				if( sdInfo==NULL )
				{
					fclose(fp);
					WebcfgError("Unable to allocate memory\n");
					return;
				}
				memset(sdInfo, 0, sizeof(SubDocSupportMap_t));

				subtoken = strtok(subdoc,":");//portforwarding or lan

				if(subtoken == NULL)
				{
					fclose(fp);
					WEBCFG_FREE(sdInfo);
					return;
				}
				
				strncpy(sdInfo->name,subtoken,(sizeof(sdInfo->name)-1));
				subtoken = strtok(NULL,":");//skip bitposition
				subtoken = strtok(NULL,":");//skip support
				if(subtoken != NULL)
				{
				webcfgStrncpy(sdInfo->support, subtoken, sizeof(sdInfo->support));//To handle subtoken null case
				}
				#ifdef WEBCONFIG_BIN_SUPPORT				
				subtoken = strtok(NULL,":");//skip rbus_listner
				if(subtoken != NULL)
				{
					webcfgStrncpy(sdInfo->rbus_listener, subtoken, sizeof(sdInfo->rbus_listener));
				}
				
				if(sdInfo->rbus_listener != NULL)
				{
					if(strncmp(sdInfo->rbus_listener, "true", strlen("true")) == 0)
					{
						subtoken = strtok(NULL,":");//skip destination
						webcfgStrncpy(sdInfo->dest, subtoken,  sizeof(sdInfo->dest));
					}
				}
				#endif
				token =strtok_r(p,",",&p);
				sdInfo->next = NULL;

				if(g_sdInfoTail == NULL)
				{
					g_sdInfoHead = sdInfo;
					g_sdInfoTail = sdInfo;
				}
				else
				{
					SubDocSupportMap_t *temp =NULL;
					temp = get_global_sdInfoTail();
					temp->next = sdInfo;
					g_sdInfoTail = sdInfo;
				}

			}

		}
		if(NULL != (value =strstr(str,"WEBCONFIG_SUPPLEMENTARY_DOCS")))
		{
			WebcfgDebug("The value stored is %s\n", str);
			value = value + strlen("WEBCONFIG_SUPPLEMENTARY_DOCS=");
			value[strlen(value)-1] = '\0';
			setsupplementaryDocs(value);
			value = NULL;
			supplementaryDocs();
		}
		
	}
	fclose(fp);

	if(g_sdInfoHead != NULL)
	{
		displaystruct();
	}
}

void setsupplementaryDocs( char * value)
{
	if(value != NULL)
	{
		supplementary_docs = strdup(value);
	}
	else
	{
		supplementary_docs = NULL;
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
	WebcfgDebug("The value in supportedbits get is %s\n",supported_bits);
	return supported_bits;
}

char * getsupportedVersion()
{
      WebcfgDebug("The value in supportedversion get is %s\n",supported_version);
      return supported_version;
}

char * getsupplementaryDocs()
{
      WebcfgDebug("The value in supplementary_docs get is %s\n",supplementary_docs);
      return supplementary_docs;
}

WEBCFG_STATUS isSubDocSupported(char *subDoc)
{

	SubDocSupportMap_t *sd = NULL;
	
	sd = get_global_sdInfoHead();

	while(sd != NULL)
	{
		if(strncmp(sd->name, subDoc, strlen(subDoc)) == 0)
		{
			WebcfgDebug("The subdoc %s is present\n",sd->name);
			if(strncmp(sd->support, "true", strlen("true")) == 0)
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
		sd = sd->next;
		
	}
	WebcfgError("Supported doc bit not found for %s\n",subDoc);
	return WEBCFG_FAILURE;
}
#ifdef WEBCONFIG_BIN_SUPPORT
//To find the given subdoc is rbus_listener supported or not
bool isRbusListener(char *subDoc)
{

	SubDocSupportMap_t *sd = NULL;

	sd = get_global_sdInfoHead();

	while(sd != NULL)
	{
		if(strncmp(sd->name, subDoc, strlen(subDoc)) == 0)
		{
			WebcfgDebug("The subdoc %s is present\n",sd->name);
			if(strncmp(sd->rbus_listener, "true", strlen("true")) == 0)
			{
				WebcfgDebug("%s is rbus_listener supported\n",subDoc);
				return true;

			}
			else
			{
				WebcfgDebug("%s is not rbus_listener supported\n",subDoc);
				return false;
			}
		}
		sd = sd->next;

	}
	return false;
}

//To get dest field from SubDocSupportMap struct 
WEBCFG_STATUS get_destination(char* subDoc, char* destination)
{
	SubDocSupportMap_t *sd = NULL;
	sd = get_global_sdInfoHead();

	while(sd != NULL)
	{
		if(strncmp(sd->name, subDoc, strlen(subDoc)) == 0)
		{
			if(strlen(sd->dest) > 0)
			{
				webcfgStrncpy(destination, sd->dest, strlen(sd->dest)+1);
				return WEBCFG_SUCCESS;
			}
		}
		sd = sd->next;	
	}
	return WEBCFG_FAILURE;	
}
#endif

//To check if the doc received during poke is supplementary or not.
WEBCFG_STATUS isSupplementaryDoc(char *subDoc)
{
	SupplementaryDocs_t *sp = NULL;
	sp = get_global_spInfoHead();

	while(sp != NULL)
	{
		WebcfgDebug("Supplementary check for docname %s, subDoc received is %s\n", sp->name, subDoc);
		if(strlen(sp->name) == strlen(subDoc))
		{
			if(strncmp(sp->name, subDoc, strlen(subDoc)) == 0)
			{
				WebcfgDebug("subDoc %s is supplementary\n",subDoc);
				return WEBCFG_SUCCESS;

			}
			else
			{
				WebcfgDebug("subDoc %s is not supplementary\n",subDoc);
				return WEBCFG_FAILURE;
			}
		}
		sp = sp->next;

	}
	return WEBCFG_FAILURE;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
SubDocSupportMap_t * get_global_sdInfoHead(void)
{
    SubDocSupportMap_t *tmp = NULL;
    tmp = g_sdInfoHead;
    return tmp;
}

SubDocSupportMap_t * get_global_sdInfoTail(void)
{
    SubDocSupportMap_t *tmp = NULL;
    tmp = g_sdInfoTail;
    return tmp;
}

SupplementaryDocs_t * get_global_spInfoHead(void)
{
    SupplementaryDocs_t *tmp = NULL;
    tmp = g_spInfoHead;
    return tmp;
}

SupplementaryDocs_t * get_global_spInfoTail(void)
{
    SupplementaryDocs_t *tmp = NULL;
    tmp = g_spInfoTail;
    return tmp;
}

void supplementaryDocs()
{
	int count = 0;
	char* docs = NULL;
	char* docs_var = NULL;
	docs = getsupplementaryDocs();
	if (docs !=NULL)
	{
		docs_var = strndup(docs,strlen(docs));
		char* token = strtok(docs_var, ",");

		while(token != NULL)
		{
			SupplementaryDocs_t *spInfo = NULL;
			spInfo = (SupplementaryDocs_t *)malloc(sizeof(SupplementaryDocs_t));

			if(spInfo == NULL)
			{
				WebcfgError("Unable to allocate memory for supplementary docs\n");
				WEBCFG_FREE(docs_var);
				return;
			}

			memset(spInfo, 0, sizeof(SupplementaryDocs_t));

			WebcfgDebug("The value is %s\n",token);
			spInfo->name = strdup(token);
			spInfo->next = NULL;

			if(g_spInfoTail == NULL)
			{
				g_spInfoHead = spInfo;
				g_spInfoTail = spInfo;
			}
			else
			{
				SupplementaryDocs_t *temp = NULL;
				temp = get_global_spInfoTail();
				temp->next = spInfo;
				g_spInfoTail = spInfo;
			}

			WebcfgDebug("The supplementary_doc[%d] is %s\n", count, spInfo->name);
			count++;
			token = strtok(NULL, ",");
		}
		WEBCFG_FREE(docs_var);
	}
}

void displaystruct()
{
	SubDocSupportMap_t *temp =NULL;
	temp = get_global_sdInfoHead();
	while(temp != NULL)
	{
		WebcfgDebug(" %s %s\n",temp->name,temp->support);
		temp=temp->next;
	}
}

void delete_supplementary_list()
{
	SupplementaryDocs_t *temp = NULL;
	SupplementaryDocs_t *head = NULL;
	head = get_global_spInfoHead();

	while(head != NULL)
	{
		temp = head;
		head = head->next;
		WebcfgDebug("Deleted node: temp->name:%s\n", temp->name);
		free(temp);
		temp = NULL;
	}
	g_spInfoHead = NULL;
	g_spInfoTail = NULL;
}
