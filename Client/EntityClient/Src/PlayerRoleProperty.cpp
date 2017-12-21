/*******************************************************************
** 文件名:	EntityProperty.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

玩家属性集对象
********************************************************************/

#include "stdafx.h"
#include "PlayerRoleProperty.h"
#include "IClientEntity.h"
#include "ISpell.h"
#include "ICamp.h"
#include "ISchemeCenter.h"
#include "IFreezeClient.h"
#include "IBankPart.h"


// 类的静态成员需要实例化下
DATA_CHUNK_DECLARE(PlayerRoleProperty);

STATIC_INITIALIZE( EntityPart_PlayerRoleProperty );

EntityPart_PlayerRoleProperty::EntityPart_PlayerRoleProperty()
{
    memcpy(&m_PropertyDeclare, &s_PropertyDeclare, sizeof(m_PropertyDeclare));

    // 最后一次HP更新到显示层Tick
    m_dwLastViewHPTick = 0;
}


EntityPart_PlayerRoleProperty::~EntityPart_PlayerRoleProperty()
{

}

// 更新显示层属性信息(参数为PROPERTY_MAX表示更新所有属性到显示层)
void EntityPart_PlayerRoleProperty::updateView( int prop_id )
{
    if ( prop_id==PROPERTY_MAX )
    {
        for ( int i=0;i<PROPERTY_MAX;++i )
        {
            updateView( i );
        }
        return;
    }

    // 同步属性到显示层
    onUpdateViewPropertyValue( prop_id );
}


// 同步属性到显示层
void EntityPart_PlayerRoleProperty::onUpdateViewPropertyValue(int prop_id)
{
	ENTITY_PROPERTY ePropertyClientMaxLimit = m_pEntity->isHero() ? PROPERTY_MAX:PROPERTY_CLIENT_MAX;
	// 过滤掉不用同步到显示层的属性
	if(prop_id >= ePropertyClientMaxLimit)
	{
		return;
	}

	if(m_pEntity == nullptr)
	{
		return;
	}
	int viewID =  CAST_TYPE(IClientEntity* ,m_pEntity)->getViewID();
	if ( viewID==INVALID_ENTITY_ID )
	{
		return;
	}

	// 设置数值属性
    entity_update_Numproperty updatePropData;
    updatePropData.nOtherData = 0;		// 状态数据
    updatePropData.uID = viewID;        //显示层的实体ID 
    updatePropData.isIntproperty = 0;
    updatePropData.isRaiseIprEvent = 0;

    //如果是人物，取得PBUID并且赋值，如果以后怪物也需要PBUID，则删掉判断
    int uidSelf=m_pEntity->getUID();
    if(UID_2_TYPE(uidSelf)==TYPE_PLAYER_ROLE)
    {
        int PBUID=getProperty_Integer(PROPERTY_ID);
        updatePropData.nPBUID=PBUID;
    }

    // 通告显示层更新
    switch (prop_id)
    {
    case PROPERTY_NAME:			// 名字
        {
            const char * name = getProperty_String( PROPERTY_NAME );
            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SYNC_NAME,0,name,0,0 );
        }
        break;
        //数值属性处理
        // 统一使用 ENTITY_TOVIEW_UPDATE_INTEGERPROPERTY消息 来更新
        // 需要实时更新的将使用消息 IntPropertyResponseEventHanlder iprEvent 通知显示层更新属性
        // 数值属性分成两类  需要实时更新  不需要实时更新

        // 需要实时更新的属性	
        //包含其他处理数值属性
    case PROPERTY_SKIN:			// 皮肤
        {
            updatePropData.nPropID = prop_id;	// 属性ID
            updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
            updatePropData.nOtherData = -1;//暂时写死-1，以后再定义 
            updatePropData.isIntproperty = 1;
            updatePropData.isRaiseIprEvent = 1;

            // 发送技能改变事件
            UID uid = m_pEntity->getUID();
            BYTE type  = UID_2_TYPE(uid);

            event_entity_skin_changed eventData;
            eventData.uChangedUID = uid;
            gClientGlobal->getEventEngine()->FireExecute(EVENT_ENTITY_SKIN_CHANGED, type, uid, (LPCSTR)&eventData, sizeof(eventData));

        }
        break;
    case PROPERTY_HP:			// 血量
    case PROPERTY_MAX_HP:
    case PROPERTY_CAMP:			// 阵营
    case PROPERTY_SHIELD:		// 护盾（临时血量）
        {
            updatePropData.nPropID = prop_id;	// 属性ID
            updatePropData.nValue  =  getProperty_Integer(prop_id);	// 属性数值
            updatePropData.nOtherData = m_pEntity->isHero()? CampFlag_Self : Camp::getCampFlag( gClientGlobal->getHero(),m_pEntity );;	// 属性数值
            updatePropData.isIntproperty = 1;
            updatePropData.isRaiseIprEvent = 1;
        }
        break;
    case PROPERTY_EXP:			// 经验	
        {
            updatePropData.nPropID = prop_id;	// 属性ID
            updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
            updatePropData.isIntproperty= 1;
            updatePropData.isRaiseIprEvent = 1;

            // 主角时获取一下升级最大经验
            if (m_pEntity->isHero() || (gClientGlobal->getHero() && gClientGlobal->getHero()->getIntProperty(PROPERTY_OBSERVER)))
			{
                int nCurLevel = getProperty_Integer(PROPERTY_LEVEL);
				ISchemeCenter* pSchemeCenter = gClientGlobal->getSchemeCenter();
				if (pSchemeCenter == NULL)
				{
					return;
				}
				ISchemeLevelUpgrade* pSchemeLevelUpgrade = pSchemeCenter->getSchemeLevelUpgrade();
				if (pSchemeLevelUpgrade == NULL)
				{
					return;
				}
				// 超出等级上限，不再进行属性更新
				if (nCurLevel >= pSchemeLevelUpgrade->GetSchemeLevelUpgradeMaxLevel())
				{
					return;
				}

                if(nCurLevel>0) 
				{
                    //当前等级下已获经验值
                    int nCurLevelExpValue = getProperty_Integer(prop_id) - pSchemeLevelUpgrade->GetLevelAllExp(nCurLevel);
                    //距下一等级所需经验值
                    int nNextLevelExpMax = pSchemeLevelUpgrade->GetNextLevelExp(nCurLevel+1);
                    if(nCurLevelExpValue>nNextLevelExpMax)
                    {
						ErrorLn(__FUNCTION__": PlayerExp Error: CurLevelAddExpValue = " << nCurLevelExpValue <<" > NextLevelNeedExpValue ="<<nNextLevelExpMax);
                        return;
                    }

                    updatePropData.nValue =   nCurLevelExpValue;
                    updatePropData.nOtherData = nNextLevelExpMax;
				}
            }
        }
        break;

    case PROPERTY_TALENT_POINT:		// 天赋点
    case PROPERTY_STAMINA:			// 体力
    case PROPERTY_MAX_STAMINA:		// 基础体力
    case PROPERTY_HP_REPLY:			// 血量回复
    case PROPERTY_PA:				// 物理攻击力
    case PROPERTY_PDP:				// 物理防御穿透
    case PROPERTY_PCT_PDRAINS:		// 百分比物理吸血
    case PROPERTY_PASD:				// 攻击速度
    case PROPERTY_AF:               // 普攻系数
    case PROPERTY_CRC:				// 暴击几率
    case PROPERTY_MP_REPLY:			// 法力回复
    case PROPERTY_MA:				// 法术强度
    case PROPERTY_MDP:				// 法术防御穿透
    case PROPERTY_PCT_MDRAINS:		// 百分比法术吸血
    case PROPERTY_PCT_COOL_REDUCE:	// 百分比冷却缩减
    case PROPERTY_PD:				// 物理防御
    case PROPERTY_MD:				// 法术防御
    case PROPERTY_TENACITY:			// 韧性
    case PROPERTY_SCENEID:      // 场景ID
    case PROPERTY_AIM_DISTANCE:
        {
            updatePropData.nPropID = prop_id;	// 属性ID
            updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
            updatePropData.isIntproperty= 1;

            if(m_pEntity->isHero() || (gClientGlobal->getHero() && gClientGlobal->getHero()->getIntProperty(PROPERTY_OBSERVER)))  // 只主角的天赋点改变需要更新界面
            {
                updatePropData.isRaiseIprEvent = 1;
            }
            else
            {
                updatePropData.isRaiseIprEvent = 0;
            }

        }
        break;

    case PROPERTY_MP:
    case PROPERTY_MAX_MP:
        {
            int nMagicType = getProperty_Integer(PROPERTY_MAGIC_TYPE);
            int nType = nMagicType/MAGIC_TYPE_VALUE;
            if (nType != MAGIC_TYPE_MP)
            {
                break;
            }

            updatePropData.nPropID = prop_id;	// 属性ID
            updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
            updatePropData.isIntproperty= 1;
            updatePropData.isRaiseIprEvent = 1;
        }
    case PROPERTY_ENERGY:
    case PROPERTY_MAX_ENERGY:
        {
            int nMagicType = getProperty_Integer(PROPERTY_MAGIC_TYPE);
            int nType = nMagicType/MAGIC_TYPE_VALUE;
            if (nType != MAGIC_TYPE_ENERGY)
            {
                break;
            }

            updatePropData.nPropID = prop_id;	// 属性ID
            updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
            updatePropData.isIntproperty= 1;
            updatePropData.isRaiseIprEvent = 1;
        }
        break;

    case PROPERTY_MAGIC_TYPE:		// 魔法类型
        {
            ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
            if (pSchemeCenter == NULL)
            {
                break;
            }
            ISchemeMagicType *pSchemeMagicType = pSchemeCenter->getSchemeMagicType();
            if (pSchemeMagicType == NULL)
            {
                break;
            }
            int nMagicType = getProperty_Integer(PROPERTY_MAGIC_TYPE);
            int nType = nMagicType/MAGIC_TYPE_VALUE;
            if (nType < MAGIC_TYPE_NONE || nType >= MAGIC_TYPE_MAX)
            {
                break;
            }
            cmd_creature_magicType magicType;
            magicType.nType = (MAGIC_TYPE)nType;
            sstrcpyn(magicType.strMagicName, MagicTypeName[nType], sizeof(magicType.strMagicName));

            SMagicType *pMagicType = pSchemeMagicType->getMagicType(nMagicType);
            if (pMagicType != NULL)
            {
                sstrcpyn(magicType.strMagicName, pMagicType->szName, sizeof(magicType.strMagicName));
                magicType.nProgressBarType = pMagicType->nProgressBarType;
            }

            m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_MAGIC_TYPE, 0, 0, &magicType, sizeof(magicType));

            return;
        }
        break;
	case PROPERTY_GOLD:
		{
			updatePropData.nPropID = prop_id;	// 属性ID
			updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
			updatePropData.isIntproperty= 1;
			updatePropData.isRaiseIprEvent = 1;

			if (!m_pEntity->isHero() && !(gClientGlobal->getHero() && gClientGlobal->getHero()->getIntProperty(PROPERTY_OBSERVER)))
			{
				break;
			}
			m_pEntity->sendCommandToEntityView( ENTITY_TOVIEW_HINT_PLAYER_UPGRADE_STAR, updatePropData.nValue, 0, 0, 0);
		}
		break;
    default:  // 默认都直通，如果有要校验自己或其他特殊的东西，在ddefault前面加逻辑
        {
            updatePropData.nPropID = prop_id;	// 属性ID
            updatePropData.nValue = getProperty_Integer(prop_id);	// 属性数值
            updatePropData.isIntproperty= 1;
            updatePropData.isRaiseIprEvent = 1;
        }
        break;
    }

    // 数值属性更新
    if( updatePropData.isIntproperty )
    {
        m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_UPDATE_INTEGERPROPERTY,sizeof(updatePropData),0,&updatePropData,sizeof(updatePropData));  //更新实体的数值属性
    }
}

void EntityPart_PlayerRoleProperty::onPropertyChange( int prop_id )
{
    if ( m_pEntity==NULL )
        return;

    //////////////////////////////////////////////////////////////////////////
    // TODO 很不好的处理，后面再来优化
    if(prop_id == PROPERTY_LEVEL)
    {
        m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_SYNC_HEROVISION_LEVEL,0,0,0,0);// 玩家视野中的实体（包括自己）等级改变后的事件，实体先升级再进入视野不会调用
    }
    else if(prop_id==PROPERTY_SKIN)
    {
        int nSkin = getProperty_Integer(PROPERTY_SKIN);
        if(!gClientGlobal->getSchemeCenter()->getSchemeSkinInfo()->getSkinShemeInfo(nSkin))
        {
            ErrorLn("sync property, error skin_id============" << nSkin);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    event_entity_property_change context;
    context.nPropertyID = prop_id;
    m_pEntity->fireEvent( EVENT_ENTITY_PROPERTY_CHANGE,&context,sizeof(context) );

    push_update_view( prop_id );
}

bool EntityPart_PlayerRoleProperty::onCommand( int cmdid,void * data,size_t len )
{
	switch( cmdid )
	{
	case ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL:
		{
			if (len != sizeof(cmd_Entity_Property_Prefamce_Req))
			{
				ErrorLn("CWarPart::onCommand len != sizeof(cmd_Entity_Property_Prefamce_Req)");
				return false;
			}

			cmd_Entity_Property_Prefamce_Req* pData = (cmd_Entity_Property_Prefamce_Req *)data;

			msg_entity_property_req_dragonball sendData;
			sendData.uidReq = pData->uidReq;
			SendMessageToEntityScene(m_pEntity->getUID(), PART_PROPERTY, CS_MSG_ENTITY_PROPERTY_REQ_DRAGONBALL, &sendData,sizeof(msg_entity_property_req_dragonball));
			return true;
		}
		break;
	case ENTITY_CMD_ENTITY_PROPERTY_DRAGONBALL_CLOSE:
		{
			SendMessageToEntityScene(m_pEntity->getUID(), PART_PROPERTY, CS_MSG_ENTITY_PROPERTY_REQ_DRAGONBALL_CLOSE, NULL, 0);
			return true;
		}
		break;
	default:
		break;
	}

	return false;
}

bool EntityPart_PlayerRoleProperty::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	switch( msgId )
	{
	case SC_MSG_ENTITY_PROPERTY_RETURN_DRAGONBALL:
		{
			OnMessageEntityPropertyReturn(data, len);
		}
		break;
	default:
		break;
	}
	return true;
}




void EntityPart_PlayerRoleProperty::push_update_view(int prop_id)
{
    // 针对血量进行控制频率，防止更新UI太猛
    bool need_update_view = true;

    // 是否开启延迟刷新功能，默认人物关闭此功能
//#define OPEN_DELAY_REFRESH  
#ifdef OPEN_DELAY_REFRESH
    do 
    {
        if(m_pEntity->isHero())
        {
            break;
        }

        if(prop_id != PROPERTY_HP)
        {
            break;
        }

        // 没有血量或最大血量时直接下发
        int hp = getProperty_Integer(PROPERTY_HP);
        int max_hp = getProperty_Integer(PROPERTY_MAX_HP);
        if( hp<=0 || hp==max_hp )
        {
            break;
        }

        // 控制50ms内刷很多次，范围内的将不下发
        DWORD dwCurTick = getTickCount();
        if(dwCurTick > m_dwLastViewHPTick + 50)
        {
            m_dwLastViewHPTick = dwCurTick;
            break;
        }

        // 不下发
        need_update_view = false;

    } while (false);
#endif

    // 伤害下发到显示层
    if(need_update_view)
    {
        updateView( prop_id );
    }
}

void EntityPart_PlayerRoleProperty::OnMessageEntityPropertyReturn(void *pData, size_t nLen)
{
	if (m_pEntity == NULL || pData == NULL)
	{
		return;
	}

	if (nLen < sizeof(msg_entity_property_return_dragonball))
	{
		return;
	}

	msg_entity_property_return_dragonball *pMsg = (msg_entity_property_return_dragonball*)pData;
	if (nLen != sizeof(msg_entity_property_return_dragonball))
	{
		return;
	}

	obuf ob;
	cmd_Entity_Property_Prefamce_Return info;
	info.uidReq = pMsg->uidReq;
	info.nHp = pMsg->nHp;
	info.nMd = pMsg->nMd;
	info.nPa = pMsg->nPa;
	info.nPd = pMsg->nPd;
	ob.push_back(&info, sizeof(info));

	// 通告显示层更新
	m_pEntity->sendCommandToEntityView(ENTITY_TOVIEW_ENTITY_PROPERTY_DRAGONBALL, 0, 0, ob.data(), ob.size());
}