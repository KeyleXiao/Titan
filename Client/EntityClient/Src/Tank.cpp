/*******************************************************************
** 文件名:	Tank.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			怪物/NPC对象
********************************************************************/


#include "stdafx.h"
#include "IClientGlobal.h"
#include "ITankClient.h"
#include "ITank.h"
#include "Tank.h"
#include "TankProperty.h"
#include "ICamp.h"

IEntityPart * Tank::createEntityPart( ENTITY_PART id )
{
    switch ( id )
    {
    case PART_PROPERTY:
        return new EntityPart_TankProperty;
    case PART_TANK_BOX:
        return gClientGlobal->getTankClient()->CreateTankBoxPart();
    default:
        return EntityBase::createEntityPart( id );
    }
}

/** 获取实体显示层的基础信息(用于createView)
@name				: 
@param				: 
@return
*/
bool Tank::getBasicViewInfo( EntityViewItem & item )
{
    const STankSchemeInfo * pScheme = GetTankSchemeInfo();
    if (NULL==pScheme )
    {
        ErrorLn("Don't find the Tank["<< getName() <<"] config data!");
        return false;
    }

    ZeroMemory( &item,sizeof(EntityViewItem) );

    item.EntityID   = getUID();
    item.nSkinID    = getProperty()->getProperty_Integer(PROPERTY_SKIN);
    item.EntityType = ENTITY_TYPE::TYPE_TANK;
    item.byCamp		= getProperty()->getProperty_Integer( PROPERTY_CAMP );
    item.byCampFlag	= Camp::getCampFlag( gClientGlobal->getHero(),this );
    item.byIsHero	= 0;
	item.nMaskFlag	= MASKFLAG_NORMAL;		// 实体标记 ENTITY_MASKFLAG	
    item.nSight = pScheme->fEyeshot;

    sstrcpyn( item.szName,pScheme->szName,sizeof(item.szName) );

    return true;
}

/** 取得怪物配置数据
@param   
@param   
@return  
*/
STankSchemeInfo * Tank::GetTankSchemeInfo(void)
{
	return m_pTankSchemeInfo;
}

/////////IClientEntity//////////////////////////////////////////////////////
// 取得数值属性
int Tank::getIntProperty(int nPropID)
{
	IEntityProperty * pPropertyPart = getProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_Integer(nPropID) : 0);
}

// 取得字符串属性
char* Tank::getStringProperty(int nPropID)
{
	IEntityProperty * pPropertyPart = getProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_String(nPropID) : "");
}

// 取得来源游戏世界ID
int Tank::getFromGameWorldID()
{
	// todo
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/** 
@param   
@param   
@return  
*/
LPCSTR Tank::GetName(void)
{
	if (m_pTankSchemeInfo == NULL)
	{
		return "";
	}

	return m_pTankSchemeInfo->szName;
}

/** 
	@param   
	@param   
	@return  
	*/
DWORD Tank::GetMaxSkillIndex(void)
{
	if (IsPKTank())
		return TANK_MAX_PK_SKILL_INDEX;
	else
		return TANK_MAX_SKILL_INDEX;
}

/** 取技能ID
	@param   
	@param   
	@return  
	*/
DWORD Tank:: GetSkillIDByIndex(BYTE nSkillIndex)
{
	if (nSkillIndex==0)
		return 0;

	if (IsPKTank())
	{
		std::map<int, ushort>::iterator MainIDIt = m_mPKSkillMainID.find(nSkillIndex-1);
		if (MainIDIt == m_mPKSkillMainID.end())
		return MainIDIt->second;
	}

	STankSchemeInfo * pTankSchemeInfo = GetTankSchemeInfo();
	if (pTankSchemeInfo==NULL)
	{
		return 0;
	}
	long lSkillIDList[TANK_MAX_SKILL_INDEX];
	/*lSkillIDList[0] = pTankSchemeInfo->lUsableSkill1[0];
	lSkillIDList[1] = pTankSchemeInfo->lUsableSkill2[0];
	lSkillIDList[2] = pTankSchemeInfo->lUsableSkill3[0];
	lSkillIDList[3] = pTankSchemeInfo->lUsableSkill4[0];
	lSkillIDList[4] = pTankSchemeInfo->lUsableSkill5[0];*/

	return lSkillIDList[nSkillIndex-1];
}


/** 是否是PK载具
	@param   
	@param   
	@return  
	*/
bool Tank::IsPKTank()
{
	STankSchemeInfo * pTankSchemeInfo = GetTankSchemeInfo();
	if (pTankSchemeInfo==NULL)
	{
		return false;
	}
	return true;
}

/** 设置pk载具技能
@param   
@param   
@return  
*/
void Tank::SetPKTankSkill(int nPostion, long nSkillID)
{
	ushort mainID = (ushort)(nSkillID);
	if (mainID == 0)
	{
		return;
	}

	m_mPKSkillMainID[nPostion] = mainID;
}

/** 设置pk载具技能mainID
@param   
@param   
@return  
*/
void Tank::SetPKTankSkillMainID(int nPostion, ushort nSkillMainID)
{
	m_mPKSkillMainID[nPostion] = nSkillMainID;
}

/** 检测是否存在PK技能
@param   
@param   
@return  
*/
bool Tank::HavePKTankSkill(long nSkillID)
{
	std::map<int, ushort>::iterator iter = m_mPKSkillMainID.begin();
	for (;iter != m_mPKSkillMainID.end(); ++iter)
	{
		if (nSkillID == iter->second)
		{
			return true;
		}
	}

	return false;
}


/** 取指定主位置的SkillID
@param   
@param   
@return  
*/
int Tank::GetPKTankSkillByPostion(int nPostion)
{
	std::map<int, ushort>::iterator mainIDIt = m_mPKSkillMainID.find(nPostion);
	if (mainIDIt == m_mPKSkillMainID.end())
	{
		return 0;
	}

	return mainIDIt->second;
}

/** 清除mianid列表
@param   
@param   
@return  
*/
void Tank:: ClearPKTankMainID()
{
	m_mPKSkillMainID.clear();
}


/** 取PK载具技能个数
@return  
*/
int Tank::GetPKSkillNum()
{
	int nSkillNum = (int)m_mPKSkillMainID.size();
	if (nSkillNum<=0)
	{
		ErrorLn("载具里面没有有效技能")
	}
	return nSkillNum;
}

/** 判断是否能随时跳出载具
@name				: 
@param				: 
@return	
*/
bool Tank::CanJumpOut()
{
	return true;
}

/** 队友在此载具中时, 左键头像点击直接选中载具
@name				: 
@param				: 
@return
*/
bool Tank::IsSelectByFace()
{
	return true;
}