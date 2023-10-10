#include <rbus/rbus.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "../src/webcfg_generic.h"
int Get_Mqtt_LocationId( char *pString);
int Get_Mqtt_NodeId( char *pString);
int Get_Mqtt_Broker( char *pString);
int Get_Mqtt_Port( char *pString);
char* Get_Mqtt_ClientId();
#define UNUSED(x) (void )(x)
#define CCSP_Msg_Bus_OK             100
#define CCSP_Msg_Bus_ERROR          102
#define CCSP_Msg_BUS_TIMEOUT        191
#define CCSP_ERR_INVALID_PARAMETER_VALUE 9007
#define CCSP_ERR_UNSUPPORTED_NAMESPACE 204

bool isRbusEnabled() 
{

	return true;
}
int set_rbus_ForceSync(char* pString, int *pStatus)
{
    UNUSED(pString);
    UNUSED(pStatus);    
	return 1;    
}
int get_rbus_ForceSync(char** pString, char **transactionId )
{
    UNUSED(pString);
    UNUSED(transactionId);    
	return 1;     
}
void setValues_rbus(const param_t paramVal[], const unsigned int paramCount, const int setType,char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspRetStatus)
{
    UNUSED(paramVal);
    UNUSED(paramCount);
    UNUSED(setType);
    UNUSED(transactionId);
    UNUSED(timeSpan);
    UNUSED(retStatus);
    UNUSED(ccspRetStatus);
}
void sendNotification_rbus(char *payload, char *source, char *destination)
{
    UNUSED(payload);
    UNUSED(source);
    UNUSED(destination);
}
rbusError_t registerRBUSEventElement()
{
    return RBUS_ERROR_SUCCESS;
}
rbusError_t removeRBUSEventElement()
{
    static int i = 0;
    if(i == 0)
    {
        i=1;
        return RBUS_ERROR_SUCCESS; 
    }
    else
    {
        i=0;
        return RBUS_ERROR_BUS_ERROR;
    }  
}
// Test function for rbus_GetValueFromDB
void test_setAttributes(void)
{
    param_t *attArr = NULL;
	setAttributes(attArr, 1, NULL, NULL);
}

void test_mapStatus(void)
{
	WDMP_STATUS ret = WDMP_FAILURE;
    ret = mapStatus(CCSP_Msg_Bus_OK);
    CU_ASSERT_EQUAL(ret, WDMP_SUCCESS);
    ret = mapStatus(CCSP_Msg_BUS_TIMEOUT);
    CU_ASSERT_EQUAL(ret, WDMP_ERR_TIMEOUT);
    ret = mapStatus(CCSP_ERR_INVALID_PARAMETER_VALUE);
    CU_ASSERT_EQUAL(ret, WDMP_ERR_INVALID_PARAMETER_VALUE);
    ret = mapStatus(CCSP_ERR_UNSUPPORTED_NAMESPACE);
    CU_ASSERT_EQUAL(ret, WDMP_ERR_UNSUPPORTED_NAMESPACE);         
    ret = mapStatus(CCSP_Msg_Bus_ERROR);
    CU_ASSERT_EQUAL(ret, WDMP_FAILURE);           
}

void test_unregisterWebcfgEvent()
{
    int ret = 0;
    ret = unregisterWebcfgEvent();
    CU_ASSERT_EQUAL(ret, 1); 
    ret = unregisterWebcfgEvent();
    CU_ASSERT_EQUAL(ret, 0);     
}

void test_registerWebcfgEvent()
{
    WebConfigEventCallback webcfgEventCB;
    webcfgEventCB = NULL;
    int ret = 0;
    ret = registerWebcfgEvent(webcfgEventCB);
    CU_ASSERT_EQUAL(ret, 1);         
}

void test_sendNotification()
{
    sendNotification(NULL,NULL,NULL);
}

void test_setValues()
{
	int ccspStatus=0;
	WDMP_STATUS ret = WDMP_FAILURE;    
	setValues(NULL, 0, 0, NULL, NULL, &ret, &ccspStatus);
}

void test_getForceSync()
{
	char* syncTransID = NULL;
	char* ForceSyncDoc = NULL; 
    int ret = -1;   
	ret = getForceSync(&ForceSyncDoc, &syncTransID);
    CU_ASSERT_EQUAL(ret, 0);
}

void test_setForceSync()
{
    int ret = -1;      
	ret = setForceSync("", "", 0);
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_set_global_systemReadyTime()
{
    set_global_systemReadyTime("test1");
    char *pString = NULL;
    pString = get_global_systemReadyTime();
    CU_ASSERT_STRING_EQUAL(pString, "test1");    
    set_global_systemReadyTime("test2");
    pString = get_global_systemReadyTime();
    CU_ASSERT_STRING_EQUAL(pString, "test2");        
}

void test_Get_Mqtt_LocationId()
{
    int ret = -1;
    ret = Get_Mqtt_LocationId("test1");
    CU_ASSERT_EQUAL(ret, 0);        
}

void test_Get_Mqtt_NodeId()
{
    int ret = -1;
    ret = Get_Mqtt_NodeId("test1");
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_Get_Mqtt_Broker()
{
    int ret = -1;
    ret = Get_Mqtt_Broker("test1");
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_Get_Mqtt_Port()
{
    int ret = -1;
    ret = Get_Mqtt_Port("test1");
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_Get_Mqtt_ClientId()
{
    Get_Mqtt_ClientId();
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "test setAttributes", test_setAttributes);
    CU_add_test( *suite, "test mapStatus", test_mapStatus);
    CU_add_test( *suite, "test unregisterWebcfgEvent", test_unregisterWebcfgEvent);    
    CU_add_test( *suite, "test registerWebcfgEvent", test_registerWebcfgEvent);   
    CU_add_test( *suite, "test sendNotification", test_sendNotification);         
    CU_add_test( *suite, "test setValues", test_setValues);    
    CU_add_test( *suite, "test getForceSync", test_getForceSync);
    CU_add_test( *suite, "test setForceSync", test_setForceSync);
    CU_add_test( *suite, "test set_global_systemReadyTime", test_set_global_systemReadyTime);
    CU_add_test( *suite, "test Get_Mqtt_LocationId", test_Get_Mqtt_LocationId);
    CU_add_test( *suite, "test Get_Mqtt_NodeId", test_Get_Mqtt_NodeId);      
    CU_add_test( *suite, "test Get_Mqtt_Broker", test_Get_Mqtt_Broker);      
    CU_add_test( *suite, "test Get_Mqtt_Port", test_Get_Mqtt_Port);  
    CU_add_test( *suite, "test Get_Mqtt_ClientId", test_Get_Mqtt_ClientId);  
}

int main( int argc, char *argv[] )
{
    unsigned rv = 1;
    CU_pSuite suite = NULL;

    (void ) argc;
    (void ) argv;

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
 
