/*******************************************************************
** 文件名:	ViewRedisData.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/26
** 版  本:	1.0
** 描  述:	观战Redis中的数据结构定义
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "ISerializableData.h"


#pragma pack(1)
namespace ViewRedisData
{
	/*******************************************************************/
	// 预备消息定义 <[战报ID]:Prep, List>
	// List中的具体元素如下:
	static const long indexBattlePrepInfo = 0;
	static const long indexBeginTime = 1;
	static const long indexEndTime = 2;
	/*******************************************************************/
	// 0.进战场之前的预备信息（客户端需要根据这个信息来加载场景和英雄等）
	struct BattlePrepInfo
	{

	};
	// 1.STime 开始时间 标识时间的结构体（精确到毫秒）
	// 2.STime 结束时间 标识时间的结构体（精确到毫秒）
	/*******************************************************************/



	/*******************************************************************/
	// 具体战报消息定义
	/*******************************************************************/
	// 战报
	struct BattleInfo : ISerializableData
	{
		BYTE	byType;		// Battle消息号(ViewServer不关心这个字段，场景服和客户端关心)
		DWORD	tTime;		// 时间戳(从本战场第一个打点消息开始，经过的毫秒数)
		DWORD	dwLength;	// 消息体长度
		char*	psz;		// 消息体

		BattleInfo()
		{
			memset(this, 0, sizeof(BattleInfo));
		}

		// 析构函数
		~BattleInfo()
		{
			safeDelete(psz);
			dwLength = 0;
		}

		// 拷贝构造
		BattleInfo(const BattleInfo& rhs)
			: byType(rhs.byType)
			, tTime(rhs.tTime)
			, dwLength(rhs.dwLength)
		{
			psz = new char[rhs.dwLength];
			memcpy(psz, rhs.psz, dwLength);
		}

		//移动构造函数
		BattleInfo(BattleInfo&& rhs) : psz(rhs.psz)
			, byType(rhs.byType)
			, tTime(rhs.tTime)
			, dwLength(rhs.dwLength)
		{
			rhs.psz = nullptr;//将临时值的指针成员置空
			rhs.dwLength = 0;
		}

		virtual void fromBytes(const char* p, size_t len) override
		{
			ibuffer ibufData(p, len);
			ibufData >> byType >> tTime >> dwLength;
			if (dwLength > 0)
				ibufData.pop(psz, dwLength);
		}


		virtual void toBytes(obuf& obufData) const override
		{
			obufData << byType << tTime << dwLength;
			if (dwLength > 0)
				obufData.push_back(psz, dwLength);
		}
	};
	/*******************************************************************/
};
#pragma pack()