/**
 * @file main.c
 *
 * @description This file defines WebPA's main function
 *
 * Copyright (c) 2015  Comcast
 */
#include "stdlib.h"
#include "signal.h"
#include <curl/curl.h>
#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif
#include "webcfg.h"
#include "webcfg_log.h"
#include "webcfg_rbus.h"

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
#ifndef INCLUDE_BREAKPAD
static void sig_handler(int sig);
#endif

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

int main()
{
	//int status = 0;
#ifdef INCLUDE_BREAKPAD
    breakpad_ExceptionHandler();
#else
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	signal(SIGSEGV, sig_handler);
	signal(SIGBUS, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGFPE, sig_handler);
	signal(SIGILL, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGHUP, sig_handler);
	signal(SIGALRM, sig_handler);
#endif
	const char *pComponentName = WEBCFG_COMPONENT_NAME;
	WebcfgInfo("********** Starting component: %s **********\n ", pComponentName); 
	curl_global_init(CURL_GLOBAL_DEFAULT);

	if(isRbusEnabled())
	{
		WebcfgInfo("webconfigRbusInit\n");
		webconfigRbusInit(pComponentName);
		regWebConfigDataModel();
	}

	WebcfgInfo("WebConfig Rfc is enabled, starting WebConfigMultipartTask\n");
	//initWebConfigMultipartTask(0);
	//WebcfgInfo("B4 WebConfigMultipartTask\n");
	WebConfigMultipartTask(0);

	curl_global_cleanup();
	WebcfgInfo("Exiting webconfig main thread!!\n");
	return 1;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
#ifndef INCLUDE_BREAKPAD
static void sig_handler(int sig)
{

	if ( sig == SIGINT ) 
	{
		signal(SIGINT, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGINT received!\n");
		exit(0);
	}
	else if ( sig == SIGUSR1 ) 
	{
		signal(SIGUSR1, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGUSR1 received!\n");
	}
	else if ( sig == SIGUSR2 ) 
	{
		WebcfgInfo("SIGUSR2 received!\n");
	}
	else if ( sig == SIGCHLD ) 
	{
		signal(SIGCHLD, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGHLD received!\n");
	}
	else if ( sig == SIGPIPE ) 
	{
		signal(SIGPIPE, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGPIPE received!\n");
	}
	else if ( sig == SIGALRM ) 
	{
		signal(SIGALRM, sig_handler); /* reset it to this function */
		WebcfgInfo("SIGALRM received!\n");
	}
	else 
	{
		WebcfgInfo("Signal %d received!\n", sig);
		exit(0);
	}
	
}
#endif
