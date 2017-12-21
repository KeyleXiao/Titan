//==========================================================================
/**
* @file	  : MyTime.h
* @author : 刘晓锋
* created : 2010-10-29
* purpose : 系统时间接管
*/
//==========================================================================

#ifndef _MYTIME_H_
#define _MYTIME_H_

#include "Common.h"
#include <time.h>


namespace rkt {

	RKT_API __time64_t getTime(__time64_t* t);
	RKT_API bool setTime(__time64_t t);
	RKT_API void clearTime();
	RKT_API bool setDate(int year, int mon, int day,int hour, int min, int sec);	

};

#endif //_MYTIME_H_