/*******************************************************************
** 文件名:	AntiAddictServer.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/8/2016
** 版  本:	1.0
** 描  述:	防沉迷服务

********************************************************************/

#pragma once

#include "TimerHandler.h"
#include "IEventEngine.h"
#include "AntiAddictDef.h"
#include "IMessageDispatch.h"
#include "IDBEngineService.h"
#include "IAntiAddictServer.h"
#include "lockfree/hashmap.h"

class CPupil;


// 防沉迷服务器类
class CAntiAddictServer : public IAntiAddictServer, public ITransmitHandler, public IMessageHandler, public IEventExecuteSink, public IDBRetSink
{
    // 人物UID列表
    typedef list<UID>   UID_LIST;
    typedef DWORD       IDCARD;

public:
	/** 构造函数
	*/
	CAntiAddictServer(void);

	/** 析构函数
	*/
	virtual ~CAntiAddictServer(void);

	/** 创建
	*/
	bool                    create(void);

	/** 释放服务
	*/
    virtual void			onstop(void);

	/** 释放对象
	*/
	virtual void            release(void);

	/** 取得玩家的衰减比例
	@param uid: 角色uid
	*/
    virtual int             getReduceRate(UID uid, int nPlayLevel=0);

	/** 设置防沉迷等级
	*/
    virtual void            setReduceLevel(int nReduceLevel);

	/** 查看防沉迷等级
	*/
    virtual int            getReduceLevel();

    /** 玩家下线事件处理
    */
    virtual void           onActorLogout(PDBID pdbid, UID uid, DWORD dwUserID, bool bAudlt, DWORD nIdCard);

	/////////////////////////////// ITransmitHandler ////////////////////////////////////
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

	/////////////////////////////// IMessageHandler ////////////////////////////////////
	/** 消息处理
	@param msg 网络消息
	*/
	virtual void	        onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

	/////////////////////////////// IEventExecuteSink ////////////////////////////////////
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
	virtual void			OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    /////////////////////////////// IDBRetSink ////////////////////////////////////
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
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
	/** 玩家登陆事件处理
	*/
	void                    onEventLogin(DWORD dwSrcID, LPCSTR pData, int nLen);

    /** 玩家身份认证数据库回调
	*/
	void                    onReturnActorAuth(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	/** 获取未成年对象
	*/
    CPupil*                 getPupil(IDCARD dwIdCard);

	/** 将用户身份证信息插入到队列中
	*/
    bool                    addPupilCardInfo(IDCARD dwIdCard, DWORD dwPDBID, UID uid);

	/** 将用户身份证信息从队列中移除
	*/
    void                    removePupilCardInfo(IDCARD dwIdCard);

	/** 更新用户身份证信息
	*/
    void                    updatePupilCardInfo(IDCARD dwOldIdCard, IDCARD dwNewIdCard);

	/** 删除未成年沉迷信息
	*/
    bool                    removeAddictInfo(IDCARD dwIdCard);

	/** 更新未成年身份证信息
	*/
    bool                    reqUpdateIdCardInfo(IDCARD dwOldIdCard, DWORD dwNewIdCard);

	/** 发送消息到世界服
	*/
    void                    sendToWorldSvr(BYTE byAction, char* pData, size_t nSize);

	/** 刷新玩家反沉迷信息
	*/
	void                    onTransmitUpdateAddictInfo(void * data, size_t len);

	/** 批量刷新玩家反沉迷信息(新加入场景服被同步全部的防沉迷信息)
	*/
	void                    onTransmitBatchUpdateAddictInfo(void * data, size_t len);

	/** 移除玩家反沉迷信息
	*/
	void                    onTransmitRemovePupil(void * data, size_t len);

	/** 更新玩家身份证信息
	*/
	void                    onTransmitUpdateIdCard(void * data, size_t len);

	/** 更新玩家实名认证状态
	*/
	void                    onTransmitUpdateRealNameAuther(void * data, size_t len);

	/** 游戏中玩家身份证认证处理
	*/
	void                    onMessageAuthPupil(ClientID clientID, SNetMsgHead* head, void* data, size_t len);

	/** 取得IP
	*/
    string                  getIpAddress(ClientID clientID);

	/** 发送客户端认证结果
	*/
    void					sendToClientAuthResult(ClientID clientID, int nDBRetCode, char * pszDBRetDesc);

	/** 更新实名认证信息
	*/
    bool                    reqUpdateRealNameAuther(IDCARD dwIdCard);

    /** 从容器中删除IDCARD 
    */ 
    void                    removeContainerIDCard(IDCARD card);


private:
    // 防沉迷等级
    int m_nReduceLevel;

    //用于记录身份证为索引，关联其值的所有的用户对象
    speed::hashmap<IDCARD, CPupil*>   m_mapPupilIdCard;
};


extern CAntiAddictServer*	g_pAntiAddictServer;