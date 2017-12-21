#include "stdafx.h"
#include "HandleCommandQueue.h"
#include "HandleCommandManager.h"
#include "IClientGlobal.h"
#include "EventDef.h"

/////////////////////////////////////////IHandleCommandManager/////////////////////////////////////////
/** 获取命令工厂接口
*/
IHandleCommandFactory *HandleCommandManager::getHandleCommandFactory()
{
	return &m_HandleCommandFactory;
}

bool HandleCommandManager::create()
{
	//TimerAxis *pTimerAxis = gClientGlobal->getTimerAxis();
	//if (pTimerAxis == NULL)
	//{
	//	return false;
	//}
	//pTimerAxis->SetTimer(HANDLE_COMMAND_LOGIC_TIMER, HANDLE_COMMAND_LOGIC_TIME, this, INFINITY_CALL, __FILE__);


    IEventEngine * pEventEngine = gClientGlobal->getEventEngine();
    if (pEventEngine)
    {
        // 订阅 技能改变事件
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_SPELL_CHANGE, TYPE_PLAYER_ROLE, 0, __FILE__);
    }

	return true;
}

void HandleCommandManager::close()
{
    IEventEngine * pEventEngine = gClientGlobal->getEventEngine();
    if (pEventEngine)
    {
        // 取消订阅 技能改变事件
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_SPELL_CHANGE, TYPE_PLAYER_ROLE, 0);
    }

	//TimerAxis *pTimerAxis = gClientGlobal->getTimerAxis();
	//if (pTimerAxis != NULL)
	//{
	//	pTimerAxis->KillTimer(HANDLE_COMMAND_LOGIC_TIMER, this);
	//}

	// 清除数据
	clear();

	delete this;
}

void HandleCommandManager::appendCommandTail(IHandleCommand *pCmd)
{
	m_commandQueue.pushBack(pCmd);
}

void HandleCommandManager::insertCommandFront(IHandleCommand *pCmd)
{
	m_commandQueue.pushFront(pCmd);
}

/** 清除队列
*/
void HandleCommandManager::clear()
{
	m_commandQueue.clearAll();
}

void HandleCommandManager::clearBefore(IHandleCommand *pCmd)
{
	m_commandQueue.clearBefore(pCmd);
}

void HandleCommandManager::clearAfter(IHandleCommand *pCmd)
{
	m_commandQueue.clearAfter(pCmd);
}

void HandleCommandManager::update()
{
	m_commandQueue.update();
}

/** 移除某项指令
*/
void HandleCommandManager::RemoveCommand(IHandleCommand *pCommand)
{
	m_commandQueue.RemoveCommand(pCommand);
}

// 获取队列命令数量
int HandleCommandManager::getCommandCount(void)
{
    return m_commandQueue.count();
}

/////////////////////////////////////////TimerHandler/////////////////////////////////////////
// 定时器到了
void HandleCommandManager::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case HANDLE_COMMAND_LOGIC_TIMER:
		{
			update();

			break;
		}
	default:
		break;
	}
}


/////////////////////////////////////////TimerHandler/////////////////////////////////////////
// 事件
void HandleCommandManager::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
    switch (wEventID)
    {
    case EVENT_ENTITY_SPELL_CHANGE:     // 技能改变事件
        {
            // 不是人物跳过
            if (bSrcType != TYPE_PLAYER_ROLE)
            {
                return;
            }
            if (nLen != sizeof(event_spell_change))
            {
                return;
            }

            event_spell_change* pEvent = (event_spell_change*)pszContext;
            // 技能增加不处理
            if (pEvent->bAdd)
            {
                return;
            }
            //TraceLn("EVENT_ENTITY_SPELL_CHANGE remove, id="<< pEvent->nSpellID);
            // 非主角不处理
            if (!pEvent->bIsMaster)
            {
                return;
            }

            // 从命令队列中移除
            m_commandQueue.removeCommandBySpellID(pEvent->nSpellID);

            // 从攻击命令中移除
            m_HandleCommandFactory.removeCommandBySpellID(pEvent->nSpellID);
        }
        break;

    default: break;
    }
}

/////////////////////////////////////////HandleCommandManager/////////////////////////////////////////
HandleCommandManager::HandleCommandManager()
{
}

HandleCommandManager::~HandleCommandManager()
{
}