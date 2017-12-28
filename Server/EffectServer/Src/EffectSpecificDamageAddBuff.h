/*******************************************************************
** �ļ���:	EffectSpecificDamageAddBuff.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	7/13/2016
** ��  ��:	1.0
** ��  ��:	Ч��-�ض��˺�����Buff
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "IEntity.h"
using namespace EFFECT_SERVER;
#include "IEventEngine.h"
#include "IEntityEvent.h"
#include "DamageDef.h"
#include "EntityHelper.h"

class CEffectSpecificDamageAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_SpecificDamageAddBuff    SCHEME_DATA;

	CEffectSpecificDamageAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0)
	{
	}

    // ��ȡЧ��ID
    virtual int             GetID()
    {
        return m_data.nID;
    }

	// Ч������
	virtual bool			Start( EFFECT_CONTEXT * context,CONDITION_CONTEXT *pConditionContext )
	{
		if ( context==0 || context->pEntity==0 )
			return false;

		m_pEntity = context->pEntity;

        //DWORD dwSceneID = m_pEntity->getSceneID();

        //SceneLogicEventHelper eventHelper(dwSceneID);
        //IEventEngine *pGlobalEvent = eventHelper.m_ptr;
        //if (pGlobalEvent == NULL)
        //{
        //    return false;
        //}

        //if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
        //{
            //// ע��Ӣ���˺��¼�
            //pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_PLAYER_ROLE, 0, "CEffectSpecificDamageAddBuff");
            //// ע������˺��¼�
            //pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, 0, "CEffectSpecificDamageAddBuff");
            //// ע��Tank�˺��¼�
            //pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_TANK, 0, "CEffectSpecificDamageAddBuff");

            // �����Լ����˺��¼�
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE, __FUNCTION__);
            // ����Ŀ����˺��¼�
            g_EHelper.Subscibe(m_pEntity, this, EVENT_ENTITY_TARGET_DAMAGE_RESULT, __FUNCTION__);

            // ����Ŀ���˺�֪ͨ������
            IUseFlag * pUseFlag = m_pEntity->getUseFlag();
            if (pUseFlag) {
                pUseFlag->add(USE_FLAG_DAMAGE_NOTIFY_OPERATOR);
            }
        //}
        //else
        //{
        //    // ע�������˺��¼�
        //    pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DAMAGE, m_data.nEntityType, 0, "CEffectSpecificDamageAddBuff");
        //}

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
            //DWORD dwSceneID = m_pEntity->getSceneID();

            //SceneLogicEventHelper eventHelper(dwSceneID);
            //IEventEngine *pGlobalEvent = eventHelper.m_ptr;
            //if (pGlobalEvent != NULL)
            //{
            //    if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
            //    {
                    //// ע��Ӣ���˺��¼�
                    //pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_PLAYER_ROLE, 0);
                    //// ע�������˺��¼�
                    //pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_MONSTER, 0);
                    //// ע��Tank�˺��¼�
                    //pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, TYPE_TANK, 0);
                    
                    // �Ƴ�Ŀ���˺�֪ͨ������
                    IUseFlag * pUseFlag = m_pEntity->getUseFlag();
                    if (pUseFlag)
                    {
                        pUseFlag->remove(USE_FLAG_DAMAGE_NOTIFY_OPERATOR);
                    }

                    // ȡ�������Լ����˺��¼�
                    g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_DAMAGE);
                    // ȡ������Ŀ����˺��¼�
                    g_EHelper.UnSubscibe(m_pEntity, this, EVENT_ENTITY_TARGET_DAMAGE_RESULT);
                //}
                //else
                //{
                //    // ע�������˺��¼�
                //    pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DAMAGE, m_data.nEntityType, 0);
                //}
            //}

			m_pEntity = 0;
		}
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectSpecificDamageAddBuff(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ���¼�ID
	@param   bSrcType ������Դ����
	@param   dwSrcID : ����Դ��ʶ��ʵ��ΪUID��"���к�"���ݣ���ʵ���Ϊ0��
	@param   pszContext : ������
	@param   nLen : �����ĳ���
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
    {
		switch (wEventID)
		{
        case EVENT_ENTITY_DAMAGE:
        case EVENT_ENTITY_TARGET_DAMAGE_RESULT:
		//case EVENT_GAME_ENTITY_DAMAGE:
			{
                if (m_pEntity == NULL)
                {
                    break;
                }

                if (pszContext == NULL || nLen != sizeof(event_entity_damage))
                {
                    break;
                }
                event_entity_damage *pDamage = (event_entity_damage*)pszContext;

                int nSpellEffectType = pDamage->nSpellEffectType;
                // �жϼ���Ч������
                if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
                {
                    break;
                }

                UID uidSelf = m_pEntity->getUID();
                if (isInvalidUID(uidSelf))
                {
                    return;
                }

                UID uidOperator = pDamage->uidOperator;
                if (isInvalidUID(uidOperator))
                {
                    break;
                }

                UID uidTarget = pDamage->uidTarget;
                if (isInvalidUID(uidTarget))
                {
                    break;
                }

                // ��������Ŀ��
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nOperatorTargetFilter, uidOperator))
                {
                    break;
                }

                // ���Ŀ��Ŀ��
                if (!g_EHelper.chooseTarget(m_pEntity, m_data.nTargetTargetFilter, uidTarget))
                {
                    break;
                }

                UID uid = INVALID_UID;
                switch (m_data.nTargetType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uid = uidSelf;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uid = uidOperator;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uid = uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectSpecificDamageAddBuff error nTargetType="<<m_data.nTargetType);
                        return;
                    }
                    break;
                }

                UID uidAddBuff = INVALID_UID;
                switch (m_data.nAddBuffTagetType)
                {
                case MA_TAGET_TO_SELF:
                    {
                        uidAddBuff = uidSelf;
                    }
                    break;
                case MA_TAGET_TO_OPERATOR:
                    {
                        uidAddBuff = uidOperator;
                    }
                    break;
                case MA_TAGET_TO_TARGET:
                    {
                        uidAddBuff = uidTarget;
                    }
                    break;
                default:
                    {
                        ErrorLn("CEffectSpecificDamageAddBuff error nAddBuffTagetType="<<m_data.nAddBuffTagetType);
                        return;
                    }
                    break;
                }
                
                SBuffContext BuffContext;
                BuffContext.nID = pDamage->nID;
                AddBuff(uid, m_data.nBuffID, m_data.nBuffLevel, uidAddBuff, 0, &BuffContext, sizeof(BuffContext));

                //TraceLn(__FUNCTION__": uid="<< uid<<",wEventID="<< wEventID <<", buff="<< m_data.nBuffID <<", level="<< m_data.nBuffLevel);
			}
			break;
		default:
			break;
		}
    }

private:
	SCHEME_DATA             m_data;

	// ʵ��ָ��
	__IEntity				*m_pEntity;
};
