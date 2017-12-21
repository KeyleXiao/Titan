#include "stdafx.h"
#include "HandleCommandAttack.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
#include "SpellMessageDef.h"
#include "EntityDef.h"
#include "IMovePart.h"
#include "EventDef.h"
#include "TimeSyncClient.h"
#include "ISchemeCenter.h"
#include "Entity_inl.h"
#include "ExternalHelper.h"
#include "IFreezeClient.h"
#include "ISlotLogicPart.h"
#include "IEntityProperty.h"

/////////////////////////////////////////IHandleCommand/////////////////////////////////////////
void HandleCommandAttack::release()
{
	delete this;
}

bool HandleCommandAttack::run()
{
	// 取得主角对象
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return true;
	}

	ISpellPart *pSpellPart = (ISpellPart *)pClientEntity->getEntityPart(PART_SPELL);
	if (pSpellPart == NULL)
	{
		return true;
	}

    // 取得目标信息
    STargetInfo *pTargetInfo = pSpellPart->getTargetInfo();
    if (pTargetInfo == NULL)
    {
        return true;
    }

	SpellResult nResult = pSpellPart->castResult(m_Data);
	switch (nResult)
	{
	case SpellResult_Success:
		{
			return doCommand(pSpellPart, pTargetInfo);
		}
		break;
	case SpellResult_Far:
		{
			IHandleCommandManager *pHandleCommandManager = gClientGlobal->getHandleCommandManager();
			if (pHandleCommandManager == NULL)
			{
				return true;
			}
			IHandleCommandFactory *pHandleCommandFactory = pHandleCommandManager->getHandleCommandFactory();
			if (pHandleCommandFactory == NULL)
			{
				return true;
			}

			ISpell *pSpell = pSpellPart->findSpell(m_Data.nID);
			if (pSpell == NULL)
			{
				return true;
			}
			// 取得技能使用类型
			int nCastType = pSpell->getSpellData(SPDATA_CAST_TYPE);
			// 不寻路，直接释放技能
			if ((nCastType&SpellCastType_Pathing) != SpellCastType_Pathing)
			{
                return doCommand(pSpellPart, pTargetInfo);
			}
			else
			{
				float fDistance = abs(pSpell->getSpellData(SPDATA_DISTANCE)/SPELL_FLOAT_VALUE);

                // 没目标时
				if (m_Data.uidTarget == INVALID_UID || m_Data.uidTarget == 0)
				{
					// 配置技能无目标时单体变成直线使用
					if ((nCastType&SpellCastType_NoTargetSingToLine) == SpellCastType_NoTargetSingToLine)
					{
						return doCommand(pSpellPart, pTargetInfo);
					}

                    // 循环技能，不再寻路
                    if (m_dwCount > 0)
                    {
                        return true;
                    }

                    // 绘制技能贴花
                    drawSpellDecal();

					// 向目标点移动
					SHandleCommand_Close close;
					close.ptTargetTile = m_Data.ptTargetTile;
					close.fDistance = fDistance;
                    close.ptSelfTile = m_Data.ptSelfTile;
					IHandleCommand *pCommand = pHandleCommandFactory->CreateCommand_Close(close);
					pHandleCommandManager->insertCommandFront(pCommand);

                    AttackCmdQueue::getInstance().m_nRunningSpell = 0;

                    return false;
				}
				else
				{
                    // 循环技能，不再寻路
                    if (m_dwCount > 0)
                    {
                        return true;
                    }

                    // 绘制技能贴花
                    drawSpellDecal();

					// 向目标移动
					SHandleCommand_Pursuit pursuit;
					pursuit.uidTarget = m_Data.uidTarget;
					pursuit.fDistance = fDistance;
					pursuit.fGiveUpDistance = ATTACK_GIVE_UP_DISTANCE;
                    pursuit.ptSelfTile = m_Data.ptSelfTile;
					IHandleCommand *pCommand = pHandleCommandFactory->CreateCommand_Pursuit(pursuit);
					pHandleCommandManager->insertCommandFront(pCommand);

                    // 当前spell已经不处于正在运行状态
                    AttackCmdQueue::getInstance().m_nRunningSpell = 0;

                    return false;
				}
			}
		}
		break;
    case SpellResult_Busying:
        {
            ISpell *pSpell = m_Data.pSpell; // pSpellPart->findSpell(m_Data.nID);
            if (pSpell == NULL)
            {
                return true;
            }
            // 取得技能标识
            int nFlag = pSpell->getSpellData(SPDATA_FLAG);
            // 是连招技能,丢弃包，因为等待的话，边走边打会导致是滞后位置点
            if ((nFlag&SpellFlag_Combo) == SpellFlag_Combo)
            {
                if (loopAttack())
                {
                    return false;
                }
                else
                {
					// WarningLn(__FUNCTION__ << __LINE__ << "spell=" << m_Data.nID << ", discard because of busying...");
                    return true;
                }
            }

            return false;
        }
        break;
    case SpellResult_OverlayNoUse:
        {
            g_EHelper.showSystemMessage(CHAT_TIP_ENTITY_TALENT_SKILL_NOT_READY);
            return true;
        }
        break;
	default:
		{
			WarningLn(__FUNCTION__ << __LINE__ << ", spell=" << m_Data.nID << ", discard...");
			return true;
		}
		break;
	}

	// 返回false表示继续执行
	return false;
}

bool HandleCommandAttack::doCommand(ISpellPart* pSpellPart, STargetInfo* pTargetInfo)
{
    // 发送攻击数据
    SendAttackData();

    // 是循环技能
    if (loopAttack())
    {
        // 如果是打断循环技能
        if (pSpellPart->getBreakLoopAttack())
        {
            pSpellPart->setBreakLoopAttack(false);
            return true;
        }

        // 设置下超时时间,继续攻击
        m_dwTimeOut = getTickCountEx() + COMMAND_TIME_OUT;

        // 改变数据等
        int nID = m_Data.nID;
        do 
        {
            ISlotLogicPart *pSlotLogicPart = (ISlotLogicPart *)gClientGlobal->getHero()->getEntityPart(PART_SLOTLOGIC);
            if (pSlotLogicPart == NULL)
            {
                break;
            }
            int nSpellID = pSlotLogicPart->GetSlotSkillID((SLOT_TYPE)m_Data.nSlotType, m_Data.nSlotIndex);
            if (nSpellID == 0)
            {
                break;
            }

            nID = nSpellID;
        } while (false);
        m_Data.nID = nID;
        m_Data.uidTarget = pTargetInfo->uidTarget;
        m_Data.ptTargetTile = pTargetInfo->ptTargetTile;

        return false;
    }
    else
    {
        pSpellPart->setBreakLoopAttack(false);
    }

    return true;
}

void HandleCommandAttack::update()
{

}

DWORD HandleCommandAttack::getTimeOut()
{
	return m_dwTimeOut;
}

/////////////////////////////////////////HandleCommandAttack/////////////////////////////////////////
// 构造函数
HandleCommandAttack::HandleCommandAttack(const SHandleCommand_Attack &data):m_Data(data)
{
	m_dwTimeOut = getTickCountEx() + COMMAND_TIME_OUT;
    m_dwCount = 0;
}

// 析构函数
HandleCommandAttack::~HandleCommandAttack()
{

}

// 发送攻击数据
void HandleCommandAttack::SendAttackData()
{
	// 取得主角对象
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}
    // 设置停止移动
    setStopMove();

	IMovePart *pMovePart = (IMovePart *)pClientEntity->getEntityPart(PART_MOVE);
	if (pMovePart == NULL)
	{
		return;
	}
	TimeSyncClient *pTimeSyncClient = gClientGlobal->getTimeSyncer();
	if (pTimeSyncClient == NULL)
	{
		return;
	}
    ISpellPart *pSpellPart = (ISpellPart *)pClientEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return;
    }
    ISpell *pSpell = m_Data.pSpell; //pSpellPart->findSpell(m_Data.nID);
    if (pSpell == NULL)
    {
        return;
    }

    event_entity_send_cast_spell sendCastSpell;
    sendCastSpell.nID = m_Data.nID;
    // 向服务器发送施法技能事件
    pClientEntity->fireEvent(EVENT_ENTITY_SEND_CAST_SPELL, (char *)&sendCastSpell, sizeof(sendCastSpell));

    // 技能使用类型
    int nCastType = pSpell->getSpellData(SPDATA_CAST_TYPE);
    // 配置了使用最新位置
    if ((nCastType&SpellCastType_GetNewTarget) == SpellCastType_GetNewTarget)
    {
        SNewTargetInfo newTargetInfo;
        newTargetInfo.nID = m_Data.nID;
        newTargetInfo.nSlotIndex = m_Data.nSlotIndex;
        newTargetInfo.nSlotType = m_Data.nSlotType;
        // 增加新目标点询问
        pSpellPart->addNewTargetInfo(newTargetInfo);

        // 技能相关属性
        cmd_spell_attribute attribute;
        do 
        {
            // 死亡状态，不再可使用技能
            if (pClientEntity->getState() == ENTITY_STATE_DIE)
            {
                break;
            }

            // 技能使用类型
            int nCastType = pSpell->getSpellData(SPDATA_CAST_TYPE);

            // 技能选择类型
            int nSelectType = pSpell->getSpellData(SPDATA_SELECT_TYPE);

            // 读条时间
            int nTime = pSpell->getSpellData( SPDATA_PREPARE_TIME );
            if ((pSpell->getSpellData(SPDATA_CAST_FLAG)&SpellCastFlag_ContinueFocoEnergia) == SpellCastFlag_ContinueFocoEnergia)
            {
                // 累加持续时间
                nTime += pSpell->getSpellData( SPDATA_DURATION );
            }

            attribute.nSelectType = nSelectType;
            attribute.fDistance = (float)pSpell->getSpellData(SPDATA_DISTANCE)/SPELL_FLOAT_VALUE;
            attribute.fAttackRange = (float)pSpell->getSpellData(SPDATA_ATTACK_RANGE)/SPELL_FLOAT_VALUE;
            attribute.fChooseRange = (float)pSpell->getSpellData(SPDATA_CHOOSE_RANGE)/SPELL_FLOAT_VALUE;
            if ((nSelectType&SpellSelectType_SelectHeight) == SpellSelectType_SelectHeight)
            {
                ISchemeEntity *pSchemeEntity = gClientGlobal->getSchemeCenter()->getSchemeEntity();
                if (pSchemeEntity != NULL)
                {
                    attribute.fHeight = pSchemeEntity->getEntityXml()->fSpellHeight;
                }
            }
            attribute.nDrawType = pSpell->getSpellData(SPDATA_DRAW_TYPE);
            attribute.nTargetType = pSpell->getSpellData(SPDATA_SELECT_TARGET);
            attribute.nAttributeType = 1;	// 可使用
            attribute.nCastType = nCastType;
            attribute.nSlotIndex = m_Data.nSlotIndex;
            attribute.nSlotType = m_Data.nSlotType;
            attribute.nSpellID = m_Data.nID;
            attribute.nAttackType = pSpell->getSpellData(SPDATA_ATTACK_TYPE);
            attribute.nSpellType = pSpell->getSpellData(SPDATA_TYPE);
            attribute.nFocoEnergiaTime = nTime;
            attribute.nCastFlag = pSpell->getSpellData( SPDATA_CAST_FLAG );
            attribute.nSpellFlag = pSpell->getSpellData( SPDATA_FLAG );
            if ((attribute.nSpellFlag&SpellFlag_Roll) == SpellFlag_Roll)
            {
                ISlotLogicPart *pSlotLogicPart = (ISlotLogicPart *)pClientEntity->getEntityPart(PART_SLOTLOGIC);
                if (pSlotLogicPart != NULL)
                {
                    attribute.nRollSpellStartID = pSlotLogicPart->GetSlotSkillID(m_Data.nSlotType, ROLL_SPELL_START_SLOT_INDEX);
                }
            }
        } while (false);

        // 设置技能相关属性
        pClientEntity->sendCommandToEntityView(ENTITY_TOVIEW_SETSPELLATTRIBUTE, 0, 0, &attribute, sizeof(attribute));
    }
    else
    {
	    // 发送数据
	    SSpellContextImp_CS sendData;
	    sendData.uidOperator = m_Data.uidOperator;
	    sendData.dwOPTick = pTimeSyncClient->GetTick();
	    sendData.moveState = pMovePart->getMoveContext();
	    sendData.uidTarget = m_Data.uidTarget;
	    sendData.ptTargetTile = m_Data.ptTargetTile;
	    sendData.nID = m_Data.nID;
	    sendData.nSlotIndex = m_Data.nSlotIndex;
	    sendData.nSlotType = m_Data.nSlotType;

#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
        sendData.dwViewTick = m_Data.dwViewOptTick;      // 显示层触发时间
        sendData.dwCmdQueueTick = m_Data.dwTick;         // 命令队列收包时间
        sendData.dwMsgSendTick = getTickCountEx();       // 消息发送时间

        //if (sendData.dwMsgSendTick - sendData.dwCmdQueueTick > 20)
        //{
        //    WarningLn(__FUNCTION__": id="<< m_Data.nID<<", cost "<< (sendData.dwMsgSendTick - sendData.dwCmdQueueTick) <<" ms");
        //}
#endif

	    SendMessageToEntityScene(sendData.uidOperator, PART_SPELL, SPELL_MSG_PREPARE, (char *)&sendData, sizeof(sendData));
    }

    ++m_dwCount;
}


// 取得攻击数据
const SHandleCommand_Attack *HandleCommandAttack::getAttackData(void)
{
	return &m_Data;
}


// 设置停止移动
void HandleCommandAttack::setStopMove(void)
{
    // 取得主角对象
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return;
    }

    ISpellPart *pSpellPart = (ISpellPart *)pClientEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return;
    }

    ISpell *pSpell = m_Data.pSpell; //pSpellPart->findSpell(m_Data.nID);
    if (pSpell == NULL)
    {
        return;
    }

    int nCastFlag = pSpell->getSpellData(SPDATA_CAST_FLAG);
    // 攻击不停止移动
    if ((nCastFlag&SpellCastFlag_AttackNoStopMove) == SpellCastFlag_AttackNoStopMove)
    {
        return;
    }
    // 停止移动位置
    pClientEntity->sendCommandToEntityView(ENTITY_TOVIEW_STOP_MOVE_POS, 0, 0, NULL, 0);
}


// 镜头自动转向攻击对象
void HandleCommandAttack::rotateCameraToTarget(void)
{
    // 没目标时
    if (m_Data.uidTarget == INVALID_UID || m_Data.uidTarget == 0)
    {
        return;
    }

    /// 取得主角对象
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return;
    }

    ISpellPart *pSpellPart = (ISpellPart *)pClientEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return;
    }
    ISpell *pSpell = pSpellPart->findSpell(m_Data.nID);
    if (pSpell == NULL)
    {
        return;
    }
    int nCastType = pSpell->getSpellData(SPDATA_CAST_TYPE);
    // 使用时镜头自动转向攻击对象未配置
    if ((nCastType&SpellCastType_RotateCameraToTarget) != SpellCastType_RotateCameraToTarget)
    {
        return;
    }

    // 镜头自动转向攻击对象
    cmd_creature_rotate_camera_to_target RotateCameraToTarget;
    RotateCameraToTarget.uidTarget = m_Data.uidTarget;
    pClientEntity->sendCommandToEntityView(ENTITY_TOVIEW_ROTATE_CAMERA_TO_TARGET, 0, 0, &RotateCameraToTarget, sizeof(RotateCameraToTarget));
}

// 循环攻击
bool HandleCommandAttack::loopAttack()
{
    /// 取得主角对象
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return false;
    }

    ISpellPart *pSpellPart = (ISpellPart *)pClientEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return false;
    }
    ISpell *pSpell = m_Data.pSpell; //pSpellPart->findSpell(m_Data.nID);
    if (pSpell == NULL)
    {
        return false;
    }
    int nSpellFlag = pSpell->getSpellData(SPDATA_FLAG);
    // 不是循环技能
    if ((nSpellFlag&SpellFlag_LoopSpell) != SpellFlag_LoopSpell)
    {
        return false;
    }

    // 没目标时
    if (m_Data.uidTarget == INVALID_UID || m_Data.uidTarget == 0)
    {
        return false;
    }
    IEntity *pEntity = g_EHelper.GetEntity(m_Data.uidTarget);
    if (pEntity == NULL)
    {
        return false;
    }
    // 只对防御塔、基地、水晶、野生怪、小龙、大龙进行自动连击
    if( TYPE_MONSTER == UID_2_TYPE(m_Data.uidTarget) )
    {
        IEntityProperty* pPropertyPart = (IEntityProperty *)pEntity->getEntityPart(PART_PROPERTY);
        if(pPropertyPart)
        {
            int nSubType = pPropertyPart->getProperty_Integer(PROPERTY_VOCATION);
            if(nSubType != MONSTER_SUB_TYPE_DEFENSIVE_TOWER && nSubType != MONSTER_SUB_TYPE_BASE && nSubType != MONSTER_SUB_TYPE_CRYSTAL_TOWER 
			   && nSubType != MONSTER_SUB_TYPE_WILD_MONSTER && nSubType != MONSTER_SUB_TYPE_SMALL_DRAGON && nSubType != MONSTER_SUB_TYPE_HUGE_DRAGON)
            {
                return false;
            }
        }
    }

    float fDistance = abs(pSpell->getSpellData(SPDATA_DISTANCE)/SPELL_FLOAT_VALUE);
    // 超过技能距离
    if ((pClientEntity->getLocation() - pEntity->getLocation()).getLength() >= fDistance)
    {
        return false;
    }

    // 后续技能已冷却好,不再循环攻击
    if (AttackCmdQueue::getInstance().checkNextAttackCommand())
    {
        return false;
    }

    return true;
}


// 绘制技能贴花
void HandleCommandAttack::drawSpellDecal(void)
{
    // 取得主角对象
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return;
    }

    ISpellPart *pSpellPart = (ISpellPart *)pClientEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return;
    }

    ISpell *pSpell = pSpellPart->findSpell(m_Data.nID);
    if (pSpell == NULL)
    {
        return;
    }

    // 画技能贴花
    cmd_creature_draw_spell_decal drawSpellDecal;
    drawSpellDecal.nDraw = 1;
    drawSpellDecal.nDrawType = pSpell->getSpellData(SPDATA_DRAW_TYPE);
    drawSpellDecal.fDistance = (float)pSpell->getSpellData(SPDATA_DISTANCE)/SPELL_FLOAT_VALUE;;
    drawSpellDecal.fAttackRange = (float)pSpell->getSpellData(SPDATA_ATTACK_RANGE)/SPELL_FLOAT_VALUE;
    drawSpellDecal.fChooseRange = (float)pSpell->getSpellData(SPDATA_CHOOSE_RANGE)/SPELL_FLOAT_VALUE;
    drawSpellDecal.nShowTime = 1000;
    pClientEntity->sendCommandToEntityView(ENTITY_TOVIEW_DRAW_SPELL_DECAL, 0, 0, &drawSpellDecal, sizeof(drawSpellDecal));
}


/////////////////////////////////////////AttackBackup/////////////////////////////////////////
// 构造函数
AttackCmdQueue::AttackCmdQueue():m_nRunningSpell(0), m_bBlocking(false), m_dwBlockTick(0)
{
    m_CommandAttackList.clear();
}

// 析构函数
AttackCmdQueue::~AttackCmdQueue()
{
}

bool AttackCmdQueue::create()
{
	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
	{
		return false;
	}
    pEventEngine->Subscibe(this, EVENT_ENTITY_DIE, TYPE_PLAYER_ROLE, 0, __FILE__);
    pEventEngine->Subscibe(this, EVENT_SELF_SHIFT_STATE_CHANGE, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
	return true;
}

void AttackCmdQueue::release()
{
	IEventEngine *pEventEngine = gClientGlobal->getEventEngine();
	if (pEventEngine == NULL)
	{
		return;
	}
    pEventEngine->UnSubscibe(this, EVENT_ENTITY_DIE, TYPE_PLAYER_ROLE, 0);
    pEventEngine->UnSubscibe(this, EVENT_SELF_SHIFT_STATE_CHANGE, TYPE_PLAYER_ROLE, 0);
}

void AttackCmdQueue::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_DIE:
		{
            // 取得主角对象
            IClientEntity *pClientEntity = gClientGlobal->getHero();
            if (pClientEntity == NULL)
            {
                break;
            }

            UID uid = pClientEntity->getUID();
            if (uid != dwSrcID)
            {
                break;
            }

			// 清除攻击命令
			clearAttackCommand();
		}
		break;
    case EVENT_SELF_SHIFT_STATE_CHANGE:
        {
            if(pszContext == NULL || nLen < sizeof(event_self_shift_state_change))
                return;

            event_self_shift_state_change* pEvent = (event_self_shift_state_change*)pszContext;
            
            // 清除攻击命令
            clearAttackCommand();

            if(pEvent->bShift)
            {
                // 开始冲锋 阻塞攻击命令
                m_bBlocking = true;
                m_dwBlockTick = getTickCountEx();
            }
            else
            {
                // 结束冲锋 取消阻塞
                m_bBlocking = false;
            }

            IHandleCommandManager* pHandleCommandManager = gClientGlobal->getHandleCommandManager();
            if(pHandleCommandManager == NULL)
            {
                return;
            }

            // 清除命令队列
            pHandleCommandManager->clear();
        }
        break;
	default:
		break;
	}
}

void AttackCmdQueue::addAttackCommand(const SHandleCommand_Attack &data)
{
    if(m_bBlocking)
    {
        // 阻塞超时
        if(getTickCountEx() >= m_dwBlockTick + 3000)
        {
            WarningLn(__FUNCTION__ << __LINE__ << ", attack queue block > 3s");
            m_bBlocking = false;
        }
        else
        {
			WarningLn(__FUNCTION__ << __LINE__ << "spellid=" << data.nID << ", blocking because of shift...");
            return;
        }
    }

	IHandleCommandManager *pHandleCommandManager = gClientGlobal->getHandleCommandManager();
	if (pHandleCommandManager == NULL)
	{
		return;
	}
	IHandleCommandFactory *pHandleCommandFactory = pHandleCommandManager->getHandleCommandFactory();
	if (pHandleCommandFactory == NULL)
	{
		return;
	}
	ISchemeCenter *pSchemeCenter = gClientGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	ISchemeSpellSlot *pSchemeSpellSlot = pSchemeCenter->getSchemeSpellSlot();
	if (pSchemeSpellSlot == NULL)
	{
		return;
	}
	// 取得主角对象
	IClientEntity *pClientEntity = gClientGlobal->getHero();
	if (pClientEntity == NULL)
	{
		return;
	}
    ISpellPart *pSpellPart = (ISpellPart *)pClientEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return;
    }
	int nVocation = pClientEntity->getIntProperty(PROPERTY_VOCATION);
	// 取得技能槽位信息
	SPELL_SLOT *pSpellSlot = pSchemeSpellSlot->getSpellSlot(nVocation);
	if (pSpellSlot == NULL)
	{
		return;
	}
    // 如果技能不存在就不要放到队列中了
    ISpell *pNewSpell = data.pSpell; //pSpellPart->findSpell(data.nID);
    if (pNewSpell == NULL)
    {
        return;
    }
    // 获取技能命令优先级
    int nNewSpellCmdLevel = pNewSpell->getSpellData(SPDATA_COMMAND_LEVEL);
    int nNewSpellEffectType = pNewSpell->getSpellData()->getInt(SPDATA_SPELL_EFFECT_TYPE);
    // 取到跳跃技能ID
	int nJumpSpellID = pSpellSlot->nSpellID[ATTACK_JUMP_SPELL_SLOT_INDEX];

    // 跳跃技能不缓存 不打断任何技能
    if(m_nRunningSpell != 0 && data.nID == nJumpSpellID)
    {
        return;
    }

    // 增加技能时，此时正在执行的是普攻命令可以直接被打断掉
    if(m_nRunningSpell != 0)
    {
        if( (nNewSpellEffectType&SpellEffectType_NormalAttack) != SpellEffectType_NormalAttack)
        {
            ISpell* pRunningSpell = pSpellPart->findSpell(m_nRunningSpell);
            if(pRunningSpell != NULL && (pRunningSpell->getSpellData()->getInt(SPDATA_SPELL_EFFECT_TYPE)&SpellEffectType_NormalAttack) == SpellEffectType_NormalAttack )
            {
                WarningLn("skill interrupt the running normal attack! current skill id="<< data.nID <<",run skill id="<< m_nRunningSpell);
                pHandleCommandManager->clear();
                m_nRunningSpell = 0;
            }
        }
    }

    // 当前有技能正在处理 缓存到队列
    if(m_nRunningSpell != 0)
    {
        COMMAND_LIST::iterator it = m_CommandAttackList.begin();
        for(; it != m_CommandAttackList.end(); ++it)
        {
            SHandleCommand_Attack& command = *it;
            // 同一个技能只能缓存一个
            if(command.nID == data.nID)
            {
                // 把前一个删除掉(假如返回end时 insert(end, data)应该没问题吧todo.)
                it = m_CommandAttackList.erase(it);
                // 插入新的
                m_CommandAttackList.insert(it, data);

                //WarningLn(__FUNCTION__": find same command, id="<< data.nID);
                // 缓存成功 可以返回
                return;
            }

            // 无效技能 擦除
            ISpell* pSpell = command.pSpell; //pSpellPart->findSpell(command.nID);
            if(pSpell == NULL)
            {
                it = m_CommandAttackList.erase(it);
                continue;
            }

            // 找第一个优先级比nNewSpellCmdLevel小的 插到他的前面
            if(pSpell->getSpellData(SPDATA_COMMAND_LEVEL) < nNewSpellCmdLevel)
            {
                m_CommandAttackList.insert(it, data);
                // 缓存成功 可以返回
                return;
            }
        }

        // 优先级都比新的高 或者原来队列是空的 存到末尾
        m_CommandAttackList.push_back(data);
        return;
    }

    // 没有正在处理的技能命令 直接加入处理队列
    pHandleCommandManager->clear();
    IHandleCommand *pCommand = pHandleCommandFactory->CreateCommand_Attack(data);
    pHandleCommandManager->appendCommandTail(pCommand);

    m_nRunningSpell = data.nID; 
}

void AttackCmdQueue::clearAttackCommand()
{
	m_CommandAttackList.clear();
	m_nRunningSpell = 0;
}


bool AttackCmdQueue::isAttackFinished() const
{
	return m_nRunningSpell == 0;
}

void AttackCmdQueue::setNextAttack()
{
    // 先把当前的置为无效
    m_nRunningSpell = 0;

    if (m_CommandAttackList.empty())
    {
        return;
    }
    IHandleCommandManager *pHandleCommandManager = gClientGlobal->getHandleCommandManager();
    if (pHandleCommandManager == NULL)
    {
        return;
    }
    IHandleCommandFactory *pHandleCommandFactory = pHandleCommandManager->getHandleCommandFactory();
    if (pHandleCommandFactory == NULL)
    {
        return;
    }

    // 取得当前时间
    DWORD dwTick = getTickCountEx();

    COMMAND_LIST::iterator Iter = m_CommandAttackList.begin();
    while (Iter != m_CommandAttackList.end()) // 有后备技能，看看冷却时间到没，如果没到，而目前的是自动攻击的话，继续目前的
    {
        if (dwTick >= Iter->dwTick + COMMAND_TIME_OUT)
        {
            Iter = m_CommandAttackList.erase(Iter);
            continue;
        }

        // 清除命令队列
        pHandleCommandManager->clear();
        IHandleCommand *pCommand = pHandleCommandFactory->CreateCommand_Attack(*Iter);
        pHandleCommandManager->appendCommandTail(pCommand);

        m_nRunningSpell = Iter->nID; 

        // 在命令队列中移除
        m_CommandAttackList.erase(Iter);
        break;
    }
}

// 检测后续技能是否已冷却好
bool AttackCmdQueue::checkNextAttackCommand()
{
    if (m_CommandAttackList.empty())
    {
        return false;
    }

    // 取得主角对象
    IClientEntity *pClientEntity = gClientGlobal->getHero();
    if (pClientEntity == NULL)
    {
        return false;
    }
    ISpellPart *pSpellPart = (ISpellPart *)pClientEntity->getEntityPart(PART_SPELL);
    if (pSpellPart == NULL)
    {
        return false;
    }
    // 取得冷却部件
    IFreezePart *pFreezePart = (IFreezePart *)pClientEntity->getEntityPart(PART_FREEZE);
    if (pFreezePart == NULL)
    {
        return false;
    }

    // 取得当前时间
    DWORD dwTick = getTickCountEx();

    COMMAND_LIST::iterator Iter = m_CommandAttackList.begin();
    while (Iter != m_CommandAttackList.end()) // 有后备技能，看看冷却时间到没，如果没到，而目前的是自动攻击的话，继续目前的
    {
        if (dwTick >= Iter->dwTick + COMMAND_TIME_OUT)
        {
            Iter = m_CommandAttackList.erase(Iter);
            continue;
        }

        ISpell *pSpell = pSpellPart->findSpell(Iter->nID);
        if (pSpell == NULL)
        {
            Iter = m_CommandAttackList.erase(Iter);
            continue;
        }

        // 只需要检测第一次,所以直接return
        return pFreezePart->CanWork(EFreeze_ClassID_Spell, pSpell->getSpellData(SPDATA_COOLID));
    }

    return false;
}


// 从队列中移除指定技能
void AttackCmdQueue::removeCommandBySpellID(int nSpellID)
{
    COMMAND_LIST::iterator Iter = m_CommandAttackList.begin();
    while (Iter != m_CommandAttackList.end())
    {
        if (nSpellID == Iter->nID)
        {
            Iter = m_CommandAttackList.erase(Iter);
            WarningLn(__FUNCTION__": spell=" << nSpellID);
        }
        else
        {
            ++Iter;
        }
    }
}
