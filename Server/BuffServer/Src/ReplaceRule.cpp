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
#include "StdAfx.h"
#include "ReplaceRule.h"
#include "ExternalFacade.h"
#include "IBuffServer.h"
#include "BuffPart.h"
#include "EntityHelper.h"

/** 
@param   
@param   
@return  
*/
CReplaceRule::CReplaceRule(void)
{

}

/** 
@param   
@param   
@return  
*/
CReplaceRule::~CReplaceRule(void)
{

}

/** 此方法检测是否能够添加
@param   dwRule ：规则
@param   dwBuffID ：buffID
@param   dwBuffLevel ：buff等级
@param   uidAdd ：添加者
@param   uidEffect ：作用者
@return  
*/
typedef bool (CReplaceRule::* CANADD_PROC)(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);
bool CReplaceRule::CanAdd(__IEntity *pEntity, DWORD dwRule, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
{
	static CANADD_PROC canaddproc[4] =
	{		
		(CANADD_PROC)&CReplaceRule::CanAdd0,
		(CANADD_PROC)&CReplaceRule::CanAdd1,
		(CANADD_PROC)&CReplaceRule::CanAdd2,
		(CANADD_PROC)&CReplaceRule::CanAdd3,
	};

	if(dwRule < 0 || dwRule >= 4|| canaddproc[dwRule] == NULL)
	{		
		return false;
	}

	return (this->*canaddproc[dwRule])(pEntity, dwBuffID, dwBuffLevel, uidAdd);
}
/** 0：如果已经存在相同ID，并且产生者是同一个人，并且级别比待加BUFF高，则不能添加
@param   
@param   
@return  
*/
bool CReplaceRule::CanAdd0(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
{
    if (pEntity == NULL)
    {
        return false;
    }

    __IBuffPart *pBuffPart = (__IBuffPart *)pEntity->getEntityPart(PART_BUFF);
    if (pBuffPart == NULL)
    {
        return false;
    }

	DWORD dwBuffLev = pBuffPart->GetBuffLevel(dwBuffID, uidAdd);
	if (dwBuffLev > 0 && dwBuffLev > dwBuffLevel)
	{
		return false;
	}	
	return true;
}

/** 1：如果已经存在相同ID，并且级别比待加BUFF高，则不能添加 
@param   
@param   
@return  
*/
bool CReplaceRule::CanAdd1(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
{
    if (pEntity == NULL)
    {
        return false;
    }

    __IBuffPart *pBuffPart = (__IBuffPart *)pEntity->getEntityPart(PART_BUFF);
    if (pBuffPart == NULL)
    {
        return false;
    }

	DWORD dwBuffLev = pBuffPart->GetBuffLevel(dwBuffID, INVALID_UID);
	if (dwBuffLev > 0 && dwBuffLev > dwBuffLevel)
	{
		return false;
	}
	return true;
}

/** 2：如果已经存在相同ID，并且产生者是同一个人，并且级别比待加BUFF高或者相等，则不能添加
@param   
@param   
@return  
*/
bool CReplaceRule::CanAdd2(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
{
    if (pEntity == NULL)
    {
        return false;
    }

    __IBuffPart *pBuffPart = (__IBuffPart *)pEntity->getEntityPart(PART_BUFF);
    if (pBuffPart == NULL)
    {
        return false;
    }

	DWORD dwBuffLev = pBuffPart->GetBuffLevel(dwBuffID, uidAdd);
	if (dwBuffLev > 0 && dwBuffLev >= dwBuffLevel)
	{
		return false;
	}	
	return true;
}

/** 3：无论无何都不允许添加同一BUFF
@param   
@param   
@return  
*/
bool CReplaceRule::CanAdd3(__IEntity *pEntity, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
{
    if (pEntity == NULL)
    {
        return false;
    }

    __IBuffPart *pBuffPart = (__IBuffPart *)pEntity->getEntityPart(PART_BUFF);
    if (pBuffPart == NULL)
    {
        return false;
    }

	if (pBuffPart->IsExist(dwBuffID, 0, INVALID_UID))
	{
		return false;
	}	
	return true;
}
