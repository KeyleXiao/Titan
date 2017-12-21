/*******************************************************************
** 文件名:	E:\speed\Social\MentorshipServer\Src\MentorshipService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/24/2016	19：30
** 版  本:	1.0
** 描  述:	师徒服务
********************************************************************/

#pragma once

class CMentorship;
#include "Mentorship.h"
#include "IMentorshipService.h"
#include "IGameDatabaseDef.h"
#include "IGatewayAcceptorService.h"
#include "IDBEngineService.h"
#include "IDBEngine.h"
#include "MentorshipDef.h"
#include "Event_ManagedDef.h"
#include "MentorshipPersonData.h"
#include "ICenterConnectorService.h"
#include "IShareServer.h"
#include "IMessageHandler.h"
#include "MentorshipIssueManager.h"
#include "MentorshipMsgDef.h"

#define MENTORSHIP_CLEAR_MENTORSHIP_LIST_TIMER	1		// 清除师徒关系列表定时器

// 删除师徒关系结构
struct SMentorshipDelInfo
{
	DWORD dwMasterPDBID;								// 师傅玩家ID
	DWORD dwPrenticePDBID;								// 学徒玩家ID

	SMentorshipDelInfo()
	{
		memset(this, 0, sizeof(*this));
	}	
};

class MentorshipService: public IMentorshipService, 
	public IEventExecuteSink,
	public IDBRetSink, 
	public ISharePersonHandler, 
	public ITransmitHandler,
	public IMessageHandler,
	public TimerHandler
{
// 师徒列表
typedef stdext::hash_map<DWORD, CMentorship*>						TMap_Mentorship;
// 师徒列表迭代器
typedef stdext::hash_map<DWORD, CMentorship*>::iterator				TMap_MentorshipIt;

// 个人师徒列表
typedef stdext::hash_map<DWORD, list<DWORD>>						TMap_PersonMentorship;
// 个人师徒列表迭代器
typedef stdext::hash_map<DWORD, list<DWORD>>::iterator				TMap_PersonMentorshipIt;

// 师徒个人数据列表
typedef stdext::hash_map<DWORD, CMentorshipPersonData*>				TMap_MentorshipPersonData;
// 师徒个人数据列表迭代器
typedef stdext::hash_map<DWORD, CMentorshipPersonData*>::iterator	TMap_MentorshipPersonDataIt;

public:
    /// purpose: 构造函数
    MentorshipService();

    /// purpose: 虚构函数
    virtual ~MentorshipService();

public:
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
	virtual void			OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

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
	virtual void            OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

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

    ////////////////////////////////IMessageHandler//////////////////////////////////////////
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void	        onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

	////////////////////////////////ITransmitHandler//////////////////////////////////////////
	/** 消息处理
	@param server	源服务器ID
	@param uMsgID	消息ID
	@param head		消息头
	@param data		消息
	@param len		消息长度
	*/
	virtual void	        onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

    /**
	@name         : 通知服务器列表已经更新
	@brief        : 服务器列表更新只会在有服务器进入退出时触发
	@note         : 具体的列表调用ICenterServerConnector::GetServerList获得
	*/
	virtual void	        onServerListUpdated();

	/** 通知服务器信息更新
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : 例如服务器人数变化等等
	*/
	virtual void	        onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	////////////////////////////////TimerHandler//////////////////////////////////////////
	virtual void            OnTimer( unsigned long dwTimerID );

	
    //////////////////////////////////IMentorshipService////////////////////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	///////////////////////////////MentorshipSerivce////////////////////////////////////////

	/// purpose: _Stub::release() call
	void release();
	/// purpose: _Stub::on_start call
	bool create();

	/// purpose: 发送消息给玩家
	void send2Client( ClientID clientID, obuf256& obuf);

    /// purpose: 更新师徒个人数据到客户端
    virtual void updateMentorshipPersonToClient(CMentorshipPersonData* pMentorshipPerson);

	/// purpose: 更新基础数据to客户端
	virtual void updateMentorshipPersonBaseToClient(CMentorshipPersonData* pMentorshipPerson);

	/// purpose: 添加师徒关系数据到客户端
	virtual void addMentorshipToClient(CMentorship* pMentorship, SSharePersonProperty &shareProperty);

	/// purpose: 获取师徒个人数据接口
	virtual IMentorshipPerson* getMentorshipPerson(DWORD dwPDBID);

	/// purpose: 获取师徒个人数据
	virtual SMentorshipPerson getMentorshipPersonInfo(CMentorshipPersonData* pMentorshipPerson);

	/// purpose: 查找师徒数据
    virtual CMentorship* findMentorship(DWORD dwMentorshipID);

	/// purpose: 查找师徒数据
	virtual CMentorship* findMentorshipByPDBID(DWORD dwMasterPDBID, DWORD dwPrenticePDBID);

    /// purpose: 查找师徒个人数据
    virtual CMentorshipPersonData* findMentorshipPersonData(DWORD dwPDBID);

	/// purpose: 更新全部师徒数据到场景服
	bool refreshMentorshipToScene(CSID dwServerID = INVALID_SERVER_ID);

    /// purpose: 清除个人师徒数据
    void removeMentorshipPersonData(DWORD dwPDBID);

	/// purpose: 清除师徒数据
	void removeMentorship(DWORD dwMenorshipID);

	/// purpose: 玩家是否在线
	bool isOnline(DWORD dwPDBID);

	/// purpose: 获取师徒正式关系个数
	int getMentorshipFormalCount(DWORD dwPDBID, BYTE nMentorshipPost);

	/// purpose: 获取师徒见习关系个数
	int getMentorshipInternCount(DWORD dwPDBID, BYTE nMentorshipPost);

    // purpose: 执行存储过程
    void ExecSP(DWORD cmdID,LPCSTR pData, int len);
  
private:
    // 读取师徒关系列表信息
    bool readMentorshipList(DWORD dwPDBID);

	// 获取师徒关系索引
	int getMentorshipIndex();

    /// purpose: 删除师徒数据从DB
    void delMentorshipFromDB();

    /// purpose: 处理模块Client消息
	void onModuleClientMsg(DWORD dwClientID, DWORD dwMsgID, LPCSTR pszData, size_t nLen);

    /// purpose: 处理模块服务器消息
	void onModuleHandleMsg(DWORD dwServerID, DWORD dwMsgID, LPCSTR pszData, size_t nLen);

	/// purpose: 读取师徒关系列表         					-- 来自数据库
	void onReturnReadMentorshipList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 添加师徒关系数据         					-- 来自数据库
	void onReturnAddMentorship(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 更新师徒关系数据         					-- 来自数据库
	void onReturnUpdateMentorship(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 删除师徒关系数据         					-- 来自数据库
	void onReturnDelMentorship(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 读取师徒个人数据         					-- 来自数据库
	void onReturnReadMentorshipPersonData(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 更新师徒个人数据(没有则添加)         		-- 来自数据库
	void onReturnUpdateMentorshipPersonData(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 添加师徒成员师徒值消息处理					-- 来自场景服
	void onSceneAddMentorshipValue(DWORD dwServerID, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒数据						-- 来自客户端
	void onClientMentorshipPersonData(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒消息发布						-- 来自客户端
	void onClientMentorshipIssue(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒发布消息列表					-- 来自客户端
	void onClientMentorshipIssueList(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒关系申请					-- 来自客户端
	void onClientMentorshipApply(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒关系申请应答					-- 来自客户端
	void onClientMentorshipApplyAnswer(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒发布消息自动发布设置					-- 来自客户端
	void onClientMentorshipIssueAutoSet(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒关系解散					-- 来自客户端
	void onClientMentorshipDismiss(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒关系导师的学徒列表					-- 来自客户端
	void onClientMentorshipMasterPrenticeList(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 师徒关系提升学徒				-- 来自客户端
	void onClientMentorshipPromote(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 请求领取首胜奖励				-- 来自客户端
	void onClientMentorshipReqFirstAward(SSharePersonProperty &shareProperty, LPCSTR pszData, size_t nLen);

	/// purpose: 申请导师					
	void applyMentorshipMaster(SSharePersonProperty &shareProperty, DWORD dwDesPDBID, BYTE nApplyPost, BYTE nRelationType);

	/// purpose: 申请学徒					
	void applyMentorshipPrentice(SSharePersonProperty &shareProperty, DWORD dwDesPDBID, BYTE nApplyPost, BYTE nRelationType);

	/// purpose: 申请导师回答					
	void applyMentorshipMasterAnswer(SSharePersonProperty &shareProperty, SSharePersonProperty &srcShareProperty, BYTE nApplyPost, BYTE nRelationType);

	/// purpose: 申请学徒回答				
	void applyMentorshipPrenticeAnswer(SSharePersonProperty &shareProperty, SSharePersonProperty &srcShareProperty, BYTE nApplyPost, BYTE nRelationType);

	/// purpose: 添加新的师徒关系
	void addMentorship(SSharePersonProperty &masterShareProperty, SSharePersonProperty &prenticeShareProperty, BYTE nRelationType);

	/// purpose: 更新个人师徒值Rank信息
	void updateMentorshipPersonRank(void* pEventData);

	/// purpose: 是否有该学徒
	bool isHaveMentorshipPrentice(DWORD dwPDBID, DWORD dwPrenticePDBID);

	/// purpose: 是否有该导师
	bool isHaveMentorshipMaster(DWORD dwPDBID, DWORD dwMasterPDBID);

	// 师傅存在师徒关系
	bool isHaveMentorship(DWORD dwSrcPDBID, DWORD dwDesPDBID);

	/// purpose: 获取师徒关系个数
	int getMentorshipCount(TMap_PersonMentorshipIt it, TMap_PersonMentorshipIt itEnd, DWORD dwPDBID, BYTE nRelationType);

	/// purpose:  自动添加到发布消息
	void autoAddMentorshipIssue(SSharePersonProperty &shareProperty, CMentorshipPersonData *pMentorshipPerson);

	/// purpose: 发放师徒首胜奖励
	void giveMentorshipFirstWinAward(SSharePersonProperty &shareProperty, CMentorshipPersonData *pMentorshipPerson, BYTE nMentorshipPost);

	/// purpose: 发送师徒首胜奖励
	void sendMentorshipFirstWinAward(SSharePersonProperty &shareProperty, BYTE nMentorshipPost);
	
	/// purpose: 处理玩家上下线通知
	void processMemberLoginAndLogout(DWORD dwUDBID, DWORD dwPDBID, bool bFlag);

	/// purpose: 发送玩家上下线通知
	void sendMemberLoginAndLogout(DWORD dwPDBID, DWORD dwDesPDBID, bool bFlag);

	/// purpose: 添加师徒首胜奖励
	void processAddFirstWinPrize(CMentorship *pMentorship, BYTE nAddPost, SMentorshipConfig *pConfig);

	/// purpose: 更改师徒玩家属性
	void changeMentorshipPlayerName(DWORD dwPDBID, SHAREUPDATE_REASON nUpdateReason, ISharePerson * pSharePerson);

	/// purpose: 是否需要加载师徒关系fromDB
	bool IsNeedLoadMentorshipFromDB(DWORD dwPDBID);

	/// purpose: 师傅和徒弟是否在一个场景
	bool isSameScene(CMentorship *pMentorship);

private:
	// 师徒关系索引
	int m_nMentorshipIndex;

    // 保存需要删除的临时师徒列表
    list<SMentorshipDelInfo> m_DeleteMentorshipList;

    TMap_Mentorship				m_MentorshipList;				// 师徒关系列表
    TMap_PersonMentorship		m_MasterList;					// 师徒导师列表
	TMap_PersonMentorship		m_PrenticeList;					// 师徒学徒列表
    TMap_MentorshipPersonData	m_PersonDataList;				// 师徒个人数据列表
};