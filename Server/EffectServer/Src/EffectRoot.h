/*******************************************************************
** 文件名:	EffectRoot.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	penglu
** 日  期:	2014/12/29
** 版  本:	1.0
** 描  述:	定身效果(不可移动)类声明
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IMovePart.h"
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "HeightFieldMgr.h"

class CEffectRoot : public IEffectEx,public IEventVoteSink
{
public:
	typedef  EffectServer_Root    SCHEME_DATA;

	CEffectRoot( SCHEME_DATA & data ) : m_data(data),m_pEntity(0)
	{
	}

    // 获取效果ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;

        // 策划要求正在冲锋不能被定身，在vote事件处理了。这里不需要再调stopMoving，因为这个接口不会拉客户端显示层
        // 会造成客户端冲锋被冲掉而显示层不能马上知道 等客户端下次同步位置的话可能被拉回定身效果开始的位置
// 		__IMovePart *pMovePart = (__IMovePart *)m_pEntity->getEntityPart(PART_MOVE);
// 		if (pMovePart != NULL)
// 		{
// 			pMovePart->stopMoving();
// 		}

		__IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
		if (pEntityEvent == NULL)
		{
			return false;
		}

		// 注册客户端操作移动消息
		pEntityEvent->Subscibe(this, EVENT_ENTITY_CLIENTOPT_MOVE, "EffectRoot");

		// 注册实体移动
		pEntityEvent->Subscibe(this, EVENT_ENTITY_MOVE, "EffectRoot");


		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			__IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
			if (pEntityEvent != NULL)
			{
				// 注销客户端操作移动消息
				pEntityEvent->UnSubscibe(this, EVENT_ENTITY_CLIENTOPT_MOVE);

				// 注销实体移动
				pEntityEvent->UnSubscibe(this, EVENT_ENTITY_MOVE);
			}

			m_pEntity = 0;
		}
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectRoot(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  如果返回false : 则中断执行，否则继续向下执行
	@note     
	@warning 
	@retval buffer 
	*/
	virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
        if(pszContext == NULL || nLen < sizeof(event_entity_move))
            return false;

        event_entity_move* pEvent = (event_entity_move*)pszContext;

        // 冲锋以及假人跳的过程中不能定身
        if(pEvent->curState == BEHAVIOUR_TYPE_SHIFT || pEvent->curState == BEHAVIOUR_TYPE_JUMP)
        {
            return true;
        }

        // 在空中不能定身
        if(wEventID == EVENT_ENTITY_CLIENTOPT_MOVE && m_pEntity != NULL && m_pEntity->getType() == TYPE_PLAYER_ROLE)
        {
            // 向下0.6米没地面 在空中
            Vector3 locCurrent = m_pEntity->getLocation();
            if(!gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(m_pEntity->getMapID(), locCurrent, 0, 0.6))
            {
                return true;
            }
        }

        return false;
	}

private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;
};
