/*******************************************************************
** 文件名:	BuffClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/16  10:58
** 版  本:	1.0
** 描  述:	BUFF 客户端
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IEntity.h"
#include "IBuffClient.h"
#include "SchemeBuff.h"

class CBuffClient : public IBuffClient
{
public:
	///////////////////////////////////////////////////////////////////
	/** 创建
	*/
	virtual bool			create();

	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     释放所有资源，并且销毁此对像
	@warning 
	@retval buffer 
	*/
	virtual void			release(void);

	/** 给实体创建buff部件，重新new的部件才调此方法。如果是回收的，则不需要
	@param   
	@param   
	@param   
	@return   成功: 非NULL,失败: NULL
	@note     
	@warning 
	@retval buffer 
	*/
	virtual IBuffPart *		CreatePart(void);

	/** 取得某个buff的描述
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetBuffDesc(int nBuffID, int nBuffLevel);

	/** 取得buff名字
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetBuffName(DWORD dwBuffID);

	/** 取得buff图标ID
	@param dwBuffID: BUFF的ID   
	@return  
	*/
	virtual DWORD			GetBuffIconID(DWORD dwBuffID);

	///////////////////////////////////////////////////////////////////
	/** 取得BUFF配置信息
	@param   
	@param   
	@return  
	*/
	SBuffSchemeInfo *		GetBuffSchemeInfo(DWORD dwBuffID);

	/** 
	@param   
	@param   
	@return  
	*/
	CBuffClient(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CBuffClient(void);

private:
	// buff配置
	CSchemeBuff				m_SchemeBuff;
};
extern CBuffClient *		g_pBuffClient;