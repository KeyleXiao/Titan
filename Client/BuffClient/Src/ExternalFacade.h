/*******************************************************************
** 文件名:	ExternalFacade.h
** 版  权:	(C) 深圳未名网络技术有限公司(www.weimingtech.com)
** 创建人:	隆寒辉(Forlion Lon)
** 日  期:	2008/1/23  10:31
** 版  本:	1.0
** 描  述:	接口外观类
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "TimerAxis.h"
#include "IEffect.h"
#include "IBuffClient.h"

// 申明
namespace rkt
{
	struct IEventEngine;
	struct ISchemeEngine;
}

// 定义
class CExternalFacade
{
public:
	/** 启用定时器
	@param   
	@param   
	@return  
	*/ 
	virtual bool					SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes = INFINITY_CALL);

	/** 销毁定时器
	@param   
	@param   
	@return  
	*/
	virtual bool					KillTimer(DWORD timerID, ITimerHandler * handler);

	/** 取得事件引擎
	@param   
	@param   
	@return  
	*/
	virtual IEventEngine *			GetEventEngine(void);

	/** 取得配置引擎
	@param   
	@param   
	@return  
	*/
	virtual ISchemeEngine *			GetSchemeEngine(void);

	/** 取得效果客户端
	@param   
	@param   
	@return  
	*/
	virtual IEffectFactory *		GetEffectFactory(void);

	/** 取得BUFF客户端
	@param   
	@param   
	@return  
	*/
	virtual IBuffClient *			GetBuffClient(void);

	/** 发送事件，包括投票事件和执行事件一起发
	@param   
	@param   
	@return  
	*/
	virtual void					FireEvent(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/** 发送否决事件
	@param   
	@param   
	@return  
	*/
	virtual bool					FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual bool					IsExistEffectID(int nEffectID);

	/** 
	@param   
	@param   
	@return  
	*/
	CExternalFacade(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CExternalFacade(void);
};
extern CExternalFacade			g_ExternalFacade;