/*******************************************************************
** 文件名: Common\GUID.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/10/12
** 版  本: 1.0
** 描  述: 生成唯一GUID
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "Api.h"
#include <WinBase.h>


/**
@param nServerID:场景服ID，当在社会服使用该函数时候，该值为255
*/
namespace rkt
{

#define SCENE_GUID(nServerID) GenGUID(nServerID)
#define SOCIAL_GUID() GenGUID(255)

	long long static GenGUID(int nServerID)
	{
		struct GUID_DATA
		{
			unsigned long long Time : 27;			// 从0点0秒开始经历过的毫秒数
			unsigned long long Day : 15;			// 从2010年1月1日开始经历过的天数
			unsigned long long ServerID : 8;		// 服务器ID
			unsigned long long WorldID : 14;		// 世界ID

			GUID_DATA()
			{
				WorldID = 0;
				ServerID = 0;
				Day = 0;
				Time = 0;
			}
		};

		// 2010年1月1日0小时0分0秒到1970年的天数
		const int nBaseDays = 14609;

		// 当前Unix时间戳
		DWORD now = time(NULL);

		// 当前到1970年的天数
		int nCurDays = now / (3600 * 24);

		// 本日0点0分0秒到1970年的秒数
		int DayTime = nCurDays * (3600 * 24);

		//获取系统的UTC时间。  
		SYSTEMTIME stUTC;
		::GetSystemTime(&stUTC);


		GUID_DATA guidData;
		guidData.WorldID = getThisGameWorldID();
		guidData.ServerID = nServerID;
		guidData.Day = nCurDays - nBaseDays;
		guidData.Time = (now - DayTime) * 1000 + stUTC.wMilliseconds;//(stUTC.wHour * 3600 + stUTC.wMinute * 60 + stUTC.wSecond) * 1000 + stUTC.wMilliseconds;

		unsigned long long guid = *(unsigned long long*)((GUID_DATA*)&guidData);

		//GUID_DATA* pData = (GUID_DATA*)&guid;
		//WarningLn("guid,WorldID=" << pData->WorldID <<",ServerID=" << pData->ServerID << ",Day=" << pData->Day << ",Time=" << pData->Time);

		return guid;
	}
}