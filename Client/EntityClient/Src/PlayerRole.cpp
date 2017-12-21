/*******************************************************************
** 文件名:	PlayerRole.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家实体对象
********************************************************************/

#include "stdafx.h"
#include "PlayerRole.h"
#include "PlayerRoleProperty.h"
#include "IClientGlobal.h"
#include "CommonStructDef.h"
#include "ISchemeCenter.h"
#include "IWarClient.h"
#include "ITankClient.h"
#include "DragonBallPart.h"
#include "ICamp.h"
#include "IBankPart.h"
#include "IChatClient.h"
#include "PlayerBankPart.h"
#include "TitlePart.h"
#include "WingPart.h"
#include "CommonPart.h"
#include "TestModuleDef.h"
#include "IMiscManager.h"

IEntityPart * PlayerRole::createEntityPart( ENTITY_PART id )
{
    switch ( id )
    {
    case PART_PROPERTY:
        return new EntityPart_PlayerRoleProperty;
    case PART_DRAGONBALL:
#ifdef OPEN_TEST
		return new DragonBallPart;
#else
        return NULL;
#endif
	case PART_BANK:
		return new CPlayerBankPart;
    case PART_PERSON_TANK:
        {
            ITankClient *pTankClient = gClientGlobal->getTankClient();
            return pTankClient!=NULL? pTankClient->CreatePersonTankPart() : NULL;
        }
    case PART_WAR:
        {
            IWarClient *pWarClient = gClientGlobal->getWarClient();
            return pWarClient!=NULL? pWarClient->CreatePart() : NULL;
        }
	case PART_MOBAGOODS:
		{
			IWarClient *pWarClient = gClientGlobal->getWarClient();
			return pWarClient!=NULL? pWarClient->CreateMobaGoodsPart() : NULL;
		}
	case PART_CHAT:
		{
 			IChatClient* pChatClient = gClientGlobal->getChatClient();
 			return pChatClient != NULL ? pChatClient->CreatePart():NULL;
		}
	case PART_TITLE:
		{
			return new TitlePart;
		}
    case PART_WING:
        {
            return new WingPart;
        }
    case PART_COMMON:
        {
            return new CommonPart;
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
bool PlayerRole::getBasicViewInfo( EntityViewItem & item )
{
    const PersonSkinModelItem *pScheme = getPersonSkinModelItemScheme();
    if ( NULL==pScheme )
    {
        ErrorLn("Don't find the player["<< getName() <<"] config data!");
        return false;
    }

    IEntityProperty * pProperty = getProperty();
    if (NULL == pProperty)
    {
        ErrorLn("Don't find the player[" << getName() << "]'s Property object!");
        return false;
    }

    ZeroMemory( &item,sizeof(EntityViewItem) );

    int nCampFlag = m_bHero? CampFlag_Self : Camp::getCampFlag( gClientGlobal->getHero(),this );
    int nSkinID = pProperty->getProperty_Integer(PROPERTY_SKIN);

    item.EntityType = ENTITY_TYPE::TYPE_PLAYER_ROLE;
    item.EntityID   = getUID();
    item.nHeroID    = pProperty->getProperty_Integer(PROPERTY_VOCATION);
    item.nSkinID    = (nSkinID > 0 ? nSkinID : pScheme->nDefaultSkinID);
    item.bySexType  = pProperty->getProperty_Integer(PROPERTY_SEX);
    item.nLevel	    = pProperty->getProperty_Integer(PROPERTY_LEVEL);
    item.byCamp		= pProperty->getProperty_Integer( PROPERTY_CAMP );	// 阵营
    item.byCampFlag	= nCampFlag;
    item.byIsHero   = (int)m_bHero;
	item.nMaskFlag	= MASKFLAG_NORMAL;		// 实体标记 ENTITY_MASKFLAG	
    item.nMinimapIcon = pScheme->nMinimapIconID;
    item.nSight = pScheme->nSightArray[0];

    IMiscManager* pMiscMgr = gClientGlobal->getMiscManager();
    if(pMiscMgr != NULL)
    {
        int nMapID = pMiscMgr->getMapID();   
        if(nMapID >= 0 && nMapID < MAP_MAXCOUNT && pScheme->nSightArray[nMapID] != 0)
        {
            item.nSight = pScheme->nSightArray[nMapID];
        }
    }

    sstrcpyn( item.szName,pScheme->szHeroName,sizeof(item.szName) );
    int nPDBID = pProperty->getProperty_Integer(PROPERTY_ID);
    Vector3 location = getLocation();

    if(m_bHero)
    {
        EmphasisLn(item.szName <<"["<< item.EntityID <<"],hero="<< item.nHeroID<<",skin="<< item.nSkinID <<",camp="<< item.byCamp 
            <<",x="<<location.x<<",y="<<location.y<<",z="<<location.z);
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////
bool PlayerRole::create( UID uid,SEntiyExportShadowContext &shadowData,void * context,int len )
{
    if ( shadowData.byFlag&TO_CLIENT_SELF )
    {
        m_bHero = true;
    }
    return EntityBase::create( uid,shadowData,context,len );
}

const PersonSkinModelItem* PlayerRole::getPersonSkinModelItemScheme()
{
	ISchemeCenter *pCenter = gClientGlobal->getSchemeCenter();
	if ( NULL==pCenter )
	{
		return NULL;
	}

	ISchemePersonModel* pPersonScheme = pCenter->getSchemePersonModel();
	if ( NULL==pPersonScheme )
	{
		return NULL;
	}

	int nHeroID = getProperty()->getProperty_Integer(PROPERTY_VOCATION);

    return pPersonScheme->getPersonSkinModelItem(nHeroID);
}

/////////IClientEntity//////////////////////////////////////////////////////
// 取得数值属性
int PlayerRole::getIntProperty(int nPropID)
{
	IEntityProperty * pPropertyPart = getProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_Integer(nPropID) : 0);
}

// 取得字符串属性
char* PlayerRole::getStringProperty(int nPropID)
{
	IEntityProperty * pPropertyPart = getProperty();
	return (pPropertyPart ? pPropertyPart->getProperty_String(nPropID) : "");
}

// 取得来源游戏世界ID
int PlayerRole::getFromGameWorldID()
{
	IBankPart * pBankPart = (IBankPart *)getEntityPart(PART_BANK);
	return (pBankPart ? pBankPart->getWorldID() : INVALID_WORLDID);
}