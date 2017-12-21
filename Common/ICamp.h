/*******************************************************************
** 文件名:	ICamp.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	2/11/2015
** 版  本:	1.0
** 描  述:	

			阵营管理
********************************************************************/

#pragma once

#include "SpellDef.h"
#include "EntityDef.h"
#include "IEntity.h"
#include "CampDef.h"

#ifndef ENTITY_CLIENT
typedef __IEntity  IEntity;
typedef __IEntityProperty IEntityProperty;
#endif



// 暂时只支持最简单的阵营管理
class Camp
{
public :
	/**
	检测阵营:
	@param nTargetFilter : 目标过滤类型
	@param uidSelf: 自身UID
	@param nSelfCamp: 自身阵营
	@param uidTarget: 目标UID
	@param nTargetCamp: 目标阵营
	*/
	static bool TestTargetCamp( int nTargetFilter,UID uidSelf,int nSelfCamp,UID uidTarget,int nTargetCamp, int nMonsterType, int nReserve)
	{
		// 取得死亡状态
		bool bDie = getDie(uidTarget);
		if (!bDie)
		{
			// 不可对活体使用
			if ((nTargetFilter&TARGET_NO_ALIVE_USE) == TARGET_NO_ALIVE_USE)
			{
				return false;
			}
		}
		else
		{
			// 可对尸体使用不满足
			if ((nTargetFilter&TARGET_BODY_USE) != TARGET_BODY_USE)
			{
				return false;
			}
		}

		// 自己
		if ((nTargetFilter&TARGET_SELF) == TARGET_SELF)
		{
			if (uidTarget == uidSelf)
				return true;
		}

		// 目标类型
		int nTargetType = UID_2_TYPE(uidTarget);
		if (nSelfCamp == nTargetCamp)
		{
			if (nTargetType==TYPE_PLAYER_ROLE)
			{
				// 友方英雄
				if ((nTargetFilter&TARGET_FRIENDLY_ROLE) == TARGET_FRIENDLY_ROLE)
				{
					// 不是自己时
					if (uidTarget != uidSelf)
					{
						return true;
					}
				}
			}
			else
			{
                // 友方小兵
                if ((nTargetFilter&TARGET_FRIENDLY_SOLDIER) == TARGET_FRIENDLY_SOLDIER)
                {
                    if (nReserve == MONSTER_SUB_TYPE_SOLDIER || nReserve == MONSTER_SUB_TYPE_WILD_MONSTER)
                        return true;
                }

                // 友方召唤物
                if ((nTargetFilter&TARGET_FRIENDLY_SUMMON_ENTITY) == TARGET_FRIENDLY_SUMMON_ENTITY)
                {
                    if (nReserve == MONSTER_SUB_TYPE_SUMMON_ENTITY 
                        || nReserve == MONSTER_SUB_TYPE_EYE_STONE
                        || nReserve == MONSTER_SUB_TYPE_SMALL_DRAGON
                        || nReserve == MONSTER_SUB_TYPE_HUGE_DRAGON)
                        return true;
                }

                // 友方推塔小龙
                if ((nTargetFilter&TARGET_FRIENDLY_SMALL_DRAGON) == TARGET_FRIENDLY_SMALL_DRAGON)
                {
                    if (nReserve == MONSTER_SUB_TYPE_SUMMON_SMALL_DRAGON)
                        return true;
                }

				// 友方堡垒
				if ((nTargetFilter&TARGET_FRIENDLY_TOWER) == TARGET_FRIENDLY_TOWER)
				{
					if (nMonsterType == MONSTER_TYPE_TOWER)
						return true;
				}
			}
		}
		else
		{
			// 是英雄
			if (nTargetType==TYPE_PLAYER_ROLE)
			{
				// 敌方英雄
				if ((nTargetFilter&TARGET_ENEMY_ROLE) == TARGET_ENEMY_ROLE)
				{
					if (nTargetCamp != CAMP_NEUTRAL)
						return true;
				}
				// 中立英雄
				if ((nTargetFilter&TARGET_NEUTRAL_ROLE) == TARGET_NEUTRAL_ROLE)
				{
					if (nTargetCamp == CAMP_NEUTRAL)
						return true;
				}
			}
			else if (nTargetType==TYPE_MONSTER)
			{
                // 怪物子类型
                int nMonsterSubType = nReserve;

				if (nTargetCamp != CAMP_NEUTRAL)
				{
					// 敌方小兵
					if ((nTargetFilter&TARGET_ENEMY_SOLDIER) == TARGET_ENEMY_SOLDIER)
					{
						if (nMonsterSubType == MONSTER_SUB_TYPE_SOLDIER || nMonsterSubType == MONSTER_SUB_TYPE_WILD_MONSTER)
							return true;
					}

                    // 敌方召唤物
                    if ((nTargetFilter&TARGET_ENEMY_SUMMON_ENTITY) == TARGET_ENEMY_SUMMON_ENTITY)
                    {
                        if (nMonsterSubType == MONSTER_SUB_TYPE_SUMMON_ENTITY 
                            || nMonsterSubType == MONSTER_SUB_TYPE_SMALL_DRAGON
                            || nMonsterSubType == MONSTER_SUB_TYPE_HUGE_DRAGON
                            || nMonsterSubType == MONSTER_SUB_TYPE_EYE_STONE)
                            return true;
                    }

                    // 敌方推塔小龙
                    if ((nTargetFilter&TARGET_ENEMY_SMALL_DRAGON) == TARGET_ENEMY_SMALL_DRAGON)
                    {
                        if (nMonsterSubType == MONSTER_SUB_TYPE_SUMMON_SMALL_DRAGON)
                            return true;
                    }

					// 敌方堡垒
					if ((nTargetFilter&TARGET_ENEMY_TOWER) == TARGET_ENEMY_TOWER)
					{
						if (nMonsterType == MONSTER_TYPE_TOWER)
							return true;
					}
					
				}
				else
				{
					// 中立小怪
					if ((nTargetFilter&TARGET_NEUTRAL_LITTLE_MONSTER) == TARGET_NEUTRAL_LITTLE_MONSTER)
					{
						if (nMonsterSubType == MONSTER_SUB_TYPE_WILD_MONSTER 
                            || nMonsterSubType == MONSTER_SUB_TYPE_SOLDIER
                            || nMonsterSubType == MONSTER_SUB_TYPE_EYE_STONE
                            || nMonsterSubType == MONSTER_SUB_TYPE_SUMMON_SMALL_DRAGON
                            || nMonsterSubType == MONSTER_SUB_TYPE_SUMMON_ENTITY)
							return true;
					}

					// 中立堡垒
					if ((nTargetFilter&TARGET_NEUTRAL_TOWER) == TARGET_NEUTRAL_TOWER)
					{
						if (nMonsterType == MONSTER_TYPE_TOWER)
							return true;
					}

                    // 中立小龙
                    if ( ((nTargetFilter&TARGET_NEUTRAL_SMALL_DRAGON) == TARGET_NEUTRAL_SMALL_DRAGON) && ( nMonsterSubType == MONSTER_SUB_TYPE_SMALL_DRAGON) )
                    {
                        //TraceLn(__FUNCTION__": TARGET_SMALL_DRAGON");
                        return true;
                    }
                    // 中立大龙
                    if ( ((nTargetFilter&TARGET_NEUTRAL_HUGE_DRAGON) == TARGET_NEUTRAL_HUGE_DRAGON) && ( nMonsterSubType == MONSTER_SUB_TYPE_HUGE_DRAGON) )
                    {
                        //TraceLn(__FUNCTION__": TARGET_HUGE_DRAGON");
                        return true;
                    }
				}
			}
		}

		return false;
	}


	// 返回2个实体之间的阵营关系
	static CampFlag getCampFlag( IEntity * pSelf,IEntity * pTarget )
	{
		if ( pSelf == 0 )
			return CampFlag_Unknow;

		if ( pSelf==pTarget )
			return CampFlag_Self;

		int nSelfCamp = 0 ;
		int nTargetCamp = 0;
        IEntityProperty *pProperty = (IEntityProperty*)pSelf->getEntityPart(PART_PROPERTY);
        if ( pProperty!=0 )
        {
            nSelfCamp = pProperty->getProperty_Integer( PROPERTY_CAMP );
        }

		pProperty = (IEntityProperty*)pTarget->getEntityPart(PART_PROPERTY);
		if ( pProperty!=0 )
		{
			nTargetCamp = pProperty->getProperty_Integer( PROPERTY_CAMP );
		}

		if ( nSelfCamp==nTargetCamp )
			return CampFlag_Friend;
		else
			return CampFlag_Enemy;
	}


	// 返回实体是否死亡
	static bool getDie(UID uid)
	{
#ifndef ENTITY_CLIENT
		// 取得实体死亡状态
		if (isDieState(uid))
		{
			return true;
		}
#else
		IEntityFactory *pEntityWorld = gClientGlobal->getEntityFactory();
		if(pEntityWorld == NULL)
		{
			return false;
		}

		IEntity *pEntity = pEntityWorld->getEntity(uid);
		if (pEntity == NULL)
		{
			return false;
		}
		IClientEntity *pClientEntity = (IClientEntity *)pEntity;

		return pClientEntity->getState() == ENTITY_STATE_DIE;
#endif
		return false;
	}
};
