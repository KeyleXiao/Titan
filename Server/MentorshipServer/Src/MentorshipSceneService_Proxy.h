/*******************************************************************
** 文件名:	MentorshipSceneService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/29/2016
** 版  本:	1.0
** 描  述:	师徒系统场景代理
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "buffer.h"
#include "IMentorshipSceneService.h"
#include "MentorshipSceneService.h"

using namespace rkt;

class MentorshipSceneService_Proxy : public IMentorshipSceneService
{
public:
	//IMentorshipSceneService///////////////////////////////////////////////////////////////////////
	
    // 处理其它服务器发送过来的消息
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void *data, size_t len);

    // 处理客户端发送过来的消息
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void *data, size_t len);

	// 发送战斗结果
	virtual void	sendFightResult(DWORD dwPDBID, bool bFightResult, bool bFirstWin, BYTE nMentorshipPost);

	// 获取师徒职位
	virtual BYTE	getMyMentorshipPost(DWORD dwSrcPDBID, DWORD dwDesPDBID);

	// 获取师徒奖励
	virtual void	getMentorshipAward(DWORD dwPDBID, int* nExpPercent, int* nHeroExpPercent, int* nGoldPercent);

	// 获取师傅个数
	virtual int getPlayerMasterCount(DWORD dwDBID);

	// 获取学徒个数
	virtual int getPlayerPrenticeCount(DWORD dwDBID);

	// 检测目标是否为玩家徒弟
	virtual int checkIsPlayerPrentice(DWORD dwDBID, DWORD dwTargetDBID);

    MentorshipSceneService_Proxy() : m_pContainer(0)
    {

    }

public:
	SERVICE_PTR    m_pContainer;

};
