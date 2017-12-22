/*******************************************************************
** 文件名:	Buff.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/13  11:29
** 版  本:	1.0
** 描  述:	BUFF
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "Buff.h"
#include "BuffServer.h"
#include "ExternalFacade.h"
#include "SchemeBuff.h"
#include "BuffPart.h"
#include "IEntityProperty.h"


/** 
@param   
@param   
@return  
*/
CBuff::CBuff(void)
{
	// buff id
	m_dwBuffID = INVALID_BUFF_ID;

	// 等级
	m_dwBuffLevel = 0;

	// 时间
	m_dwTime = 0;

	// 标志
	m_dwSchemeFlag = 0;

	// 效果表
	m_listSchemeEffect.clear();

	// 随机效果概率
	m_dwSchemeRandPer = 0;

	// 随机效果
	m_listSchemeRandEffect.clear();

	// 后续触发概率
	m_dwSchemeEndArosePer = 0;

	// 后续触发BUFF：id, level,id, level, id, level
	m_listSchemeEndAroseBuff.clear();

	// 启动时间
	m_dwStartTick = 0;

	// 序号
	m_dwIndex = 0;

	// 效果表
	m_listEffect.clear();

	// 添加者
	m_uidAdd = INVALID_UID;

	// 被作用者部件
	m_pBuffPart = NULL;

	// 效果数据
	memset(&m_effectContext, 0, sizeof(m_effectContext));

	// 是否已经创建效果
	m_bBuildEffectFlag = false;

	// BUFF是否已经启动
	m_bStartBuffFlag = false;

	// 是否需要check才能启动
	m_bIsCheckStart = false;

	// 是否有随机效果
	m_bRandEffect = false;

	// 是否回收利用的，调试用
	m_bRecycle = false;

	// 本BUFF是否依然有效
	m_bThisValidFlag = false;

    m_bEffectStartFlag = false;
}

/** 
@param   
@param   
@return  
*/
CBuff::~CBuff(void)
{
	
}

/** 释放,会释放内存
@param   
@param   
@return  
*/
void CBuff::Release(void)
{
	Restore(false);

	delete this;
}

/** 还原,不释放内存，只将BUFF还原到创建时BUFF
@param   未来继续使用
@param   
@return  
*/
void CBuff::Restore(bool bCreateEndArose)
{
    if(m_bEffectStartFlag)
    {
        __debugbreak();
    }

	m_bThisValidFlag = false;
	
    // Kill生命定时器
    g_ExternalFacade.KillTimer(ETimerEventID_Life, this);

	///////////////////////////////////////////////////////////////////
	// 产生后续BUFF
	int nCreateRandNum = 0; rktRangeRand(nCreateRandNum, 0, (10000 - 1));
	if(bCreateEndArose && nCreateRandNum <= (int)m_dwSchemeEndArosePer)
	{
		TLIST_DWORD::iterator it = m_listSchemeEndAroseBuff.begin();
		for( ; it != m_listSchemeEndAroseBuff.end(); ++it)
		{
			DWORD dwBuffID = (*it);
			
			++it;
			if(it == m_listSchemeEndAroseBuff.end())
			{
				break;
			}
			DWORD dwBuffLevel = (*it);

			if (m_pBuffPart != NULL)
			{
				m_pBuffPart->Add(dwBuffID, dwBuffLevel, m_uidAdd);
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	// 释放EFFECT
	TLIST_EFFECT::iterator itE = m_listEffect.begin();
	for( ; itE != m_listEffect.end(); ++itE)
	{
		IEffect * pEffect = (*itE);

		if(pEffect != NULL)
		{
			// 释放效果
			pEffect->Release();
		}
	}
	m_listEffect.clear();

	///////////////////////////////////////////////////////////////////
	// 数据的清空
	m_HasCheck.Close();

	m_NoHasCheck.Close();

	m_EndConditionAttack.Close();

	m_EndConditionChangeLoc.Close();

	m_EndConditionDie.Close();

	m_EndConditionInjured.Close();

	m_EndConditionStartMove.Close();

	m_EndConditionAttackReady.Close();

    m_EndConditionInjuredByActor.Close();

    m_EndConditionClientRemoveByActor.Close();

	m_dwBuffID = INVALID_BUFF_ID;
	m_dwBuffLevel = 0;
	m_dwSchemeFlag = 0;
	m_listSchemeEffect.clear();
	m_dwSchemeRandPer = 0;
	m_listSchemeRandEffect.clear();
	m_dwSchemeEndArosePer = 0;
	m_listSchemeEndAroseBuff.clear();

	m_dwTime = 0;
	m_dwStartTick = 0;
	m_dwIndex = 0;
	m_uidAdd = INVALID_UID;
	m_pBuffPart = NULL;
	memset(&m_effectContext, 0, sizeof(m_effectContext));
	m_bBuildEffectFlag = false;
	m_bStartBuffFlag = false;
	m_bIsCheckStart = false;
	m_bRandEffect = false;	
	m_bRecycle = true;
}

/** 创建
@param   
@param   
@return  
*/
bool CBuff::Create(DWORD dwIndex, CBuffPart *pBuffPart, UID uidAdd, DWORD dwBuffID, DWORD dwBuffLevel, DWORD dwTime, EFFECT_CONTEXT *pContext)
{
	if(pBuffPart == NULL || dwBuffID == INVALID_BUFF_ID || dwBuffLevel <= 0 || dwIndex <= 0)
	{
		return false;
	}

	__IEntity* pMaster = pBuffPart->getHost();

	if(pMaster == NULL)
	{
		return false;
	}

    __IEntityProperty *pEntityProperty = (__IEntityProperty *)pMaster->getEntityPart(PART_PROPERTY);
    if (pEntityProperty == NULL)
    {
        return false;
    }

	SBuffSchemeInfo * pSchemeInfo = g_pBuffServer->GetBuffSchemeInfo(dwBuffID);
	if(pSchemeInfo == NULL)
	{
		return false;
	}

	SBuffSchemeInfo::SLevelInfo * pLevelInfo = NULL;
	list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchemeInfo->listLevelInfo.begin();
	for( ; it != pSchemeInfo->listLevelInfo.end(); ++it)
	{
		if((*it).dwBuffLevel == dwBuffLevel)
		{
			pLevelInfo = &(*it);
			break;
		}		
	}
	if(pLevelInfo == NULL)
	{
		return false;
	}

	// buff id
	m_dwBuffID = dwBuffID;

	// 等级
	m_dwBuffLevel = (WORD)dwBuffLevel;

	// 本BUFF有效
	m_bThisValidFlag = true;

	// 时间
	if(dwTime <= 0)
	{
		if(m_dwTime == 0)
		{
			m_dwTime = pLevelInfo->dwTimeLong;
		}
	}
	else
	{
		m_dwTime = dwTime;
	}

	// 标志
	m_dwSchemeFlag = pSchemeInfo->dwFlag;

	// 韧性免疫
	if ((m_dwSchemeFlag&TENACITY_IMMUNE_FLAG) > 0)
	{
		// 取得韧性
		int nTenacity = pEntityProperty->getProperty_Integer(PROPERTY_TENACITY);
		// 算出韧性影响时间
		m_dwTime = (DWORD)((float)m_dwTime * (1.0-nTenacity/ZOOM_IN_MULTIPLE));
	}

    // 控制免疫
    if ((m_dwSchemeFlag&CONTROL_IMMUNE_FLAG) > 0)
    {
        // 取得控制免疫
        int nControlInmune = pEntityProperty->getProperty_Integer(PROPERTY_CONTROL_IMMUNE);
        // 算出控制免疫影响时间
        m_dwTime = (DWORD)((float)m_dwTime * (1.0-nControlInmune/ZOOM_IN_MULTIPLE));
    }

	// 效果表
	m_listSchemeEffect = pLevelInfo->listEffect;

	// 随机效果概率
	m_dwSchemeRandPer = pLevelInfo->dwRandPer;

	// 随机效果
	m_listSchemeRandEffect = pLevelInfo->listRandEffect;

	// 后续触发概率
	m_dwSchemeEndArosePer = pLevelInfo->dwEndArosePer;

	// 后续触发BUFF：id, level,id, level, id, level
	m_listSchemeEndAroseBuff = pLevelInfo->listEndAroseBuff;	

	// BUFF条件
	if(pLevelInfo->dwHasCheck == 1/*不停地检查*/ && !pLevelInfo->listHasBuff.empty())
	{
		if(!m_HasCheck.Create(this, pBuffPart, pLevelInfo->listHasBuff))
		{
			return false;
		}

		m_bIsCheckStart = true;
	}

	// 非BUFF条件
	if(pLevelInfo->dwNoCheck == 1/*不停地检查*/ && !pLevelInfo->listNoBuff.empty())
	{
		if(!m_NoHasCheck.Create(this, pBuffPart, pLevelInfo->listNoBuff))
		{
			return false;
		}

		m_bIsCheckStart = true;
	}

	// 序号
	m_dwIndex = dwIndex;

	// 被作用者部件
	m_pBuffPart = pBuffPart;

	// 效果数据
	if (pContext == NULL)
	{
		memset(&m_effectContext, 0, sizeof(m_effectContext));
	}
	else
	{
		memcpy(&m_effectContext, pContext, sizeof(m_effectContext));
	}

	// 添加者
	m_uidAdd = uidAdd;

	// 结束条件
	list<DWORD>::iterator itEnd = pSchemeInfo->listEndFlag.begin();
	for( ; itEnd != pSchemeInfo->listEndFlag.end(); ++itEnd)
	{
		switch((*itEnd))
		{
			case BUFF_END_DIE: 
                {
                    m_EndConditionDie.Create(m_pBuffPart, this); 						
                }
			    break;
			case BUFF_END_INJURED:
                {
                    m_EndConditionInjured.Create(m_pBuffPart, this); 						
                }
                break;
			case BUFF_END_MOVE: 
                {
                    m_EndConditionChangeLoc.Create(m_pBuffPart, this); 
                    m_EndConditionStartMove.Create(m_pBuffPart, this);
                }
                break;
            case BUFF_END_ATTACK:
                {
                    m_EndConditionAttack.Create(m_pBuffPart, this);
                }
                break;
			case BUFF_END_ATTACK_READY:	//暂未实现
                {				
                    m_EndConditionAttackReady.Create(m_pBuffPart, this);
                }
                break;
            case BUFF_END_INJURED_BY_ACTOR:
                {
                    m_EndConditionInjuredByActor.Create(m_pBuffPart, this);
                }
                break;
            case BUFF_END_CLIENT_REMOVE:
                {
                    m_EndConditionClientRemoveByActor.Create(m_pBuffPart, this);
                }
                break;
			default:
                break;
		}
	}

	return true;
}

/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CBuff::OnTimer(unsigned long dwTimerID)
{
	if(dwTimerID == ETimerEventID_Life)
	{
		g_ExternalFacade.KillTimer(ETimerEventID_Life, this);

		if(m_pBuffPart != NULL)
		{		
			m_pBuffPart->Remove(this, INVALID_UID, true);
		}
	}
}

/** 启用
@param   
@param   
@return  
*/
bool CBuff::Start()
{
	if(!m_bThisValidFlag/*此BUFF已无效，在等待释放状态*/)
	{
		return false;
	}

	if(m_bStartBuffFlag)
	{
		return true;
	}
	m_bStartBuffFlag = true;

	if(m_dwTime > 0)
	{
        // Kill生命定时器
        g_ExternalFacade.KillTimer(ETimerEventID_Life, this);

        char szDebug[32]= {0};
        ssprintf_s(szDebug, sizeof(szDebug), "CBuff::Start %d",m_dwBuffID);
		g_ExternalFacade.SetTimer(ETimerEventID_Life, m_dwTime, this, szDebug);
	}

	// 启动时间
	m_dwStartTick = getTickCountEx();

	// 如果需要BUFF条件,则不启用
	if(!m_bIsCheckStart)
	{
		return StartEffect();
	}

	return true;
}

/** 构建属性表
@param   
@param   
@return  
*/
bool CBuff::BuildEffectList()
{
	if(!m_bThisValidFlag/*此BUFF已无效，在等待释放状态*/)
	{
		return false;
	}

	// 创建固定效果表
	TLIST_DWORD::iterator it = m_listSchemeEffect.begin();
	for( ; it != m_listSchemeEffect.end(); ++it)
	{
		IEffect * pEffect = g_ExternalFacade.CreateEffect((*it));
		if(pEffect != NULL)
		{
			m_listEffect.push_back(pEffect);
		}
	}

	return true;
}

/** 启用
@param   
@param   
@return  
*/
bool CBuff::StartEffect()
{
	if(!m_bThisValidFlag/*此BUFF已无效，在等待释放状态*/)
	{
		return false;
	}

	if(m_bBuildEffectFlag)
	{
		return true;
	}
	m_bBuildEffectFlag = true;

	// 先创建效果	
	m_bRandEffect = false;
	BuildEffectList();	

	// 启动效果
	TLIST_EFFECT::iterator it = m_listEffect.begin();
	for( ; it != m_listEffect.end(); ++it)
	{
		// ******这里出过异常，因为pEffect->Start效果启动，导致怪物死亡，
		// 然后会发送EVENT_CREATURE_DIE事件，当CEndConditionDie捕获到此
		// 事件时，将此buff释放掉，而此处m_listEffect还在循环之中，而
		// m_pBuffPart为空，所以异常。又是坐在凳子上，把凳子搬掉的问题
		if(m_pBuffPart == NULL || !m_bThisValidFlag)
		{
			return true;
		}

		IEffect * pEffect = (*it);
		if(pEffect != NULL)
		{
            m_bEffectStartFlag = true;
			pEffect->Start(&m_effectContext, NULL);
            m_bEffectStartFlag = false;
		}

		// ******这里也出现过异常，因为pEffect->Start也可能会导致生物死亡，
		// 而结束条件CEndConditionDie订阅此生物死亡事件，此时CEndConditionDie
		// 会移除buff，也就会调CBuff.Stop()，并且会清掉m_listEffect，然而
		// 返回时，导致此迭代器出现异常．此种问题防不胜防
		if(!m_bThisValidFlag)
		{
			return true;
		}
	}

	// 关闭 BUFF 条件
	m_HasCheck.Close();

	// 关闭 非BUFF条件
	m_NoHasCheck.Close();

	// 通知部件创建效果
	if (m_pBuffPart != NULL)
	{
		m_pBuffPart->OnStartBuff(this);
	}
	
	return true;
}

/** BUFF ID
@param   
@param   
@return  
*/
DWORD CBuff::GetBuffID(void)
{
	return m_dwBuffID;
}

/** 等级
@param   
@param   
@return  
*/
DWORD CBuff::GetBuffLevel(void)
{
	return m_dwBuffLevel;
}

/** 标志
@param   
@param   
@return  
*/
DWORD CBuff::GetFlag(void)
{
	return m_dwSchemeFlag;
}

/** 添加者
@param   
@param   
@return  
*/
UID CBuff::GetAdd(void)
{
	return m_uidAdd;
}

/** 
@param   
@param   
@return  
*/
DWORD CBuff::GetIndex(void)
{
	return m_dwIndex;
}

/** 
@param   
@param   
@return  
*/
DWORD CBuff::GetTotalTime(void)
{
	return m_dwTime;
}

/** 剩余时间
@param   
@param   
@return －1：表示时间已越过，不用通知客户端创建。0：表示永久时间。正数表示剩余时间 
*/
int CBuff::GetLeftTime(void)
{	
	if(!m_bThisValidFlag || !m_bStartBuffFlag)
	{
		return -1;
	}

	if(m_dwTime <= 0)
	{
		return 0;
	}

	int nLeftTime = m_dwTime - (getTickCountEx() - m_dwStartTick);
	if(nLeftTime <= 0)
	{
		return -1;
	}	

	return nLeftTime;
}

/** 
@param   
@param   
@return  
*/
bool CBuff::GetBuildEffectFlag(void)
{
	return m_bBuildEffectFlag;
}

/** 是否有随机效果
@param   
@param   
@return  
*/
bool CBuff::IsRandEffect(void)
{
	return m_bRandEffect;
}

/** 本BUFF是否有效
@param   
@param   
@return  
*/
bool CBuff::IsValid(void)
{
	return m_bThisValidFlag;
}

/** 判断某个效果是否存在
@param   dwEffectID:效果ID
@param  lIgnoreUid:忽略的添加者
@return  
*/
bool CBuff::IsEffectIDExist(DWORD dwEffectID, UID lIgnoreUid)
{
	TLIST_EFFECT::iterator iter = m_listEffect.begin();
	for (; iter != m_listEffect.end(); ++iter)
	{
		if ((*iter)->GetID() == dwEffectID && lIgnoreUid != m_uidAdd)
		{
			return true;
		}
	}

	return false;
}

Vector3 CBuff::GetEffectTargetTile()
{
	return m_effectContext.ptTargetTile;
}

int CBuff::GetSpellID()
{
	return m_effectContext.nID;
}

bool CBuff::AddBuffOnlyReplaceTime(DWORD dwBuffLevel, DWORD dwTime)
{
	SBuffSchemeInfo * pSchemeInfo = g_pBuffServer->GetBuffSchemeInfo(m_dwBuffID);
	if (pSchemeInfo == NULL)
	{
		return false;
	}

	SBuffSchemeInfo::SLevelInfo * pLevelInfo = NULL;
	list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchemeInfo->listLevelInfo.begin();
	for (; it != pSchemeInfo->listLevelInfo.end(); ++it)
	{
		if ((*it).dwBuffLevel == dwBuffLevel)
		{
			pLevelInfo = &(*it);
			break;
		}
	}
	if (pLevelInfo == NULL)
	{
		return false;
	}

	// 等级
	m_dwBuffLevel = (WORD)dwBuffLevel;

	// 重新设置新的时间
	if (dwTime <= 0)
	{
		m_dwTime = pLevelInfo->dwTimeLong;
	}
	else
	{
		m_dwTime = dwTime;
	}

	if (m_dwTime > 0)
	{
		// Kill生命定时器
		g_ExternalFacade.KillTimer(ETimerEventID_Life, this);

		char szDebug[32] = { 0 };
		ssprintf_s(szDebug, sizeof(szDebug), "CBuff::ResetTime %d", m_dwBuffID);
		g_ExternalFacade.SetTimer(ETimerEventID_Life, m_dwTime, this, szDebug);
	}

	// 启动时间
	m_dwStartTick = getTickCountEx();

	return true;

}
