/*******************************************************************
** 文件名:	EndConditionInjuredByActor.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/09/2015
** 版  本:	1.0
** 描  述:	被英雄攻击取消buff
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "IEventEngine.h"

// 申明
class CBuff;
class CBuffPart;

// 定义
class CEndConditionInjuredByActor : public IEventExecuteSink
{
public:
	///////////////////////////IEventVoteSink//////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	////////////////////// CEndConditionInjuredByActor ///////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	bool				Create(CBuffPart * pBuffPart, CBuff * pBuff);

	/** 
	@param   
	@param   
	@return  
	*/
	void				Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CEndConditionInjuredByActor(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CEndConditionInjuredByActor(void);

private:
	// 主人
	CBuffPart *			m_pBuffPart;

	// BUFF
	CBuff *				m_pBuff;

	// 是否创建
	bool				m_bCreate;
};