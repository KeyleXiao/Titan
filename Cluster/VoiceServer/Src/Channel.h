/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ClientUser.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	谢晓鑫
** 日  期:	2012-12-10
** 版  本:	1.0
** 描  述:	频道
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "VoiceDef.h"
#include "Room.h"
#include "IDBEngine.h"

// 频道观察者
struct SChannelObserver
{
	DWORD dwClientID;		// 客户端ID
	DWORD dwTimeout;		// 超时时间

	SChannelObserver(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 观察者ID列表缓存信息
struct SObserverListInfo
{
	DWORD *pData;		// 数据
	DWORD dwCount;		// 实际个数
	DWORD dwSize;		// 数据空间个数
	DWORD dwClearTime;	// 清理时间

	DWORD *pRoomData[VoiceRoomType_Max];	// 房间观察者列表
	DWORD dwRoomCount[VoiceRoomType_Max];	// 房间观察者个数

	SObserverListInfo(void)
	{
		memset( this, 0, sizeof(SObserverListInfo) );
	}
};

// 频道观察者列表,以客户端ID为键值
typedef map<DWORD,SChannelObserver> TMAP_ChannelObserver;

// 马甲结构体
struct SVoiceVestInfo
{
	DWORD dwUDBID;
	char szName[VOICE_NAME_MAXSIZE];
	BYTE nSex;

	BYTE nNewUserType;		// 新用户类型
};

typedef map<DWORD, SVoiceVestInfo> TMAP_VestList;

// 当前频道在线管理员列表 ClientID为索引
typedef map<DWORD,DWORD> TMAP_CurAdminList;

enum EMVoiceAddVestError
{
	VoiceAddVestError_None = 0,		// 无错误
	VoiceAddVestError_Full,			// 管理员满
	VoiceAddVestError_Exist,		// 已有管理
	VoiceAddVestError_Normal,		// 一般错误

};


// 频道类
class CChannel : public TimerHandler, public IDBRetSink
{
	enum 
	{
		ETimerEventID_Destroy = 0,		// 无人时延时关闭定时器
		ETimerEventID_Close,			// 频道关闭定时器
		ETimerEventID_Active,			// 激活定时器
		ETimerEventID_DB,				// 保存数据库定时器
		ETimerEventID_Kill,				// 释放自己定时器，顺便检查是否保存数据库
	};
public:
	
	// 构造函数
	CChannel(LONGLONG nID);

	// 析构函数
	~CChannel(void);

	// 创建
	bool Create(void);

	// 释放
	void Release(void);

	///////////////////// TimerHandler /////////////////////////////////////////////////////
	// 定时器，计算麦序时间等
	virtual void OnTimer( unsigned long dwTimerID );

	//数据库请求返回回调方法
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	// 获取频道类型
	const SVoiceChannelID & GetIDInfo(void);

	// 获取频道ID
	LONGLONG GetID(void);

	// 获得频道内总人数
	DWORD Count(void);

	// 获得频道内总人数,可排除国家频道中挂机的人数
	DWORD CountEx(bool bNoCountryHang = false);

	// 获取房间, 如果找不到，返回默认房间
	CRoom & GetRoom( DWORD dwRoomID = 0 );

	// 获取子房间指针, 如果找不到房间，返回空
	CRoom * GetRoomPtr( DWORD dwRoomID );

	// 获取类型
	DWORD GetType(void);

	// 用户数变化
	void OnUsersChange( CRoom & room, CClientUser & user, bool bAdd = true );

	// 广播数据给所有成员 dwBanClientID 排除ID
	//void BroadcastData( const char *data, DWORD dwLen, DWORD dwBanClientID = 0 );

	// 广播数据给观察者 dwBanClientID 排除ID
	void BroadcastDataToObserver( const char *data, DWORD dwLen, DWORD dwBanClientID = 0 );

	// 添加一个观察者，新增的返回TRUE
	bool AddObserver( DWORD dwClientID, DWORD dwRoomID );

	// 删除一个观察者
	bool DelObserver( DWORD dwClientID, DWORD dwRoomID );

	// 移动一个观察者，dwSrcRoomID原房间ID，dwDesRoomID目的房间ID
	bool MoveObserver( DWORD dwClientID, DWORD dwSrcRoomID, DWORD dwDesRoomID );

	// 清空观察者
	void ClearObserver( DWORD dwRoomID = VoiceRoomType_Max );

	// 是否是观察者
	bool IsObserver( DWORD dwClientID, DWORD dwRoomID );

	// 获取观察者ID列表，dwCount得到的个数 暂时关闭此功能，好像有问题
	//const DWORD * GetObserverList( DWORD & dwCount, DWORD dwRoomID = VoiceRoomType_Max );

	// 获取频道里指定房间观察者ID列表 VoiceRoomType_Max 取所有房间
	DWORD GetObserverIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, DWORD dwRoomID = VoiceRoomType_Max );

	// 取得某个房间的观察者数量 VoiceRoomType_Max 取所有房间
	DWORD GetObserversCount( DWORD dwRoomID = VoiceRoomType_Max );

	// 获取频道里所有房间人ID列表 bNoMute为true时，取非静音用户
	DWORD GetClientIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, bool bNoMute = false );

	// 获取当前在频道里的所有管理员列表
	DWORD GetCurAdminIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, DWORD dwBanClientID = 0 );

	// 获取频道的所有在线管理员列表
	DWORD GetAdminIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts );

	// 更新频道信息到客户端
	void UpdateChannelInfoToClient( DWORD * pDesClientArray, DWORD wDesClientNum, BYTE nUpdateType, DWORD dwRoomID = VoiceRoomType_Max );

	// 获取观察者ID信息
	const SObserverListInfo & GetObserverListInfo(void);

	// 移动一个房间里的人到另外一个房间
	bool MoveUser( DWORD dwSrcRoomID, DWORD dwDesRoomID );

	// 踢出所有用户
	void KickUser(void);

	// 设置位状态标记
	void SetFlag( DWORD dwFlag, bool bSet, bool bSync = true );

	// 获取位状态标记
	bool GetFlag( DWORD dwFlag );

	// 设置关闭信息
	void SetCloseInfo( const char * szCloseInfo, DWORD dwTimeDelay );

	// 获得关闭文字信息
	const char* GetCloseMsg(void);

	// 发送聊天信息
	void SendChatMsg( const char * pMsg, DWORD dwRoomID = VoiceRoomType_Max, WORD nPos = VoiceInfoPos_ActorRight|VoiceInfoPos_ChatWindow, WORD nTipsType = VoiceTipType_Operate, DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0 );

	// 启动频道关闭定时器
	void StartCloseTimer(void);

	// 关闭频道关闭定时器
	void StopCloseTimer(void);

	// 设置频道拥有者名称
	void SetOwnerName( const char * szOwnerName, bool bSync = true, bool bSaveDB = true );

	// 获取频道拥有者名称
	const char * GetOwnerName(void);

	// 添加马甲（临时管理）
	bool AddVest( CClientUser & user, DWORD nNewUserType, DWORD & dwError );

	// 修改马甲
	bool EditVest( DWORD dwUDBID, DWORD nNewUserType, DWORD & dwError );

	// 删除马甲（临时管理）
	bool DelVest( DWORD dwUDBID, CClientUser * pUser = NULL );

	// 获取马甲（临时管理）
	SVoiceVestInfo * GetVest( DWORD dwUDBID );

	// 获取马甲（临时管理）列表
	void UpdateChannelVestListToClient( DWORD dwClientID, BYTE nUpdateType );

	// 是否可以参加个人排行榜
	bool CanInsertPersonalTopList(void);

	// 获取历史最高在线人数
	DWORD GetMaxMemberCount(void);

	// 设置频道拥有者信息
	void SetOwnerInfo( DWORD dwUDBID, DWORD dwWorldID, DWORD dwCountryID, bool bSaveDB );

	// 获取频道拥有者世界信息
	void GetOwnerWorldInfo( DWORD & dwWorldID, DWORD & dwCountryID );

	// 语音检测，用于推送功能
	void OnAudioCheck( DWORD dwNow, DWORD dwAudioRoomID );

	void OnActiveChange(void);

	// 频道是否推送
	bool IsPushed(void);

	// 出发推送
	void OnPush( bool bActive );

	// 发送语音提醒到客户端
	void SendVoiceTipsToClient( bool bShow );
	
	// 是否支持数据库存储,团队不需要存储数据
	bool IsSupportDB();

	// 数据是否加载完成
	bool IsLoaded(void);

	// 请求保存数据,有缓冲机制,数据变化都调用 OnDataChanged()
	bool SaveData(void);

	// 定时检查是否要保存数据
	void OnTimerCheckSaveDB(void);

	// 数据是否已经保存
	bool IsDBSaved(void);

	// 设置运行位状态标记
	void SetRunFlag( DWORD dwFlag, bool bSet );

	// 获取运行位状态标记
	bool GetRunFlag( DWORD dwFlag );

	// 获取频道人数上限
	DWORD GetMaxCount(void);

	// 设置频道人数上限
	void SetMaxCount( DWORD dwCount );

	// 设置频道语音品质类型  EMVoiceQualityType
	bool SetQuality( DWORD dwQuality );

	// 频道是否可进
	bool CanChannelEnter( DWORD & dwResult );

	// 获取频道基本信息
	SVoiceChannelBaseInfo & GetBaseInfo(void);

	// 更新管理员进入房间信息给其他管理员
	void UpdateAdminEnterInfoToOther( CClientUser & admin, DWORD dwRoomID, bool bEnter = true );

private:

	// 数据变更了,定时或调用SaveData() 时会触发保存
	void OnDataChanged();

	// 冷却请求保存数据
	bool CanSaveData();

	// 保存数据到数据库,只能SaveData()内部调用,其他地方不要调用
	void SaveDataToDB(void);

	// 分配ObserverList内存，返回true表示重新分配，
	bool GetObserverListBuff(void);

	// 是否能销毁 dwDestroyTime多少分钟后销毁
	bool CanDestroy( DWORD & dwDestroyTime );

	// 添加管理员到当前频道在线管理员列表，方便查询使用
	bool AddCurAdmin( DWORD dwClientID, bool bAdd );

	// 获取当前频道所有在线管理列表个数
	DWORD GetCurAdminCount(void);

private:
	// 频道ID
	LONGLONG m_nID;

	// 频道基本信息
	SVoiceChannelBaseInfo m_baseInfo;

	// 请求保存数据冷却信息数据
	SVoiceSaveCoolInfo m_saveCoolInfo;

	// 是否已加载数据
	bool	m_bLoaded;

	// 是否已保存数据
	bool	m_bSaved;

	// 频道ID结构信息
	SVoiceChannelID m_IDInfo;
	
	// 频道名称，默认情况下是拥有者名称
	char m_szOwnerName[VOICE_NAME_MAXSIZE];	

	// 拥有者的世界ID
	DWORD m_dwWorldID;

	// 拥有者国家ID
	DWORD m_dwCountryID;

	// 房间列表
	CRoom	m_roomList[VoiceRoomType_Max];

	// 频道类型 EMVoiceChannelType
	DWORD	m_dwType;

	// 频道观察者
	TMAP_ChannelObserver m_ChannelObserver[VoiceRoomType_Max];

	// 观察者ID列表
	SObserverListInfo m_ObserverListInfo;

	// 观察者列表是否变化
	bool m_bObserverChange;

	// 销毁定时器是否已经起来
	bool m_bDestroyTimerStart;

	// 频道关闭信息
	string m_strCloseInfo;

	// 关闭延时时间 秒
	DWORD m_dwCloseTimeDelay;
	DWORD m_dwCloseTimeStart;

	// 临时管理列表
	TMAP_VestList m_vestList;

	// 历史最高在线人数
	DWORD m_dwMaxMemberCount;
	// 上榜时间
	DWORD m_dwTopListTime;

	// 激活功能
	bool m_bActive;

	// 已经推送
	bool m_bPushed;

	// 上次激活的时间
	DWORD m_dwLastActiveTime;

	// 激活开始时间
	bool m_bActiveTimerStart;

	// 激活定时器开始时间
	DWORD m_dwActiveTimerStartTime;

	// 激活定时器结束时间
	DWORD m_dwActiveTimerEndTime;

	// 触发激活的语音次数
	DWORD m_dwAudioCount;
	
	// 触发检测时间
	DWORD m_dwActiveTimerCheckTime;

	// 触发激活的房间ID
	DWORD m_dwActiveRoomID;

	// 运行状态标识
	DWORD m_dwRunFlag;

	// 频道最大人数上限
	DWORD m_dwMaxCount;

	// 当前频道在线管理员列表
	TMAP_CurAdminList m_CurAdminList;

public:
	// 最后语音tick数
	DWORD m_dwLastAudioTime;
	DWORD m_dwAudioCheckTime;
};



#endif