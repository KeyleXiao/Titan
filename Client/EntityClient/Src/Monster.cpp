/*******************************************************************
** 文件名:	Monster.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			怪物/NPC对象
********************************************************************/

#include "stdafx.h"
#include "CommonStructDef.h"
#include "ISchemeCenter.h"
#include "MonsterProperty.h"
#include "Monster.h"
#include "ICamp.h"
#include "IWarClient.h"

IEntityPart * Monster::createEntityPart( ENTITY_PART id )
{
    switch ( id )
    {
    case PART_PROPERTY:
        return new EntityPart_MonsterProperty;
    case PART_WAR:
        {
            IWarClient *pWarClient = gClientGlobal->getWarClient();
            return pWarClient!=NULL? pWarClient->CreatePart() : NULL;
        }

    default:
        return EntityBase::createEntityPart( id );
    }
}

/** 获取实体显示层的基础信息(用于createView)
@name				: 
@param				: 
@return
*/
bool Monster::getBasicViewInfo( EntityViewItem & item )
{
    const SMonsterScheme* pScheme = getMonsterScheme();
    if ( NULL==pScheme )
    {
        ErrorLn("Don't find the monster["<< getName() <<"] config data!");
        return false;
    }

    ZeroMemory( &item,sizeof(EntityViewItem) );

    // 怪物子类型 MONSTER_SUB_TYPE
    int nSubType = getProperty()->getProperty_Integer(PROPERTY_VOCATION);

    item.EntityID   = getUID();
    // 只需要传递实体类型、默认皮肤ID、名字即可
    item.nSkinID    = pScheme->nDefaultSkin;
    item.EntityType = ENTITY_TYPE::TYPE_MONSTER;
    item.byCamp     = getProperty()->getProperty_Integer( PROPERTY_CAMP );	// 阵营
    // 中立阵营的NPC不应当设置为敌方NPC,依旧保留为中立NPC属性
    if ( CAMP_NEUTRAL == item.byCamp && MONSTER_SUB_TYPE_NPC == nSubType)
    {
        item.byCampFlag = CampFlag_Neutral;
    }
    else
    {
        item.byCampFlag = Camp::getCampFlag(gClientGlobal->getHero(), this);
    }
    item.byIsHero   = 0;
    item.nHeroID = getProperty()->getProperty_Integer(PROPERTY_ID); // 怪物ID
	item.nMaskFlag	= getProperty()->getProperty_Integer(PROPERTY_MASK);	// 实体标记 ENTITY_MASKFLAG
	item.nBossBloodPriority = pScheme->nBossBloodShowPriority;
	item.nBossBloodShowDistance = pScheme->nBossBloodShowDistance;
	item.nBloodStyle = pScheme->nBloodStyle;
    sstrcpyn( item.szName,pScheme->szName,sizeof(item.szName) );
    item.nMinimapIcon = pScheme->nMinimapIconID;
    item.nSight = pScheme->fEyeshot;

    item.ndata1 = nSubType;       // 怪物子类型 MONSTER_SUB_TYPE

    return true;
}

const SMonsterScheme* Monster::getMonsterScheme()
{
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return NULL;
	}

	ISchemeMonster* pScheme = pCenter->getSchemeMonster();
	if ( NULL==pScheme )
	{
		return  NULL;
	}

    int nMonsterID = getProperty()->getProperty_Integer(PROPERTY_ID);
	return pScheme->getMonsterShemeInfo(nMonsterID);
}
/////////IClientEntity//////////////////////////////////////////////////////
// 取得数值属性
int Monster::getIntProperty(int nPropID)
{
	IEntityProperty * pPropertyPart = getProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_Integer(nPropID) : 0);
}

// 取得字符串属性
char* Monster::getStringProperty(int nPropID)
{
	IEntityProperty * pPropertyPart = getProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_String(nPropID) : "");
}

// 取得来源游戏世界ID
int Monster::getFromGameWorldID()
{
	// todo
	return 0;
}
