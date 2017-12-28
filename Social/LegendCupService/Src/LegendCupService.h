/*******************************************************************
** 文件名:	E:\speed\Social\LegendCupServer\Src\LegendCupService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	17：45
** 版  本:	1.0
** 描  述:	杯赛服务类相关
********************************************************************/

#pragma once
#include "ILegendCupService.h"
#include "IGameDatabaseDef.h"
#include "IGatewayAcceptorService.h"
#include "IDBEngineService.h"
#include "IDBEngine.h"
#include "MatchDef.h"
#include "LegendCupDef.h"
#include "EventDef.h"
#include "IMessageHandler.h"
#include "IMessageDispatch.h"
#include "IShareServer.h"
#include "LegendCupInfoMgr.h"
#include "LegendCup.h"
#include "DateHandler.h"
#include "IDateTriggerService.h"

class CLegendCupInfoMgr;

class CLegendCup;

class LegendCupService:public ILegendCupService, public IDBRetSink, public IEventExecuteSink, public ITransmitHandler,public IMessageHandler, public ISharePersonHandler, public DateHandler
{
// 玩家数据列表
typedef multimap<DWORD, SLegendCupPlayerInfo>				TMap_LegendCupPlayerList;
// 玩家数据列表迭代器
typedef multimap<DWORD, SLegendCupPlayerInfo>::iterator		TMap_LegendCupPlayerListIt;

private:
    enum eDataComeTrigger
    {
        DCT_DataTrigger_LegendCupUpdate,
    };

public:
    LegendCupService();

    ////////////////////////////////ITransmitHandler//////////////////////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void     handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen);

    // 处理客户端发送过来的消息
    virtual void     handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen);

	/** 消息处理
	@param server	源服务器ID
	@param uMsgID	消息ID
	@param head		消息头
	@param data		消息
	@param len		消息长度
	*/
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

	/**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void	onServerListUpdated();

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

    // 指定参加某杯赛的战队列表（报名阶段有效）
    virtual void    luaAddCupRegister(LONGLONG llLegendCupID, PACKAGE_PTR pkKinList);

    // 移除杯赛所有参赛战队列表（报名阶段有效）
    virtual void    luaRemoveCupRegister(LONGLONG llLegendCupID);

    // 退出正在比赛的节点
    virtual void    exitStartCompetitionNode(LONGLONG llLegendCupID, int nSerchID, DWORD dwNewStartTime);

	// 弃权战队邮件提示
	virtual void	sendAbstentionKinMail(LONGLONG llLegendCupID, DWORD dwFailedKinID);
	////////////////////////////////IMessageHandler//////////////////////////////////////////
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

    /////////////////////////////////////IDBRetSink/////////////////////////////////////
    /** 数据库请求返回回调方法
	@param   pDBRetSink		：结果返回接口	
	@param   nCmdID ：命令ＩＤ
	@param   nDBRetCode：数据库请求返回值，参考上面定义
	@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
	@param   nQueueIndex：队列定义
	@param   pOutData：输出数据
	@param   nOutLen：输出数据长度
	@return  
	@note     
	@warning 此对像千万不能在数据库返回前释放，否则会非法！
	@retval buffer 
	*/	
	virtual void    OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

    ////////////////////////////////IEventExecuteSink//////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void    OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/////////////////////////////////////ILegendCupService/////////////////////////////////////

    // _Stub::release() call
    void    Release();
    // _Stub::on_start call
    bool    Create();

    // 玩家掉线
	virtual void    onClientDisconnect(ClientID client,DWORD dwReason);

	// 设置杯赛比赛结果
	virtual void    setLegendCupMatchResult(SMsgRecordNodeWarResult sNodeWarResult);

    // 设置杯赛比赛节点状态
    virtual void    setLegendCupNodeState(SMsgSetCompetitionNodeState sNodeState);

    // 得到联盟杯赛信息
    virtual SCreaterLegendCupInfo getCreaterLegendCupInfo(BYTE byCupCreateType,DWORD dwCreaterID);

	// 获取免费联盟杯赛的奖励次数
	virtual int		getPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID);

	// 设置免费联盟杯赛的奖励次数
	virtual bool	setPlayerFreeClanLegendCupPrizeCount(DWORD dwPDBID, int nRecvPrizeCount);

    // 是否在参加杯赛
    virtual bool	isKinInLegendCup(DWORD dwKinID);

    ////////////////////////////////ISharePersonHandler//////////////////////////////////////////
    /** 上线或者跨进程切换地图后，也是在这里得到通知
	@param   SHARELINE_TYPE ：在线回调类型
	@param   
	@return  
	*/
	virtual void			OnLogin(ISharePerson * pSharePerson, ELoginMode nLineType);

	/** 下线或者跨进程切换地图后，也是在这里得到通知，当调完以后，就无法再取到
	@param   SHARELINE_TYPE ：在线回调类型
	@param   
	@return  
	*/
	virtual void			OnLogout(ISharePerson * pSharePerson, ELogoutMode nLineType);

	/** 更新数据前，此处分一前一后，是让应用层知道什么数据改变了，而nUpdateFlag表示是因为什么原因改变了
	@param   pSharePerson ：更新前的对像
	@param   nUpdateFlag  ：改变原因
	@return  
	*/
	virtual void			OnPre_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason);

	/** 更新数据后，此处分一前一后，是让应用层知道什么数据改变了，而nUpdateFlag表示是因为什么原因改变了
	@param   pSharePerson ：更新后的对像
	@param   nUpdateFlag  ：改变原因
	@return  
	*/
	virtual void			OnPost_Update(ISharePerson * pSharePerson, SHAREUPDATE_REASON nUpdateReason);

    ////////////////////////////////DateHandler//////////////////////////////////////////
    virtual void            DateCome(unsigned long nTriggerID);

    virtual void            endCupRemoveCreaterRecInfo(LONGLONG llLegendCupID);

    // 移除杯赛所有相关信息
    virtual void            removeServiceCupInfo(LONGLONG llLegendCupID);
public:
	// 保存杯赛玩家数据toDB
	void SaveLegendCupPlayerInfoToDB(DWORD dwPDBID);

    // 发送消息给玩家
    void send2Client( ClientID clientID, obuf256& obuf);

private:

    // 创建杯赛检查
    bool canCreateLegendCup(SMsgCreateCup* pCreateCup);

    //添加杯赛到管理
    bool addLegendCupToMgr(CLegendCup *pLegendCup);

    // 请求创建杯赛
    void onMsgCreateLegendCup(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);


    // 读取杯赛附加数据（成员列表 和比赛信息）
    void readIniCupPlusData(LONGLONG llLegendCupID);

    // 查找杯赛
    CLegendCup* findLegendCup(LONGLONG llLegendCupID);

    void addCupAllKinMapedLegendID(CLegendCup *pLegendCup);

    //////////////////////////////////事件响应///////////////////////////////////////////

    // 从数据库读取全部杯赛数据
    void readAllCupInfoFromDB();

    // DB读取完毕
    void readCupInfoDBFinished(LONGLONG llLegendCupID);

    void onEventKinMemberDelte(LPCSTR pszContext, int nLen);

    void onEventKinMemberQuit(LPCSTR pszContext, int nLen);

    void onEventKinMemberLogin(LPCSTR pszContext, int nLen);

    //////////////////////////////////数据库返回///////////////////////////////////////////
    // 返回杯赛基础信息
    void onReturnReadAllCupDataBaseInfo(char* pOutData, int nOutLen);

    // 返回杯赛战队信息
    void onReturnCupKinInfo(char* pOutData, int nOutLen);

    // 返回杯赛 黑名单列表 申请列表 排名列表...
    void onReturnCupEXListDataInfo(char* pOutData, int nOutLen);

    // 返回杯赛参赛成员信息
    void onReturnCupMemberList(char* pOutData, int nOutLen);

    // 返回杯赛比赛信息
    void onReturnCupCompetitionNode(char* pOutData, int nOutLen);

    // 返回每轮位置信息
    void onReturnCupRoundKinPos(char* pOutData, int nOutLen);

    // 返回每轮时间信息
    void onReturnCupRoundTimeInfo(char* pOutData, int nOutLen);

	// 返回玩家杯赛数据信息
	void onReturnLegendCupPlayerInfo(char* pOutData, int nOutLen);

    /////////////////////////////////////服务器消息/////////////////////////////////////
    // 创建杯赛检查
    void onServerMsgCreateCupCheck(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 处理场景服创建杯赛结果
    void onServerMsgCreateCupResult(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);
    
    // 处理场景服请求进入杯赛房间
    void onServerMsgReqJoinLegendcupRoom(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);

	// 处理场景服单个比赛结果
	void onServerMsgCupRecordWarResult(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 处理申请加入杯赛成功
    void onServerMsgReqJoinCupSuccess(DWORD dwServerID, SNetMsgHead* pHead, void* pData, size_t stLen);

    /////////////////////////////////////客户端消息/////////////////////////////////////
    // 响应客户端请求开始杯赛
    void onClientPublicCompetition(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 响应客户端请求杯赛列表
    void onClientRequestCupList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 响应客户端申请加入杯赛
    void onClientMsgReqJoinCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 响应客户端申请退出杯赛
    void onClientMsgExitCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 将战队添加到黑名单
    void onClientMsgMoveToBlackList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 移除黑名单
    void onClientMsgRemoveToBlackList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 响应查看杯赛
    void onClientMsgLoockOverLegendCup(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);
    
    // 请求黑名单列表
    void onClientMsgRequestAllBlackKinlist(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 请求战队成员列表
    void onClientMsgRequestKinMemberList(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 裁定比赛结果
    void  onClientMsgRequestJudgeWarResult(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 查看杯赛节点战场信息
    void onClientMsgRequestLookUpCupNodeWarInfo(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 关闭查看杯赛
    void onClientCloseCupView(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

    // 请求线下结束报名阶段
    void onClientOfflineEndRegisterState(ClientID clientID, SNetMsgHead* pHead, void* pData, size_t stLen);

private:
    map<LONGLONG, bool> m_ReadDBRecord;                 // 数据库读取完成标示记录

    map<LONGLONG, BYTE> m_LegendCupTypeList;            // LegendID 对应的 ELegendCupType 类型

    map<BYTE, CLegendCupInfoMgr*> m_AllLegendCupMgr;    // ELegendCupType 对应的杯赛管理类

	TMap_LegendCupPlayerList m_LegendCupPlayerList;		// 玩家杯赛数据列表
 
    bool bReadDBCupInfoFinished;                        // 读取完成标示
};