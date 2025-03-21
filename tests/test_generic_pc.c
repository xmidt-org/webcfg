#ifdef WEBCONFIG_BIN_SUPPORT
#include <rbus/rbus.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include "../src/webcfg_generic.h"
#define UNUSED(x) (void )(x)
#define CCSP_Msg_Bus_OK             100
#define CCSP_Msg_Bus_ERROR          102
#define CCSP_Msg_BUS_TIMEOUT        191
#define CCSP_ERR_INVALID_PARAMETER_VALUE 9007
#define CCSP_ERR_UNSUPPORTED_NAMESPACE 204
bool rbus_status=true;
bool isRbusEnabled() 
{
	return rbus_status;
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

#ifdef WEBCONFIG_BIN_SUPPORT
rbusError_t registerRBUSEventElement()
{
    return RBUS_ERROR_SUCCESS;
}
int remove_rbus =  RBUS_ERROR_SUCCESS;
rbusError_t removeRBUSEventElement()
{
    return remove_rbus; 
}
#endif

void test_rbus_waitUntilSystemReady()
{
    int ret = -1;
    ret = rbus_waitUntilSystemReady();
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_rbus_StoreValueIntoDB()
{
    int ret = -1;
    ret = rbus_StoreValueIntoDB("paramName","value");
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_rbus_GetValueFromDB()
{
    int ret = -1;
    ret = rbus_GetValueFromDB("paramName",NULL);
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_rbus_setAttributes()
{
    param_t *attArr = NULL;
	setAttributes(attArr, 1, NULL, NULL);
}

void test_mapStatus(void)
{
	WDMP_STATUS ret = WDMP_FAILURE;
#ifdef WEBCONFIG_BIN_SUPPORT      
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
    rbus_status = false;
    ret = mapStatus(0);
    CU_ASSERT_EQUAL(ret, 0);  
    rbus_status = true; 
#else
    ret = mapStatus(0);
    CU_ASSERT_EQUAL(ret, 0);  
#endif               
}

void test_unregisterWebcfgEvent()
{
#ifdef WEBCONFIG_BIN_SUPPORT     
    int ret = 0;   
    ret = unregisterWebcfgEvent();
    CU_ASSERT_EQUAL(ret, 1); 
    remove_rbus = RBUS_ERROR_BUS_ERROR;
    ret = unregisterWebcfgEvent();
    CU_ASSERT_EQUAL(ret, 0);  
    remove_rbus = RBUS_ERROR_SUCCESS; 
#endif          
}

void test_registerWebcfgEvent()
{
    WebConfigEventCallback webcfgEventCB;
    webcfgEventCB = NULL;
    int ret = 0;
#ifdef WEBCONFIG_BIN_SUPPORT    
    ret = registerWebcfgEvent(webcfgEventCB);
    CU_ASSERT_EQUAL(ret, 1);   
#else
    ret = registerWebcfgEvent(webcfgEventCB);
    CU_ASSERT_EQUAL(ret, 0);  
#endif          
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

void test_Set_Supplementary_URL()
{
    int ret = -1;
    ret = Set_Supplementary_URL("name","pString");
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_Get_Supplementary_URL()
{
    int ret = -1;
    ret = Get_Supplementary_URL("name","pString");
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_Set_Webconfig_URL()
{
    int ret = -1;
    ret = Set_Webconfig_URL("pString");
    CU_ASSERT_EQUAL(ret, 0);    
}

void test_Get_Webconfig_URL()
{
    int ret = -1;
    ret = Get_Webconfig_URL("pString");
    CU_ASSERT_EQUAL(ret, 0);    
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

void test_getFirmwareUpgradeEndTime()
{
    char *pString = "test";
    pString = getFirmwareUpgradeEndTime();
    CU_ASSERT_EQUAL(pString, NULL);        
}

void test_getFirmwareUpgradeStartTime()
{
    char *pString = "test";
    pString = getFirmwareUpgradeStartTime();
    CU_ASSERT_EQUAL(pString, NULL);        
}

void test_get_deviceWanMAC()
{
    char *pString = "test";
    pString = get_deviceWanMAC();
    CU_ASSERT_STRING_EQUAL(pString, "123456789000");        
}

void test_get_deviceMAC()
{
    char *pString = "test";
    pString = get_deviceMAC();
    CU_ASSERT_STRING_EQUAL(pString, "123456789000");        
}

void test_getFirmwareVersion()
{
    char *pString = "test";
    pString = getFirmwareVersion();
    CU_ASSERT_STRING_EQUAL(pString, "CGM4140COM_DEV_master_202100000000000sdy");        
}

void test_getConnClientParamName()
{
    char *pString = "test";
    pString = getConnClientParamName();
    CU_ASSERT_EQUAL(pString, NULL);        
}

void test_getRebootReason()
{
    char *pString = "test";
    pString = getRebootReason();
    CU_ASSERT_STRING_EQUAL(pString, "Forced_Software_upgrade");        
}

void test_getAccountID()
{
    char *pString = "test";
    pString = getAccountID();
    CU_ASSERT_STRING_EQUAL(pString, "unknown");
}

void test_getPartnerID()
{
    char *pString = "test";
    pString = getPartnerID();
    CU_ASSERT_STRING_EQUAL(pString, "comcast");        
}
#ifdef WAN_FAILOVER_SUPPORTED
void test_getInterfaceName()
{
    char *pString = "test";
    pString = getInterfaceName();
    CU_ASSERT_STRING_EQUAL(pString, "erouter0");        
}
#endif

void test_getModelName()
{
    char *pString = "test";
    pString = getModelName();
    CU_ASSERT_STRING_EQUAL(pString, "CGM4140COM");        
}

void test_getProductClass()
{
    char *pString = "test";
    pString = getProductClass();
    CU_ASSERT_STRING_EQUAL(pString, "XB6");        
}

void test_getSerialNumber()
{
    char *pString = "test";
    pString = getSerialNumber();
    CU_ASSERT_STRING_EQUAL(pString, "123456789123456789");        
}

void test_getDeviceBootTime()
{
    char *pString = "test";
    pString = getDeviceBootTime();
    CU_ASSERT_STRING_EQUAL(pString, "1634658266");        
}

void test_getTimeOffset()
{
    int ret = -1;      
	ret = getTimeOffset();
    CU_ASSERT_EQUAL(ret, 0);    
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "test rbus_waitUntilSystemReady", test_rbus_waitUntilSystemReady);
    CU_add_test( *suite, "test rbus_StoreValueIntoDB", test_rbus_StoreValueIntoDB);
    CU_add_test( *suite, "test rbus_GetValueFromDB", test_rbus_GetValueFromDB);    
    CU_add_test( *suite, "test setAttributes", test_rbus_setAttributes);  
    CU_add_test( *suite, "test mapStatus", test_mapStatus);    
    CU_add_test( *suite, "test unregisterWebcfgEvent", test_unregisterWebcfgEvent); 
    CU_add_test( *suite, "test registerWebcfgEvent", test_registerWebcfgEvent);  
    CU_add_test( *suite, "test sendNotification", test_sendNotification); 
    CU_add_test( *suite, "test setValues", test_setValues);       
    CU_add_test( *suite, "test Set_Supplementary_URL", test_Set_Supplementary_URL);    
    CU_add_test( *suite, "test Get_Supplementary_URL", test_Get_Supplementary_URL);    
    CU_add_test( *suite, "test Set_Webconfig_URL", test_Set_Webconfig_URL); 
    CU_add_test( *suite, "test Get_Webconfig_URL", test_Get_Webconfig_URL);                          
    CU_add_test( *suite, "test getForceSync", test_getForceSync);  
    CU_add_test( *suite, "test setForceSync", test_setForceSync);                  
    CU_add_test( *suite, "test set_global_systemReadyTime", test_set_global_systemReadyTime);
    CU_add_test( *suite, "test getFirmwareUpgradeEndTime", test_getFirmwareUpgradeEndTime);
    CU_add_test( *suite, "test getFirmwareUpgradeStartTime", test_getFirmwareUpgradeStartTime);    
    CU_add_test( *suite, "test get_deviceWanMAC", test_get_deviceWanMAC);     
    CU_add_test( *suite, "test get_deviceMAC", test_get_deviceMAC);      
    CU_add_test( *suite, "test getFirmwareVersion", test_getFirmwareVersion);      
    CU_add_test( *suite, "test getConnClientParamName", test_getConnClientParamName);  
    CU_add_test( *suite, "test getRebootReason", test_getRebootReason); 
    CU_add_test( *suite, "test getAccountID", test_getAccountID);
    CU_add_test( *suite, "test getPartnerID", test_getPartnerID);     
#ifdef WAN_FAILOVER_SUPPORTED
    CU_add_test( *suite, "test getInterfaceName", test_getInterfaceName);  
#endif    
    CU_add_test( *suite, "test getModelName", test_getModelName);  
    CU_add_test( *suite, "test getProductClass", test_getProductClass);  
    CU_add_test( *suite, "test getSerialNumber", test_getSerialNumber);  
    CU_add_test( *suite, "test getDeviceBootTime", test_getDeviceBootTime);     
    CU_add_test( *suite, "test getTimeOffset", test_getTimeOffset);        
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
 
