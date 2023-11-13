 /**
  * Copyright 2019 Comcast Cable Communications Management, LLC
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
 */
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include <pthread.h>
#include <time.h>
#include "../src/webcfg_log.h"
#include "../src/webcfg_metadata.h"

/*----------------------------------------------------------------------------*/
/*                             Mock Functions                             */
/*----------------------------------------------------------------------------*/
void webcfgStrncpy(char *destStr, const char *srcStr, size_t destSize)
{
    strncpy(destStr, srcStr, destSize-1);
    destStr[destSize-1] = '\0';
}

int writeToFile(char *file_path, char *data, size_t size)
{
	FILE *fp;
	fp = fopen(file_path , "w+");
	if (fp == NULL)
	{
		WebcfgError("Failed to open file in db %s\n", file_path );
		return 0;
	}
	if(data !=NULL)
	{
		fwrite(data, size, 1, fp);
		fclose(fp);
		return 1;
	}
	else
	{
		WebcfgError("Write failed, Data is NULL\n");
		fclose(fp);
		return 0;
	}
}
/*----------------------------------------------------------------------------*/
/*                             Test Functions                             */
/*----------------------------------------------------------------------------*/

void test_initWebcfgProperties()
{
	char buf[512] = {'\0'};
	snprintf(buf,sizeof(buf),"WEBCONFIG_SUPPORTED_DOCS_BIT=00000000000000000000000000000000|00000000000000000000000000000000");
	int out = writeToFile(WEBCFG_PROPERTIES_FILE, buf, strlen(buf));
	CU_ASSERT_EQUAL(out, 1);
	initWebcfgProperties(WEBCFG_PROPERTIES_FILE);

	snprintf(buf,sizeof(buf),"WEBCONFIG_DOC_SCHEMA_VERSION=1234-v0,2345-v0");
	out = writeToFile(WEBCFG_PROPERTIES_FILE, buf, strlen(buf));
	CU_ASSERT_EQUAL(out, 1);
	initWebcfgProperties(WEBCFG_PROPERTIES_FILE);

	snprintf(buf,sizeof(buf),"WEBCONFIG_SUBDOC_MAP=privatessid:1:true,homessid:2:true,radio:3:false");
        writeToFile(WEBCFG_PROPERTIES_FILE, buf, strlen(buf));
        initWebcfgProperties(WEBCFG_PROPERTIES_FILE);
}

void err_initWebcfgProperties()
{
	char command[128] = {'\0'};
	sprintf(command,"rm -rf %s",WEBCFG_PROPERTIES_FILE);
	system(command);
	initWebcfgProperties(WEBCFG_PROPERTIES_FILE);
}

void test_supportedDocs()
{
	char *docs = NULL;
	setsupportedDocs("00000001000000000000000000000001");
	docs = getsupportedDocs();
	printf("docs: %s\n",docs);
	CU_ASSERT_STRING_EQUAL(docs, "00000001000000000000000000000001");
	free(docs);
	setsupportedDocs("00000000000000000000000000000000");
	docs = getsupportedDocs();
	printf("docs: %s\n",docs);
	CU_ASSERT_STRING_EQUAL(docs, "00000000000000000000000000000000");
	free(docs);
	setsupportedDocs("00000000000000000000000000000000|10010001000000000000000000000011");
	docs = getsupportedDocs();
	printf("docs: %s\n",docs);
	CU_ASSERT_STRING_EQUAL(docs, "00000000000000000000000000000000|10010001000000000000000000000011");
	free(docs);
	setsupportedDocs("");
	docs = getsupportedDocs();
	printf("docs: %s\n",docs);
	CU_ASSERT_STRING_EQUAL(docs, "");
	free(docs);
	setsupportedDocs(NULL);
	docs = getsupportedDocs();
	printf("docs: %s\n",docs);
	CU_ASSERT_EQUAL(docs, NULL);
}

void test_supportedVersions()
{
	char *versions = NULL;
	setsupportedVersion("1234-v0");
	versions = getsupportedVersion();
	CU_ASSERT_STRING_EQUAL("1234-v0", versions);
	printf("versions: %s\n",versions);
	free(versions);
	setsupportedVersion("2345-v0,432-v1");
	versions = getsupportedVersion();
	CU_ASSERT_STRING_EQUAL("2345-v0,432-v1", versions);
	printf("versions: %s\n",versions);
	free(versions);
}

void test_isSubDocSupported()
{
	int sup; 
	setsupportedDocs("00000010000000000000000000000011|00001010000000000000000000000010");
	sup = isSubDocSupported("homessid");
	CU_ASSERT_EQUAL(0,sup);
	sup = isSubDocSupported("aker");
	CU_ASSERT_NOT_EQUAL(0,sup);
	sup = isSubDocSupported("privatessid");
	CU_ASSERT_EQUAL(0,sup);
	sup = isSubDocSupported("radioreport");
	CU_ASSERT_NOT_EQUAL(0,sup);
	sup = isSubDocSupported("interfacereport");
	CU_ASSERT_EQUAL(1,sup);
	setsupportedDocs("10000010000000000000000000000011|00001010100000000000000000000010|00000000000000000000000000000000");
	sup = isSubDocSupported("lan");
	CU_ASSERT_NOT_EQUAL(0,sup);
	sup = isSubDocSupported("wifi");
	CU_ASSERT_EQUAL(1,sup);
	setsupportedDocs("");
	sup = isSubDocSupported("wan");
	CU_ASSERT_NOT_EQUAL(0,sup);
	setsupportedDocs(NULL);
	sup = isSubDocSupported("mesh");
	CU_ASSERT_EQUAL(1,sup);
}

void test_supplementaryDocs_validInput(void)
{
	char buf[512] = {'\0'};
        snprintf(buf,sizeof(buf),"telemetry");
	setsupplementaryDocs(buf);
	char *get = getsupplementaryDocs();
	CU_ASSERT_STRING_EQUAL("telemetry",get);
	WebcfgInfo("The get value is %s\n",get);
	snprintf(buf,sizeof(buf),"NULL");
	setsupplementaryDocs(buf);
	get = getsupplementaryDocs();
	CU_ASSERT_STRING_EQUAL("NULL",get);
}

void test_get_set_supplementaryDocs(void)
{
	char str[1024] = {'\0'};
        char *value = NULL;
        strcpy(str, "WEBCONFIG_SUPPLEMENTARY_DOCS=telemetry");
        if(NULL != (value =strstr(str,"WEBCONFIG_SUPPLEMENTARY_DOCS")))
        {
                WebcfgInfo("The value stored is %s\n", value);
                value = value + strlen("WEBCONFIG_SUPPLEMENTARY_DOCS=");
                value[strlen(value)] = '\0';
                setsupplementaryDocs(value);
		value = NULL;
	}
	char *get = getsupplementaryDocs();
	WebcfgInfo("The get value is %s\n",get);
	CU_ASSERT_STRING_EQUAL(get,"telemetry");
}

void test_isSupplementaryDoc(void)
{
	int result;
	char validSubDoc[]="wan";
	char str[1024] = {'\0'};
        char *value = NULL;
        strcpy(str, "WEBCONFIG_SUPPLEMENTARY_DOCS=00000000000000000000000000000000|00000000000000000000000000000000");
        if(NULL != (value =strstr(str,"WEBCONFIG_SUPPLEMENTARY_DOCS")))
        {
                WebcfgInfo("The value stored is %s\n", value);
                value = value + strlen("WEBCONFIG_SUPPLEMENTARY_DOCS=");
                value[strlen(value)] = '\0';
                setsupplementaryDocs(value);
                value = NULL;
        }
	char *get = getsupplementaryDocs();
        WebcfgInfo("The get value is %s\n",get);
	CU_ASSERT_STRING_EQUAL("00000000000000000000000000000000|00000000000000000000000000000000",get);
	result = isSupplementaryDoc(validSubDoc);
	CU_ASSERT_EQUAL(1,result);
	WebcfgInfo("The result value is %d\n",result);
}

void test_get_spInfoHead(void)
{
	SupplementaryDocs_t *sd = NULL;
	sd = (SupplementaryDocs_t *)malloc(sizeof(SupplementaryDocs_t));
	if(sd != NULL)
	{
		sd->name="privatessid";
		sd->next=NULL;
	}

	SupplementaryDocs_t *sd1 = NULL;
        sd1 = (SupplementaryDocs_t *)malloc(sizeof(SupplementaryDocs_t));
	if(sd1 != NULL)
	{
		sd1->name="wan";
                sd1->next=NULL;
		sd->next=sd1;
	}
	SupplementaryDocs_t *sd2 = NULL;
        sd2 = (SupplementaryDocs_t *)malloc(sizeof(SupplementaryDocs_t));
        if(sd2 != NULL)
        {
                sd2->name="wifi";
                sd1->next=NULL;
                sd1->next=sd2;
        }
		
	CU_ASSERT_PTR_NOT_NULL(sd);
	set_global_spInfoHead(sd);
	
	SupplementaryDocs_t *result = get_global_spInfoHead();
	CU_ASSERT_EQUAL("privatessid",result->name);
}

void test_get_spInfotail(void)
{
        SupplementaryDocs_t *sd = NULL;
        sd = (SupplementaryDocs_t *)malloc(sizeof(SupplementaryDocs_t));
        if(sd != NULL)
        {
                sd->name="privatessid";
                sd->next=NULL;
        }

        SupplementaryDocs_t *sd1 = NULL;
        sd1 = (SupplementaryDocs_t *)malloc(sizeof(SupplementaryDocs_t));
        if(sd1 != NULL)
        {
                sd1->name="wan";
                sd1->next=NULL;
                sd->next=sd1;
        }
        SupplementaryDocs_t *sd2 = NULL;
        sd2 = (SupplementaryDocs_t *)malloc(sizeof(SupplementaryDocs_t));
        if(sd2 != NULL)
        {
                sd2->name="wifi";
                sd1->next=NULL;
                sd1->next=sd2;
        }

        CU_ASSERT_PTR_NOT_NULL(sd);
	set_global_spInfoTail(sd2);

        SupplementaryDocs_t *result = get_global_spInfoTail();
        CU_ASSERT_STRING_EQUAL("wifi",result->name);
}

void test_displaystruct(void)
{
	SubDocSupportMap_t *test = NULL;
        test = (SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
        if(test!=NULL)
        {
                strcpy(test->name,"privatessid");
                strcpy(test->support,"true");
				#ifdef WEBCONFIG_BIN_SUPPORT
                strcpy(test->rbus_listener,"true");
                strcpy(test->dest,"webconfig.pam.portforwarding");
				#endif
                test->next=NULL;
        }
	SubDocSupportMap_t *test2 = NULL;
        test2 =(SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
        if(test2!=NULL)
        {
                strcpy(test2->name,"radio");
                strcpy(test2->support,"true");
				#ifdef WEBCONFIG_BIN_SUPPORT
                strcpy(test2->rbus_listener,"true");
                strcpy(test2->dest,"webconfig.pam.portforwarding");
				#endif
                test2->next=NULL;
                test->next=test2;
        }
        CU_ASSERT_PTR_NOT_NULL(test);
	set_global_sdInfoHead(test);
	set_global_sdInfoTail(test2);	

        displaystruct();
	
	SubDocSupportMap_t *tmp = get_global_sdInfoHead();
	WebcfgInfo("The tmp value is %s\n",tmp->name);
	CU_ASSERT_PTR_NOT_NULL(tmp);
	CU_ASSERT_STRING_EQUAL("privatessid",tmp);
	tmp = get_global_sdInfoTail();
	WebcfgInfo("The tmp value is %s\n",tmp->name);
	CU_ASSERT_PTR_NOT_NULL(tmp);
	CU_ASSERT_STRING_EQUAL("radio",tmp);

}
#ifdef WEBCONFIG_BIN_SUPPORT
void test_get_destination(void)
{
	char dest[64];
	memset(dest, 0, sizeof(dest));
	SubDocSupportMap_t *test = NULL;
        test = (SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
        if(test!=NULL)
        {
                strcpy(test->name,"homessid");
                strcpy(test->support,"true");
                strcpy(test->rbus_listener,"true");
                strcpy(test->dest,"webconfig.pam.portforwarding");
                test->next=NULL;
        }
        
        SubDocSupportMap_t *test2 = NULL;
        test2 =(SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
        if(test2!=NULL)
        {
                strcpy(test2->name,"privatessid");
                strcpy(test2->support,"true");
                strcpy(test2->rbus_listener,"true");
                strcpy(test2->dest,"webconfig.pam.portforwarding");
                test2->next=NULL;
                test->next=test2;
        }
        CU_ASSERT_PTR_NOT_NULL(test);
	set_global_sdInfoHead(test);
	
	//positive scenario
	int get_dest = get_destination("homessid",dest);
	WebcfgInfo("Return value of get_destination %d\n",get_dest);
	CU_ASSERT_EQUAL(0,get_dest);

	get_dest = get_destination("privatessid",dest);
        WebcfgInfo("Return value of get_destination %d\n",get_dest);
        CU_ASSERT_EQUAL(0,get_dest);

	//negative scenario
	get_dest = get_destination("radio",dest);
        WebcfgInfo("Return value of get_destination %d\n",get_dest);
        CU_ASSERT_EQUAL(1,get_dest);
	set_global_sdInfoHead(NULL);	
}

void test_isRbusListener_true()
{
	SubDocSupportMap_t *test = NULL;
	test = (SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
	if(test!=NULL)
	{
		strcpy(test->name,"homessid");
		strcpy(test->support,"true");
		strcpy(test->rbus_listener,"true");
		strcpy(test->dest,"webconfig.pam.portforwarding");
		test->next=NULL;
	}
	
	SubDocSupportMap_t *test2 = NULL;
	test2 =(SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
	if(test2!=NULL)
	{
		strcpy(test2->name,"privatessid");
                strcpy(test2->support,"true");
                strcpy(test2->rbus_listener,"true");
                strcpy(test2->dest,"webconfig.pam.portforwarding");
		test2->next=NULL;
		test->next=test2;
	}
	CU_ASSERT_PTR_NOT_NULL(test);
	set_global_sdInfoHead(test);

	bool result = isRbusListener("homessid");
	CU_ASSERT_EQUAL(1,result);
	result = isRbusListener("privatessid");
	CU_ASSERT_EQUAL(1,result);
}

void test_isRbusListener_false()
{
SubDocSupportMap_t *test = NULL;
        test = (SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
        if(test!=NULL)
        {
                strcpy(test->name,"homessid");
                strcpy(test->support,"true");
                strcpy(test->rbus_listener,"false");
                strcpy(test->dest,"webconfig.pam.portforwarding");
                test->next=NULL;
        }

        SubDocSupportMap_t *test2 = NULL;
        test2 =(SubDocSupportMap_t *)malloc(sizeof(SubDocSupportMap_t));
        if(test2!=NULL)
        {
                strcpy(test2->name,"privatessid");
                strcpy(test2->support,"true");
                strcpy(test2->rbus_listener,"false");
                strcpy(test2->dest,"webconfig.pam.portforwarding");
                test2->next=NULL;
                test->next=test2;
        }
	CU_ASSERT_PTR_NOT_NULL(test);
	set_global_sdInfoHead(test);
	bool result = isRbusListener("homessid");
        CU_ASSERT_EQUAL(0,result);
        result = isRbusListener("privatessid");
        CU_ASSERT_EQUAL(0,result);
	
	//subdoc which is not present
	result = isRbusListener("wan");
	CU_ASSERT_EQUAL(0,result);
}
#endif
void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Test initWebcfgProperties\n", test_initWebcfgProperties);
	CU_add_test( *suite, "Error initWebcfgProperties\n", err_initWebcfgProperties);
	CU_add_test( *suite, "Test Supported docs\n", test_supportedDocs);
	CU_add_test( *suite, "Test Supported versions\n", test_supportedVersions);
	CU_add_test( *suite, "Test isSubDocSupported\n", test_isSubDocSupported);
	CU_add_test( *suite, "Test supplementary docs valid\n", test_supplementaryDocs_validInput);
	CU_add_test( *suite, "Test set get supplementaryDocs\n", test_get_set_supplementaryDocs);
	CU_add_test( *suite, "Test isSupplementaryDoc\n", test_isSupplementaryDoc);
	CU_add_test( *suite, "Test get_spInfoHead\n", test_get_spInfoHead);
	CU_add_test( *suite, "Test get_spInfoTail\n", test_get_spInfotail);
	CU_add_test( *suite, "Test displaystruct\n", test_displaystruct);
#ifdef WEBCONFIG_BIN_SUPPORT	
	CU_add_test( *suite, "Test get_destination\n", test_get_destination);
	CU_add_test( *suite, "Test isRbusListener true\n", test_isRbusListener_true);	
	CU_add_test( *suite, "Test isRbusListener false\n", test_isRbusListener_false);
#endif	
		
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main()
{
	unsigned rv = 1;
	CU_pSuite suite = NULL;
	if( CUE_SUCCESS == CU_initialize_registry() ) {
	add_suites( &suite );

	if( NULL != suite ) {
	    CU_basic_set_mode( CU_BRM_VERBOSE );
	    CU_basic_run_tests();
	    printf( "\n" );
	    CU_basic_show_failures( CU_get_failure_list() );
	    printf( "\n\n" );
	    rv = CU_get_number_of_tests_failed();
	}

	CU_cleanup_registry();

	}
	return rv;
}
