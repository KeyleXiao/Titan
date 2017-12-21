/*******************************************************************
** �ļ���:	EffectContinuedChangeProperty.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/07
** ��  ��:	1.0
** ��  ��:	�����ı�ʵ������
** Ӧ  ��:
**************************** �޸ļ�¼ ******************************
** �޸���:
** ��  ��:
** ��  ��:
********************************************************************/
#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "TimerHandler.h"
#include "IServerGlobal.h"
#include "IDiePart.h"
#include "ExternalHelper.h"
using namespace EFFECT_SERVER;

class CEffectContinuedChangeProperty : public IEffectEx, public rkt::TimerHandler
{
public:
	typedef  EffectServer_ContinuedChangeProperty    SCHEME_DATA;

	CEffectContinuedChangeProperty(const SCHEME_DATA & data) :
		m_data(data),
		m_bBusying(false),
		m_nTotal(0),
		m_pEntity(NULL)
	{
	}

	// ��ȡЧ��ID
	virtual int             GetID() { return m_data.nID; }

	// ����������ݵĺϷ���
	virtual bool			Check()
	{
		if (m_data.nType == ENTITY_PROPERTY_::CHANGE_SKIN)
		{
			//ErrorLn("�����ı�ʵ������,��������Ƥ����CHANGE_SKIN�� EffectID=" << m_data.nID);
			return false;
		}

		return true;
	}

	bool isRoleForLog() {
		return false;
		bool isFindAI = false;
		bool isRole = (m_pEntity->getIntProperty(PROPERTY_AI_SCHEME_ID) <= 0);
		if (isFindAI && !isRole)
		{
			static const auto uFixID = m_pEntity->getUID();
			auto uTempID = m_pEntity->getUID();
			if (uFixID== uTempID)
			{
				return true;
			}
			return false;
		}
		return isRole;
	}

	// Ч������
	virtual bool			Start(EFFECT_CONTEXT * context, CONDITION_CONTEXT *pConditionContext)
	{
		if (context == 0 || context->pEntity == 0)
			return false;

		if (m_bBusying)
			return false;

		m_pEntity = context->pEntity;
		if (m_pEntity == NULL)
			return false;
		__IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
		if (pEntityEvent == NULL)
			return false;

		__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
		if (pEntityProperty == NULL)
			return false;

		m_bBusying = true;

		if (m_data.nEvent == MA_TRUE)
		{
			// ���¼�
			event_prepare_change_property prepareChangeProperty;
			prepareChangeProperty.nID = m_data.nType;
			prepareChangeProperty.dwFlag = context->dwFlag;
			prepareChangeProperty.nValue = m_data.nBaseVal;
			pEntityEvent->FireExecute(EVENT_ENTITY_PREPARE_CHANGE_PROPERTY, (char *)&prepareChangeProperty, sizeof(prepareChangeProperty));
		}

		// ���ӻ�����ֵ
		pEntityProperty->addProperty_Integer(m_data.nType, m_data.nBaseVal);
		m_nTotal = m_data.nBaseVal;

		// ע��Timer��ִ�к����ĳ��������Բ���
		g_EHelper.SetTimer(0, m_data.nInterval, this, m_data.nCount, "CEffectContinuedChangeProperty::Start()");

		if (isRoleForLog())
		{
			auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
			int sudu = m_pEntity->getIntProperty(PROPERTY_SD);
			TraceLn("+++++Player��" << m_pEntity->getUID() << "��,currSpeed:" << sudu << ",OnTimer Start!++++++++++++++++ time:" << curTick);
		}

		return true;
	}

	// Ч��ֹͣ
	virtual void			Stop()
	{
		if (m_bBusying && m_pEntity != NULL)
		{
			__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
			if (pEntityProperty != NULL)
			{
				// �ݼ���ֵ
				pEntityProperty->addProperty_Integer(m_data.nType, -m_nTotal);
				m_nTotal = 0;
			}

			if (isRoleForLog())
			{
				auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
				int sudu = m_pEntity->getIntProperty(PROPERTY_SD);
				TraceLn("+++++Player��" << m_pEntity->getUID() << "��,currSpeed:" << sudu << ",OnTimer Stop!++++++++++++++++ time:" << curTick);
			}

			m_bBusying = false;
			m_pEntity = NULL;
		}
		g_EHelper.KillTimer(0, this);
	}

	// ��¡һ����Ч��
	virtual IEffect *       Clone()
	{
		return new CEffectContinuedChangeProperty(m_data);
	}

	// �ͷ�
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	virtual void OnTimer(unsigned long dwTimerID)
	{
		if (m_bBusying && m_pEntity != NULL)
		{
			__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
			if (pEntityProperty != NULL)
			{
				// �ݼ���ֵ
				pEntityProperty->addProperty_Integer(m_data.nType, m_data.nAddValue);
				m_nTotal += m_data.nAddValue;
			}
		}

		if (isRoleForLog())
		{
			auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
			int sudu = m_pEntity->getIntProperty(PROPERTY_SD);
			TraceLn("+++++Player��" << m_pEntity->getUID() << "��,currSpeed:" << sudu << ",OnTimer++++++++++++++++ time:" << curTick);
		}
	}

private:
	SCHEME_DATA				m_data;
	bool					m_bBusying;
	int						m_nTotal;

	__IEntity				*m_pEntity;
};
