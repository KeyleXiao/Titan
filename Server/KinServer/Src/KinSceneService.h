/*******************************************************************
** 文件名:	E:\speed\Server\KinServer\Src\KinSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	战队服务
********************************************************************/

#pragma once
#include "IKinSceneService.h"
#include "NetMessageDef.h"
#include "Kin.h"
#include "KinMember.h"
#include "IDBEngine.h"
#include "IMessageHandler.h"
#include "Kin_ManagedDef.h"
#include "Simple_Atom_Lock.h"
using namespace Kin_ManagerDef;

class KinScenService:public IKinSceneService, public IDBRetSink,  public IEventExecuteSink, public ITransmitHandler, public IMessageHandler
{
public:
	////////////////////////////////IKinSceneService//////////////////////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len);

	// purpose:发送社会服消息
	virtual void sendToSocial(BYTE nMsgID, void * data, size_t len);
	/////////////////////////////////KinSceneServer/////////////////////////////////////////
	// purpose:  _Stub::release() call
	void release();

	// purpose:  _Stub::on_start call
	bool create();

    //  purpose: 取得职位的名字
	void getIdentityName(int nIdentity, char* szName, int nLen);

	/// purpose: 根据成员的PDBID查找战队成员
	CKinMember* findKinMemberEx(DWORD dwPDBID);

	/// purpose: 删除一个成员
	void removeKinMember(DWORD dwPDBID);

	/// purpose:调用DB存储过程创建战队
	virtual void DBProcCreateKin(DWORD pdbid, const char* szKinName, const char* szKinLabel, bool bCrtAsFormal = false);


	///// purpose: 根据成员的PDBID查找战队成员
	virtual SSceneKinMember getKinMember(DWORD dwPDBID);

    /** 根据战队ID获取Kin数据
	@param dwKinID		:战队ID
	*/
	virtual SKinInfo getKinInfo(DWORD dwKinID);

    /** 根据战队ID获取成员列表接口
	@param dwKinID		:战队ID 
    @return obuf (都会有第一个DWORD 来保存成员数量 >= 0)
	*/
	virtual DWORD   getKinMemberList(DWORD dwKinID,PDBID* pReturnArray, DWORD dwArrayMaxSize);

    /** 获取战队成员数量
	@param dwKinID		:战队ID 
	*/
	virtual DWORD   getKinMemberCount(DWORD dwKinID);

	/** 设置周活跃度
	* @param dwKinID  		:战队ID
	* @param nValue		    :周活跃度值
	*/
	virtual bool setWeekActivity(DWORD dwKinID, int nValue);

	/** 获取周活跃度
	* @param dwKinID		 :战队ID
	*/
	virtual int getWeekActivity(DWORD dwKinID);

    /////////////////////////////ITransmitHandler/////////////////////////////////////////////
    /// purpose: 消息处理
    virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

    ////////////////////////////IMessageHandler//////////////////////////////////////////////
    /// purpose: 消息处理
    virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

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

public:

	//////////////////////////////////IDBRetSink///////////////////////
	/// purpose: 数据库请求返回回调方法
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	////////////////// IEventExecuteSink ////////////////////////////////////////////////////////
	/// purpose: 执行事件接口实现
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

private:

	/// purpose: 创建战队					-- 来自社会服
	void onTransmitCreateKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

	/// purpose: 战队改名					-- 来自社会服
	void onTransmitKinRename(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

	/// purpose: 刷新战队名称号				-- 来自社会服
	void onTransmitNameTitle(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);
	
	/// purpose: 设置玩家的战队ID			-- 来自社会服
	void onTransmitUpdateKinID(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

	/// purpose: 转发消息					-- 来自社会服
	void onTransmitPostMsg(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: 删除成员				-- 来自社会服
    void onTransmitRemoveMember(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: 更新成员（没有则添加）	-- 来自社会服
    void onTransmitUpdateSceneMember(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: 删除战队				-- 来自社会服
    void onTransmitRemoveKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: 更新战队（没有则添加）	-- 来自社会服
    void onTransmitUpdateKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: 更新战队属性			-- 来自社会服
    void onTransmitUpdateKinProp(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: 清除战队				-- 来自社会服
    void onTransmitClearKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

    /// purpose: 清除战队				-- 来自社会服
    void OnTransmitClearKin(DWORD dwMsgID, LPCSTR pszMsg, size_t nLen);

	/// purpose: 中心服检测战队名称			-- 来自数据库
	void onReturnCenterKinNameCheck(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 中心服创建战队				-- 来自数据库
	void onReturnCenterCreateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 创建战队					-- 来自数据库
	void onReturnCreateKin(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 回退战队操作
	//@ note   : 当通知数据库创建战队成功返回，场景服发现创建战队的条件已经不成立，需要取消先前的创建操作
	void backUpCreateKin(DWORD dwKinID, LPCSTR szTrackNote);

    virtual CKin* findKin(DWORD dwKinID);

    ///// purpose: 根据战队ID查找战队
    virtual IKin* getKin(DWORD dwKinID);

    // 游戏修改玩家信息返回
    //bool AddService_ResponseUpdateUserInfoFromGameDB(const DBREQ_RESULT_ADDSERVICES_USER_UPDATE & param);

    /// purpose: 更新战队ID
    void updateKinIDInfo(DWORD dwActorID, DWORD dwKinID);

private:
	map<DWORD, CKin*>			m_KinList;					// 战队列表
	map<DWORD, CKinMember*>		m_KinMemberList;			// 战队成员列表

    volatile	LONG    m_mutex;

};