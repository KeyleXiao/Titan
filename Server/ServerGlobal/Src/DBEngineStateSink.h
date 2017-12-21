/*******************************************************************
** 文件名:	e:\Rocket\Server\ZoneServer\Src\DBEngineStateSink.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/3/14  21:59
** 版  本:	1.0
** 描  述:	数据库状态信息回调接口
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IDBEngine.h"

class CDBEngineStateSink : public IDBEngineStateSink
{
public:
	///////////////////////////////////////////////////////////////////
	/** 如果数据引擎有状态信息回调给上层
	@param   nInforID：信息ID，参考上层定义
	@param   pszDesc：信息描述
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void			OnState(int nInfoID, LPCSTR pszDesc);

	///////////////////////////////////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	CDBEngineStateSink(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CDBEngineStateSink(void);
};
