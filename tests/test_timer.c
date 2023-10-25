#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <CUnit/Basic.h>
#include "../src/webcfg_timer.h"
#include "../src/webcfg_log.h"

#define UNUSED(x) (void )(x)
char *firmwareUpgradeStartTime=NULL;
char *firmwareUpgradeEndTime=NULL;

long getTimeOffset(void)
{
    	return 0;
}

uint16_t generateRandomId()
{
    	return 1;
}

void set_global_supplementarySync(int value)
{
    	UNUSED(value);
}

//mock function to change upgradestarttime for null and different values
void setFirmwareUpgradeStartTime(char* startTime) 
{
    	firmwareUpgradeStartTime = startTime;
}

void setFirmwareUpgradeEndTime(char* endTime)
{
	firmwareUpgradeEndTime = endTime;
}

char *getFirmwareUpgradeStartTime(void)
{
	if (firmwareUpgradeStartTime != NULL) 
	{
		return firmwareUpgradeStartTime;
	} 
	else 
	{
		return NULL; 
    	}
}

char *getFirmwareUpgradeEndTime(void)
{
    	if (firmwareUpgradeEndTime != NULL)
        {
                return firmwareUpgradeEndTime;
        }
        else
        {
                return NULL; 
        }
}

void test_printTime()
{
	long long timestamp = 1618232435;
	char* formattedTime = printTime(timestamp);
	WebcfgInfo("Formatted Time: %s\n", formattedTime);
	CU_ASSERT_STRING_NOT_EQUAL(formattedTime,"");

	//to compare current time
	long long currentTimestamp = (long long)time(NULL);
	formattedTime = printTime(currentTimestamp);
	// Get the current date and time in the expected format
	time_t rawtime = currentTimestamp;
	struct tm* ts = localtime(&rawtime);
	char expectedTime[80];
	strftime(expectedTime, sizeof(expectedTime), "%a %y%m%d %H:%M:%S", ts);
	// Compare the actual formatted time string with the expected string
	CU_ASSERT_STRING_EQUAL(formattedTime, expectedTime);
}

void test_initMaintenanceTimer()
{
	//default values
	initMaintenanceTimer();
	CU_ASSERT_TRUE(get_global_maintenance_time() > 0);
	
	//starttime and endtime are different
	setFirmwareUpgradeStartTime("3344");
	setFirmwareUpgradeEndTime("5544");
	initMaintenanceTimer();
	CU_ASSERT_TRUE(get_global_maintenance_time() > 0);
	
	//starttime equal to endtime
	setFirmwareUpgradeStartTime("3344");
	setFirmwareUpgradeEndTime("3344");
	initMaintenanceTimer();
	CU_ASSERT_TRUE(get_global_maintenance_time() >= 3600 && get_global_maintenance_time() <= 14400);	

	//starttime > endtime
	setFirmwareUpgradeStartTime("6453");
	setFirmwareUpgradeEndTime("3344");
	initMaintenanceTimer();
	CU_ASSERT_TRUE(get_global_maintenance_time() >= 3600 && get_global_maintenance_time() <= 14400);
}

void test_checkMaintenanceTimer()
{
	//Maintenance time equal to current time	
	int result = checkMaintenanceTimer();
	CU_ASSERT_EQUAL(1,result);
	
	//not equal
	struct timespec rt;
	clock_gettime(CLOCK_REALTIME, &rt);
	long cur_time_in_sec = rt.tv_sec;
	set_global_maintenance_time(cur_time_in_sec);
	result = checkMaintenanceTimer();
	CU_ASSERT_EQUAL(result, 0);
}

void test_getMaintenanceSyncSeconds()
{
	//count equal to 0
	int maintenance_count = 0;
	int result = getMaintenanceSyncSeconds(maintenance_count);

	struct timespec ct;
	clock_gettime(CLOCK_REALTIME, &ct);
	long current_time_in_sec = getTimeInSeconds(ct.tv_sec + getTimeOffset());
	long expected_maintenance_secs = get_global_maintenance_time() - current_time_in_sec;
	CU_ASSERT_EQUAL(result, expected_maintenance_secs);

	//count equal to 1
	maintenance_count = 1;
	result = getMaintenanceSyncSeconds(maintenance_count);
    	clock_gettime(CLOCK_REALTIME, &ct);
       	current_time_in_sec = getTimeInSeconds(ct.tv_sec + getTimeOffset());
     	long sec_to_12 = 86400 - current_time_in_sec;
    	expected_maintenance_secs = sec_to_12 + get_global_maintenance_time();
	CU_ASSERT_EQUAL(result, expected_maintenance_secs);
}

void test_getTimeInSeconds()
{
	struct tm timeinfo;
	timeinfo.tm_year = 2023-1900;  // Years since 1900 (e.g., 2023)
	timeinfo.tm_mon = 9;    // Month (0 - 11, so 9 is October)
	timeinfo.tm_mday = 25;  // Day of the month (1 - 31)
	timeinfo.tm_hour = 13;  // Hour (0 - 23)
	timeinfo.tm_min = 45;   // Minutes (0 - 59)
	timeinfo.tm_sec = 50;   // Seconds (0 - 59)
	timeinfo.tm_isdst = 0;   //+1 Dayslight saving, 0 no dayloght saving, -1 i dont know.
	
	// Convert the time to a time_t value
	time_t time_value = mktime(&timeinfo);
	
	if (time_value == -1) {
        	// Handle the case where mktime returns -1
		WebcfgError("Error: Invalid date or time\n");
	} 
     	else
	{
		WebcfgInfo("time_value: %lld\n", (long long)time_value);
		// Call the function with the defined time
		long seconds_since_midnight = getTimeInSeconds(time_value);
		WebcfgInfo("The value is %ld\n",seconds_since_midnight);
		// Calculate the expected result for the given time
		long expected_result = 13 * 3600 + 45 * 60 + 50;  // 13 hours, 45 minutes, and 50 seconds since midnight
		WebcfgInfo("The expected result is %ld\n",expected_result);
		CU_ASSERT_EQUAL(seconds_since_midnight, expected_result);
	}
}

void test_updateRetryTimeDiff()
{
	struct timespec ct;
      	clock_gettime(CLOCK_REALTIME, &ct);
     	long long present_time = ct.tv_sec;

	//expire time 10 sec in the future
    	long long expiry_time = present_time + 10;
	
	//get_retry_timer > time_diff
	set_retry_timer(60);
	
	//retry_timer updated but global_retry_timestamp remains same
	int time_diff = updateRetryTimeDiff(expiry_time);
	int expected_retry_timer = (get_retry_timer() > time_diff) ? time_diff : get_retry_timer();
	CU_ASSERT_EQUAL(get_retry_timer(), expected_retry_timer);
	
	//global_retry_timestamp equal to 0
	set_global_retry_timestamp(0);
	time_diff = updateRetryTimeDiff(expiry_time);		
	
	WebcfgInfo("The get_global_retry_timestamp is %ld\n",get_global_retry_timestamp());
	WebcfgInfo("The getTimeInSeconds is %ld\n",getTimeInSeconds(expiry_time + 900));	
	CU_ASSERT_EQUAL(get_global_retry_timestamp(), getTimeInSeconds(present_time + 900));
}

void test_getRetryExpiryTimeout()
{
	// Get the actual retry timestamp using the function
	long long retry_timestamp = getRetryExpiryTimeout();

	struct timespec ct;
	clock_gettime(CLOCK_REALTIME, &ct);
	long long current_time = ct.tv_sec;
	long long expected_retry_timestamp = current_time + 900;

	CU_ASSERT_EQUAL(retry_timestamp, expected_retry_timestamp);
}

void test_checkRetryTimer_past()
{
	struct timespec ct;
	clock_gettime(CLOCK_REALTIME, &ct);
    	long long current_time = ct.tv_sec;
	long long past_timestamp = current_time - 10;
	int result_past = checkRetryTimer(past_timestamp);

	CU_ASSERT_EQUAL(result_past, 1);
}

void test_checkRetryTimer_future()
{
	struct timespec ct;
        clock_gettime(CLOCK_REALTIME, &ct);    
        long long current_time = ct.tv_sec;
	long long past_timestamp = current_time + 10;
	int result_future = checkRetryTimer(past_timestamp);
        CU_ASSERT_EQUAL(result_future, 0);
}

void test_retrySyncSeconds()
{
	struct timespec ct;
	clock_gettime(CLOCK_REALTIME, &ct);
	long long current_time = 0;
	current_time = ct.tv_sec;
	long current_time_in_sec = getTimeInSeconds(current_time);	
	long retry_secs = 0;
	retry_secs =  get_global_retry_timestamp() - current_time_in_sec;
	int get = retrySyncSeconds();
	WebcfgInfo("The value of retry is %ld\n",retry_secs);
	CU_ASSERT_EQUAL(retry_secs,get);

	//retry_secs = 0
	set_global_retry_timestamp(current_time_in_sec);
	get = retrySyncSeconds();
	CU_ASSERT_EQUAL(0,get);

	//retry_sec < 0
	set_global_retry_timestamp(current_time_in_sec - 10);
	get = retrySyncSeconds();
	CU_ASSERT_EQUAL(0,get);
}

void test_set_get_maintenance_time()
{
        long unixTimestamp = 1635626400;
        set_global_maintenance_time(unixTimestamp);

        CU_ASSERT_EQUAL(unixTimestamp,get_global_maintenance_time());
}

void test_set_get_retry_timer()
{
	int time = 60;
	set_retry_timer(time);

	CU_ASSERT_EQUAL(time, get_retry_timer());
}

void test_set_get_retry_timestamp()
{
	long retry_time = 1635626400;
	set_global_retry_timestamp(retry_time);
	
	CU_ASSERT_EQUAL(retry_time, get_global_retry_timestamp());
}

void add_suites( CU_pSuite *suite )
{
	*suite = CU_add_suite( "tests", NULL, NULL );
	CU_add_test( *suite, "test printTIme", test_printTime);
	CU_add_test( *suite, "test initMaintenanceTimer", test_initMaintenanceTimer);
	CU_add_test( *suite, "test checkMaintenanceTimer", test_checkMaintenanceTimer);
	CU_add_test( *suite, "test getMaintenanceSyncSeconds", test_getMaintenanceSyncSeconds);
	CU_add_test( *suite, "test getTimeInSeconds", test_getTimeInSeconds);
	CU_add_test( *suite, "test updateRetryTimeDiff", test_updateRetryTimeDiff);
	CU_add_test( *suite, "test getRetryExpiryTimeout", test_getRetryExpiryTimeout);
	CU_add_test( *suite, "test checkRetryTimer_past", test_checkRetryTimer_past);
	CU_add_test( *suite, "test checkRetryTimer_future", test_checkRetryTimer_future);
	CU_add_test( *suite, "test retrySyncSeconds", test_retrySyncSeconds);
	CU_add_test( *suite, "test set_get_maintenance_time", test_set_get_maintenance_time);
	CU_add_test( *suite, "test set_get_retry_timer", test_set_get_retry_timer);
	CU_add_test( *suite, "test set_get_retry_timestamp", test_set_get_retry_timestamp);
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

