/*******************************************************************
** 文件名:	e:\Rocket\Client\EntityClient\Src\ExternalFacade.h
** 版  权:	(C) 深圳未名网络技术有限公司(www.weimingtech.com)
** 创建人:	秦其勇
** 日  期:	2015/2/23  10:31
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "ExternalFacade.h"
#include "IClientEntity.h"

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


/** 取得实体对象
@param   
@param   
@return  
*/
IEntity* CExternalFacade::GetEntity(UID uid)
{
	// 获取实体工厂
	IEntityFactory *pEntityFactory = gClientGlobal->getEntityFactory();
	if (!pEntityFactory)
	{
		return NULL;
	}

	return pEntityFactory->getEntity(uid);
}