#include "stdafx.h"
#include "HandleCommandQueue.h"
#include "HandleCommand.h"
#include <list>
#include "HandleCommandAttack.h"

//////////////////////////////////////////////////////////////////////////////////

HandleCommandQueue::HandleCommandQueue()
{

}

HandleCommandQueue::~HandleCommandQueue()
{
    clearAll();
}


void HandleCommandQueue::update()
{
	if(m_vCommands.empty())
		return;

    ulong begin = getTickCountEx();
    ulong end = 0;

    IHandleCommand *pCommand = m_vCommands.front();
    if(!pCommand)
    {
        WarningLn(__FUNCTION__ << __LINE__ << ", pCommand is null");
        m_vCommands.pop_front();
        return;
    }

    // 技能超时打印警告
    bool bTimeOut = (begin >= pCommand->getTimeOut()); 
    bool bAttacking = (pCommand->getComandType() == CommandType_Attack); 
    if(bTimeOut && bAttacking)
    {
        pCommand->printInfo(COMMAND_TIME_OUT + begin - pCommand->getTimeOut());
    }

    // 命令超时不执行命令对象 或者 执行命令对象完后 
    bool bCanRemove = bTimeOut;
    if (!bTimeOut)
    {
        bCanRemove = pCommand->run();
    }

    // 擦除对象
    if (bCanRemove)
    {
        m_vCommands.pop_front();

        if (pCommand)
        {
            pCommand->release();
        }

        // 激活下一个攻击命令
        if(bAttacking)
        {
             AttackCmdQueue::getInstance().setNextAttack();
        }
    }
}

void HandleCommandQueue::clearAll()
{
	while(!m_vCommands.empty())
	{
		IHandleCommand *pCommand = *m_vCommands.begin();
		m_vCommands.pop_front();
		if(pCommand)
			pCommand->release();
	}
}

void HandleCommandQueue::pushBack(IHandleCommand* pCommand)
{
	m_vCommands.push_back(pCommand);
}

void HandleCommandQueue::pushFront(IHandleCommand* pCommand)
{
	m_vCommands.push_front(pCommand);
}

void HandleCommandQueue::clearBefore(IHandleCommand *pCmd)
{
	while(!m_vCommands.empty())
	{
		IHandleCommand *pCommand = *m_vCommands.begin();
		if(pCmd == pCommand)
			break;
		m_vCommands.pop_front();
		pCommand->release();
	}	
}

void HandleCommandQueue::clearAfter(IHandleCommand* pCmd)
{
	bool bStarted = false;
	IHandleCommand* pCommand = NULL;
	std::list<IHandleCommand*>::iterator it = m_vCommands.begin();
	while (it != m_vCommands.end())
	{
		pCommand = *it;
		if (!bStarted)
		{
			if (pCommand == pCmd)
				bStarted = true;
			++it;
		}
		else
		{
			pCommand->release();
			it = m_vCommands.erase(it);
		}
	}
}

// 移除某项指令
void HandleCommandQueue::RemoveCommand(IHandleCommand *pCommand)
{
	if (pCommand == NULL || m_vCommands.empty())
	{
		return;
	}

	m_vCommands.remove(pCommand);  // pCommand可能已经被销毁
}


// 从队列中移除指定技能
void HandleCommandQueue::removeCommandBySpellID(int nSpellID)
{
    IHandleCommand* pCommand = NULL;
    std::list<IHandleCommand*>::iterator it = m_vCommands.begin();
    while (it != m_vCommands.end())
    {
        pCommand = (*it);
        if (pCommand==NULL || (pCommand->getComandType() == CommandType_Attack && ((HandleCommandAttack*)pCommand)->getAttackData()->nID == nSpellID) )
        {
            it = m_vCommands.erase(it);
            if(pCommand)
            {
                WarningLn(__FUNCTION__": spell=" << nSpellID);
            }
        }
        else
        {
            ++it;
        }
    }
}
