/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ClientUser.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2012-12-10
** 版  本:	1.0
** 描  述:	频道列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifndef __CHANNEL_LIST_H__
#define __CHANNEL_LIST_H__

#include "VoiceDef.h"
#include "Channel.h"
#include "Singleton.h"
#include <list>

// 频道类
class CChannelList : public Singleton<CChannelList>
				   , public TimerHandler
{
	enum 
	{
		ETimerEventID_PersonalTopList = 0,		// 个人频道热度排行榜定时器，1分钟排行一次
	};

public:
	typedef std::map<LONGLONG, CChannel*> CHANNEL_LIST;
	typedef std::vector<SVoicePersonalTopData> TOP_LIST;	// 个人频道排行榜
	typedef std::vector<SVoiceChannelSortInfo> CHANNELSORT_LIST;	// 频道排序,自动流量控制中的频道列表

	///////////////////// TimerHandler /////////////////////////////////////////////////////
	// 定时器
	virtual void OnTimer( unsigned long dwTimerID );

	// 定时器计算个人频道排行
	void OnTimerUpdatePersonalTopList(void);


	bool Create(void);

	void Release(void);


	// 增加频道，bNew为ture表示新加频道
	CChannel * Add( DWORD dwType, DWORD dwWorldID, DWORD dwID, bool & bNew );

	// 删除频道
	bool Del( LONGLONG nChannelID );

	// 查找频道
	CChannel * Get( LONGLONG nChannelID );

	// 查找频道
	CChannel * Get( DWORD dwType, DWORD dwWorldID, DWORD dwID );

	// 查找房间
	CRoom * GetRoom( LONGLONG nRoomID );

	// 获取频道数量
	DWORD Count(void);

	// 清除所有频道
	void Clear(void);

	// 同步个人频道排行榜到客户端
	void UpdatePersonalTopListToClient( DWORD dwClientID );

	// 获取个人频道排行榜更新时间
	DWORD GetPersonalTopListUpdateTime(void);

	// 计算得到自动流量控制中的频道列表 dwMinOnline:最小人数要求,dwNeedMaxCount:最多取多少个频道
	void GetAutoNetSpeedList(DWORD dwMinOnline,DWORD dwNeedMaxCount);

	// 保存用户信息列表到csv文件
	void SaveUserListToCSV();

	void WriteData2File(BYTE* pData,int nLens,const _TCHAR * szFileName);

	// 取得频道列表引用，特殊情况下只读遍历使用
	CHANNEL_LIST & GetList(void);

	// 构造函数
	CChannelList(void);

	// 析构函数
	virtual ~CChannelList(void);

private:
	// 频道列表
	CHANNEL_LIST m_list;

	// 个人频道排行榜
	TOP_LIST m_topList;
	DWORD m_dwTopListUpdateTime;	// 个人频道排行榜生成时间

public:

	CHANNELSORT_LIST m_autoNetSpeed; // 自动流量控制中的频道列表
};



#endif