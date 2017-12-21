/*******************************************************************
** �ļ���: Common\GUID.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾(www.weimingtech.com)
** ������: ���к�
** ��  ��: 2017/10/12
** ��  ��: 1.0
** ��  ��: ����ΨһGUID
** Ӧ  ��:
**************************** �޸ļ�¼ ******************************
** �޸���:
** ��  ��:
** ��  ��:
********************************************************************/

#include "Api.h"
#include <WinBase.h>


/**
@param nServerID:������ID����������ʹ�øú���ʱ�򣬸�ֵΪ255
*/
namespace rkt
{

#define SCENE_GUID(nServerID) GenGUID(nServerID)
#define SOCIAL_GUID() GenGUID(255)

	long long static GenGUID(int nServerID)
	{
		struct GUID_DATA
		{
			unsigned long long Time : 27;			// ��0��0�뿪ʼ�������ĺ�����
			unsigned long long Day : 15;			// ��2010��1��1�տ�ʼ������������
			unsigned long long ServerID : 8;		// ������ID
			unsigned long long WorldID : 14;		// ����ID

			GUID_DATA()
			{
				WorldID = 0;
				ServerID = 0;
				Day = 0;
				Time = 0;
			}
		};

		// 2010��1��1��0Сʱ0��0�뵽1970�������
		const int nBaseDays = 14609;

		// ��ǰUnixʱ���
		DWORD now = time(NULL);

		// ��ǰ��1970�������
		int nCurDays = now / (3600 * 24);

		// ����0��0��0�뵽1970�������
		int DayTime = nCurDays * (3600 * 24);

		//��ȡϵͳ��UTCʱ�䡣  
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