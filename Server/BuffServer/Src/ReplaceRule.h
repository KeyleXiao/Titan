/*******************************************************************
** 文件名:	ReplaceRule.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/13  15:03
** 版  本:	1.0
** 描  述:	叠加规则
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
/*
0：如果已经存在相同ID，并且产生者是同一个人，并且级别比待加BUFF高，则不能添加 
1：如果已经存在相同ID，并且级别比待加BUFF高，则不能添加 
2：如果已经存在相同ID，并且产生者是同一个人，并且级别比待加BUFF高或者相等，则不能添加
3：无论无何都不允许添加同一BUFF
//*/
#pragma once

#include "GameDef.h"
#include "IEntity.h"

class CReplaceRule
{
public:
	/** 此方法检测是否能够添加
    @param   pEntity ：实体指针
	@param   dwRule ：规则
	@param   dwBuffID ：buffID
	@param   dwBuffLevel ：buff等级
	@param   uidAdd ：添加者
	@return  
	*/
	bool		CanAdd(__IEntity *pEntity, DWORD dwRule, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);

	/** 
	@param   
	@param   
	@return  
	*/
	CReplaceRule(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CReplaceRule(void);

private:
	/** 0：如果已经存在相同ID，并且产生者是同一个人，并且级别比待加BUFF高，则不能添加
	@param   
	@param   
	@return  
	*/
	bool		CanAdd0(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);

	/** 1：如果已经存在相同ID，并且级别比待加BUFF高，则不能添加 
	@param   
	@param   
	@return  
	*/
	bool		CanAdd1(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);

	/** 2：如果已经存在相同ID，并且产生者是同一个人，并且级别比待加BUFF高或者相等，则不能添加
	@param   
	@param   
	@return  
	*/
	bool		CanAdd2(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);

	/** 3：无论无何都不允许添加同一BUFF
	@param   
	@param   
	@return  
	*/
	bool		CanAdd3(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);
};