/*******************************************************************
** 文件名:	EndConditionAttack.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/15  10:30
** 版  本:	1.0
** 描  述:	无蓄气主动攻击后取消
** 应  用:  	
	
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
class CEndConditionAttack : public IEventExecuteSink
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

	/////////////////////////CEndConditionDie//////////////////////////
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
	CEndConditionAttack(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CEndConditionAttack(void);

private:
	// 主人
	CBuffPart *			m_pBuffPart;

	// BUFF
	CBuff *				m_pBuff;

	// 是否创建
	bool				m_bCreate;
};