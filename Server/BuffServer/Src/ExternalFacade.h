/*******************************************************************
** 文件名:	ExternalFacade.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/13  11:36
** 版  本:	1.0
** 描  述:	
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "TimerAxis.h"
#include "ISchemeCenter.h"
#include "IEffect.h"
#include "IActorService.h"
#include "BuffDef.h"

// 定义
class CExternalFacade
{
public:
	/** 
	*/
	CExternalFacade(void);

	/** 
	*/
	~CExternalFacade(void);

	/** 启用定时器
	*/ 
	bool					SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes = INFINITY_CALL);

	/** 销毁定时器
	*/
	bool					KillTimer(DWORD timerID, ITimerHandler * handler);

	/** 创建现场
	*/
	IEffect *				CreateEffect(DWORD dwEffectID);

	/** 
	*/
	bool					IsExistEffectID(int nEffectID);

    /** 英雄发送数据到客户端
	*/
	void                    playerSend2Client(__IEntity *pEntity, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);
	
    /** 发送数据
	*/
	bool					broadCastInAOI(__IEntity *pEntity, Vector3 vLoc, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

	// 广播数据给所有人
	bool					broadcastDataToAllPlayer(__IEntity *pMaster, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

	// 发送数据给某个玩家的客户端
	void					playerSend2OtherClient(UID curUID, UID otherUID, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);
};
extern CExternalFacade				g_ExternalFacade;