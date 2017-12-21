/*******************************************************************
** 文件名:	EffectContinuedChangeProperty.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/07
** 版  本:	1.0
** 描  述:	持续改变实体属性
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
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

	// 获取效果ID
	virtual int             GetID() { return m_data.nID; }

	// 检查配置数据的合法性
	virtual bool			Check()
	{
		if (m_data.nType == ENTITY_PROPERTY_::CHANGE_SKIN)
		{
			//ErrorLn("持续改变实体属性,不允许填皮肤：CHANGE_SKIN， EffectID=" << m_data.nID);
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

	// 效果启用
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
			// 发事件
			event_prepare_change_property prepareChangeProperty;
			prepareChangeProperty.nID = m_data.nType;
			prepareChangeProperty.dwFlag = context->dwFlag;
			prepareChangeProperty.nValue = m_data.nBaseVal;
			pEntityEvent->FireExecute(EVENT_ENTITY_PREPARE_CHANGE_PROPERTY, (char *)&prepareChangeProperty, sizeof(prepareChangeProperty));
		}

		// 增加基础数值
		pEntityProperty->addProperty_Integer(m_data.nType, m_data.nBaseVal);
		m_nTotal = m_data.nBaseVal;

		// 注册Timer，执行后续的持续加属性操作
		g_EHelper.SetTimer(0, m_data.nInterval, this, m_data.nCount, "CEffectContinuedChangeProperty::Start()");

		if (isRoleForLog())
		{
			auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
			int sudu = m_pEntity->getIntProperty(PROPERTY_SD);
			TraceLn("+++++Player【" << m_pEntity->getUID() << "】,currSpeed:" << sudu << ",OnTimer Start!++++++++++++++++ time:" << curTick);
		}

		return true;
	}

	// 效果停止
	virtual void			Stop()
	{
		if (m_bBusying && m_pEntity != NULL)
		{
			__IEntityProperty *pEntityProperty = (__IEntityProperty *)m_pEntity->getEntityPart(PART_PROPERTY);
			if (pEntityProperty != NULL)
			{
				// 递减数值
				pEntityProperty->addProperty_Integer(m_data.nType, -m_nTotal);
				m_nTotal = 0;
			}

			if (isRoleForLog())
			{
				auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
				int sudu = m_pEntity->getIntProperty(PROPERTY_SD);
				TraceLn("+++++Player【" << m_pEntity->getUID() << "】,currSpeed:" << sudu << ",OnTimer Stop!++++++++++++++++ time:" << curTick);
			}

			m_bBusying = false;
			m_pEntity = NULL;
		}
		g_EHelper.KillTimer(0, this);
	}

	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectContinuedChangeProperty(m_data);
	}

	// 释放
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
				// 递减数值
				pEntityProperty->addProperty_Integer(m_data.nType, m_data.nAddValue);
				m_nTotal += m_data.nAddValue;
			}
		}

		if (isRoleForLog())
		{
			auto curTick = gServerGlobal->getTimeSyncService()->GetTick();
			int sudu = m_pEntity->getIntProperty(PROPERTY_SD);
			TraceLn("+++++Player【" << m_pEntity->getUID() << "】,currSpeed:" << sudu << ",OnTimer++++++++++++++++ time:" << curTick);
		}
	}

private:
	SCHEME_DATA				m_data;
	bool					m_bBusying;
	int						m_nTotal;

	__IEntity				*m_pEntity;
};
