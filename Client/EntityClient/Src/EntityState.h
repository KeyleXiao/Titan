/*******************************************************************
** 文件名:	EntityState.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-3-13
** 版  本:	1.0
** 描  述:	生物状态接口
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


// 定义
class CEntityState
{
public:
	/** 创建
	@return  
	*/ 
	virtual bool				create(IEntity * pMaster)
	{
		return false;
	}

	/** 取得本状态
	*/
	virtual DWORD				getState(void)	
	{
		return ENTITY_STATE_UNKNOW;
	}

	/** 消息过滤
	@return  
	*/ 
	virtual bool				onMessage(BYTE byPartID, int msgId,  void * data, size_t len)
	{
		return true;
	}

	/** 启动状态
	@return  
	*/	
	virtual void				start(void)
	{

	}

	/** 结束状态
	@return  
	*/	
	virtual void				end(void)
	{

	}

	/** 构造函数
	@return  
	*/
	CEntityState(void)
	{
		// 主人
		m_pMaster = NULL;
	}

	/** 析构函数
	@return  
	*/
	virtual ~CEntityState(void)
	{
		// 主人
		m_pMaster = NULL;
	}	

protected:
	IEntity*				m_pMaster;			// 实体对象
};