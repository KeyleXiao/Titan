/*******************************************************************
** 文件名:	PlayerRoleProperty.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			玩家属性集对象
********************************************************************/

#include "stdafx.h"
#include "PlayerRoleProperty.h"
#include "ITimerService.h"
#include "DamageDef.h"
#include "ExternalHelper.h"

// 静态角色等级
#define PROPERTY_ACTOR_LEVEL        100000

// 类的静态成员需要实例化下
DATA_CHUNK_DECLARE(PlayerRoleProperty);

STATIC_INITIALIZE( EntityPart_RoleProperty );


//////////////////////////////////////////////////////////////////////////
// 构造函数
EntityPart_RoleProperty::EntityPart_RoleProperty()
{
    memcpy(&m_PropertyDeclare, &s_PropertyDeclare, sizeof(m_PropertyDeclare));
    // 初始化最后衰减时间
    m_dwLastDecayTick = 0;
    // 初始化衰减状态
    m_bDecayState = false;

    // 普通属性上次更新时间
    m_dwLastNormUpdateTick = 0;
}


// 析构函数
EntityPart_RoleProperty::~EntityPart_RoleProperty()
{

}


bool EntityPart_RoleProperty::onLoad(__IEntity* pEntity, int nLoginMode)
{
    EntityProperty::onLoad(pEntity, nLoginMode);

    if (pEntity)
    {
        __IEntityEvent *pEntityEvent = pEntity->getEntityEvent();
        if (pEntityEvent)
        {
            // 订阅 玩家静态角色等级改变事件
            pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ACTOR_CHANGE_LEVEL, __FUNCTION__);
        }
    }

    return true;
}

// 切换英雄
void EntityPart_RoleProperty::onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore)
{
    EntityProperty::onChangeHero(nOldHeroID, nNewHeroID, isBefore);

    if (isBefore)
    {
        // 魔法类型释放
        onMagicRelease();
        // 血量魔法回复释放
        onHpMpReplyRelease();

        ITimerService *pTimerService = gServerGlobal->getTimerService();
        if (pTimerService != NULL)
        {
            // 删除 设置自动增长定时器
            pTimerService->KillTimer(SELF_CHECK_TIMER, this);

            // 删除 龙珠实体属性请求定时器
            pTimerService->KillTimer(ENTITY_PROPERTY_TIMER, this);
        }
    }
    else
    {
        onEntityLoadFinish();
    }
}

void EntityPart_RoleProperty::onEntityLoadFinish()
{
	ITimerService *pTimerService = gServerGlobal->getTimerService();
	if (pTimerService != NULL)
	{
		// 设置自动增长定时器
		pTimerService->SetTimer(SELF_CHECK_TIMER, SELF_CHECK_INTVAL, this, INFINITY_CALL, __FILE__);
	}

	// 更新基础属性
	OnPropertyChange(PROPERTY_LEVEL, 0, 0);	

	// todo 现在还区分不了切进程还是创建人物，目前把血量设置最大血量
	// 设置血量
	setProperty_Integer(PROPERTY_HP, getProperty_Integer(PROPERTY_MAX_HP));

	// 设置法术
	setProperty_Integer(PROPERTY_MP, getProperty_Integer(PROPERTY_MAX_MP));

	// 设置体力
	setProperty_Integer(PROPERTY_STAMINA, getProperty_Integer(PROPERTY_MAX_STAMINA));

	// 魔法类型初始化
	onMagicInit();

	// 血量魔法回复初始化
	onHpMpReplyInit();

	// 设置属性限制
	onSetPropertyLimit();

    // 设置英雄基础属性
    onSetPersonBaseProp();
}

//////////////////////////////////////////////////////////////////////////
// 取得属性优先级
BYTE EntityPart_RoleProperty::getPropPriority(int prop_id)
{
    // 策划要求等级、血量、魔法值、攻击速度、移动速度、皮肤、天赋点 改变的时候，需要立即刷新。
    // 貌似这几个属性需要统一处理了。加这儿自己刷新一下。
    // 调用这个接口尽量在更上层使用，要不然影响比较大。
    if ( prop_id == PROPERTY_LEVEL|| prop_id == PROPERTY_HP     || prop_id == PROPERTY_MAX_HP
            || prop_id == PROPERTY_MP || prop_id == PROPERTY_MAX_MP || prop_id == PROPERTY_PASD || prop_id == PROPERTY_AF
            || prop_id == PROPERTY_SD || prop_id == PROPERTY_SKIN   || prop_id == PROPERTY_TALENT_POINT
            || prop_id == PROPERTY_AIM_DISTANCE || prop_id == PROPERTY_SHIELD || prop_id == PROPERTY_MASK 
            || prop_id == PROPERTY_BULLET_COUNT || PROPERTY_ENERGY
       )
       return PROP_PRIO_HIGH;
    else
        return PROP_PRIO_NORMAL;
}

/** 处理广播属性
@remark
1、精度要求不高的属性变化时，直接进入延迟池，等待时间点触发广播下去
2、精度要求高
  1）非血量直接广播
  2）血量更新：如果周围实体比较多时，将延迟一下广播下去，否则直接广播
  3）血量更新：血量位0是将直接进入广播，不用延迟
*/
void EntityPart_RoleProperty::onPropertyBroadcast(int prop_id)
{
	// 移动属性不用同步，会在MOVE_PART里同步
	if(prop_id==PROPERTY_LOCATION || PROPERTY_ANGLE == prop_id )
		return;

    if( getPropPriority(prop_id) == PROP_PRIO_NORMAL )
    {
        m_arrayNormalPropID.insert(prop_id);
        return;
    }

    // 人物自己血量实时下发，通过给其他人
    if (prop_id == PROPERTY_HP)
    {
        std::set<int> notBroadcastProerties;
        notBroadcastProerties.insert(prop_id);
        // 不广播其他人，实时下发给自己客户端
        bool isBroadcast = false;
        onBroadcastChangeProperties(&notBroadcastProerties, isBroadcast);
    }


    // 插入到队列中
    m_arrayHighPropID.insert(prop_id);
	
	// 如果是紧急属性修改，则发一个可覆盖的消息给AOI模块处理，这样如果瞬间修改多个紧急属性，也只触发一次广播
	UID uidMyUID = INVALID_UID;
	getProperty_custom(PROPERTY_UID,&uidMyUID,sizeof(uidMyUID) );

	SNetMsgHead head;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;	
	head.byDestEndPoint= MSG_ENDPOINT_SCENE;	
	head.byKeyModule   = MSG_MODULEID_ENTITY;	
	head.byKeyAction   = SS_MSG_ENTITY_PROPERTY_CHANGE;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = uidMyUID;			// 实体UID
	entityHead.byPartID = PART_PROPERTY;
	
	obuf256 data;
	data << head << entityHead;

	unsigned long ulKey = UID_2_SERIAL(uidMyUID);
	ulKey = (ulKey << 8) | SS_MSG_ENTITY_PROPERTY_CHANGE;
	handleMessageEx(uidMyUID,ulKey,data.data(),data.size());
}

// 执行广播属性
void EntityPart_RoleProperty::doBroadcast(EPROPERTY_PRIORITY priority)
{
    std::set<int> * pArray = (priority == PROP_PRIO_NORMAL ? &m_arrayNormalPropID : &m_arrayHighPropID);

    // 默认都是广播出去
    bool isBroadcast = true;
    onBroadcastChangeProperties(pArray, isBroadcast);
}

void EntityPart_RoleProperty::onBroadcastChangeProperties(std::set<int> * pPropertyArray, bool isBroadcast)
{
    if(pPropertyArray==NULL || pPropertyArray->empty())
    {
        return;
    }

    event_entity_properties event_head;
    event_head.isBroadcast = isBroadcast;
    event_head.nPropCount = pPropertyArray->size();

    obuf event_data;
    event_data.push_back( &event_head, sizeof(event_head) );

    for(std::set<int>::const_iterator it = pPropertyArray->begin(); it != pPropertyArray->end(); ++it)
    {
        const int &nPropID = *it;
        event_data << nPropID;
    }

    pPropertyArray->clear();

    __IEntityEvent* pEventEngine = m_pEntity->getEntityEvent();
    if(pEventEngine != NULL)
    {
        pEventEngine->FireExecute(EVENT_ENTITY_PROPERTY_BROADCAST, event_data.data(), event_data.size());
    }
}

//如果需要回写数据千万要注意修改了data的内容后len值的改变 需要考虑 字符串 bool int short byte 类型的不同 具体可以参看写入数据时的实现
void EntityPart_RoleProperty::OnPropertyChange(int prop_id, void * data,int32 len)
{
	if (m_pEntity != NULL)
	{
		// 实体还未创建，不发属性事件，因为其会广播数据到客户端
		if (m_bCreateEntity)
		{
			// 发起属性改变事件
			event_entity_property_change context;
			context.nPropertyID = prop_id;
            
            if(len <= sizeof(context.data))
            {
                context.nLen = len;
                memcpy(context.data, data, len);
            }
            else
            {
                ErrorLn("property data len > 128, property=" << prop_id);
            }
            
			m_pEntity->getEntityEvent()->FireExecute((ENTITYEVENT_KEY)(EVENT_ENTITY_PROPERTY_CHANGE + prop_id),(LPCSTR)&context,sizeof(context));

            // 执行属性同步
            onSyncShareProperty(prop_id);

            // 执行属性广播
            onPropertyBroadcast(prop_id);
		}


        // 校验皮肤ID是否合法(DEBUG版本)
#ifdef ENTITY_CHECK_SKIN
        if(prop_id == PROPERTY_SKIN)
        {
            int nSkin = *((int*)(((char*)&m_data) + m_PropertyDeclare[PROPERTY_SKIN].nDataOffset));

            if(!gServerGlobal->getSchemeCenter()->getSchemeSkinInfo()->getSkinShemeInfo(nSkin))
            {
                ErrorLn(__FUNCTION__":["<< m_pEntity->getName() <<"] skin is invalid, pdbid="<< getProperty_Integer(PROPERTY_ID)
                    <<", heroid="<< getProperty_Integer(PROPERTY_VOCATION) <<", skin="<< nSkin << ", scene="<< getProperty_Integer(PROPERTY_SCENEID));

                __debugbreak();
            }
        }
#endif

	}

	switch(prop_id)
	{
	case PROPERTY_EXP://暂时只需要处理添加经验以后改变等级的内容
		{
			OnPropertyChange_Exp_After(data,len);
		}
		break;
		// 基础血量
	case PROPERTY_BASE_HP:
		// 附加血量
	case PROPERTY_APPEND_HP:
		// 附加百分比血量
	case PROPERTY_APPEND_PCT_HP:
		{
			int nOldMaxHP = getProperty_Integer(PROPERTY_MAX_HP);
			int nOldHP = getProperty_Integer(PROPERTY_HP);
			OnPropertyChange_SetBaseData(PROPERTY_BASE_HP, PROPERTY_APPEND_HP, PROPERTY_APPEND_PCT_HP, PROPERTY_MAX_HP);
			if (nOldMaxHP != 0)
			{
				int nMaxHP = getProperty_Integer(PROPERTY_MAX_HP);
				int nNowHP = min(nMaxHP, int(nMaxHP * (float(nOldHP) / nOldMaxHP) + 0.5));

                // 血量上限加了 当前血量也相应的加一点。 上限减少了 当前血量只要不超过上限就不扣血了
                if(nNowHP > nOldHP)
                {
                    setProperty_Integer(PROPERTY_HP, nNowHP);
                }
			}			
		}
		break;
		// 基础魔法
	case PROPERTY_BASE_MP:
		// 附加魔法
	case PROPERTY_APPEND_MP:
		// 附加百分比魔法
	case PROPERTY_APPEND_PCT_MP:
		{
			int nOldMaxMP = getProperty_Integer(PROPERTY_MAX_MP);
			int nOldMP = getProperty_Integer(PROPERTY_MP);
			OnPropertyChange_SetBaseData(PROPERTY_BASE_MP, PROPERTY_APPEND_MP, PROPERTY_APPEND_PCT_MP, PROPERTY_MAX_MP);
			if (nOldMaxMP != 0)
			{
				int nMaxMP = getProperty_Integer(PROPERTY_MAX_MP);
				int nNowMP = min(nMaxMP, int(nMaxMP * (float(nOldMP) / nOldMaxMP) + 0.5));
				setProperty_Integer(PROPERTY_MP, nNowMP);
			}
		}
		break;
        // 基础能量
    case PROPERTY_BASE_ENERGY:
        // 附加能量
    case PROPERTY_APPEND_ENERGY:
        // 附加百分比能量
    case PROPERTY_APPEND_PCT_ENERGY:
        {
            int nOldMaxEnergy = getProperty_Integer(PROPERTY_MAX_ENERGY);
            int nOldEnergy = getProperty_Integer(PROPERTY_ENERGY);
            OnPropertyChange_SetBaseData(PROPERTY_BASE_ENERGY, PROPERTY_APPEND_ENERGY, PROPERTY_APPEND_PCT_ENERGY, PROPERTY_MAX_ENERGY);
            if (nOldEnergy != 0)
            {
                int nMaxEnergy = getProperty_Integer(PROPERTY_MAX_ENERGY);
                int nNowEnergy = min(nMaxEnergy, int(nMaxEnergy * (float(nOldEnergy) / nOldMaxEnergy) + 0.5));
                setProperty_Integer(PROPERTY_ENERGY, nNowEnergy);
            }
        }
        break;
		// 基础物理攻击
	case PROPERTY_BASE_PA:
		// 附加物理攻击
	case PROPERTY_APPEND_PA:
		// 附加百分比物理攻击
	case PROPERTY_APPEND_PCT_PA:
		{
			OnPropertyChange_SetBaseData(PROPERTY_BASE_PA, PROPERTY_APPEND_PA, PROPERTY_APPEND_PCT_PA, PROPERTY_PA);
		}
		break;
		// 基础法术攻击
	case PROPERTY_BASE_MA:
		// 附加法术攻击
	case PROPERTY_APPEND_MA:
		// 附加百分比法术攻击
	case PROPERTY_APPEND_PCT_MA:
		{
			OnPropertyChange_SetBaseData(PROPERTY_BASE_MA, PROPERTY_APPEND_MA, PROPERTY_APPEND_PCT_MA, PROPERTY_MA);
		}
		break;
		// 基础物理防御
	case PROPERTY_BASE_PD:
		// 附加物理防御
	case PROPERTY_APPEND_PD:
		// 附加百分比物理防御
	case PROPERTY_APPEND_PCT_PD:
		{
			OnPropertyChange_SetBaseData(PROPERTY_BASE_PD, PROPERTY_APPEND_PD, PROPERTY_APPEND_PCT_PD, PROPERTY_PD);
		}
		break;
		// 基础法术防御
	case PROPERTY_BASE_MD:
		// 附加法术防御
	case PROPERTY_APPEND_MD:
		// 附加百分比法术防御
	case PROPERTY_APPEND_PCT_MD:
		{
			OnPropertyChange_SetBaseData(PROPERTY_BASE_MD, PROPERTY_APPEND_MD, PROPERTY_APPEND_PCT_MD, PROPERTY_MD);
		}
		break;
		// 基础移动速度
	case PROPERTY_BASE_SD:
		// 附加移动速度
	case PROPERTY_APPEND_SD:
		// 附加百分比移动速度
	case PROPERTY_APPEND_PCT_SD:
		{ 
			OnPropertyChange_SetBaseData(PROPERTY_BASE_SD, PROPERTY_APPEND_SD, PROPERTY_APPEND_PCT_SD, PROPERTY_SD);
		}
		break;
		// 基础攻击速度
	case PROPERTY_BASE_PASD:
		// 附加攻击速度
	case PROPERTY_APPEND_PASD:
		// 附加百分比攻击速度
	case PROPERTY_APPEND_PCT_PASD:
		{
			OnPropertyChange_SetBaseData(PROPERTY_BASE_PASD, PROPERTY_APPEND_PASD, PROPERTY_APPEND_PCT_PASD, PROPERTY_PASD);
		}
		break;
        // 基础普攻系数
    case PROPERTY_BASE_AF:
        // 附加普攻系数
    case PROPERTY_APPEND_AF:
        {
            // 此属性特殊一点,具体见附加百分比系数算法
            OnPropertyChange_SetAppendPCTBaseData(PROPERTY_BASE_AF, PROPERTY_APPEND_AF, PROPERTY_APPEND_PCT_AF, PROPERTY_AF);
        }
        break;
		// 附加百分比普攻系数
	case PROPERTY_APPEND_PCT_AF:
		{
			OnPropertyChange_SetPCTData(PROPERTY_APPEND_PCT_AF, PROPERTY_PCT_AF);
			// 此属性特殊一点,具体见附加百分比系数算法
			OnPropertyChange_SetAppendPCTBaseData(PROPERTY_BASE_AF, PROPERTY_APPEND_AF, PROPERTY_APPEND_PCT_AF, PROPERTY_AF);
		}
		break;
		// 基础暴击率
	case PROPERTY_BASE_CRC:
		// 附加暴击率
	case PROPERTY_APPEND_CRC:
		// 附加百分比暴击率
	case PROPERTY_APPEND_PCT_CRC:
		{
			OnPropertyChange_SetBaseData(PROPERTY_BASE_CRC, PROPERTY_APPEND_CRC, PROPERTY_APPEND_PCT_CRC, PROPERTY_CRC);
		}
		break;
		// 基础血量回复
	case PROPERTY_BASE_HP_REPLY:
		// 附加血量回复
	case PROPERTY_APPEND_HP_REPLY:
		// 附加百分比血量回复
	case PROPERTY_APPEND_PCT_HP_REPLY:
		{
			OnPropertyChange_SetBaseData(PROPERTY_BASE_HP_REPLY, PROPERTY_APPEND_HP_REPLY, PROPERTY_APPEND_PCT_HP_REPLY, PROPERTY_HP_REPLY);
		}
		break;
		// 基础魔法回复
	case PROPERTY_BASE_MP_REPLY:
		// 附加魔法回复
	case PROPERTY_APPEND_MP_REPLY:
		// 附加百分比魔法回复
	case PROPERTY_APPEND_PCT_MP_REPLY:
		{
			OnPropertyChange_SetBaseData(PROPERTY_BASE_MP_REPLY, PROPERTY_APPEND_MP_REPLY, PROPERTY_APPEND_PCT_MP_REPLY, PROPERTY_MP_REPLY);
		}
		break;
        // 基础能量回复
    case PROPERTY_BASE_ENERGY_REPLY:
        // 附加能量回复
    case PROPERTY_APPEND_ENERGY_REPLY:
        // 附加百分比能量回复
    case PROPERTY_APPEND_PCT_ENERGY_REPLY:
        {
            OnPropertyChange_SetBaseData(PROPERTY_BASE_ENERGY_REPLY, PROPERTY_APPEND_ENERGY_REPLY, PROPERTY_APPEND_PCT_ENERGY_REPLY, PROPERTY_ENERGY_REPLY);
        }
        break;
		// 基础百分比冷却缩减
	case PROPERTY_BASE_PCT_COOL_REDUCE:
		{
			setProperty_Integer(PROPERTY_PCT_COOL_REDUCE, getProperty_Integer(PROPERTY_BASE_PCT_COOL_REDUCE));
		}
		break;
		// 基础韧性
	case PROPERTY_BASE_ENACITY:
		{
			setProperty_Integer(PROPERTY_TENACITY, getProperty_Integer(PROPERTY_BASE_ENACITY));
		}
		break;
        // 基础伤害加成(固定值)
    case PROPERTY_BASE_DAMAGE_BONUS:
        {
            setProperty_Integer(PROPERTY_DAMAGE_BONUS, getProperty_Integer(PROPERTY_BASE_DAMAGE_BONUS));
        }
        break;
        // 基础伤害加成百分比
    case PROPERTY_BASE_DAMAGE_BONUS_PCT:
        {
            setProperty_Integer(PROPERTY_DAMAGE_BONUS_PCT, getProperty_Integer(PROPERTY_BASE_DAMAGE_BONUS_PCT));
        }
        break;
		// 等级改变
	case PROPERTY_LEVEL:
		{
			// 计算基础属性-基础血量
			OnCalculate_Base(PROPERTY_BASE_HP);
			// 计算基础属性-基础魔法
			OnCalculate_Base(PROPERTY_BASE_MP);
			// 计算基础属性-基础物理攻击
			OnCalculate_Base(PROPERTY_BASE_PA);
			// 计算基础属性-基础法术攻击
			OnCalculate_Base(PROPERTY_BASE_MA);
			// 计算基础属性-基础物理防御
			OnCalculate_Base(PROPERTY_BASE_PD);
			// 计算基础属性-基础法术防御
			OnCalculate_Base(PROPERTY_BASE_MD);
			// 计算基础属性-基础速度
			OnCalculate_Base(PROPERTY_BASE_SD);
			// 计算基础属性-基础攻击速度
			OnCalculate_Base(PROPERTY_BASE_PASD);
            // 计算基础属性-基础普攻系数
            OnCalculate_Base(PROPERTY_BASE_AF);
			// 计算基础属性-暴击率
			OnCalculate_Base(PROPERTY_BASE_CRC);
			// 计算基础属性-基础血量回复
			OnCalculate_Base(PROPERTY_BASE_HP_REPLY);
			// 计算基础属性-基础魔法回复
			OnCalculate_Base(PROPERTY_BASE_MP_REPLY);
			// 计算基础属性-最大体力
			OnCalculate_Base(PROPERTY_MAX_STAMINA);

            // 等级改变，影响此属性
            OnPropertyChange_SetAppendPCTBaseData(PROPERTY_BASE_AF, PROPERTY_APPEND_AF, PROPERTY_APPEND_PCT_AF, PROPERTY_AF);

			// 计算此百分比属性
			OnPropertyChange_SetPCTData(PROPERTY_APPEND_PCT_AF, PROPERTY_PCT_AF);

			// 发送等级改变事件
			if(m_pEntity != NULL)
			{
				event_entity_upgrade_level context;
				context.uChangedUID = m_pEntity->getUID();
				context.nLevel =  getProperty_Integer(PROPERTY_LEVEL);

                __IEntityEvent * pEntityEvent = m_pEntity->getEntityEvent();
                if(pEntityEvent)
                {
				    pEntityEvent->FireExecute(EVENT_ENTITY_UPGRADE_LEVEL,(LPCSTR)&context,sizeof(context));
                    pEntityEvent->FireExecute(EVENT_GAME_ACTOR_UPGRADE_LEVEL,(LPCSTR)&context,sizeof(context));
                }
			}
		}
		break;
	case PROPERTY_MAX_HP:
		{
            setCurLimitProperty(PROPERTY_HP, PROPERTY_MAX_HP);
		}
		break;
	case PROPERTY_MAX_MP:
		{
            setCurLimitProperty(PROPERTY_MP, PROPERTY_MAX_MP);
		}
		break;
	case PROPERTY_MAX_STAMINA:
		{
            setCurLimitProperty(PROPERTY_STAMINA, PROPERTY_MAX_STAMINA);
		}
		break;
	case PROPERTY_MAX_ENERGY:
		{
            setCurLimitProperty(PROPERTY_ENERGY, PROPERTY_MAX_ENERGY);
		}
		break;
	default:
		break;
	}
}

/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
/** 容器事件触发后执行函数
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  
*/
void EntityPart_RoleProperty::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	// 执行父类事件
	EntityProperty::OnExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen);

	switch (wEventID)
	{
		// 攻击事件
	case EVENT_ENTITY_ATTACK:
		{
			// 增加魔法
			addMagic(m_MagicType.nHitValue);

			// 设置最后衰减时间
			m_dwLastDecayTick = getTickCountEx();
			// 设置非衰减状态
			m_bDecayState = false;
		}
		break;
    case EVENT_GAME_ENTITY_DIE:
		{
			if (m_pEntity == NULL)
			{
				break;
			}
			if (pszContext == NULL || nLen != sizeof(event_entity_die))
			{
				break;
			}
			event_entity_die *pDie = (event_entity_die *)pszContext;
			// 不是英雄击杀
			if (m_pEntity->getUID() != pDie->uidOperator)
			{
				break;
			}
			// 死亡者UID
			UID uidTarget = pDie->uidTarget;
			// 增加值
			int nValue = 0;
			// 实体类型
			int nType = UID_2_TYPE(uidTarget);
			if (nType == TYPE_PLAYER_ROLE)
			{
				nValue = m_MagicType.nKillRoleValue;
			}
			else if (nType == TYPE_MONSTER)
			{
				nValue = m_MagicType.nKillMonsterValue;
			}
			// 增加魔法
			addMagic(nValue);

			// 设置最后衰减时间
			m_dwLastDecayTick = getTickCountEx();
			// 设置非衰减状态
			m_bDecayState = false;
		}
		break;

    case EVENT_ENTITY_ACTOR_CHANGE_LEVEL:   // 玩家静态角色等级改变事件
        {
            if (nLen == sizeof(event_actor_change_level))
            {
                event_actor_change_level * pEvent = CAST_TYPE(event_actor_change_level *, pszContext);
                onSyncShareProperty(PROPERTY_ACTOR_LEVEL, pEvent->nLevel);   // 执行属性同步
            }
        }
        break;

	default:
		break;
	}
}


/////////////////////////////////////////TimerHandler/////////////////////////////////////////
// 定时器到了
void EntityPart_RoleProperty::OnTimer( unsigned long dwTimerID )
{
	switch (dwTimerID)
	{
		// 自动增长定时器
	case AUTO_GROW_TIMER:
		{
			// 增加魔法
			addMagic(m_MagicType.nAutoGrowValue);
		}
		break;
		// 自动衰减定时器
	case AUTO_DECAY_TIMER:
		{
			int nAutoDecayValue = 0;
			// 取得当前时间
			DWORD dwTick = getTickCountEx();
			if (!m_bDecayState)
			{
				// 没到衰减时间
				if (dwTick < m_dwLastDecayTick + m_MagicType.nAutoDecayTime)
				{
					break;
				}
				nAutoDecayValue = m_MagicType.nAutoDecayValue;
			}
			else
			{
				// 没到二次衰减时间
				if (dwTick < m_dwLastDecayTick + m_MagicType.nSecondaryAutoDecayTime)
				{
					break;
				}
				nAutoDecayValue = m_MagicType.nSecondaryAutoDecayValue;
			}

			// 增加魔法
			addMagic(nAutoDecayValue);

			// 设置最后衰减时间
			m_dwLastDecayTick = dwTick;
			// 设置衰减状态
			m_bDecayState = true;
		}
		break;
		// 血量魔法回复定时器
	case HP_MP_REPLY_TIMER:
		{
			if (m_pEntity == NULL)
			{
				return;
			}
			// 死亡状态不处理
			if (m_pEntity->getState() == ENTITY_STATE_DIE)
			{
				return;
			}

			UID uid = m_pEntity->getUID();
			// 取得血量回复
			int nHpReply = getProperty_Integer(PROPERTY_HP_REPLY);
			// 取得魔法回复
			int nMpReply = getProperty_Integer(PROPERTY_MP_REPLY);

			// 治疗消息
			msg_entity_cure cure;
			cure.uidOperator = uid;
			cure.uidTarget = uid;
			cure.nCureHP = nHpReply;
			cure.nCureMP = nMpReply;

			// 发送实体消息
			g_EHelper.sendEntityMessage(uid, PART_DAMAGE, DAMAGE_MSG_CURE, (char *)&cure, sizeof(cure));
		}
		break;

    case SELF_CHECK_TIMER:      // 自检定时器
        {            
            DWORD dwCurTick = getTickCount();
            
            // 普通属性广播
            if(dwCurTick > m_dwLastNormUpdateTick + 1000)
            {
                m_dwLastNormUpdateTick = dwCurTick;
                doBroadcast(PROP_PRIO_NORMAL);
            }
        }
        break;

	case ENTITY_PROPERTY_TIMER:		// 龙珠指定实体属性定时器
		{
			sendClientMsgEntityProperty();
		}
		break;

	default:
		break;
	}
}

bool EntityPart_RoleProperty::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch ( msgId )
    {
	case SS_MSG_ENTITY_PROPERTY_CHANGE:
		doBroadcast(PROP_PRIO_HIGH);
		break;
	case CS_MSG_ENTITY_PROPERTY_REQ_DRAGONBALL:
		{
			if ( nullptr == data)
			{
				ErrorLn("EntityPart_RoleProperty::onClientMsg() failed! [len]" << len );
				return false;
			}

			onClientMsgEntityProperty(data, len );
		}
		break;
	case CS_MSG_ENTITY_PROPERTY_REQ_DRAGONBALL_CLOSE:
		{
			onClientMsgEntityPropertyClose();
		}
		break;
	default:
		break;
	}

	return true;
}

void EntityPart_RoleProperty::OnPropertyChange_Exp_After(void * data,int32 len)
{
	int nExp = getProperty_Integer(PROPERTY_EXP);
	ISchemeLevelUpgrade* pSchemeLevelUpgrade = gServerGlobal->getSchemeCenter()->getLevelUpgradeSheme();
	if (pSchemeLevelUpgrade == NULL)
	{
		return ;
	}
	int nMaxLevel = pSchemeLevelUpgrade->GetSchemeLevelUpgradeMaxLevel();
	int nOldLevel = getProperty_Integer(PROPERTY_LEVEL);
	for (int nLevel = nOldLevel+1; nLevel <= nMaxLevel; nLevel++)
	{
		int nNeedExp = pSchemeLevelUpgrade->GetLevelAllExp(nLevel);
		if (nNeedExp >= 0 && nExp >= nNeedExp)
		{
			setProperty_Integer(PROPERTY_LEVEL,nLevel);	
		}
		else
		{
			break;
		}
	}
}


// 属性改变，数据限制，设置数据，通用公式【(基础值 + 附加值之和) * (1 + 附加百分比值之和)】
void EntityPart_RoleProperty::OnPropertyChange_SetBaseData(int nBaseID, int nAppendID, int nAppendPCTID, int nChangeID)
{
	// 取得基础值
	int nBase = getProperty_Integer(nBaseID);
	// 取得附加值
	int nAppend = getProperty_Integer(nAppendID);
	// 取得附加百分比值
	int nAppendPCT = getProperty_Integer(nAppendPCTID);

	// 当前值＝（基础值+固定值之和）*（1+百分比值之和）
	int nCur =(nBase + nAppend) * (1+ nAppendPCT/ZOOM_IN_MULTIPLE)/BASE_IN_MULTIPLE;

    // 是速度的时候，速度衰减
    if (nChangeID == PROPERTY_SD)
    {
        // 设置速度衰减
        onSetSpeedDecay(nCur);
    }
	// 设置改变值
	setProperty_Integer(nChangeID, nCur);
}

// 属性改变,设置附加百分比基础数据
void EntityPart_RoleProperty::OnPropertyChange_SetAppendPCTBaseData(int nBaseID, int nAppendID, int nAppendPCTID, int nChangeID)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return;
    }
    // 取得配置人物基础属性对象
    ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
    if (pSchemePersonBaseProp == NULL)
    {
        return;
    }

    int nPropType = BASE_PROP_TYPE;
    int nMapID = m_pEntity->getMapID();
    if (IsMainMap(nMapID))
    {
        nPropType = BASE_PROP_HKEE_TYPE;
    }
    // 职业
    int nVocation = getProperty_Integer(PROPERTY_VOCATION);
    SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
    if (pPersonBasePropScheme == NULL)
    {
        return;
    }

    // 属性索引
    int nPropIndex = -1;
    for (int nIndex=0; nIndex<PERSON_APPEND_PCT_PROP_COUNT; ++nIndex)
    {
        if (pPersonBasePropScheme->sAppendPCTProp[nIndex].nPropID == nAppendPCTID)
        {
            nPropIndex = nIndex;
            break;
        }
    }
    if (nPropIndex < 0 || nPropIndex >= PERSON_APPEND_PCT_PROP_COUNT)
    {
        return;
    }

    // 等级
    int nLevel = getProperty_Integer(PROPERTY_LEVEL);
    // 基础值 = 等级基础值 + (等级-1)*基础值成长率
    int nAppendPCTBase = pPersonBasePropScheme->sAppendPCTProp[nPropIndex].nBase + (nLevel-1)*pPersonBasePropScheme->sAppendPCTProp[nPropIndex].fGrowRate;
    
    // 取得基础值
    int nBase = getProperty_Integer(nBaseID);
    // 取得附加值
    int nAppend = getProperty_Integer(nAppendID);
    // 取得附加百分比值
    int nAppendPCT = getProperty_Integer(nAppendPCTID) + nAppendPCTBase;

    // 当前值＝（基础值+固定值之和）*（1+百分比值之和）
    int nCur =(nBase + nAppend) * (1+ nAppendPCT/ZOOM_IN_MULTIPLE)/BASE_IN_MULTIPLE;

    // 设置改变值
    setProperty_Integer(nChangeID, nCur);
}

void EntityPart_RoleProperty::OnPropertyChange_SetPCTData(int nAppendPCTID, int nChangeID)
{
	if (m_pEntity == NULL)
	{
		return;
	}

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	// 取得配置人物基础属性对象
	ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
	if (pSchemePersonBaseProp == NULL)
	{
		return;
	}

	int nPropType = BASE_PROP_TYPE;
	int nMapID = m_pEntity->getMapID();
	if (IsMainMap(nMapID))
	{
		nPropType = BASE_PROP_HKEE_TYPE;
	}
	// 职业
	int nVocation = getProperty_Integer(PROPERTY_VOCATION);
	SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
	if (pPersonBasePropScheme == NULL)
	{
		return;
	}

	// 属性索引
	int nPropIndex = -1;
	for (int nIndex = 0; nIndex<PERSON_APPEND_PCT_PROP_COUNT; ++nIndex)
	{
		if (pPersonBasePropScheme->sAppendPCTProp[nIndex].nPropID == nAppendPCTID)
		{
			nPropIndex = nIndex;
			break;
		}
	}
	if (nPropIndex < 0 || nPropIndex >= PERSON_APPEND_PCT_PROP_COUNT)
	{
		return;
	}

	// 等级
	int nLevel = getProperty_Integer(PROPERTY_LEVEL);
	// 基础值 = 等级基础值 + (等级-1)*基础值成长率
	int nAppendPCTBase = pPersonBasePropScheme->sAppendPCTProp[nPropIndex].nBase + (nLevel - 1)*pPersonBasePropScheme->sAppendPCTProp[nPropIndex].fGrowRate;

	// 取得附加百分比值
	int nAppendPCT = getProperty_Integer(nAppendPCTID) + nAppendPCTBase;

	// 设置改变值
	setProperty_Integer(nChangeID, nAppendPCT);
}

// 属性改变，设置附加数据，通用公式【附加值之和 * (1 + 附加百分比值之和)】
void EntityPart_RoleProperty::OnPropertyChange_SetAppendData(int nAppendID, int nAppendPCTID, int nChangeID)
{
	// 取得附加值
	int nAppend = getProperty_Integer(nAppendID);
	// 取得附加百分比值
	int nAppendPCT = getProperty_Integer(nAppendPCTID);

	// 当前值＝ 附加值之和*(1+附加百分比值之和)
	int nCur = nAppend * (1 + nAppendPCT/ZOOM_IN_MULTIPLE);

	// 设置改变值
	setProperty_Integer(nChangeID, nCur);
}


// 计算基础属性
void EntityPart_RoleProperty::OnCalculate_Base(int nBaseID)
{
    if (m_pEntity == NULL)
    {
        return;
    }
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	// 取得配置人物基础属性对象
	ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
	if (pSchemePersonBaseProp == NULL)
	{
		return;
	}

    int nPropType = BASE_PROP_TYPE;
    int nMapID = m_pEntity->getMapID();
    if (IsMainMap(nMapID))
    {
        nPropType = BASE_PROP_HKEE_TYPE;
    }
	// 职业
	int nVocation = getProperty_Integer(PROPERTY_VOCATION);
	SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
	if (pPersonBasePropScheme == NULL)
	{
		return;
	}

	// 属性索引
	int nPropIndex = -1;
	for (int nIndex=0; nIndex<PERSON_BASE_PROP_COUNT; ++nIndex)
	{
		if (pPersonBasePropScheme->sProp[nIndex].nPropID == nBaseID)
		{
			nPropIndex = nIndex;
			break;
		}
	}
	if (nPropIndex < 0 || nPropIndex >= PERSON_BASE_PROP_COUNT)
	{
		return;
	}

	// 等级
	int nLevel = getProperty_Integer(PROPERTY_LEVEL);
	// 基础值 = 等级基础值 + (等级-1)*基础值成长率
	int nBase = pPersonBasePropScheme->sProp[nPropIndex].nBase + (nLevel-1)*pPersonBasePropScheme->sProp[nPropIndex].fGrowRate;

	// 设置改变值
	setProperty_Integer(nBaseID, nBase);
}


// 魔法类型初始化
void EntityPart_RoleProperty::onMagicInit(void)
{
	if (m_pEntity == NULL)
	{
		return;
	}
	__IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return;
	}
	ITimerService *pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
	{
		return;
	}

	DWORD dwSceneID = m_pEntity->getSceneID();
	SceneLogicEventHelper eventHelper(dwSceneID);
	IEventEngine *pGlobalEvent = eventHelper.m_ptr;
	if (pGlobalEvent == NULL)
	{
		return;
	}
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	// 取得配置人物基础属性对象
	ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
	if (pSchemePersonBaseProp == NULL)
	{
		return;
	}

    int nPropType = BASE_PROP_TYPE;
    int nMapID = m_pEntity->getMapID();
    if (IsMainMap(nMapID))
    {
        nPropType = BASE_PROP_HKEE_TYPE;
    }
	// 职业
	int nVocation = getProperty_Integer(PROPERTY_VOCATION);
	SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
	if (pPersonBasePropScheme == NULL)
	{
		return;
	}
	ISchemeMagicType *pSchemeMagicType = pSchemeCenter->getSchemeMagicType();
	if (pSchemeMagicType == NULL)
	{
		return;
	}
	// 取得魔法类型
	int nMagicType = pPersonBasePropScheme->nMagicType;
	// 设置魔法类型
	setProperty_Integer(PROPERTY_MAGIC_TYPE, nMagicType);

	int nType = nMagicType/MAGIC_TYPE_VALUE;
	if (nType > MAGIC_TYPE_NONE && nType < MAGIC_TYPE_MAX)
	{
		// 取得获得能量值方式
		SMagicType *pMagicType = pSchemeMagicType->getMagicType(pPersonBasePropScheme->nEnergyGetMode);
		if (pMagicType != NULL)
		{
			// 注册攻击事件
            pEntityEvent->Subscibe(static_cast<IEventExecuteSink *>(this), EVENT_ENTITY_ATTACK, "nHitValue");

			// 击杀怪物增加值
			if (pMagicType->nKillMonsterValue != 0)
			{
				// 注册怪物死亡事件
				pGlobalEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0, "nKillMonsterValue");
			}

			// 击杀英雄增加值
			if (pMagicType->nKillRoleValue != 0)
			{
				// 注册英雄死亡事件
				pGlobalEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0, "nKillRoleValue");
			}

			// 助攻事件，暂无，不实现todo

			// 自动增长定时器
			if (pMagicType->nAutoGrowTime > 0 && pMagicType->nAutoGrowValue > 0)
			{
				// 设置自动增长定时器
				pTimerService->SetTimer(AUTO_GROW_TIMER, pMagicType->nAutoGrowTime, this, INFINITY_CALL, __FILE__);
			}

			// 自动衰减定时器
			if (pMagicType->nAutoDecayTime > 0 && pMagicType->nAutoDecayValue < 0)
			{
				// 设置最后衰减时间
				m_dwLastDecayTick = getTickCountEx();

				// 设置自动衰减定时器
				pTimerService->SetTimer(AUTO_DECAY_TIMER, AUTO_DECAY_TIME, this, INFINITY_CALL, __FILE__);
			}

			if (nType == MAGIC_TYPE_ENERGY)
			{
				// 设置最大能量
				setProperty_Integer(PROPERTY_MAX_ENERGY, pMagicType->nLimitVale);
				// 取得当前能量
				setProperty_Integer(PROPERTY_ENERGY, pMagicType->nInitialValue);
			}

			// 保存魔法类型
			m_MagicType = *pMagicType;
		}
	}
}


// 魔法类型释放
void EntityPart_RoleProperty::onMagicRelease(void)
{
	if (m_pEntity == NULL)
	{
		return;
	}
	__IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		return;
	}
	ITimerService *pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
	{
		return;
	}
	DWORD dwSceneID = m_pEntity->getSceneID();
	SceneLogicEventHelper eventHelper(dwSceneID);
	IEventEngine *pGlobalEvent = eventHelper.m_ptr;
	if (pGlobalEvent == NULL)
	{
		return;
	}
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	// 取得配置人物基础属性对象
	ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
	if (pSchemePersonBaseProp == NULL)
	{
		return;
	}

    int nPropType = BASE_PROP_TYPE;
    int nMapID = m_pEntity->getMapID();
    if (IsMainMap(nMapID))
    {
        nPropType = BASE_PROP_HKEE_TYPE;
    }
	// 职业
	int nVocation = getProperty_Integer(PROPERTY_VOCATION);
	SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
	if (pPersonBasePropScheme == NULL)
	{
		return;
	}
	ISchemeMagicType *pSchemeMagicType = pSchemeCenter->getSchemeMagicType();
	if (pSchemeMagicType == NULL)
	{
		return;
	}
	// 取得魔法类型
	int nMagicType = getProperty_Integer(PROPERTY_MAGIC_TYPE);
	if (nMagicType != pPersonBasePropScheme->nMagicType)
	{
		// 此处数据不一致，说明值被改过，可能释放不全，问题严重
		ErrorLn("serious problems!!!nMagicType="<<nMagicType<<",pPersonBasePropScheme->nMagicType="<<pPersonBasePropScheme->nMagicType);
	}
	int nType = nMagicType/MAGIC_TYPE_VALUE;
	if (nType < MAGIC_TYPE_NONE || nType >= MAGIC_TYPE_MAX)
	{
		return;
	}
	// 取得魔法类型数据
	SMagicType *pMagicType = pSchemeMagicType->getMagicType(pPersonBasePropScheme->nEnergyGetMode);
	if (pMagicType == NULL)
	{
		return;
	}
	// 注销攻击事件
	pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink *>(this), EVENT_ENTITY_ATTACK);

	// 击杀怪物增加值
	if (pMagicType->nKillMonsterValue != 0)
	{
		// 注销怪物死亡事件
		pGlobalEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, TYPE_MONSTER, 0);
	}

	// 击杀英雄增加值
	if (pMagicType->nKillRoleValue != 0)
	{
		// 注销英雄死亡事件
		pGlobalEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DIE, TYPE_PLAYER_ROLE, 0);
	}

	// 助攻事件，暂无，不实现todo

	// 自动增长定时器
	if (pMagicType->nAutoGrowTime > 0 && pMagicType->nAutoGrowValue > 0)
	{
		// 设置自动增长定时器
		pTimerService->KillTimer( AUTO_GROW_TIMER, this);
	}

	// 自动衰减定时器
	if (pMagicType->nAutoDecayTime > 0 && pMagicType->nAutoDecayValue < 0)
	{
		// 设置自动衰减定时器
		pTimerService->KillTimer( AUTO_DECAY_TIMER, this);
	}
}


// 增加魔法
void EntityPart_RoleProperty::addMagic(int nValue)
{
	if (m_pEntity == NULL)
	{
		return;
	}
	// 死亡状态不处理
	if (m_pEntity->getState() == ENTITY_STATE_DIE)
	{
		return;
	}

	if (nValue == 0)
	{
		return;
	}

	// 取得魔法类型
	int nMagicType = getProperty_Integer(PROPERTY_MAGIC_TYPE);
	int nType = nMagicType/MAGIC_TYPE_VALUE;
	switch (nType)
	{
	case MAGIC_TYPE_MP:
		{
			// 增加法术
			addProperty_Integer(PROPERTY_MP, nValue);
		}
		break;
	case MAGIC_TYPE_ENERGY:
		{
			// 增加能量
			addProperty_Integer(PROPERTY_ENERGY, nValue);
		}
		break;
	default:
		break;
	}
}


// 血量魔法回复初始化
void EntityPart_RoleProperty::onHpMpReplyInit(void)
{
	ITimerService *pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
	{
		return;
	}

	// 设置血量魔法回复定时器
	pTimerService->SetTimer(HP_MP_REPLY_TIMER, HP_MP_REPLY_TIME, this, INFINITY_CALL, __FILE__);
}


// 血量魔法回复释放
void EntityPart_RoleProperty::onHpMpReplyRelease(void)
{
	ITimerService *pTimerService = gServerGlobal->getTimerService();
	if (pTimerService == NULL)
	{
		return;
	}

	// 杀掉血量魔法回复定时器
	pTimerService->KillTimer(HP_MP_REPLY_TIMER, this);
}


// 还原属性
void EntityPart_RoleProperty::destroy()
{
    if (m_pEntity)
    {
        __IEntityEvent *pEntityEvent = m_pEntity->getEntityEvent();
        if (pEntityEvent)
        {
            // 取消订阅 玩家静态角色等级改变事件
            pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ACTOR_CHANGE_LEVEL);
        }
    }

    // 魔法类型释放
    onMagicRelease();
    // 血量魔法回复释放
    onHpMpReplyRelease();

    ITimerService *pTimerService = gServerGlobal->getTimerService();
    if (pTimerService != NULL)
    {
        // 删除 设置自动增长定时器
        pTimerService->KillTimer(SELF_CHECK_TIMER, this);

		// 删除 龙珠实体属性请求定时器
		pTimerService->KillTimer(ENTITY_PROPERTY_TIMER, this);
    }

    EntityProperty::destroy();
}


// 属性限制
void EntityPart_RoleProperty::onSetPropertyLimit(void)
{
    if (m_pEntity == NULL)
    {
        return;
    }

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	// 取得配置人物基础属性对象
	ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
	if (pSchemePersonBaseProp == NULL)
	{
		return;
	}

    int nPropType = BASE_PROP_TYPE;
    int nMapID = m_pEntity->getMapID();
    if (IsMainMap(nMapID))
    {
        nPropType = BASE_PROP_HKEE_TYPE;
    }
	// 职业
	int nVocation = getProperty_Integer(PROPERTY_VOCATION);
	SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
	if (pPersonBasePropScheme == NULL)
	{
		return;
	}

	//循环设置限制属性
	for (int nIndex=0;nIndex<PERSON_PROP_LIMIT_COUNT; ++nIndex)
	{
		setProperty_Limit(pPersonBasePropScheme->sLimitProp[nIndex].nPropID,pPersonBasePropScheme->sLimitProp[nIndex].nMinLimit,pPersonBasePropScheme->sLimitProp[nIndex].nMaxLimit);
	}
}


// 设置英雄基础属性
void EntityPart_RoleProperty::onSetPersonBaseProp(void)
{
    if (m_pEntity == NULL)
    {
        return;
    }
    ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return;
    }
    // 取得配置人物基础属性对象
    ISchemePersonBaseProp *pSchemePersonBaseProp = pSchemeCenter->getShemePersonBaseProp();
    if (pSchemePersonBaseProp == NULL)
    {
        return;
    }

    int nPropType = BASE_PROP_TYPE;
    int nMapID = m_pEntity->getMapID();
    if (IsMainMap(nMapID))
    {
        nPropType = BASE_PROP_HKEE_TYPE;
    }
    // 职业
    int nVocation = getProperty_Integer(PROPERTY_VOCATION);
    SPersonBasePropScheme *pPersonBasePropScheme = pSchemePersonBaseProp->getPersonBaseProp(nVocation, nPropType);
    if (pPersonBasePropScheme == NULL)
    {
        return;
    }

    // 目前只有一个，后面可扩展
    // 设置英雄瞄准距离
    setProperty_Integer(PROPERTY_AIM_DISTANCE, pPersonBasePropScheme->nAimDistance);
}

// 执行属性同步,只同步数值类型属性
void EntityPart_RoleProperty::onSyncShareProperty(int prop_id, int prop_value/* = 0)*/)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    switch(prop_id)
    {
    case PROPERTY_CLAN:                 // 帮会ID
    case PROPERTY_KIN:                  // 战队ID 
    case PROPERTY_FIGHTCAPACITY:        // 玩家战力
    case PROPERTY_TICKET:               // 点券
    case PROPERTY_GOLD:                 // 金币
    case PROPERTY_FACEID:               // 头像ID
    case PROPERTY_TEAMID:               // 队伍
    case PROPERTY_MATCHTYPE:            // 排位类型ID
    case PROPERTY_RANKSCORE:            // 排位得分
    case PROPERTY_RANKGRADE:            // 排位等级
    case PROPERTY_ACTOR_LEVEL:          // 静态角色等级，这个同步有点特殊
    case  PROPERTY_KINGRANK_ORDER:      // 王者排位
        {
            IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
            if (pEventEngine)
            {
                // 执行属性同步
                event_entity_sync_share_property context;
                context.uidMaster   = m_pEntity->getUID();
                context.dwDBID      = getProperty_Integer(PROPERTY_ID); 
                context.nPropertyID = (PROPERTY_ACTOR_LEVEL == prop_id ? PROPERTY_LEVEL : prop_id);
                context.nValue      = (prop_value==0 ? getProperty_Integer(context.nPropertyID) : prop_value);

                pEventEngine->FireExecute(EVENT_GAME_ACTOR_SYNC_PROPERTY, SOURCE_TYPE_PERSON, context.dwDBID, (LPCSTR)&context, sizeof(context));
            }
        }
        break;
    default:
        break;
    }
}

// 设置速度衰减
void EntityPart_RoleProperty::onSetSpeedDecay(int &nValue)
{
    if (m_pEntity == NULL)
    {
        return;
    }

    ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
    if (pSchemeCenter == NULL)
    {
        return;
    }

    ISchemeSpeedDecay *pSchemeSpeedDecay = pSchemeCenter->getSchemeSpeedDecay();
    if (pSchemeSpeedDecay == NULL)
    {
        return;
    }

    int nOldSpeed = nValue;
    SSpeedDecay *pSpeedDecay = pSchemeSpeedDecay->getSpeedDecay(nOldSpeed);
    if (pSpeedDecay == NULL)
    {
        return;
    }

    nValue = nOldSpeed*pSpeedDecay->fDecayFactor + pSpeedDecay->nValue;
}

// F11请求指定实体属性
void EntityPart_RoleProperty::onClientMsgEntityProperty(void* pData, size_t stLen )
{
	if ( stLen < sizeof(msg_entity_property_req_dragonball) )
	{
		ErrorLn(__FUNCTION__": failed! message length is too short! stLen=" << stLen );
		return;
	}

	msg_entity_property_req_dragonball * pReqData = (msg_entity_property_req_dragonball *)pData;

	nReqUID = pReqData->uidReq;

	sendClientMsgEntityProperty();

	// 启动定时器
	ITimerService * pTimerService  = gServerGlobal->getTimerService();
	if(pTimerService != NULL)
	{
		pTimerService->KillTimer(ENTITY_PROPERTY_TIMER, this);
		pTimerService->SetTimer(ENTITY_PROPERTY_TIMER, ENTITY_PROPERTY_INTVAL, this, INFINITY_CALL, __FILE__);
	}
}

// F11发送给客户端指定实体属性
void EntityPart_RoleProperty::sendClientMsgEntityProperty()
{
	msg_entity_property_return_dragonball returnData;
	returnData.uidReq = nReqUID;
	returnData.nHp = ::getProperty_Integer(nReqUID, PROPERTY_HP);
	returnData.nMd = ::getProperty_Integer(nReqUID, PROPERTY_MD);
	returnData.nPa = ::getProperty_Integer(nReqUID, PROPERTY_PA);
	returnData.nPd = ::getProperty_Integer(nReqUID, PROPERTY_PD);

	obuf obData;

	SNetMsgHead head;
	head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.byKeyModule    = MSG_MODULEID_ENTITY;
	head.byKeyAction    = SC_MSG_ENTITY_PROPERTY_RETURN_DRAGONBALL;
	obData << head;

	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pEntity->getUID();
	entityHead.byPartID = PART_PROPERTY;
	obData << entityHead;

	obData.push_back(&returnData, sizeof(returnData));

	send2Client(m_pEntity->getUID(), PACKAGE_PTR(new std::string(obData.data(), obData.size())));
}

// F11指定实体属性关闭
void EntityPart_RoleProperty::onClientMsgEntityPropertyClose()
{
	ITimerService *pTimerService = gServerGlobal->getTimerService();
	if (pTimerService != NULL)
	{
		// 删除 龙珠实体属性请求定时器
		pTimerService->KillTimer(ENTITY_PROPERTY_TIMER, this);
	}
}