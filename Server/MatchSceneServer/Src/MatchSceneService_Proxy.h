/*******************************************************************
** 文件名:	E:\speed\Server\MatchSceneServer\Src\MatchSceneService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	14:43
** 版  本:	1.0
** 描  述:	
            比赛大厅场景服 代理
********************************************************************/

#pragma once


#include "IServiceContainer.h"
#include "buffer.h"
#include "IMatchSceneService.h"
#include "MatchSceneService.h"

using namespace rkt;

class MatchSceneService;

class MatchSceneService_Proxy : public IMatchSceneService
{
public:
    MatchSceneService_Proxy(MatchSceneService * pReal);
    ~MatchSceneService_Proxy();
	///////////////////////////////IMatchSceneService///////////////////////////////////////////
    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

	virtual void start();
	virtual void warEnd(DWORD dwWarID, LONGLONG llDBWarID);

	// 发送创建结果
	virtual void sendCreateMobaResult(DWORD dwRoomID, int nWarID, int nSceneID, bool bResult);
	
    // 设置玩家场景加载进度
    virtual void setClientSceneLoadingProgress(PDBID idActor, int nProgress, long lPing);

    // 设置战场流程结束 但是未释放
    virtual void setWarProcessEnd(int nWarID);

    virtual void setRuneTalentPageIndex(PDBID idActor,int nRunePageIndex,int nTalentPageIndex);

    virtual int  getTalentPageSelectIndex(PDBID idActor);

    virtual int  getRunePageSelectIndex(PDBID idActor);

    virtual void onUpdateRoomPlayerClient(PDBID idActor, ClientID client);

    // 主动退出战场
    virtual void accordLeaveWar(PDBID idActor);

    // 角色是否在战场
    virtual bool isActorInBattleField(PDBID idActor);

    virtual int getRoomID(PDBID idActor);

	// 设置战场结果
	virtual void setWarResult(SMsgSetWarResultData resultData);

    // 释放无效的玩家（包括电脑）
    virtual void warEndDestoryInvalidActor(int nWarID);

public:
	SERVICE_PTR    m_pContainer;

    MatchSceneService * m_pReal;
};
