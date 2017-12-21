/*******************************************************************
** 文件名:	AOIPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	12/9/2014
** 版  本:	1.0
** 描  述:	管理实体视野和属性、状态更新

********************************************************************/

#include "StdAfx.h"
#include "AOIPart.h"
#include "IEntityProperty.h"
#include "AOI_Define.h"
#include "PlayerRole.h"
#include "ISchemeCenter.h"
#include "EventDef.h"
float AOIPart::m_sGuardDistance = 10.0f;
int AOIPart::m_sGuardBufTime = 5;

AOIPart::AOIPart(void)
    : m_pMaster(NULL)
	, m_nSignRadii(AOI_DEFAULT_SIGHT)
    , m_dwUnGuardTime(0)
    , m_bGuard(false)
{

}

AOIPart::~AOIPart(void)
{
}

short AOIPart::getPartID()
{
	return PART_AOI;
}

bool AOIPart::onLoad(IEntity* pEntity)
{
	if (pEntity == NULL)
		return false;

	m_pMaster = pEntity;
    if ( m_pMaster->isHero() )
    {
		ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
		if(pSchemeCenter)
		{
			// 获取相关配置参数
			m_sGuardDistance = pSchemeCenter->getSchemeAOI()->getGuardDistance();
			m_sGuardBufTime = pSchemeCenter->getSchemeAOI()->getGuardBufTime();
		}

		// todo 要在MOBA战场才开启
        gClientGlobal->getTimerAxis()->SetTimer( ETIMER_GUARD, 1000, this );
    }

	return true;
}

// 是否在主角焦点范围内
bool AOIPart::isInMasterEyeFocus(float x, float y, float z)
{
    const int FOCUS_DISTANCE = 50;

    // 上帝模式下用显示层传来的点为焦点，否则用主角坐标为焦点
    if(m_eyeFocus.isGodMode)
    {
        return ( (abs(m_eyeFocus.x - x) <= FOCUS_DISTANCE) && (abs(m_eyeFocus.z - z) <= FOCUS_DISTANCE) );
    }
    else
    {
        Vector3 location = m_pMaster->getLocation();
        return ( (abs(location.x - x) <= FOCUS_DISTANCE) && (abs(location.z - z) <= FOCUS_DISTANCE) );
    }    
}

bool AOIPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch (msgId)
    {
    case SC_MSG_ENTITY_UPDATEPROPS:
        {
            IEntityProperty* pProperty = (IEntityProperty *)m_pMaster->getEntityPart(PART_PROPERTY);
            if(pProperty == nullptr)
            {
                break;
            }

            bool bSync = true;
            do 
            {
                // 人直接执行同步属性
                if( isActor(m_pMaster) )
                {
                    break;
                }

                // 防御塔直接执行同步属性,对于不在视线中的塔Update时加上一定的延时
                if(pProperty->getProperty_Integer(PROPERTY_VOCATION) == MONSTER_SUB_TYPE_DEFENSIVE_TOWER)
                {
                    break;
                }

                int nPropObserver = 0;
                IClientEntity* pHero = gClientGlobal->getHero();
                if(pHero != NULL)
                {
                    nPropObserver = pHero->getIntProperty(PROPERTY_OBSERVER);
                }

                // 自己是观察者 直接同步属性
                if(nPropObserver)
                {
                    break;
                }

                if( !IsInMasterEyeFocus(m_pMaster) )
                {
                    bSync = false; 
                    break;
                }


            } while (false);

            if(bSync)
            {
                pProperty->synchronize_properties(data, len);
            }
        } 
        break;

    default: break;
    }
    
    return true;
}

bool AOIPart::onCommand( int cmdid,void * data, size_t len )
{
    switch(cmdid)
    {
    case ENTITY_CMD_MASTER_EYE_FOCUS:       // 主角聚焦位置
        {
            if ( data!=0 && len==sizeof(cmd_entity_master_eye_focus) )
            {
                cmd_entity_master_eye_focus * pCmd = CAST_TYPE(cmd_entity_master_eye_focus* , data);
                bool isChangeMode= (m_eyeFocus.isGodMode != pCmd->isGodMode);

                m_eyeFocus = *pCmd;
                
                // 模式变化时同步到服务器,或者是上帝模式,更新焦点
                if(isChangeMode || pCmd->isGodMode)
                {
                    msg_entity_aoi_sync_info msg;
                    msg.isGodMode = pCmd->isGodMode;
                    msg.vLookLoc = Vector3(pCmd->x, pCmd->y, pCmd->z);

                    // 客户端同步AOI信息到服务器
                    SendMessageToEntityScene(m_pMaster->getUID(), (BYTE)getPartID(), CS_MSG_ENTITY_AOI_SYNC_INFO, (char*)&msg, sizeof(msg));
                }
                //Vector3 this_loc = m_pMaster->getLocation();
                //char szBuf[512] = {0};
                //sprintf_s(szBuf, sizeof(szBuf), "%s:e.pos[%f,%f,%f], nowpos[%f,%f,%f]",
                //    (m_eyeFocus.isGodMode==0?"change user mode" : "change god mode"),
                //    this_loc.x,this_loc.y,this_loc.z, 
                //    m_eyeFocus.x,m_eyeFocus.y,m_eyeFocus.z
                //   );
                //TraceLn(szBuf);
                return true;
            }            
        }
        break;
    default:
        break;
    }

	return false;
}

IEntity* AOIPart::getHost()
{
    return m_pMaster;
}

void AOIPart::release()
{
    if ( m_pMaster&&m_pMaster->isHero() )
    {
        gClientGlobal->getTimerAxis()->KillTimer( ETIMER_GUARD,this );
    }
    delete this;
}

bool AOIPart::onSerialize( UID uid, rkt::ibuffer& in, int flag)
{
	return true;
}

bool AOIPart::deSerialize(rkt::obuf& out, int flag)
{
	return true;
}

int AOIPart::getSight()
{
    return m_nSignRadii;
}

/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void AOIPart::OnTimer( unsigned long dwTimerID )
{
	switch(dwTimerID)
	{
	case ETIMER_GUARD:		// 警戒TIMER
		{
			onTimerGuard();
		}
		break;

	default:
		{
		}
		break;	
	}
}

void AOIPart::onTimerGuard(void)
{
	if ( m_pMaster==NULL || !m_pMaster->isHero() )
		return;

	int nSelfCamp = 0;
	IEntityProperty *pProperty = ((EntityBase*)m_pMaster)->getProperty();
	if ( pProperty!=0 )
	{
		nSelfCamp = pProperty->getProperty_Integer( PROPERTY_CAMP );
	}

	bool bGuard = false;

	// 一定范围内有敌人，则进入警戒
	Vector3 myloc = m_pMaster->getLocation();
	IClientEntity* entityArray[32];
	size_t n = gClientGlobal->getEntityFactory()->getAllPlayer( entityArray,_countof(entityArray) );
	for ( size_t i=0;i<n;++i )
	{
		int nCamp = 0;
		IEntityProperty *pObjProperty = ((EntityBase*)entityArray[i])->getProperty();
		if (pObjProperty !=0 )
		{
			nCamp = pObjProperty->getProperty_Integer( PROPERTY_CAMP );
		}
		if ( nSelfCamp!=nCamp && ((EntityBase*)entityArray[i])->isOnView() )
		{
			Vector3 otherloc = entityArray[i]->getLocation();
			if ( myloc.checkDistance( otherloc,m_sGuardDistance ) )
			{
				bGuard = true;
				break;
			}
		}
	}

	ENTITY_ID id = ((EntityBase*)m_pMaster)->getViewID();

	if ( m_bGuard&&!bGuard )
	{
		m_dwUnGuardTime = ::GetTickCount();
	}
	else if ( !m_bGuard&&bGuard )
	{
		// 通告显示层进入警戒
		m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_GOTO_GUARD,1,"",0,0);
	}
	else if ( !m_bGuard&&!bGuard )
	{
		// 通告显示层进入非警戒
		if ( m_dwUnGuardTime!=0 && ::GetTickCount()>m_dwUnGuardTime+m_sGuardBufTime*1000 )
		{
			m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_GOTO_GUARD,0,"",0,0 );
			m_dwUnGuardTime = 0;
		}
	}

	m_bGuard = bGuard;
}

void AOIPart::restore()
{

}
