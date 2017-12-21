/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\VoiceCallList.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2013-07-29
** 版  本:	1.0
** 描  述:	语音召集缓存列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifndef __VoiceCallList_H__
#define __VoiceCallList_H__

#include "VoiceDef.h"
#include "Singleton.h"

class CClientUser;


// 语音召集令缓存数据
struct SVoiceADCacheData
{
	DWORD dwTime;			// 发送邀请的时间
	DWORD dwPDBID;			// PDBID
	DWORD dwUDBID;			// 发送邀请的人
	BYTE nSex;				// 性别
	DWORD dwADType;			// 邀请类型
	DWORD dwChannelType;	// 频道类型
	DWORD dwRoomID;			// 房间ID
	DWORD dwBroadcastType;	// 广播类型
	DWORD dwValueID;		// 值ID
	LONGLONG nID;
	char szText[VOICE_TEXTSTR_MAXSIZE];	// 邀请信息

	SVoiceADCacheData(void)
	{
		memset( this, 0, sizeof(SVoiceADCacheData) );
	}
};

// 语音邀请粉丝缓存数据
struct SVoiceCallFansCacheData
{
	DWORD dwTime;			// 发送邀请的时间
	DWORD dwClientID;		// 客户端IDs
	LONGLONG nID;			// 召集房间ID
	BYTE nSex;				// 性别
	BYTE nCountryID;		// 国家
	DWORD dwFromWorldID;	// 来源世界ID
	DWORD dwUDBID;			// 发送邀请的人
	char szNickname[VOICE_NAME_MAXSIZE];
	DWORD dwData1;
	DWORD dwData2;
	char szText[VOICE_TEXTSTR_MAXSIZE];	// 邀请信息

	SVoiceCallFansCacheData(void)
	{
		memset( this, 0, sizeof(SVoiceCallFansCacheData) );
	}
};

// 粉丝邀请,召集令符合条件结果,内部用
struct SVoiceCheckCacheResultInfo
{
	bool bIsCallFans;	// 是粉丝邀请,否则是召集令
	DWORD dwScore;		// 得分数,用来排序取优先级
	void * pCallData;	// 指向缓存数据的指针,内部用,注意中间不能破坏数据

	SVoiceCheckCacheResultInfo(void)
	{
		memset( this, 0, sizeof(SVoiceCheckCacheResultInfo) );
	}
};


// 语音房间类
class CVoiceCallList : public Singleton<CVoiceCallList>, public TimerHandler
{
	enum 
	{
			ETimerEventID_TimeoutCheck = 0,		// 检测定时器
	};

public:
	// 粉丝邀请缓存列表 UDBID为索引
	typedef map<DWORD, SVoiceCallFansCacheData> TMAP_CallFansCacheList;

	// 语音召集令缓存列表 房间ID为索引
	typedef map<LONGLONG, SVoiceADCacheData> TMAP_ADCacheList;

	// 粉丝邀请,召集令符合条件结果排序列表
	typedef std::vector<SVoiceCheckCacheResultInfo> TLIST_SORTCheckCacheResult;

	// 构造函数
	CVoiceCallList();

	// 析构函数
	virtual ~CVoiceCallList(void);

	///////////////////// TimerHandler /////////////////////////////////////////////////////
	// 定时器，计算麦序时间等
	virtual void OnTimer( unsigned long dwTimerID );

	// 创建
	bool Create(void);

	// 释放
	void Release(void);

	// 添加一个语音召集令缓存
	bool AddVoiceADCacheData( const SVoiceADCacheData & cacheData );

	// 添加一个邀请粉丝缓存
	bool AddCallFansCacheData( const SVoiceCallFansCacheData & cacheData );

	// 检测召集缓存
	void CheckCache( CClientUser & user );

private:

	// 检测用户的语音召集令缓存
	void CheckUserVoiceADCache( CClientUser & user,TLIST_SORTCheckCacheResult &tmpList );

	// 检测用户的粉丝邀请缓存，必须要在用户数据读取完成之后调用
	void CheckUserCallFansCache( CClientUser & user,TLIST_SORTCheckCacheResult &tmpList );

	// 发送AD缓存数据到客户端
	void SendADCacheDataToClient( DWORD dwClientID, const SVoiceADCacheData & cacheData );

	// 发送粉丝邀请缓存数据到客户端
	void SendCallFansCacheDataToClient( DWORD dwClientID, const SVoiceCallFansCacheData & cacheData );

private:
	// 粉丝邀请缓存列表
	TMAP_CallFansCacheList m_CallFansCacheList;

	// 语音召集令缓存列表
	TMAP_ADCacheList m_ADCacheList;
};



#endif