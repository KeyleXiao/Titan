/*******************************************************************
** 文件名:	PlayerKillPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/05/2016
** 版  本:	1.0
** 描  述:	PK部件实现
********************************************************************/

#include "stdafx.h"
#include "PlayerKillPart.h"
#include "EntityViewDef.h"
#include "PlayerKillMessageDef.h"
#include "ExternalHelper.h"
#include "ISchemeCenter.h"
#include "EffectDef.h"
#include "IEffect.h"
using namespace EFFECT_CLIENT;

#define PK_PART_INNER_EFFECT( typeName,obj,pContext)  gClientGlobal->getEffectFactory()->DoInnerEffect( #typeName,&obj,sizeof(obj),pContext)

//////////////////////////////////////////////////////////////////////////
bool PlayerKillPart::deSerialize( rkt::obuf & out,int flag )
{
    return true;
}

bool PlayerKillPart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
    return true;
}

bool PlayerKillPart::onLoad( IEntity* pEntity)
{
	m_pEntity = pEntity;

    if (m_pEntity == NULL)
    {
        return false;
    }

	return true;
}



bool PlayerKillPart::onMessage(void *pEntityHead, int nMsgID, void *pData, size_t nLen)
{
    switch (nMsgID)
    {
    case PK_MSG_REQUEST_PK:
        {
            onClientMessageRequestPK((LPCSTR)pData, nLen);
        }
        break;
    case PK_MSG_START_PK:
        {
            onClientMessageStartPK(pData, nLen);
        }
        break;
    case PK_MSG_END_PK:
        {
            onClientMessageEndPK(pData, nLen);
        }
        break;
    case PK_MSG_SHOW_MESSAGE_BOX:
        {
            onClientMessageShowMessageBox(pData, nLen);
        }
        break;
    default:
        break;
    }
	return true;
}

bool PlayerKillPart::onCommand(int nCmdID, void *pData, size_t nLen)
{
    switch (nCmdID)
    {
    case ENTITY_CMD_PK_CHOOSE_TARGET:
        {
            if (m_pEntity == NULL)
            {
                return true;
            }

            // 判断参数
            if (pData == 0 || nLen != sizeof(cmd_creature_pk_choose_target))
            {
                return true;
            }
            cmd_creature_pk_choose_target *pChooseTarget = (cmd_creature_pk_choose_target *)pData;

            UID uidTarget = pChooseTarget->uidTarget;
            if (!checkPK(uidTarget))
            {
                return true;
            }

            SChooseTarget_CS ChooseTarget;
            ChooseTarget.uidTarget = uidTarget;
            SendMessageToEntityScene(m_pEntity->getUID(), PART_PK, PK_MSG_CHOOSE_TARGET, (char *)&ChooseTarget, sizeof(ChooseTarget));

            return true;
        }
        break;
    case ENTITY_CMD_PK_HIT_PROMPT:
        {
            if (m_pEntity == NULL)
            {
                return true;
            }

            ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
            if (pSchemeCenter == NULL)
            {
                return true;
            }

            ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
            if (pSchemePlayerKill == NULL)
            {
                return true;
            }

            SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
            if (pPlayerKill == NULL)
            {
                return true;
            }

            // 取得当前时间
            DWORD dwTick = getTickCountEx();
            REQUEST_PK_VECTOR::iterator Iter = m_RequestPKVector.begin();
            while (Iter != m_RequestPKVector.end())
            {
                // 移除超过时效的切磋请求
                if (dwTick >= Iter->dwTime + pPlayerKill->nRequestValidTime)
                {
                    Iter = m_RequestPKVector.erase(Iter);
                }
                else
                {
                    ++Iter;
                } 
            }
            if (m_RequestPKVector.empty())
            {
                g_EHelper.showFloatText(m_pEntity, CHAT_TIP_ENTITY_PK_NO_REQUEST);
                
                // 设置请求PK
                setRequestPK();

                return true;
            }

            Iter = m_RequestPKVector.begin();

            cmd_creature_pk_show_pk_info Info;
            Info.uidOperator = Iter->uidOperator;
            sstrcpyn(Info.szInfo, a2utf8(Iter->szInfo), sizeof(Info.szInfo));
            Info.dwTime = Iter->dwTime + pPlayerKill->nRequestValidTime;
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_PK_INFO,0,0, &Info, sizeof(Info));

            // 请求PK信息容器移除
            m_RequestPKVector.erase(Iter);

            // 设置请求PK
            setRequestPK();
        }
        break;
    case ENTITY_CMD_PK_HANDLE_INFO:
        {
            if (m_pEntity == NULL)
            {
                return true;
            }

            // 判断参数
            if (pData == 0 || nLen != sizeof(cmd_creature_pk_handle_info))
            {
                return true;
            }
            cmd_creature_pk_handle_info *pInfo = (cmd_creature_pk_handle_info *)pData;

            SHandInfo_CS handleInfo;
            handleInfo.uidOperator = pInfo->uidOperator;
            handleInfo.bAgree = pInfo->nAgree == 1 ? true : false;
            SendMessageToEntityScene(m_pEntity->getUID(), PART_PK, PK_MSG_HANDLE_INFO, (char *)&handleInfo, sizeof(handleInfo));
        }
        break;
    case ENTITY_CMD_PK_CANCEL_PK:
        {
            if (m_pEntity == NULL)
            {
                return true;
            }

            // 判断参数
            if (pData == 0 || nLen != sizeof(cmd_creature_pk_cancel_pk))
            {
                return true;
            }
            cmd_creature_pk_cancel_pk *pCancelPK = (cmd_creature_pk_cancel_pk *)pData;
            
            // 不取消PK
            if (pCancelPK->nCancel == 0)
            {
                return true;
            }

            SCancelPK_CS cancelPK;
            cancelPK.bCancel = pCancelPK->nCancel == 0 ? false : true;
            SendMessageToEntityScene(m_pEntity->getUID(), PART_PK, PK_MSG_CANCEL_PK, (char *)&cancelPK, sizeof(cancelPK));
        }
        break;
    default:
        break;
    }
    return false;
}

void PlayerKillPart::release()
{
    delete this;
}

////////////////////IPlayerKillPart//////////////////////////////////////////////////////
// 是否和此目标进行PK
bool PlayerKillPart::isWithPK(IEntity * pEntity)
{
    if(!pEntity)
    {
        return false;
    }
    if(m_uidPKTarget == INVALID_UID)
    {
        return false;
    }

    return (m_uidPKTarget == pEntity->getUID());
}

////////////////////PlayerKillPart//////////////////////////////////////////////////////
PlayerKillPart::PlayerKillPart()
    :m_pEntity(NULL)
    ,m_uidPKTarget(INVALID_UID)     /* 正在和此实体PK */
{

}

PlayerKillPart::~PlayerKillPart()
{
    m_pEntity = NULL;
    // 清空PK目标对象
    m_uidPKTarget = INVALID_UID;
}

// 检测切磋
bool PlayerKillPart::checkPK(UID uidTarget)
{
    if (m_pEntity == NULL)
    {
        return false;
    }

    UID uidSelf = m_pEntity->getUID();
    if (uidSelf == uidTarget)
    {
        g_EHelper.showFloatText(m_pEntity, CHAT_TIP_ENTITY_PK_NO_CHOOSE_SELF);
        return false;
    }

    int nType = UID_2_TYPE(uidTarget);
    // 目标不是英雄
    if (nType != TYPE_PLAYER_ROLE)
    {
        g_EHelper.showFloatText(m_pEntity, CHAT_TIP_ENTITY_PK_NO_CHOOSE_TARGET);
        return false;
    }

    if (uidTarget == 0 || uidTarget == INVALID_UID)
    {
        g_EHelper.showFloatText(m_pEntity, CHAT_TIP_ENTITY_PK_NO_CHOOSE_TARGET);
        return false;
    }

    IEntity *pEntity = g_EHelper.GetEntity(uidTarget);
    if (pEntity == NULL)
    {
        g_EHelper.showFloatText(m_pEntity, CHAT_TIP_ENTITY_PK_TARGET_NO_EXIST);
        return false;
    }

    return true;
}

/** 客户端消息请求PK
*/
void PlayerKillPart::onClientMessageRequestPK(LPCSTR pData, size_t nLen)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return;
    }

    ISchemePlayerKill *pSchemePlayerKill = pSchemeCenter->getSchemePlayerKill();
    if (pSchemePlayerKill == NULL)
    {
        return;
    }

    SPlayerKill *pPlayerKill = pSchemePlayerKill->getPlayerKill();
    if (pPlayerKill == NULL)
    {
        return;
    }

    size_t nRequestPKLen = sizeof(SRequestPK_SC);
    if (pData == NULL || nLen < nRequestPKLen)
    {
        return;
    }
    SRequestPK_SC *pRequestPK = (SRequestPK_SC *)pData;

    int nRequestPKInfoLen = sizeof(SRequestPKInfo);
    if (nLen != nRequestPKLen + pRequestPK->nRequestCount*nRequestPKInfoLen)
    {
        return;
    }

    int offset = nRequestPKLen;
    for (int i=0; i<pRequestPK->nRequestCount; ++i)
    {
        SRequestPKInfo *pInfo = (SRequestPKInfo *)(pData + offset);
        m_RequestPKVector.push_back(*pInfo);

        cmd_creature_pk_show_pk_info Info;
        Info.uidOperator = pInfo->uidOperator;
        sstrcpyn(Info.szInfo, pInfo->szInfo, sizeof(Info.szInfo));
        Info.dwTime = pPlayerKill->nRequestValidTime; // 直接传失效时间,显示层自己处理
        m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_PK_INFO,0,0, &Info, sizeof(Info));

        offset += nRequestPKInfoLen;
    }
}


/** 客户端消息开始PK
*/
void PlayerKillPart::onClientMessageStartPK(void *pData, size_t nLen)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    if (pData == NULL || nLen != sizeof(SStartPK_SC))
    {
        return;
    }
    SStartPK_SC *pStartPK = (SStartPK_SC *)pData;

    // 清空请求PK信息容器
    m_RequestPKVector.clear();
    // 设置请求PK
    setRequestPK();

    // 设置当前PK目标对象
    m_uidPKTarget = pStartPK->uidTarget;

    cmd_creature_pk_start_pk startPK;
    startPK.bStart = 1;
    startPK.uidTarget= pStartPK->uidTarget;
    startPK.vLoc[0] = pStartPK->vLoc.x;
    startPK.vLoc[1] = pStartPK->vLoc.y;
    startPK.vLoc[2] = pStartPK->vLoc.z;
    m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_START_PK,0,0, &startPK, sizeof(startPK));
}


/** 客户端消息结束PK
*/
void PlayerKillPart::onClientMessageEndPK(void *pData, size_t nLen)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    if (pData == NULL || nLen != sizeof(SEndPK_SC))
    {
        return;
    }
    SEndPK_SC *pEndPK = (SEndPK_SC *)pData;

    // 清空PK目标对象
    m_uidPKTarget = INVALID_UID;

    cmd_creature_pk_start_pk startPK;
    startPK.bStart = 0;
    startPK.uidTarget= pEndPK->uidTarget;
    startPK.nResult = pEndPK->nResult;
    startPK.nCamp = CampFlag_Friend;
    startPK.bForceEnd = pEndPK->bForceEnd ? 1: 0;
    m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_START_PK,0,0, &startPK, sizeof(startPK));
}


/** 客户端消息显示消息框
*/
void PlayerKillPart::onClientMessageShowMessageBox(void *pData, size_t nLen)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    if (pData == NULL || nLen != sizeof(SShowMessageBox_SC))
    {
        return;
    }
    SShowMessageBox_SC *pBox = (SShowMessageBox_SC *)pData;

    EFFECT_CONTEXT context;
    context.pEntity = m_pEntity;

    EffectClient_ShowCommonMessageBox box;
    box.nType = pBox->nType;
	// 注意，因为后面的模块接收的是ANSI编码，所以这里调用utf82a，一般的字符串客户端收到后直接用，这里比较特殊
    sstrcpyn(box.szCaption, utf82a(pBox->szCaption), sizeof(box.szCaption));
    sstrcpyn(box.szText, utf82a(pBox->szText), sizeof(box.szText));
    PK_PART_INNER_EFFECT(EffectClient_ShowCommonMessageBox, box, &context);
}


// 设置请求PK
void PlayerKillPart::setRequestPK()
{
    if (m_pEntity == NULL)
    {
        return;
    }

    cmd_creature_pk_request_pk requestPK;
    requestPK.bShowPKPrompt= m_RequestPKVector.size() > 0 ? 1 : 0;
    m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_REQUEST_PK,0,0, &requestPK, sizeof(requestPK));
}

void PlayerKillPart::restore()
{

}
