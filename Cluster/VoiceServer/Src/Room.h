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

#ifndef __ROOM_H__
#define __ROOM_H__

#include "VoiceDef.h"
#include "IDBEngine.h"


class CChannel;
class CClientUser;
struct DB_Table_VoiceRoom;

// 房间用户信息
struct SRoomUserData
{
	bool bMute;		// 是否静音

	SRoomUserData(void)
	{
		memset( this, 0, sizeof(SRoomUserData) );
	}
};

// 麦序信息
struct SMicOrderData
{
	DWORD dwClientID;				// 客户端ID
	DWORD dwIndex;					// 序号

	SMicOrderData(void)
	{
		memset( this, 0, sizeof(SMicOrderData) );
	}
};


// 已开麦说话的用户信息
struct SVoiceSpeakInfo
{
	DWORD dwClientID;				// 客户端ID
	DWORD dwTickTime;				// 麦说话Tick时间

	SVoiceSpeakInfo(void)
	{
		memset( this, 0, sizeof(SVoiceSpeakInfo) );
	}
};



// 广播用户ID列表缓存信息
struct SAudioListInfo
{
	DWORD *pData;		// 数据
	DWORD dwCount;		// 实际个数
	DWORD dwSize;		// 数据空间个数

	SAudioListInfo(void)
	{
		memset( this, 0, sizeof(SAudioListInfo) );
	}
};

// 麦信息
struct SVoiceMicInfo
{
	DWORD	dwMaxMicIndex;			// 当前最大麦序,当清空时可设为0
	DWORD	dwMicOrderTime;			// 默认麦序时长
	DWORD	dwCurMicUser;			// 当前在麦上的人，ClientID
	DWORD	dwMicInvalidTime;		// 如果多久不说话，麦序就失效

	DWORD	dwCurMicOrderTime;		// 当前麦序时长 
	DWORD	dwMicStartTime;			// 麦序开始说话时间
	DWORD	dwMicOrderCount;		// 有几次麦序，麦序总时间为dwMicOrderCount*dwDefaultMicOrderTime
	DWORD	dwMicTimeLeft;			// 控麦下，当前麦序上当前计时周期里还剩多少秒
	bool	bRestartTimer;			// 要重新启动计时器

	SVoiceMicInfo(void)
	{
		memset( this, 0, sizeof(SVoiceMicInfo) );
	}
};


// 语音房间类
class CRoom : public TimerHandler, public IDBRetSink
{
	enum 
	{
		ETimerEventID_MicOrder = 0,		// 麦序定时器
		ETimerEventID_MicCheck,			// 麦时间检测，如果一定时间没有说话，直接下麦
	};

public:
	// 用户MAP，以用户ClientID为键值
	typedef std::map<DWORD, SRoomUserData> TMAP_UserList;

	// 麦序MAP，以用户麦序为键值
	typedef std::map<DWORD, SMicOrderData> TMAP_MicList;

	// 已开麦说话的用户MAP，以用户ClientID为键值,用来处理自由抢麦处理,人多不让说
	typedef std::map<DWORD, SVoiceSpeakInfo> TMAP_SpeakList;

	// 构造函数
	CRoom(void);

	// 析构函数
	virtual ~CRoom(void);


	///////////////////// TimerHandler /////////////////////////////////////////////////////
	// 定时器，计算麦序时间等
	virtual void OnTimer( unsigned long dwTimerID );

	//数据库请求返回回调方法
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);


	// 创建
	bool Create( CChannel* pChannel, DWORD dwID );

	// 释放
	void Release(void);

	// 获得频道
	CChannel & GetChannel(void);

	// 获取房间子ID
	DWORD GetID(void);

	// 获取房间ID
	LONGLONG GetIDEx(void);

	// 获取房间类型
	DWORD GetType(void);

	// 设置位状态标记, bSync为false的话，不需要同步到客户端
	void SetFlag( DWORD dwFlag, bool bSet, bool bSync = true, bool bSaveDB = true );

	// 获取位状态标记
	bool GetFlag( DWORD dwFlag );

	// 获取状态标记值
	DWORD GetFlagValue(void);

	// 设置密码
	bool SetPassword( const char * szPassword );

	// 校验密码
	bool CheckPassword( const char * szPassword );

	// 获取总人数
	DWORD Count(void);

	// 添加一个用户
	bool Add( CClientUser & user );

	// 删除一个用户
	bool Del( CClientUser & user );

	// 设置用户ID列表需重新计算
	void OnChanged(void);

	// 获取广播语音用户ID列表，内部有缓存机制，有变化才重新计算列表，提高效率
	const DWORD * GetAudioList( DWORD & dwCount );

	// 获取房间里所有用户ID列表 bNoMute为true时，取非静音用户
	DWORD GetUserIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, bool bNoMute = false );

	// 获取房间里麦上人的粉丝ID列表
	DWORD GetFansIDList( DWORD dwUDBID, DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts );

	// 设置麦控类型
	void SetMicState( DWORD dwState );

	// 获取麦控类型
	DWORD GetMicState(void);

	// 加入开麦说话用户到列表,如果超了最大数就失败
	bool AddSpeak(CClientUser & user);

	// 删除开麦说话用户到列表
	bool DelSpeak(CClientUser & user);

	// 清除所有开麦说话用户列表
	bool ClearSpeak();

	////////////////////////// 麦序操作 ////////////////////////////////////////////////
	// 上麦序
	bool AddMicOrder( CClientUser & user, DWORD dwReason = 0 );

	// 下麦序
	bool DelMicOrder( CClientUser & user, DWORD & dwError, DWORD dwReason = 0 );

	// 当前麦序个数
	DWORD GetMicOrderCount(void);

	// 把一个用户添加到麦序，bAdd为true表示添加
	bool AddClientToMicOrder( DWORD dwPDBID, bool bAdd = true );

	// 麦序变化
	void OnMicOrderChange( CClientUser & user, bool bAdd = true );

	// 把当前麦上的人下掉，返回true，表示成功移除麦上的人
	bool RemoveMicUser( bool bSyncMicUser = false );

	// 把麦给麦序上第一个人
	void SetMicUser(void);

	// 是否是麦上的人
	bool IsMicUser( DWORD dwClientID );

	// 清空麦序
	bool ClearAllMicOrder();

	// 增加麦序时间
	bool AddMicOrderTime( bool bAdd, DWORD dwTimeCount );

	// 设置默认麦序时间，返回修改后的时间
	DWORD SetDefaultMicOrderTime( DWORD dwTime );

	// 移动麦序，dwMoveType为EMVoiceMoveMicOrderType
	bool MoveMicOrder( DWORD dwClientID, DWORD dwMoveType );

	// 控麦
	void HoldMic( bool bHold );

	// 是否在麦序上
	bool IsOnMicOrder( DWORD dwMicIndex, DWORD dwClientID );

	// 从麦序列表中删除一个麦序
	bool DelFromMicList( DWORD dwMicIndex );

	// 得到用户列表
	TMAP_UserList & GetUserList(void);

	// 得到麦序列表
	TMAP_MicList & GetMicList(void);

	//////////////////////////////////////////////////////////////////////////

    // 房间广播
    void broadCast(void* pData, int len);

	// 更新房间信息到客户端
	void UpdateRoomInfoToClient( DWORD * pDesClientArray, DWORD wDesClientNum, BYTE nUpdateType );

	// 更新粉丝信息到客户端
	void UpdateFansInfoToClient( CClientUser & user );

	// 设置备注信息
	void SetRemark( const char * szRemark );

	// 获取备注信息
	const char * GetRemark(void);

	// 广播文字聊天
	void sendChatMessage( CClientUser & sender, ulong channel, const char* text, DWORD target = 0, bool bSelf = false );

	// 更新用户静音状态
	void UpdateUserMuteState( DWORD dwClientID, bool bMute );

	// 连麦
	bool ShareMic( DWORD dwClientID, bool bShare );

	// 更新一下当前所有用户房间信息,主要是用来数据库加载数据后,需刷新一下相关状态和权限
	bool UpdateAllUserCurRoomInf(void);

	// 收到数据库数据
	void ImportDBInfo(DB_Table_VoiceRoom &dbInfo);

	// 是否支持数据库存储,团队不需要存储数据
	bool IsSupportDB();

	// 数据是否加载完成
	bool IsLoaded(void);

	// 设置是否加载完成
	void SetLoaded( bool bLoaded );

	// 请求保存数据,有缓冲机制,数据变化都调用 OnDataChanged()
	bool SaveData(void);

	// 定时检查是否要保存数据
	void OnTimerCheckSaveDB(void);

	// 数据是否已经保存
	bool IsDBSaved(void);

	// 设置房间名称
	void SetName( const char* szRoomName );

	// 获取房间名称
	const char * GetName(void);

	// 设置文字聊天冷却时间 秒，如果输入值小于标准值，则表示关闭文字聊天 返回设置的秒数
	DWORD SetChatCoolDown( DWORD dwTime );

	// 获取文字聊天冷却时间
	DWORD GetChatCoolDown(void);

	// 是否正在视频
	bool IsVideoShow(void);

	// 房间峰值统计处理
	void StatsMaxMemberNum(bool bOn);
	
	// 获取统计期间的房间峰值
	DWORD GetStatsMaxMemberNum(void);

private:

	// 数据变更了,定时或调用SaveData() 时会触发保存
	void OnDataChanged();

	// 冷却请求保存数据
	bool CanSaveData();

	// 保存数据到数据库,只能SaveData()内部调用,其他地方不要调用
	void SaveDataToDB(void);

	// 分配AudioList内存，返回true表示重新分配，
	bool GetAudioListBuff(void);

    // 更新房间内用户信息列表到客户端
    template<typename T>
    void updateUserList2Client(DWORD dwClientID, void* p);
private:
	// 房间ID
	LONGLONG m_nID;

	// 房间子ID  EMVoiceRoomType
	DWORD	m_dwID;

	// 房间所属频道类型 EMVoiceChannelType
	DWORD	m_dwType;

	// 房间状态标记 EMVoiceRoomStateFlag
	DWORD	m_dwStateFlag;

	// 麦类型 EMVoiceMicState
	DWORD	m_dwMicState;

	// 所属频道指针
	CChannel* m_pChannel;

	// 密码
	char	m_szPassword[VOICE_NAME_MAXSIZE];

	// 用户列表
	TMAP_UserList m_mapList;

	// 麦序
	TMAP_MicList m_mapMic;

	// 已开麦说话的用户MAP，以用户ClientID为键值,用来处理自由抢麦处理,人多不让说
	TMAP_SpeakList m_mapSpeak;

	// 麦信息
	SVoiceMicInfo m_MicInfo;
	
	// 广播用户ID列表是否需要重新计算
	bool m_bChanged;

	// 广播语音用户ID列表，内部缓存
	SAudioListInfo	m_audioList;

	// 最后清理过时已开麦说话的用户列表时间
	DWORD m_dwLastClearSpeakListTick;

	// 频道备注信息
	string	m_strRemark;
	
	// 房间名称
	char m_szName[VOICE_NAME_MAXSIZE];

	DWORD m_dwSpare1;			// 备用1
	DWORD m_dwSpare2;			// 备用2
	DWORD m_dwSpare3;			// 备用3

	// 请求保存数据冷却信息数据
	SVoiceSaveCoolInfo m_saveCoolInfo;

	// 是否已加载数据
	bool	m_bLoaded;

	// 是否已保存数据
	bool	m_bSaved;

	// 文字聊天冷却时间
	DWORD m_dwChatCoolDown;

	// 统计峰值标志
	bool m_bStatsMaxMember;
	DWORD m_dwMaxMemberNum;

public:
	// 用户数更新时间 非同步列表频道使用
	DWORD m_dwCountUpdateTime;

	// 最后语音tick数
	DWORD m_dwLastAudioTime;
};



#endif