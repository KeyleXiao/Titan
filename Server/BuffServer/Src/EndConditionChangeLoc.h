/*******************************************************************
** 文件名:	EndConditionChangeLoc.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/15  10:37
** 版  本:	1.0
** 描  述:	改变位置后取消，包括移动，瞬移，切换地图
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "IEventEngine.h"
#include "Vector3.h"

// 申明
class CBuff;
class CBuffPart;

// 定义
class CEndConditionChangeLoc : public IEventExecuteSink
{
public:
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
	CEndConditionChangeLoc(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CEndConditionChangeLoc(void);

private:
	// 主人
	CBuffPart *			m_pBuffPart;

	// BUFF
	CBuff *				m_pBuff;

	// 是否创建
	bool				m_bCreate;

    // 位置点
    Vector3             m_vLoc;
};