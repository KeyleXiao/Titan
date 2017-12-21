/*******************************************************************
** 文件名:	WingPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	翅膀部件实现
********************************************************************/

#include "stdafx.h"
#include "WingPart.h"
#include "EntityViewDef.h"
#include "WingMessageDef.h"
#include "EffectDef.h"
#include "ISchemeCenter.h"
#include "SpellContextDef.h"
using namespace EFFECT_CLIENT;

#define  WINGPART_INNER_EFFECT(typeName,obj,pContext)  gClientGlobal->getEffectFactory()->DoInnerEffect( #typeName,&obj,sizeof(obj),pContext)

//////////////////////////////////////////////////////////////////////////
bool WingPart::deSerialize( rkt::obuf & out,int flag )
{
    return true;
}

bool WingPart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
    BYTE nWingID, byState;
    in >> nWingID >> byState;

    m_nWingState = byState;

    // 没有上翅膀时不用去校验
    if(nWingID > 0)
    {
        ISchemeWing* pWingScheme = gClientGlobal->getSchemeCenter()->getSchemeWing();
        if(pWingScheme != NULL)
        {
            SWing* pWing = pWingScheme->getWing(nWingID);
            if(pWing != NULL)
            {
                memcpy(&m_Wing, pWing, sizeof(m_Wing));
            }
            else
            {
                ErrorLn("wing scheme not found, id=" << nWingID);
            }
        }
    }

    return true;
}

bool WingPart::onLoad( IEntity* pEntity)
{
	m_pEntity = pEntity;

    if (m_pEntity == NULL)
    {
        return false;
    }

    IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
    if (pEventEngine == NULL)
    {
        return false;
    }

    if ((m_pEntity->getTypeMask()&MASK_ACTOR) == MASK_ACTOR)
    {
        UID uid = m_pEntity->getUID();
        BYTE type  = UID_2_TYPE(uid);

        pEventEngine->Subscibe(this, EVENT_ENTITY_ENTER_VIEWPORT, type, uid,__FILE__);
    }

	return true;
}



bool WingPart::onMessage(void *pEntityHead, int nMsgID, void *pData, size_t nLen)
{
	switch( nMsgID )
	{
    case WING_MSG_SET_WING_INFO:
        {
            // 客户端消息设置翅膀信息
            OnClientMessageSetWingInfo(pData, nLen);
        }
        break;
    case WING_MSG_UPDATE_WING_INFO:
        {
            // 客户端消息更新翅膀信息
            OnClientMessageUpdateWingInfo(pData, nLen);
        }
        break;
    case WING_MSG_UPDATE_FLY_TIME:
        {
            // 客户端消息更新飞行时间
            OnClientMessageUpdateFlyTime(pData, nLen);
        }
        break;
	default:
		break;
	}

	return true;
}

bool WingPart::onCommand(int nCmdID, void *pData, size_t nLen)
{
	switch( nCmdID )
	{
    case ENTITY_CMD_SET_WING_STATE:
        {
            if (m_pEntity == NULL)
            {
                return true;
            }

            if (!m_pEntity->isHero())
            {
                return true;
            }

            // 判断参数
            if (pData == 0 || nLen != sizeof(cmd_creature_set_wing_state))
            {
                return true;
            }
            cmd_creature_set_wing_state *pSetWingState = (cmd_creature_set_wing_state *)pData;

            // 如果没在飞行中就不用触发下地
            if(pSetWingState->nType==WING_STATE_FLOOR && !IsFly())
            {
                WarningLn("send floor command fail because current state is not fly! state="<< m_nWingState);
                return true;
            }

            SSetWingState_CS SetWingState;
            SetWingState.nStateType = pSetWingState->nType;
            SetWingState.bEncounterFlag = pSetWingState->nEncounterFlag;
            SendMessageToEntityScene(m_pEntity->getUID(), PART_WING, WING_MSG_SET_WING_STATE, (char *)&SetWingState, sizeof(SetWingState));

            return true;
        }
        break;
	default:
		break;
	}

    return false;
}

void WingPart::release()
{
    if (m_pEntity != NULL)
    {
        IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
        if (pEventEngine != NULL)
        {
            UID uid = m_pEntity->getUID();
            BYTE type  = UID_2_TYPE(uid);

            if ((m_pEntity->getTypeMask()&MASK_ACTOR) == MASK_ACTOR)
            {
                pEventEngine->UnSubscibe(this, EVENT_ENTITY_ENTER_VIEWPORT, type, uid);
            }
        }
        m_pEntity = NULL;
    }
    delete this;
}


////////////////////IEventExecuteSink//////////////////////////////////////////////////////
void WingPart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch (wEventID)
    {
    case EVENT_ENTITY_ENTER_VIEWPORT:
        {
            // 更新翅膀信息
            UpdateWingInfo();
        }
        break;
    default:
        break;
    }
}


////////////////////WingPart//////////////////////////////////////////////////////
WingPart::WingPart():m_pEntity(0),m_nWingState(0)
{
    memset(&m_Wing, 0, sizeof(m_Wing));
}

WingPart::~WingPart()
{
}

/** 客户端消息设置翅膀信息
*/
void WingPart::OnClientMessageSetWingInfo(void *pData, size_t nLen)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    // 判断参数
    if (pData == 0 || nLen != sizeof(SSetWingInfo_SC))
    {
        return;
    }
    SSetWingInfo_SC *pSetWingInfo = (SSetWingInfo_SC *)pData;

    BYTE nWingID = pSetWingInfo->byWingID;
    if( nWingID > 0 )
    {
        ISchemeWing* pWingScheme = gClientGlobal->getSchemeCenter()->getSchemeWing();
        if(pWingScheme != NULL)
        {
            SWing* pWing = pWingScheme->getWing(nWingID);
            if(pWing != NULL)
            {
                memcpy(&m_Wing, pWing, sizeof(m_Wing));
            }
            else
            {
                ErrorLn("wing scheme not found, id=" << nWingID);
            }
        }
    }

    m_nWingState = pSetWingInfo->byStateType;

    // 更新翅膀信息
    UpdateWingInfo();
}

/** 客户端消息更新翅膀信息
*/
void WingPart::OnClientMessageUpdateWingInfo(void *pData, size_t nLen)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    // 判断参数
    if (pData == 0 || nLen != sizeof(SUpdateWingInfo_SC))
    {
        return;
    }
    SUpdateWingInfo_SC *pUpdateWingInfo = (SUpdateWingInfo_SC *)pData;
    m_nWingState = pUpdateWingInfo->nStateType;

    // 更新翅膀信息
    UpdateWingInfo();
}


/** 客户端消息更新飞行时间
*/
void WingPart::OnClientMessageUpdateFlyTime(void *pData, size_t nLen)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    IRenderViewProxy *pRenderViewStruct = gClientGlobal->getRenderView();
    if (pRenderViewStruct == NULL)
    {
        return;
    }

    // 判断参数
    if (pData == 0 || nLen != sizeof(SUpdateFlyTime_SC))
    {
        return;
    }
    SUpdateFlyTime_SC *pUpdateFlyTime = (SUpdateFlyTime_SC *)pData;

    // 是否关闭飞行读条
    bool bClose = false;
    // 是飞行状态
    if (pUpdateFlyTime->nStateType >= WING_STATE_START_FLY && pUpdateFlyTime->nStateType <= WING_STATE_ACCELERATION)
    {
        // 显示飞行读条
        if (pUpdateFlyTime->nTime > 0)
        {
            pRenderViewStruct->sendControllerCommand(ENTITY_TOVIEW_PROGRESS_BAR, pUpdateFlyTime->nTime, 0, 0, 0);
        }
        else
        {
            bClose = true;
        }
    }
    else
    {
        bClose = true;
    }

    if (pUpdateFlyTime->nTime <= 0)
    {
        bClose = true;
    }

    // 关闭读条
    if (bClose)
    {
        // 关闭进度条
        pRenderViewStruct->sendControllerCommand(ENTITY_TOVIEW_CLEAR_PROGRESS_BAR,0,0,0,0);
    }
}

/** 是否在飞行中
*/
bool WingPart::IsFly()
{
    return ( m_nWingState >= WING_STATE_START_FLY && m_nWingState <= WING_STATE_ACCELERATION );
}


/** 更新翅膀信息
*/
void WingPart::UpdateWingInfo(void)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    EffectClient_ChangeWingState ChangeWingState;
    ChangeWingState.nType = (WING_STATE_TYPE)m_nWingState;
    do 
    {
        if (m_Wing.nID <= 0)
        {
            break;
        }

        sstrcpyn(ChangeWingState.szPath, m_Wing.szPath, sizeof(ChangeWingState.szPath));
        ChangeWingState.fSpeed = m_Wing.fSpeed;
        ChangeWingState.fAcceleration = m_Wing.fAcceleration;
        ChangeWingState.fMaxSpeed = m_Wing.fMaxSpeed;
        ChangeWingState.fUpSpeed = m_Wing.fUpSpeed;
        ChangeWingState.fMinFlyHeight = m_Wing.fMinFlyHeight;
        ChangeWingState.fMaxFlyHeight = m_Wing.fMaxFlyHeight;
        ChangeWingState.fMinCameraAngle = m_Wing.fMinCameraAngle;
        ChangeWingState.fMaxCameraAngle = m_Wing.fMaxCameraAngle;
    } while (false);

    EFFECT_CONTEXT context;
    context.pEntity = m_pEntity;
    WINGPART_INNER_EFFECT(EffectClient_ChangeWingState, ChangeWingState, &context);
}

void WingPart::restore()
{

}

