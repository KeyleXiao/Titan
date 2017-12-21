/*******************************************************************
** 文件名:	EffectKillAddBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/22/2015
** 版  本:	1.0
** 描  述:	效果-击杀获得BUFF
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
#include "IAttackObject.h"
#include "buffer.h"
#include "ICamp.h"
#include "IGameSceneService.h"
class CEffectKillAddBuff : public IEffectEx,public IEventExecuteSink
{
public:
	typedef  EffectServer_KillAddBuff    SCHEME_DATA;

	CEffectKillAddBuff( SCHEME_DATA &data ) : m_data(data),m_pEntity(0),m_dwLastTick(0),m_BuffIndex(0)
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

		m_BuffIndex=context->dwIndex;
		DWORD dwSceneID = m_pEntity->getSceneID();
		SceneLogicEventHelper eventHelper(dwSceneID);
		IEventEngine *pGlobalEvent = eventHelper.m_ptr;
		if (pGlobalEvent == NULL)
		{
			ErrorLn(_GT("CEffectKillAddBuff Start 取得全局事件失败，问题严重！"));
            return false;
		}
		if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
		{
			// 注册英雄死亡事件
			pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0, "CEffectKillAddBuff");
			// 注册怪物死亡事件
			pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0, "CEffectKillAddBuff");
			// 注册Tank死亡事件
			pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_TANK, 0, "CEffectKillAddBuff");
		}
		else
		{
			// 注册类型死亡事件
			pGlobalEvent->Subscibe(this, EVENT_GAME_ENTITY_DIE, m_data.nEntityType, 0, "CEffectKillAddBuff");
		}

		return true;
	}


	// 效果停止
	virtual void			Stop()
	{
		if (m_pEntity != 0)
		{
			DWORD dwSceneID = m_pEntity->getSceneID();
			SceneLogicEventHelper eventHelper(dwSceneID);
			IEventEngine *pGlobalEvent = eventHelper.m_ptr;
			if (pGlobalEvent != NULL)
			{
				if (m_data.nEntityType == TYPE_ENTITY || m_data.nEntityType == TYPE_MAX)
				{
					// 注销英雄死亡事件
					pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0);
					// 注销怪物死亡事件
					pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0);
					// 注销Tank死亡事件
					pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, TYPE_TANK, 0);
				}
				else
				{
					// 注册类型死亡事件
					pGlobalEvent->UnSubscibe(this, EVENT_GAME_ENTITY_DIE, m_data.nEntityType, 0);
				}
			}

			m_pEntity = 0;
		}
	}
	 
	// 克隆一个新效果
	virtual IEffect *       Clone()
	{
		return new CEffectKillAddBuff(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		switch (wEventID)
		{
		case EVENT_GAME_ENTITY_DIE:
			{
				if (pszContext == NULL || nLen != sizeof(event_entity_die))
				{
					return;
				}
				if (m_pEntity == NULL)
				{
					return;
				}

				event_entity_die *pDie = (event_entity_die *)pszContext;

				int	nSpellEffectType = pDie->nSpellEffectType;
				// 技能效果类型匹配
				if ((m_data.nSpellEffectType & nSpellEffectType) != m_data.nSpellEffectType)
				{
					return;
				}

				DWORD dwTick = getTickCountEx();
				if (dwTick < m_dwLastTick + m_data.nInterval)
				{
					return;
				}

				UID uidSelf = m_pEntity->getUID();
				if (isInvalidUID(uidSelf))
				{
					return;
				}

				UID uidOperator = pDie->uidOperator;
				if (isInvalidUID(uidOperator))
				{
					return;
				}

				UID uidTarget = pDie->uidTarget;
				if (isInvalidUID(uidTarget))
				{
					return;
				}

				// 检测击杀目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nKillTargetFilter, uidOperator))
				{
					return;
				}

				// 检测死亡目标
				if (!g_EHelper.chooseTarget(m_pEntity, m_data.nDieTargetFilter, uidTarget))
				{
					return;
				}

				UID uidCheck = INVALID_UID;
				switch (m_data.nKillTagetLocType)
				{
				case KILL_TARGET_LOC_KILL:
					{
						uidCheck = uidOperator;
					}
					break;
				case KILL_TARGET_LOC_DIE:
					{
						uidCheck = uidTarget;
					}
					break;
				default:
					{
						ErrorLn("CEffectKillAddBuff error nKillTagetLocType="<<m_data.nKillTagetLocType);
						return;
					}
					break;
				}

				int nRandNum = 0;
				rktRangeRand(nRandNum, 1, EFFECT_PROB_VALUE);
				if(nRandNum > m_data.nProb)
				{
					return;
				}

                SBuffContext BuffContext;
                BuffContext.nID = pDie->nSpellID;
			
				//附加buff目标类型选择
				switch (m_data.nAddBuffTargetType)
				{
				case ADD_BUFF_TARGET_SELF:
					{
						if (!addBuffCheckDistance(uidCheck, uidSelf))
						{
							return;
						}
						
						// 给buff添加者添加buff
						AddBuff(uidSelf, m_data.nBuffID, m_data.nBuffLevel,uidSelf, 0, &BuffContext, sizeof(BuffContext) );
					}
					break;
				case ADD_BUFF_TARGET_KILLTARGET:
					{
						if (!addBuffCheckDistance(uidCheck, uidTarget))
						{
							return;
						}

						// 给buff添加者添加buff
						AddBuff(uidTarget, m_data.nBuffID, m_data.nBuffLevel,uidTarget, 0, &BuffContext, sizeof(BuffContext) );
					}
					break;
				case ADD_BUFF_TARGET_OPERATOR:
					{
						if (!addBuffCheckDistance(uidCheck, uidOperator))
						{
							return;
						}
						
						// 给buff添加者添加buff
						AddBuff(uidOperator, m_data.nBuffID, m_data.nBuffLevel,uidOperator, 0, &BuffContext, sizeof(BuffContext) );
					}
					break;
				case ADD_BUFF_TARGET_OPERATORAROUND:
					{
						//获得实体
						int nSceneID = UID_2_SCENE(uidSelf);
						SceneServiceHelper sceneHelper(nSceneID);
						if ( sceneHelper.m_ptr==0 )
						{
							return;
						}

						Vector3 vCheckLoc = getLocation(uidCheck);
						// 取得一定范围内的所有实体
						AOI_PROXY_PTR pProxyArray[MAX_INTEREST_OBJECT];
						int num = sceneHelper.m_ptr->k_nearest(vCheckLoc,m_data.fDieLocRange, pProxyArray, MAX_INTEREST_OBJECT, LAYER_ALL, true);
						if ( num<=0 )
						{
							return;
						}

                        // 增加BUFF数量
                        int nAddBuffCount = 0;
                        int nMaxAddBuffCount = m_data.nAddBuffCount == 0 ? EFFECT_CHOOSE_TARGET_COUNT : m_data.nAddBuffCount;
                        UID uidMonsterAddBuff[MAX_INTEREST_OBJECT] = {0};
                        DWORD dwMonsterCount = 0;
						obuf256 data;
						for ( int i=0;i<num;++i )
						{
							// 目标UID
							UID uidAddTarget = pProxyArray[i]->m_uid;
							if (isInvalidUID(uidAddTarget))
							{
								continue;
							}

                            if (nAddBuffCount >= nMaxAddBuffCount)
                            {
                                break;
                            }

							// 死亡状态，如果死亡也可以加buff需要将此判断屏蔽
							if (isDieState(uidAddTarget))
							{
								continue;
							}

							// 检测buff添加者
							//m_data.nAddTargetFilter
							if (!g_EHelper.chooseTarget(m_pEntity, m_data.nAddTargetFilter, uidAddTarget))
							{
								continue;
							}

                            if (isMonster(uidAddTarget))
                            {
                                uidMonsterAddBuff[dwMonsterCount++] = uidAddTarget;
                            }
                            else
                            {
							    // 给buff添加者添加buff
							    AddBuff(uidAddTarget, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
                            }

                            ++nAddBuffCount;
						}

                        if (dwMonsterCount > 0)
                        {
                            g_EHelper.BatchAddBuff(uidMonsterAddBuff, dwMonsterCount, m_data.nBuffID, m_data.nBuffLevel, uidSelf, 0, &BuffContext, sizeof(BuffContext));
                        }
					}
					break;
				default:
					{
						ErrorLn("CEffectKillAddBuff error nAddBuffTargetType="<<m_data.nAddBuffTargetType);
						return;
					}
					break;
				}
				// 删除buff,配置中默认1为删除,自己死亡移除buff
				if(m_data.nIsBuffRemove&&uidTarget==uidSelf)
				{
					
					RemoveBuffByIndex(uidSelf,m_BuffIndex);
				}
				// 设置上次时间
				m_dwLastTick = dwTick;
			}
			break;
		default:
			break;
		}	
	}

	bool addBuffCheckDistance(UID uidCheck, UID uidAddCheck)
	{
		// 小于0表示配置不需要检测距离
		if (m_data.fDieLocRange <= 0.01f)
		{
			return true;
		}

		Vector3 vCheckLoc = getLocation(uidCheck);
		Vector3 vAddCheckLoc = getLocation(uidAddCheck);
		if (!vCheckLoc.checkDistance(vAddCheckLoc, m_data.fDieLocRange))
		{
			return false;
		}

		return true;
	}
private:
	SCHEME_DATA             m_data;

	// 实体指针
	__IEntity				*m_pEntity;

	// 上次时间
	DWORD					m_dwLastTick;
	// buff索引
	DWORD                   m_BuffIndex;
};
