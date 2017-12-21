/*******************************************************************
** 文件名:	MonsterProperty.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			NPC,怪物属性集
********************************************************************/

#include "stdafx.h"
#include "MonsterProperty.h"
#include "ITimerService.h"

// 类的静态成员需要实例化下
DATA_CHUNK_DECLARE(MonsterProperty);

STATIC_INITIALIZE( EntityPart_MonsterProperty );

EntityPart_MonsterProperty::EntityPart_MonsterProperty()
{
	memcpy(&m_PropertyDeclare, &s_PropertyDeclare, sizeof(m_PropertyDeclare));
	m_arrayHighPropID.clear();
	m_arrayNormalPropID.clear();
	memset(m_VigourRatio,0,sizeof(m_VigourRatio));
}


EntityPart_MonsterProperty::~EntityPart_MonsterProperty()
{

}

bool EntityPart_MonsterProperty::onLoad(__IEntity* pEntity, int nLoginMode)
{
    EntityProperty::onLoad(pEntity, nLoginMode);

    //ITimerService *pTimerService = gServerGlobal->getTimerService();
    //if (pTimerService != NULL)
    //{
    //    // 设置自动增长定时器
    //    pTimerService->SetTimer(SELF_CHECK_TIMER, SELF_CHECK_INTVAL, this, INFINITY_CALL, __FILE__);
    //}

    return true;
}

void EntityPart_MonsterProperty::release()
{
    //ITimerService *pTimerService = gServerGlobal->getTimerService();
    //if (pTimerService != NULL)
    //{
    //    // 删除 设置自动增长定时器
    //    pTimerService->KillTimer(SELF_CHECK_TIMER, this);
    //}

    EntityProperty::release();
}


// 加载成功时回调
void EntityPart_MonsterProperty::onEntityLoadFinish()
{
	if (m_pEntity == NULL)
	{
		return;
	}

	// 设置满血满魔
	setProperty_Integer(PROPERTY_HP, getProperty_Integer(PROPERTY_MAX_HP));
	setProperty_Integer(PROPERTY_MP, getProperty_Integer(PROPERTY_MAX_MP));
}

//////////////////////////////////////////////////////////////////////////
// 取得属性优先级
BYTE EntityPart_MonsterProperty::getPropPriority(int prop_id)
{
    // 策划要求等级、血量、魔法值、攻击速度、移动速度、皮肤、天赋点 改变的时候，需要立即刷新。
    // 貌似这几个属性需要统一处理了。加这儿自己刷新一下。
    // 调用这个接口尽量在更上层使用，要不然影响比较大。
    if ( prop_id == PROPERTY_LEVEL|| prop_id == PROPERTY_HP     || prop_id == PROPERTY_MAX_HP
        || prop_id == PROPERTY_MP || prop_id == PROPERTY_MAX_MP || prop_id == PROPERTY_PASD || prop_id == PROPERTY_AF
        || prop_id == PROPERTY_SD || prop_id == PROPERTY_SKIN   || prop_id == PROPERTY_TALENT_POINT
        )
        return PROP_PRIO_HIGH;
    else
        return PROP_PRIO_NORMAL;
}

/** 处理广播属性
@remark
1、精度要求不高的属性变化时，直接进入延迟池，等待时间点触发广播下去，
注：怪物不立即广播时和正常属性一起广播，这样可以压缩些广播频率
2、精度要求高
  1）非血量直接广播
  2）血量更新：如果周围实体比较多时，将延迟一下广播下去，否则直接广播
  3）血量更新：血量位0是将直接进入广播，不用延迟
*/
void EntityPart_MonsterProperty::onPropertyBroadcast(int prop_id)
{
    // 控制广播的属性
    if( (getPropertyFlag(prop_id)&TO_CLIENT_OTHER) != TO_CLIENT_OTHER )
    {
        return;
    }

	// 移动属性不用同步，会在MOVE_PART里同步
	if(prop_id==PROPERTY_LOCATION || PROPERTY_ANGLE == prop_id )
		return;

	// 怪物没什么普通优先级的属性需要广播
    //if( getPropPriority(prop_id) == PROP_PRIO_NORMAL )
    //{
    //    m_arrayNormalPropID.insert(prop_id);
    //    return;
    //}

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
void EntityPart_MonsterProperty::doBroadcast(EPROPERTY_PRIORITY priority)
{
    std::set<int> * pArray = (priority == PROP_PRIO_NORMAL ? &m_arrayNormalPropID : &m_arrayHighPropID);
    if(pArray->empty())
    {
        return;
    }

    event_entity_properties event_head;
    event_head.nPropCount = pArray->size();
    // 怪物全部都走广播
    event_head.isBroadcast = true;

    obuf event_data;
    event_data.push_back( &event_head, sizeof(event_head) );

    for(std::set<int>::const_iterator it = pArray->begin(); it != pArray->end(); ++it)
    {
        const int &nPropID = *it;
        event_data << nPropID;
    }
    
    pArray->clear();

    __IEntityEvent* pEventEngine = m_pEntity->getEntityEvent();
    if(pEventEngine != NULL)
    {
        pEventEngine->FireExecute(EVENT_ENTITY_PROPERTY_BROADCAST, event_data.data(), event_data.size());
    }
}

/* 处理 属性事件
@parm:	bBefore为 true时表示 属性写入前调用此接口  
false 表示 属性写入后调用此接口
@parm:  data 写入前为 相应要修改的属性的值的地址写入后没有使用
*/
void EntityPart_MonsterProperty::OnPropertyChange(int prop_id, void * data,int32 len)
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

            // 执行属性广播
            onPropertyBroadcast(prop_id);
		}
	}

	switch(prop_id)
	{
	case PROPERTY_EXP://暂时只需要处理添加经验以后改变等级的内容
		{
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
				setProperty_Integer(PROPERTY_HP, nNowHP);
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
        // 附加百分比普攻系数
    case PROPERTY_APPEND_PCT_AF:
        {
            // 策划要求,怪物先不改
            OnPropertyChange_SetBaseData(PROPERTY_BASE_AF, PROPERTY_APPEND_AF, PROPERTY_APPEND_PCT_AF, PROPERTY_AF);
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
			//OnCalculate_Base(PROPERTY_BASE_HP_REPLY);
			// 计算基础属性-基础魔法回复
			//OnCalculate_Base(PROPERTY_BASE_MP_REPLY);
			// 计算基础属性-最大体力
			//OnCalculate_Base(PROPERTY_MAX_STAMINA);
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

	// 属性改变，设置数据，通用公式【(基础值 + 附加值之和) * (1 + 附加百分比值之和)】
void EntityPart_MonsterProperty::OnPropertyChange_SetBaseData(int nBaseID, int nAppendID, int nAppendPCTID, int nChangeID)
{
	// 取得基础值
	int nBase = getProperty_Integer(nBaseID);
	// 取得附加值
	int nAppend = getProperty_Integer(nAppendID);
	// 取得附加百分比值
	int nAppendPCT = getProperty_Integer(nAppendPCTID);

	// 当前值＝（基础值+固定值之和）*（1+百分比值之和）
	int nCur = (nBase + nAppend) * (1 + nAppendPCT/ZOOM_IN_MULTIPLE)/BASE_IN_MULTIPLE;
	// 取得上次此值
	int nLast = getProperty_Integer(nChangeID);
	// 不同时才改变
	if (nCur != nLast)
	{
		// 设置改变值
		setProperty_Integer(nChangeID, nCur);
	}
}
	// 属性改变，设置附加数据，通用公式【附加值之和 * (1 + 附加百分比值之和)】
void EntityPart_MonsterProperty::OnPropertyChange_SetAppendData(int nAppendID, int nAppendPCTID, int nChangeID)
{
	// 取得附加值
	int nAppend = getProperty_Integer(nAppendID);
	// 取得附加百分比值
	int nAppendPCT = getProperty_Integer(nAppendPCTID);

	// 当前值＝ 附加值之和*(1+附加百分比值之和)
	int nCur = nAppend * (1 + nAppendPCT/ZOOM_IN_MULTIPLE);
	// 取得上次此值
	int nLast = getProperty_Integer(nChangeID);
	// 不同时才改变
	if (nCur != nLast)
	{
		// 设置改变值
		setProperty_Integer(nChangeID, nCur);
	}
}
	// 计算基础属性
void EntityPart_MonsterProperty::OnCalculate_Base(int nBaseID)
{
	ISchemeMonster* pSchemeMonstor = gServerGlobal->getSchemeCenter()->getSchemeMonster();
	if (pSchemeMonstor == NULL)
	{
		return ;
	}
	int nMonsterID = getProperty_Integer( PROPERTY_ID );
	SMonsterScheme * pSchemeInfo = pSchemeMonstor->getMonsterShemeInfo(nMonsterID);
	if (pSchemeInfo == NULL)
	{
		return;
	}
	int nBaseVal = 0;
	int nBaseIndex = 0;
	int nLevel = getProperty_Integer(PROPERTY_LEVEL);
	if (nLevel <=0 )
	{
		return;
	}

	// 基础值 = 等级基础值 + (等级-1)*基础值成长率
	int nVigourRatio = 0;
	switch(nBaseID)
	{
	case PROPERTY_BASE_HP:		
		nBaseVal = pSchemeInfo->nMaxHP + int((nLevel-1)*pSchemeInfo->fAddHP);	
		nVigourRatio = m_VigourRatio[VPT_ADD_HP];
		break;
	case PROPERTY_BASE_MP:
		nBaseVal = pSchemeInfo->nMaxMP + int((nLevel-1)*pSchemeInfo->fAddMP);		
		break;
	case PROPERTY_BASE_PA:
		nBaseVal = pSchemeInfo->nPA + int((nLevel-1)*pSchemeInfo->fAddPA);	
		nVigourRatio = m_VigourRatio[VPT_ADD_PA];
		break;
	case PROPERTY_BASE_MA:
		nBaseVal = pSchemeInfo->nMA + int((nLevel-1)*pSchemeInfo->fAddMA);		
		break;
	case PROPERTY_BASE_PD:
		nBaseVal = pSchemeInfo->nPD + int((nLevel-1)*pSchemeInfo->fAddPD);		
		break;
	case PROPERTY_BASE_MD:
		nBaseVal = pSchemeInfo->nMD + int((nLevel-1)*pSchemeInfo->fAddMD);		
		break;
	case PROPERTY_BASE_PASD:
		nBaseVal = pSchemeInfo->nASD + int((nLevel-1)*pSchemeInfo->fAddAS);		
		break;
    case PROPERTY_BASE_AF:
        nBaseVal = pSchemeInfo->nASD + int((nLevel-1)*pSchemeInfo->fAddAS);		
        break;
	case PROPERTY_BASE_CRC:
		nBaseVal = pSchemeInfo->nCRC + int((nLevel-1)*pSchemeInfo->fAddCRC);		
		break;
    case PROPERTY_BASE_SD:
        nBaseVal = pSchemeInfo->nMoveSpeed;
        break;
	default:
		ErrorLn(__FUNCTION__<<" Not Find The BaseIndex:"<<nBaseID);
		return;
	}	

	nBaseVal *= (1+(float)nVigourRatio/100);
	// 取得上次此值
	int nValOld = getProperty_Integer(nBaseID);
	if (nBaseVal != nValOld)
	{
		setProperty_Integer(nBaseID, nBaseVal);
	}
}


//////////////////////////////TimerHandler/////////////////////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
//void EntityPart_MonsterProperty::OnTimer( unsigned long dwTimerID )
//{
//    switch (dwTimerID)
//    {
//    case SELF_CHECK_TIMER:       // 自检定时器
//        {
//            // 普通属性广播
//            doBroadcast(PROP_PRIO_NORMAL);
//            // 优先级高的属性
//            doBroadcast(PROP_PRIO_HIGH);
//        }
//        break;
//
//    default:
//        break;
//    }
//}

bool EntityPart_MonsterProperty::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch ( msgId )
    {
	case SS_MSG_ENTITY_PROPERTY_CHANGE:
		doBroadcast(PROP_PRIO_HIGH);
		break;
	default:
		break;
	}

	return true;
}

bool EntityPart_MonsterProperty::setVigourRatio(int* pVigourRatio)
{
	for (int i = VPT_ADD_HP;i<VPT_ADD_MAX;++i)
	{
		m_VigourRatio[i] = pVigourRatio[i];
	}

	// 更新基础属性
	OnPropertyChange(PROPERTY_LEVEL, 0, 0);	
	return true;
}