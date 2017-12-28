/*******************************************************************
** 文件名:	E:\speed\Server\KinServer\Include\IKinSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	聊天系统场景服 服务提供的接口
            
********************************************************************/

#pragma once

#include "GatewayServiceDef.h"
#include "PackagePtr.h"
#include "IKinScene.h"
#include "KinDef.h"
using Gateway::ClientID;

struct  IKinSceneService
{
	//virtual void start() = 0;

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len) = 0;

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len) = 0;

	// purpose: 发送社会服消息
	virtual void sendToSocial(BYTE nMsgID, void * data, size_t len) = 0;

    // purpose: 根据成员的PDBID查找战队成员
    virtual SSceneKinMember getKinMember(DWORD dwPDBID) = 0;

	///** 调用DB存储过程创建战队
	//@param	pdbid		: 玩家角色ID
	//@param	szKinName	: 战队名
	//@param	bCrtAsFormal: 是否创建为正式战队
	//@return
	//*/
	//virtual void DBProcCreateKin(DWORD pdbid, const char* szKinName, bool bCrtAsFormal = false) = 0;

    /** 根据战队ID获取Kin数据
	@param dwKinID		:战队ID
	*/
	virtual SKinInfo getKinInfo(DWORD dwKinID) = 0;

    /** 根据战队ID获取成员列表接口
	@param dwKinID		:战队ID 
    @return obuf (都会有第一个DWORD 来保存成员数量 >= 0)
	*/
	virtual DWORD   getKinMemberList(DWORD dwKinID,PDBID* pReturnArray, DWORD dwArrayMaxSize) = 0;

    /** 获取战队成员数量
	@param dwKinID		:战队ID 
	*/
	virtual DWORD   getKinMemberCount(DWORD dwKinID) = 0;

	/** 设置周活跃度
	* @param dwKinID		:战队ID
	* @param nValue		    :周活跃度值
	*/
	virtual bool setWeekActivity(DWORD dwKinID, int nValue) = 0;

	/** 获取周活跃度
	* @param dwKinID		:战队ID
	*/
	virtual int getWeekActivity(DWORD dwKinID) = 0;
};
