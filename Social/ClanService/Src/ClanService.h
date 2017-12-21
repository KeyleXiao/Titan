/*******************************************************************
** 文件名:	ClanService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/31/2016	17:19
** 版  本:	1.0
** 描  述:	帮会服务
********************************************************************/

#ifndef __CLANSERVICE_H__
#define __CLANSERVICE_H__

#include "IGameDatabaseDef.h"
#include "IGatewayAcceptorService.h"
#include "IDBEngineService.h"
#include "IDBEngine.h"
#include "IMessageHandler.h"
#include "ICenterConnectorService.h"
#include "IShareServer.h"
#include "IClanService.h"
#include "Clan.h"
#include "KinDef.h"
#include "DateHandler.h"
#include "DateHandler.h"
#include "IDateTriggerService.h"

// 登陆玩家缓存数据结构,这个结构保存了玩家登陆（帮会第一个玩家）到帮会数据完全读出来期间，登陆的玩家列表
struct SClanUserLogin
{
    DWORD dwClanID;			// 帮会ID
    list<DWORD> userList;	// 玩家PDBID列表
    bool  bIsRequest;		// 提交状态（未提交，提交）

    SClanUserLogin()
    {
        dwClanID = 0;
        userList.clear();
        bIsRequest = false;
    }
};

class CClanService : public IClanService,
    public ITransmitHandler,
    public IMessageHandler,
    public IDBRetSink,
    public IEventExecuteSink,
    public TimerHandler,
    public ISharePersonHandler,
	public DateHandler
{
private:
	enum EMTimerID
	{
		TimerID_Clan_Clear_User_Login_List,       // 清楚玩家缓存列表定时器
	};

	enum eDataComeTrigger
	{
		DCT_DataTrigger_ClanUpdate,
		DCT_DataTrigger_WeekActivityReset,
        DCT_DataTrigger_AutoSetShaikhState,
        DCT_DataTrigger_LegendCupDidaUpdate
	};

public:
	CClanService();
    ~CClanService();

    // _Stub::on_start call
    bool create();
public:
    /////////////////////////////////////IClanService/////////////////////////////////////
    // _Stub::release() call

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

    virtual void release();

	// 查找战队所属帮会ID
	virtual DWORD findKinClan(DWORD dwKinID);
    // 移除正在读取的战队
	virtual void removeReadingKin(DWORD dwKinID);
	// 获取帮会成员列表
	virtual int getClanKinList(DWORD dwClanID, DWORD *pKinList, int nLen);
    // 取得帮会数值
    virtual int getClanNumProp(DWORD dwClanID, int nNumIndex);
	// 获取帮会名称
	virtual string getClanName(DWORD dwClanID);
	// 联盟杯赛创建
	virtual void notifyCreateClanLegendCup(DWORD dwClanID, LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName);

	////////////////////////////////ITransmitHandler//////////////////////////////////////////
	// 消息处理
	virtual void	        onTransmit(DWORD server, ulong uMsgID, SNetMsgHead *head, void *data, size_t len);
	// 通知服务器列表已经更新
	virtual void	        onServerListUpdated();
	// 通知服务器信息更新
	virtual void	        onServerInfoUpdated(DWORD ServerID, BYTE nState, void *pServerData);

	////////////////////////////////IMessageHandler//////////////////////////////////////////
	// 消息处理
	virtual void	        onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead *head, void *data, size_t len);

    /////////////////////////////////////IDBRetSink/////////////////////////////////////
    // 数据库请求返回回调方法
	virtual void			OnReturn(IDBRetSink *pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen);

    ////////////////////////////////IEventExecuteSink//////////////////////////////////////////
	virtual void            OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////////////////TimerHandler//////////////////////////////////////////
    virtual void            OnTimer( unsigned long dwTimerID );

    ////////////////////////////////////ISharePersonHandler////////////////////////////////////
    // 上线或者跨进程切换地图后，也是在这里得到通知
	virtual void			OnLogin(ISharePerson *pSharePerson, ELoginMode nLineType);
	// 下线或者跨进程切换地图后，也是在这里得到通知，当调完以后，就无法再取到
	virtual void			OnLogout(ISharePerson *pSharePerson, ELogoutMode nLineType);
	// 更新数据前，此处分一前一后，是让应用层知道什么数据改变了，而nUpdateFlag表示是因为什么原因改变了
	virtual void			OnPre_Update(ISharePerson *pSharePerson, SHAREUPDATE_REASON nUpdateReason);
	// 更新数据后，此处分一前一后，是让应用层知道什么数据改变了，而nUpdateFlag表示是因为什么原因改变了
	virtual void			OnPost_Update(ISharePerson *pSharePerson, SHAREUPDATE_REASON nUpdateReason);

	////////////////////////////////DateHandler//////////////////////////////////////////
	virtual void            DateCome(unsigned long nTriggerID);

	////////////////////////////////CClanService//////////////////////////////////////////

    // 查找帮会
    CClan* findClan(DWORD dwClanID);
    // 查找帮会
    CClan* findClanByName(string szClanName);
    // 获得帮会战队
    DWORD findKinClanByPDBID(DWORD dwPDBID);

    // 移除帮会
    void removeClan(DWORD dwClanID);

    // 添加帮会成员（有则更新）
    void addClanMember(DWORD dwClanID, DWORD dwKinID, bool bNeedSaveDB);
    // 删除帮会成员
    void removeClanMember(DWORD dwKinID, bool bNeedSaveDB);

    // 更新成员数据到数据库
	void updateMemberToDB(DWORD dwClanID, DWORD dwKinID);
	// 更新全部联盟数据到场景服
	bool refreshClanInfoToZone(CSID dwServerID = INVALID_SERVER_ID);
    // 更新成员数据到场景服
    void updateMemberToZone(DWORD dwClanID, DWORD dwKinID, CSID ServerID = INVALID_SERVER_ID);
    // 更新成员数据到客户端
    void updateMemberToClient_OZC(DWORD dwClanID, SKinMember* pKinMember);
    // 更新成员数据到客户端
    void updateMemberToClient_OC(DWORD dwClanID, SKinMember* pKinMember);

    // 成员上线（社会服和场景服重连也会通知）
    void onMemberLogin(DWORD dwClanID, DWORD dwMemberPDBID);
    // 查找帮会的登陆缓存成员
    SClanUserLogin *findLoginMember(DWORD dwClanID);
    // 添加正在读取的战队
    bool addReadingKin(DWORD dwClanID, DWORD dwKinID);
    // 是否为空
    bool isReadingKinEmpty(DWORD dwClanID);
    // 帮会的全部数据读取完成
    void onReadDBFinish(DWORD dwClanID);

	// 根据周活跃度检查是否自动解散帮会
	void autoDismissLowerActivity();

    // 检查是否需要解散的帮会
    bool checkDBDismiss(DBREQ_TABLE_CLAN* pClanContext);
    // 提交解散帮会请求
    void onDismissClan(DWORD dwClanID);

    /// 执行存储过程
    bool onExecuteSP(DWORD dwCmdID, DWORD dwQueueIndex, LPCSTR data, int nLen);

private:
    // 读取帮会列表         					-- 来自数据库
    void onReturnReadClanList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // 读取帮会成员列表       				-- 来自数据库
    void onReturnReadMemberList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // 更新/添加成员信息       				-- 来自数据库
    void onReturnUpdateMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // 删除成员信息			  				-- 来自数据库
    void onReturnRemoveMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // 更新帮会基本数据			  			-- 来自数据库
    void onReturnUpdateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // 解散帮会			  					-- 来自数据库
    void onReturnDismissClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

    // 帮会服务器模块消息码
    void onModuleHandleMessage(DWORD dwServerID, DWORD dwMsgID, LPCSTR pszData, size_t nLen);
    // 创建帮会消息处理					-- 来自场景服
    void onSceneCreateClan(DWORD dwServerID, LPCSTR pszData, size_t nLen);
    
    // 帮会客户端模块消息码
    void onModuleClientMsg(DWORD dwClientID, DWORD dwMsgID, LPCSTR pszData, size_t nLen);
    // 创建帮会消息处理						-- 来自客户端
    void onClientCreateClan(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);
    // 申请帮会列表							-- 来自客户端
    void onClientClanList(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);
	// 邀请加入联盟							-- 来自客户端
	void onClientClanInvite(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);
	// 邀请答复							-- 来自客户端
	void onClientAnswerInvite(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

    // 读取帮会列表信息
    bool readClanList();
    // 读取帮会附加数据（成员列表）
	void readClanPlusData(DWORD dwClanID);

	// 通知场景服更改角色身上的联盟ID
	void setActorClanID(DWORD dwPDBID, DWORD dwClanID, DWORD dwServerID);

	// 修改所有联盟杯赛次数
	void UpdateAllClanLegendCupProp();

    // 更新杯赛dida标示
    void updateClanLegendDida();

	// 重置周活跃度
	void ResetWeekActivity();

    // 自动设置盟主状态,后台自动更换盟主
    void AutoSetShaikhState();

private:
    // 帮会列表
    typedef stdext::hash_map<DWORD, CClan*>                   TMap_Clan;
    // 帮会列表迭代器
    typedef stdext::hash_map<DWORD, CClan*>::iterator         TMap_ClanIt;
    TMap_Clan       m_ClanList;								// 帮会列表
    list<CClan*>    m_ClanArray;							// 帮会列表(显示用)

    // 帮会列表（名字索引）
    typedef stdext::hash_map<std::string, CClan*>             TMap_NameClan;
    // 帮会列表（名字索引）迭代器
    typedef stdext::hash_map<std::string, CClan*>::iterator   TMap_NameClanIt;
    TMap_NameClan	m_NameClanList;							// 帮会列表（名字索引）

    // 帮会成员列表
    typedef stdext::hash_map<DWORD, DWORD>				   TMap_KinClan;
    // 帮会成员列表迭代器
    typedef stdext::hash_map<DWORD, DWORD>::iterator       TMap_KinClanIt;
    TMap_KinClan	m_KinClanList;							// 帮会成员列表

private:
    // 登陆玩家缓存列表
    typedef map<DWORD, SClanUserLogin>                        TMap_ClanUserLogin;
    // 登陆玩家缓存列表迭代器
    typedef map<DWORD, SClanUserLogin>::iterator              TMap_ClanUserLoginIt;
    // 保存玩家登陆（帮会第一个玩家）到帮会数据完全读出来期间，登陆的玩家列表
    TMap_ClanUserLogin m_ClanUserLoginList;
    // 战队数据读取缓存列表
    map<DWORD, list<DWORD>> m_ReadingKinList;

};

#endif // __CLANSERVICE_H__