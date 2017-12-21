/*******************************************************************
** 文件名:	DBHttpService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2010 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#include "IUrlResponse.h"
#include "IDBHttpService.h"
#include "IActionway.h"
#include "IDBHttpServiceProtocol.h"



/*
@brief: 该模块主要职责是接受DBHttpService的请求，处理消息。
*/
class DBHttpService : public IDBHttpService, public ITransmitHandler
{
    // 回调节点，以会话ID微索引
    typedef std::map<DWORD, IUrlResponse*> TMAP_UrlResponseNode;

public:
	///////////////////////IDBHttpService///////////////////////////////////////////////////
	virtual void release(void) override;

	virtual bool create(void) override;

    /** 执行Service的on_stop
    @return
    */
    virtual void onStop(void) override;

	/**
	@name    : 请求url调用（外部接口调用）
	@param const char* pUrl: 请求地址
	@param WORD wLen : 地址长度
	@param IUrlResponse* pUrlResponse : 回调者
	*/
	virtual void requestUrl(const char* pUrl, unsigned short nLen, IUrlResponse* pUrlResponse) override;

    ///////////////////////ITransmitHandler///////////////////////////////////////////////////
    /** 消息处理
    @param server	源服务器ID
    @param uMsgID	消息ID
    @param head		消息头
    @param data		消息
    @param len		消息长度
    */
    virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len) override;

    /**
    @name         : 通知服务器列表已经更新
    @brief        : 服务器列表更新只会在有服务器进入退出时触发
    @note         : 具体的列表调用ICenterServerConnector::GetServerList获得
    */
    virtual void	onServerListUpdated() override;

    /** 通知服务器信息更新
    @param nState		: SERVER_STATE
    @param pServerData	: ServerData
    @brief        : 例如服务器人数变化等等
    */
    virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData) override;


	//////////////////////////////////////////////////////////////////////////


	bool RegisterActionwayHandler(DWORD actionway, IActionway* handler);

	void UnRegisterActionwayHandler(DWORD actionway);

	void ClearParam(){memset(&m_msg,0,sizeof(m_msg));m_value = ""; m_PDBIDList.clear();m_valueParams.clear();}

	bool ParseParam(ibuffer& buf);


	/**
	@name    : DBHttp服务器转发后后台http请求结果到场景服
	@param 
	@param 
	*/
	void HandleDBHttpUrlResponse(void* data,size_t len);

	DBHttpService();

	~DBHttpService();

private:

    IActionway *   getActionway(BYTE nAction);

    void onTransmitHandleDBHttpRequest(void * data, size_t len);
    /*
    @brief: 数据库后台请求下线
    */
    void onTransmitHandleDBHttpKickout(void* data, size_t len);

private:
	IActionway  *                   m_ActionwayFun[DBHttpAction_NUM];

	SMsgDBHttp_Request              m_msg;
	std::string                     m_value;        //''value"值
	std::vector<std::string>        m_valueParams;  //"value"对应的参数
	std::vector<DWORD>              m_PDBIDList;    //玩家pdbid列表
	TMAP_UrlResponseNode            m_mapUrlResponseNodes;

	DWORD                           m_dwSession;
};