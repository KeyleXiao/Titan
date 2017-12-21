/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ClientUser.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2012-11-21
** 版  本:	1.0
** 描  述:	语音客户端用户
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#ifndef __CLIENT_USER_H__
#define __CLIENT_USER_H__

#include "VoiceDef.h"
#include "ChannelList.h"
#include "ChatList.h"
#include "IDBEngine.h"

// 频道成员列表更新信息
struct SUserListSyncInfo
{
	bool	m_bSync;			// 是否已经同步过
	DWORD	m_dwSyncTime;	// 请求时间

	SUserListSyncInfo(void)
	{
		memset( this, 0, sizeof(SUserListSyncInfo) );	
	}
};

// 玩家的关注列表,以通行证ID为键值
typedef map<DWORD,DWORD> TMAP_VFansUser;

// 客户端类
class CClientUser : public TimerHandler, public IDBRetSink
{
    friend class CChannel;
    friend class CRoom;
public:
	/**		定时器ID		**/
	enum
	{
		ETimerEventID_LoginTimeout = 0,		// 登录超时定时器
		ETimerEventID_Kick,					// 踢出定时器
	};

public:
	// 构造函数
	CClientUser();

	// 析构函数
	virtual ~CClientUser(void);

	// 定时器
	virtual void OnTimer( unsigned long dwTimerID );

	//数据库请求返回回调方法
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	// 客户端信息
	std::string ToString();

	// 设置IP
	void SetClientIP( DWORD dwIP );

	// 客户端IP
	const char *GetClientIP();

	// 获得连入时间
	DWORD GetConnectTime(void);

	// 获得网关ID
	DWORD GetGatewayID();

	// 释放
	void Release();

	// 获得客户端ID
	DWORD GetClientID(void);

	// 设置客户端ID
	void SetClientID( DWORD dwClientID );

	// 生成密钥
	void CreateKeyData(void);

	// 校验数据
	bool CheckKeyData( char* szKeyData, DWORD dwKeyLen );

	// 设置登录
	void SetLogin( bool bLogin );

	// 获取是否登录
	bool GetLogin(void);

	// 获取用户详细信息
	SVoiceUserInfo & GetUserInfo(void);

	// 设置服务器信息，游戏世界ID，服务器ID
	void SetServerInfo( DWORD dwServerID, DWORD dwWorldID );

	// 获得所属游戏服务器的ID
	DWORD GetServerID(void);

	// 获得所属游戏世界ID
	DWORD GetWorldID(void);

	// 踢出操作
	void Kick( DWORD dwReason );

	// 设置在线
	void SetOnline( bool bOnline );

	// 是否在线
	bool IsOnline(void);

	// 获取当前所在频道权限 EMVoiceRightFlag
	bool GetRight( DWORD dwRightFlag );
	
	// 设置位状态标记
	void SetFlag( DWORD dwStateFlag, bool bSet, bool bSyncOthers = false );

	// 获取位状态标记
	bool GetFlag( DWORD dwStateFlag );

	// 获得状态标记值
	DWORD GetFlagValue(void);

	// 设置网络状态
	void SetState( DWORD dwState );

	// 获取网络状态
	DWORD GetState(void);

	// 设置静音，静音时不发送语音数据给用户，提高效率
	void SetMute( bool bMute = true );

	// 是否静音
	bool IsMute(void);

	// 获取用户类型VoiceChannel_Max为获取当前频道
	BYTE GetType( DWORD dwChannelType = VoiceChannel_Max );

	// 获取用户频道权限VoiceChannel_Max为获取当前频道
	DWORD GetChannelRight( DWORD dwChannelType = VoiceChannel_Max );

	// 更新用户信息
	bool OnUpdateUserInfo(void* data, int len);

    // 更新用户信息给客户端
    bool UpdateUserInfoToClient(bool bBroadcast, void* data, int len);

    // 更新哪些字段给客户端
	bool UpdateUserInfoToClient(bool bBroadcast, int nFieldNum, ...);

	// 根据频道类型获取ID dwType为EMVoiceChannelType
	LONGLONG GetChannelID( DWORD dwType );

	// 切换频道
	bool EnterRoom( LONGLONG nRoomID, const char * szPassword, bool bForce = false);

	// 清空当前房间对象
	void ClearCurRoom(void);

	// 设置当前房间对象
	void SetCurRoom( CRoom & room );

	// 获得当前房间对象
	CRoom * GetCurRoom(void);

	// 获取当前房间ID
	LONGLONG GetCurRoomID(void);

	// 获取当前频道类型
	DWORD GetCurChannelType(void);

	// 获取频道值ID
	DWORD GetChannelTypeID( DWORD dwChannelType );

	// 是否有权限说话，只针对在频道中
	bool CanSpeak(void);

	// 是否可以私聊
	bool CanPrivateChat(void);

	// 语音数据是否要广播
	bool NeedBroadcast(void);

	// 设置麦序
	void SetMicIndex( DWORD dwIndex );

	// 获得麦序
	DWORD GetMicIndex(void);

	// 发送聊天信息
	void SendChatMsg( const char * pMsg, WORD nPos = VoiceInfoPos_ActorRight|VoiceInfoPos_ChatWindow, WORD nTipsType = VoiceTipType_Operate, DWORD dwUserData1=0,DWORD dwUserData2=0,DWORD dwUserData3=0 );

	// 操作话筒
	bool OpenMic( bool bOpen );

	// 操作私聊话筒
	bool OpenChatMic( bool bOpen );

	// 操作开启视频
	bool OpenVideo( bool bOpen );

	// 操作听筒
	bool OpenHeadset( bool bOpen );

	// 请求观察
	bool RequestVisitor( bool bForce );

	// 抢/下麦
	bool AddMicOrder( bool bAdd );

	// 成员调度
	bool Move( bool bMoveRoom, DWORD dwData );

	// 开启频道广播
	bool OpenBroadcast( bool bOpen );

	// 禁言操作
	bool BanPDBID( DWORD dwPDBID, bool bBan );

	// 踢出用户
	bool KickPDBID( DWORD dwPDBID );

	// 设置是否开麦，在控麦下使用 在控麦下使用，临时保存
	void SetNeedOpenMic( bool bNeed );

	// 是否要开麦，在控麦下使用，临时保存
	bool IsNeedOpenMic(void);

	// 在当前房间是否可以文字聊天
	bool CanChat( CRoom & curRoom, DWORD & dwTimeLeft );

	// 获取个人频道ID
	LONGLONG GetPersonalChannelID(void);

	// 发送AD数据到社会服
	void SendADToSocial( DWORD dwBroadcastType, DWORD dwADType, char * pTextData, DWORD dwTextLen );

	// 发送AD数据到客户端
	void SendADToClient(DWORD dwChannelType, DWORD dwRoomID,  DWORD dwBroadcastType, DWORD dwADType, char * pTextData, DWORD dwTextLen );

	// 设置马甲用户类型
	void SetVestUserType( BYTE nType, bool bVest, bool bUpdate = false );

	// 添加/删除马甲
	void AddVest( DWORD dwUDBID, bool bAdd, DWORD dwAddType = 0 );

	// 设置私聊配置，接受配置
	void SetChatConfig( DWORD dwConfigType );

	// 设置私聊对象ID和私聊用户类型
	void SetChatIDAndType( DWORD dwID, DWORD dwUserType );

	// 获取私聊用户类型
	DWORD GetChatUserType(void);

	// 获取私聊对象
	CChat * GetChatInstance(void);

	// 邀请私聊
	void ChatInvite( DWORD dwPDBIDAcceptor, bool bStranger );

	// 私聊邀请回应 dwInviter 邀请者客户端ID
	void ChatInviteResponse( DWORD dwInviter, bool bAccept );

	// 离开私聊
	void LeaveChat(void);

	// 设置上次接收（不是接受）邀请时间
	void SetLastAcceptChatTime(void);

	// 获取上次接收（不是接受）邀请时间
	DWORD GetLastAcceptChatTime(void);

	// 从DB登录一下,用来记录在线状态,和加载一下用户信息
	void DBLogin(void);

	// 从DB下线,用来记录在线状态
	void DBLogout(void);

	// 语音角色用户进入频道,保存DB,暂时不用,有用时可调用一下来记录,这个调用频繁,所有没有必要,不要打开
	void DBEnterChannel(void);

	// 加入关注指定的玩家,用ClientID为参数
	bool AddFansUser(DWORD dwClientID);

	// 取消关注指定的玩家,用ClientID为参数
	bool DelFansUser(DWORD dwClientID);

	// 是否是指定通行证UDBID的粉丝
	bool IsFansUser(DWORD dwUDBID);

	// 加减粉丝数,并同步到客户端
	bool FansCountAdd(bool bAdd = true, bool bSync = true );

	// 加减粉丝到数据库
	void FansAddToDB( DWORD dwFansUDBID, bool bAdd = true);

	// 获取数据库用户类型
	DWORD GetDBUserType( DWORD dwChannelType );

	// 召集粉丝
	DWORD CallFans( const char * szText, bool bLocal,int nParam1,int nParam2 );

	// 发送在线粉丝数到客户端
	DWORD SendFansOnlineCount(void);

	// 增加接受邀请数
	void AddFansAcceptCount(void);

	// 增加取消关注数
	void AddFansCancelCount(void);

	// 邀请粉丝后，有数据变化
	void OnFansCallInfoChange(void);

	// 发送粉丝召集结果到客户端
	void SendFansCallResult(void);

	// 设置上次邀请粉丝的信息
	void SetLastCallFansInfo( DWORD dwTime, DWORD dwFansAcceptCount, DWORD dwFansCancelCount );

	// 更新昨日粉丝总数到客户端
	void SetFansCountYesterday( DWORD dwFansCountYesterday );

	// 进入对方所在的频道
	bool EnterRoomByUser( CClientUser & user );

	// 设置某个频道的发送语音召集令的时间
	void SetLastADTime( DWORD dwChannelType, DWORD dwTime );

private:
	// 粉丝关系读取完成
	void OnLoadDBFansList(void);

protected:
    SVoiceUserInfo_Table	m_UserInfo;	// 用户详细信息
private:
	DWORD	m_dwClientID;		// 语音网关上的ID
	bool	m_bLogin;			// 是否验证
	bool	m_bOnline;			// 是否在线
	DWORD	m_dwState;			// 状态
	
	DWORD	m_dwKickReason;		        // 踢出理由

	DWORD	m_dwWorldID;		/// 所在服务器世界ID
	DWORD	m_dwServerID;		/// 所属社会服务器ID

    LONGLONG m_nRoomID;          // 当前的房间ID

	// 是否可以说话
	bool m_bCanSpeak;

	// 语音是否频道广播
	bool m_bBroadcast;

	// 连入时间
	DWORD	m_dwConnectTime;	

	// 该客户端IP
	DWORD	m_dwRemoteIP;

	// 是否需要开麦
	bool	m_bNeedOpenMic;

	// 上一次文字聊天的频道
	DWORD	m_dwLastChatChannel;

	// 上一次文字聊天的时间
	DWORD	m_dwLastChatTime;

	// 私聊信息
	SVoiceChatInfo m_ChatInfo;

	// 是否已加载数据
	bool	m_bLoaded;

	// 是否已保存数据,现暂时没有要保存的数据,以后扩展用
	bool	m_bSaved;

	// 所有的关注的通行证ID列表
	TMAP_VFansUser m_mapFansUser;

	// 上一次更新粉丝数字时间
	DWORD	m_dwLastUpdateFansTime;

	// 上一次召集粉丝的时间
	DWORD	m_dwLastCallFansTime;

	// 召集之后，粉丝接受邀请的个数
	DWORD	m_dwFansAcceptCount;

	// 召集之后，粉丝取消关注的个数
	DWORD	m_dwFansCancelCount;

	// 昨日粉丝总数
	DWORD	m_dwFansCountYesterday;

	// 上次发送语音召集令的时间
	DWORD	m_dwLastADTime[VoiceChannel_Max];

public:
    int m_nUpdateFlag;
	DWORD m_dwLastAudioTime;	// 最后语音Tick时间,用来处理自由抢麦处理老不说话的人,以空出位置给别人说

	BYTE	m_nKeyLen;						// 密钥长度
	char	m_pKeyData[VOICE_RC4_MAX_SIZE];	// 密钥内容
};

#endif