/*******************************************************************
** 文件名:	E:\speed\Social\KinServer\Src\KinService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	17：45
** 版  本:	1.0
** 描  述:	战队服务
********************************************************************/

#pragma once

class CKin;
#include "Kin.h"
#include "IKinService.h"
#include "IGameDatabaseDef.h"
#include "IGatewayAcceptorService.h"
#include "IDBEngineService.h"
#include "IDBEngine.h"
#include "MatchDef.h"
#include "KinDef.h"
#include "KinEvent.h"
#include "Event_ManagedDef.h"
#include "KinMember.h"
#include "ICenterConnectorService.h"
#include "IShareServer.h"
#include "IMessageHandler.h"
#include "DateHandler.h"
#include "IDateTriggerService.h"

// 保存数据用于排名
// 保存玩家DBID list
typedef std::vector<const SKinMember *>  TVEC_KinMember;

// 登陆玩家缓存数据结构,这个结构保存了玩家登陆（战队第一个玩家）到战队数据完全读出来期间，登陆的玩家列表
struct SKinUserLogin
{
    DWORD dwKinID;			// 战队ID
    list<DWORD> userList;	// 玩家PDBID列表
    bool  bIsRequest;		// 提交状态（未提交，提交）

    SKinUserLogin()
    {
        dwKinID = 0;
        userList.clear();
        bIsRequest = false;
    }
};


class KinService: public IKinService,
	public IEventExecuteSink,
	public IDBRetSink,
	public ISharePersonHandler,
	public ITransmitHandler,
	public IMessageHandler,
	public DateHandler
{
private:
	enum eDataComeTrigger
	{
		DCT_DataTrigger_WeekActivityReset,
        DCT_DataTrigger_AutoSetShaikhState,
	};

// 登陆玩家缓存列表
typedef map<DWORD, SKinUserLogin>                        TMap_KinUserLogin;
// 登陆玩家缓存列表迭代器
typedef map<DWORD, SKinUserLogin>::iterator              TMap_KinUserLoginIt;

// 战队列表
typedef stdext::hash_map<DWORD, CKin*>                   TMap_Kin;
// 战队列表迭代器
typedef stdext::hash_map<DWORD, CKin*>::iterator         TMap_KinIt;

// 战队列表（名字索引）
typedef stdext::hash_map<std::string, CKin*>             TMap_NameKin;
// 战队列表（名字索引）迭代器
typedef stdext::hash_map<std::string, CKin*>::iterator   TMap_NameKinIt;

// 战队成员列表
typedef stdext::hash_map<DWORD, CKinMember>              TMap_KinMember;
// 战队成员列表迭代器
typedef stdext::hash_map<DWORD, CKinMember>::iterator    TMap_KinMemberIt;

typedef map<DWORD, CKin*> TMap_FormalKin;

typedef TMap_FormalKin TMap_InFormalKin;



public:
    /// purpose: 构造函数
    KinService();

    /// purpose: 虚构函数
    virtual ~KinService();

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
	
	// 日期触发器
	virtual void            DateCome(unsigned long nTriggerID);
	
    //////////////////////////////////IKinService////////////////////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

    /// purpose: 设置战队贡献度
    virtual bool    addClanCtrb(DWORD dwPDBID, int nClanCtrb);

    /** 根据战队ID获取Kin接口
	@param dwKinID		:战队ID
	*/
	virtual IKin*	getKin(DWORD dwKinID);

    /** 根据战队ID获取KinInfo
	@param dwKinID		:战队ID
	*/
    virtual SKinInfo getKinInfo(DWORD dwKinID);

    /** 根据战队ID获取成员数据接口
	@param dwKinID		:战队ID
	*/
	virtual SKinMember getKinMemberInfo(DWORD dwPBID);

    /** 根据战队ID获取成员接口
	@param dwKinID		:战队ID
	*/
	virtual SKinMember*	getKinMember(DWORD dwPBID);

    /** 根据战队ID获取成员列表接口
	@param dwKinID		:战队ID
	*/
	virtual DWORD getKinMemberList(DWORD dwKinID, PDBID* pReturnArray,DWORD dwArrayMaxSize);

    /** 获取战队成员数量
	@param dwKinID		:战队ID 
	*/
    virtual DWORD getKinMemberCount(DWORD dwKinID);

    /** 获取战队总战力
	@param dwKinID		:战队ID 
	*/
    virtual DWORD getTotalFightScore(DWORD dwKinID);

    /** 外部接口删除战队成员
	@param dwKinID		:战队ID 
	*/
    virtual bool deleteKinMember(DWORD dwKinID, DWORD ActorID);

	/// purpose: _Stub::release() call
	void release();
	/// purpose: _Stub::on_start call
	bool create();

	/// purpose: 发送消息给玩家
	void send2Client( ClientID clientID, obuf256& obuf);

    /// purpose: 添加战队成员（有则更新）
    virtual void addKinMember(SKinMember* pMember, bool bNeedSaveDB);

    /// purpose: 更新成员数据到客户端
    virtual void updateMemberToClient(SKinMember* pMember);

    /// purpose: 更新成员数据到数据库
    virtual void updateMemberToScene(SKinMember* pMember, CSID ServerID = INVALID_SERVER_ID);

    virtual CKin* findKin(DWORD dwKinID);

    /// purpose: 查找战队成员
    virtual SKinMember* findKinMember(DWORD dwPDBID);

    /// purpose: 更新成员数据到数据库
    virtual void updateMemberToDB(SKinMember* pMember);

    /// purpose: 删除战队成员
    virtual void removeKinMember(DWORD dwMemberPDBID, bool bNeedSaveDB);

    /// purpose: 取得玩家离开战队的时间
    int getQuitKinTime(DWORD dwPDBID);

    /// purpose: 设置离开战队时间
    void setQuitKinTime(DWORD dwPDBID, int nTime);

    /// purpose: 提交解散战队请求
    void DBDismissKin(DWORD dwKinID);

    /// purpose: 查找战队
    void removeKin(DWORD dwKinID);

    // purpose: 执行存储过程
    void ExecSP(DWORD cmdID,LPCSTR pData, int len);

	// purpose: 新增战队杯赛奖励
	virtual void addKinLegendAward(SKinCupAwardNode sAwardNode);

	// purpose: 新增战队杯赛战绩
	virtual void addKinLegendWarResult(SKinWarRecordNode sRecordNode);

    // purpose: 取得某个身份的名字
    virtual LPCSTR getIdentityName(int nIdentity);

	// purpose:  战队杯赛荣誉增加
	virtual void addKinLegendGlory(int nKinID, int nNum, int nGloryType);

	// purpose: 取战队杯赛荣誉
	virtual int getKinLegendGlory(int nKinID, int nGloryType);

	/// purpose: 获取周活跃度
	virtual int getWeekActivity(DWORD dwKinID);

	// purpose: 更新战队列表节点申请状态			
	void updateKinList(int nKinID, int nClientID);					

private:


    // 读取初始化数据
    void readInitalKinData();

    /// purpose: 读取战队附加数据（成员列表，联盟信息，称号数据）
    void readIniKinPlusData(DWORD dwKinID, bool bAddReadFlag);

    // 加载战队列表完成
    void onReadKinListFinish();

    // 读取战队列表信息
    bool readKinList();

    /// purpose: 战队的全部数据读取完成
    void onReadDBFinish(DWORD dwKinID);

    /// purpose: 处理模块消息
	void onModuleMessage(ulong nEndpointId, DWORD dwMsgID, SNetMsgHead* pHead, LPCSTR pszData, size_t nLen);

    /// purpose: 战队合并消息
	void OnMergeMessage(ulong nEndpointId, DWORD dwMsgID, SNetMsgHead* pHead, LPCSTR pszData, size_t nLen);

	/// purpose: 创建战队消息处理						-- 来自客户端
	void onClientCreateKin(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 邀请加入消息处理						-- 来自客户端
	void onClientInvite(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 邀请答复消息处理						-- 来自客户端
	void onClientAnswerInvite(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 请求战队列表						-- 来自客户端
    void onClientKinList(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 战队改名消息处理						-- 来自客户端
	void onClientKinRename(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 战队合并请求消息处理					-- 来自客户端
	void onClientRequestMerge(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 战队合并回应消息处理					-- 来自客户端
	void onClientMergeResponse(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 响应退出观察战队列表					-- 来自客户端
    void onClientRemoveKinListOb(ClientID clientID, LPCSTR pszData, size_t nLen);
    
	/// purpose: 修改战队名称消息处理					-- 来自场景服
	void onSceneKinRename(DWORD dwSceneID, LPCSTR pszData, size_t nLen);

    /// purpose: 修改性别					            -- 来自场景服
    void onSceneActorChangeSex(DWORD dwSceneID, LPCSTR pszData, size_t nLen);

    /// purpose: 修改战队名称消息处理					-- 来自场景服
    void onSceneActorRename(DWORD dwSceneID, LPCSTR pszData, size_t nLen);

    /// purpose: 更新战队其他属性                       -- 来自场景服
    void onSceneKinActorPropertyUpdate(DWORD dwSceneID, LPCSTR pszData, size_t nLen);
    
	/// purpose: 创建战队消息处理						-- 来自场景服
	void onSceneCreateKin(DWORD dwSceneID, LPCSTR pszData, size_t nLen);

	/// purpose: 读取战队列表         					-- 来自数据库
	void onReturnReadKinList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 读取战队基本数据     					-- 来自数据库
	void onReturnReadKinBase(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 读取战队成员列表       				-- 来自数据库
	void onReturnReadMemberList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 读取战队杯赛奖励列表       			-- 来自数据库
	void onReturnReadLegendAwardList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 读取战队杯赛战绩列表       			-- 来自数据库
	void onReturnReadLegendWarList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 更新/添加成员信息       				-- 来自数据库
	void onReturnUpdateMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 删除成员信息			  				-- 来自数据库
	void onReturnRemoveMember(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 更新战队基本数据			  			-- 来自数据库
	void onReturnUpdateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 解散战队			  					-- 来自数据库
	void onReturnDismissKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 添加杯赛奖励信息       				-- 来自数据库
	void onReturnAddLegendAward(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 添加杯赛战绩信息       				-- 来自数据库
	void onReturnAddLegendWar(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

    /// purpose: 成员上线（社会服和场景服重连也会通知）
    void onMemberLogin(DWORD dwKinID, DWORD dwMemberPDBID);

    /// purpose: 成员下线
    void onMemberLogout(DWORD dwKinID, SKinMember* pMember);

    /// purpose: 往场景服刷新战队的信息
    bool refreshKinInfoToScene(CSID dwServerID = INVALID_SERVER_ID);

    // 更新个人Rank信息
    void UpdatePersonRank(void* pEventData);

    
    // 更新个人Rank信息
    void UpdatePersonUpdateClanID(void* pEventData);
  

    // 更新个人状态到战队观察者
    void UpdatePersonInfoToKinOb(SKinMember* pMember);

    /// purpose: 添加战队列表观察者
    bool addObserve(DWORD dwPDBID);

    /// purpose: 删除战队列表观察者
    void removeObserve(DWORD dwPDBID, bool bCoerceRemove);

    /// purpose: 查找战队列表观察者
    SKinListObserve* findObserve(DWORD dwPDBID);

    // 检查全部DB数据读完
    void checkAllDBDataRead();

	// 重置周活跃度
	void ResetWeekActivity();

	// 自动解散低活跃度的战队
	void AutoDismissLowerActivity();

    // 自动设置队长状态,后台自动更换队长
    void AutoSetShaikhState();

private:
    // 玩家离开战队时间
    map<DWORD, int> m_UserQuitTimeTable;

    // 保存玩家登陆（战队第一个玩家）到战队数据完全读出来期间，登陆的玩家列表
    TMap_KinUserLogin m_KinUserLoginList;

    // 是否最后一页加载战队列表
    bool m_bFinalList;

    // 完整数据读完
    bool m_bDBKinAllDataFinal;

    // 保存需要删除的临时战队列表
    list<DWORD> m_DeleteKinList;

    map<DWORD, SKinListObserve> m_mapObserveList;               // 战队列表观察者（用于创建删除战队广播给观察人员）

    TMap_Kin			m_KinList;								// 战队列表
    TMap_InFormalKin	m_FormalKinArray;	                    // 正式战队列表，每个国家对应一个list列表,因为ID从1开始的，0的位置不用
    TMap_NameKin		m_NameKinList;							// 战队列表（名字索引）
    TMap_KinMember		m_KinMemberList;						// 战队成员列表

    volatile	LONG    m_mutex;
};