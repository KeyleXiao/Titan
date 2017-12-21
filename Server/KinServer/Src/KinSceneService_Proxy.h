/*******************************************************************
** 文件名:	E:\speed\Server\KinServer\Src\KinSceneService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	战队 代理
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "IKinSceneService.h"
#include "KinSceneService.h"


using namespace rkt;


class KinSceneService_Proxy : public IKinSceneService
{
public:
	/////////////////////////////IKinSceneService/////////////////////////////////////////////
	//virtual void start();
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	// purpose: 发送社会服消息
	virtual void sendToSocial(BYTE nMsgID, void * data, size_t len);

    // purpose: 根据成员的PDBID查找战队成员
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
	* @param dwKinID		:战队ID
	*/
	virtual int getWeekActivity(DWORD dwKinID);

public:
	SERVICE_PTR    m_pContainer;
};
