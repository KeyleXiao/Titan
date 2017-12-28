/*******************************************************************
** 文件名:	speed\Social\MailServer\Src\MailService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	邮件服务
********************************************************************/

#pragma once

class CPlayerMail;
#include "PlayerMail.h"
#include "IMailService.h"
#include "IGameDatabaseDef.h"
#include "IGatewayAcceptorService.h"
#include "IDBEngineService.h"
#include "IDBEngine.h"
#include "MailDef.h"
#include "Event_ManagedDef.h"
#include "ICenterConnectorService.h"
#include "IShareServer.h"
#include "IMessageHandler.h"
#include "MailMsgDef.h"
#include "Mail_ManagedDef.h"
using namespace Mail_ManagerDef;

struct SMailDelInfo
{
	LONGLONG llMailID;
	DWORD dwPDBID;
};

class MailService: public IMailService, 
	//public IEventExecuteSink,
	public IDBRetSink, 
	public ISharePersonHandler, 
	public ITransmitHandler,
	public IMessageHandler
{
// 玩家邮件列表
typedef stdext::hash_map<DWORD, CPlayerMail*>					TMap_PlayerMail;
// 玩家邮件列表迭代器
typedef stdext::hash_map<DWORD, CPlayerMail*>::iterator			TMap_PlayerMailIt;

public:
    /// purpose: 构造函数
    MailService();

    /// purpose: 虚构函数
    virtual ~MailService();

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
	//virtual void            OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

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
	
    //////////////////////////////////IMailService////////////////////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len) override;

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len) override;

	// 发送邮件
	virtual bool    sendMail(SMailSendData sSendData, int nMailFillID = emMailFill_Begin, LPCSTR szParam = NULL) override;

    // 读取玩家邮件列表
    virtual bool    readPlayerMailList(DWORD dwPDBID) override;
	///////////////////////////////MailSerivce////////////////////////////////////////

	/// purpose: _Stub::release() call
	void release();
	/// purpose: _Stub::on_start call
	bool create();

    // purpose: 执行存储过程
    void ExecSP(DWORD cmdID,LPCSTR pData, int len);
  
private:
	/// purpose: 读取玩家邮件列表         					-- 来自数据库
	void onReturnReadMailList(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 删除邮件     								-- 来自数据库
	void onReturnDelMail(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 添加邮件     								-- 来自数据库
	void onReturnAddMail(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

	/// purpose: 添加邮件消息处理					-- 来自场景服
	void onSceneAddMail(DWORD dwServerID, void* pData, size_t stLen);

	/// purpose: 删除邮件消息处理					-- 来自场景服
	void onSceneDelMail(DWORD dwServerID, void* pData, size_t stLen);

	/// purpose: 设置邮件已读消息处理					-- 来自场景服
	void onSceneSetMailRead(DWORD dwServerID, void* pData, size_t stLen);

	/// purpose: 请求玩家邮件列表
	void onMsgRequestMailList(DWORD dwServerID, void* pData, size_t stLen);

	/// purpose: 刷新玩家邮件列表
	void onMsgRefreshMailList(DWORD dwServerID, void* pData, size_t stLen);

private:
	// purpose: 生成邮件ID
	LONGLONG CreateMailID();
	// purpose: 获取邮件自增index
	int getMailIndex();
	// purpose: 获取节点序号
	int getQueueIndex();
	// purpose: 获取节点数据
	SMailDBNodeData* getSendNodeData(int nQueueIndex);
	// purpose: 删除节点数据
	void removeSendNodeData(int nQueueIndex);
	/// purpose: 删除超时邮件数据
	void delOverTimeMailFromDB();
	/// purpose: 移除玩家邮件数据
	void removePlayerMail(DWORD dwPDBID);
	/// purpose: 查找玩家邮件数据
	CPlayerMail* findPlayerMail(DWORD dwPDBID);

	/// purpose: 发送邮件
	bool sendMailToDB(SMailSendData *pSendData, int nMailFillID = emMailFill_Begin, LPCSTR szParam = NULL);

	/// purpose: 添加邮件日志
	void recordSendMailLog(LONGLONG llMailID, SMailSendData *pSendData);

	/// purpose: 填充邮件内容
	bool fillMailContent(SMailSendData *pSendData, int nMailFillID, LPCSTR szParam);

private:
	int							m_MailIndex;						// 邮件自增index
	int							m_nQueueIndex;						// 节点序号
	list<SMailDelInfo>			m_DelMailList;						// 删除邮件列表
	list<SMailDBNodeData>		m_SendNodeList;						// 发送节点列表
    TMap_PlayerMail				m_PlayerMailList;					// 玩家邮件列表
};