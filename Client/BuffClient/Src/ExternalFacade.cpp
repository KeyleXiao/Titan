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
#include "StdAfx.h"
#include "ExternalFacade.h"
#include "IClientGlobal.h"

CExternalFacade	g_ExternalFacade;
/** 
@param   
@param   
@return  
*/
CExternalFacade::CExternalFacade(void)
{

}

/** 
@param   
@param   
@return  
*/
CExternalFacade::~CExternalFacade(void)
{

}

/** 启用定时器
@param   
@param   
@return  
*/ 
bool CExternalFacade::SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo, DWORD callTimes)
{
	TimerAxis * pTimerAxis = gClientGlobal->getTimerAxis();
	if(pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->SetTimer(timerID, interval, handler, callTimes, debugInfo);
}

/** 销毁定时器
@param   
@param   
@return  
*/
bool CExternalFacade::KillTimer(DWORD timerID, ITimerHandler * handler)
{
	TimerAxis * pTimerAxis = gClientGlobal->getTimerAxis();
	if(pTimerAxis == NULL)
	{
		return false;
	}

	return pTimerAxis->KillTimer(timerID, handler);
}

/** 取得事件引擎
@param   
@param   
@return  
*/
IEventEngine * CExternalFacade::GetEventEngine(void)
{
	return gClientGlobal->getEventEngine();;
}

/** 取得配置引擎
@param   
@param   
@return  
*/
ISchemeEngine * CExternalFacade::GetSchemeEngine(void)
{
	return gClientGlobal->getSchemeEngine();
}

/** 取得效果客户端
@param   
@param   
@return  
*/
IEffectFactory *	CExternalFacade::GetEffectFactory(void)
{
	return gClientGlobal->getEffectFactory();
}

/** 取得BUFF客户端
@param   
@param   
@return  
*/
IBuffClient * CExternalFacade::GetBuffClient(void)
{
	return gClientGlobal->getBuffClient();
}

/** 发送事件，包括投票事件和执行事件一起发
@param   
@param   
@return  
*/
void CExternalFacade::FireEvent(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	IEventEngine * pEventEngine = GetEventEngine();
	if(pEventEngine == NULL)
	{
		return;
	}

	// 发送投票事件，
	if(!pEventEngine->FireVote(wEventID, bSrcType, dwSrcID, pszContext, nLen))
	{
		return;
	}

	// 发送执行事件
	pEventEngine->FireExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen);
}

/** 发送否决事件
@param   
@param   
@return  
*/
bool CExternalFacade::FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	IEventEngine * pEventEngine = GetEventEngine();
	if(pEventEngine == NULL)
	{
		return false;
	}

	return pEventEngine->FireVote(wEventID, bSrcType, dwSrcID, pszContext, nLen);
}

/** 
@param   
@param   
@return  
*/
bool CExternalFacade::IsExistEffectID(int nEffectID)
{
	IEffectFactory * pEffectFactory = GetEffectFactory();
	if(pEffectFactory == NULL)
	{
	return false;
	}

	return pEffectFactory->IsExistEffectID(nEffectID);
}