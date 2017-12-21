#pragma once;

//#define Times 1000000  
#define Times 100000
#define Test_String "csdskajdkasdjqkwdnwqjndewjqlelqel,ooasdadas"


#define THREAD_COUNT			20
#define THREAD_RUN_MIN_COUNT	10000
#define THREAD_RUN_MAX_COUNT	10000000

#define MIN_THREAD
#ifdef MIN_THREAD
#define THREAD_RUN_COUNT	THREAD_RUN_MIN_COUNT
#define THREAD_TIMEROUT		60*1000
#else
#define THREAD_RUN_COUNT	THREAD_RUN_MAX_COUNT
#define THREAD_TIMEROUT		10*60*1000
#endif