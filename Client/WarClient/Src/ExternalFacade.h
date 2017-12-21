/*******************************************************************
** 文件名:	e:\Rocket\Client\EntityClient\Src\ExternalFacade.h
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

// 申明
namespace rkt
{
	struct IEventEngine;
	struct ISchemeEngine;
}

// 申明
struct IEffectClient;
struct IBuffClient;

// 定义
class CExternalFacade
{
public:
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

	/** 发送事件，包括投票事件和执行事件一起发
	@param   
	@param   
	@return  
	*/
	virtual void					FireEvent(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/** 取得实体对象
	@param   
	@param   
	@return  
	*/
	virtual IEntity*				GetEntity(UID uid);

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