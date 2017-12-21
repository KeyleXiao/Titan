/*******************************************************************
** 文件名:	HandleCommand.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	14/5/2008   17:03
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "HandleCommand.h"
#include "HandleCommandAttack.h"
#include "HandleCommandClose.h"
#include "HandleCommandPursuit.h"

/////////////////////////////////////////IHandleCommandFactory/////////////////////////////////////////
/**
@name            : 增加攻击命令
*/
void HandleCommandFactory::addAttackCommand(const SHandleCommand_Attack &data)
{
	AttackCmdQueue::getInstance().addAttackCommand(data);
}


/**
@name            : 清除攻击命令
*/
void HandleCommandFactory::clearAttackCommand(void)
{
	AttackCmdQueue::getInstance().clearAttackCommand();
}


/**
@name            : 创建攻击命令
@return          : 返回攻击命令对象指针
*/
IHandleCommand *HandleCommandFactory::CreateCommand_Attack(const SHandleCommand_Attack &data)
{
	return new HandleCommandAttack(data);
}

/**
@name            : 靠近目标命令
@return          : 返回靠近目标命令对象指针
*/
IHandleCommand *HandleCommandFactory::CreateCommand_Close(const SHandleCommand_Close &data)
{
	return new HandleCommandClose(data);
}

/**
@name            : 靠近目标命令
@return          : 返回靠近目标命令对象指针
*/
IHandleCommand *HandleCommandFactory::CreateCommand_Pursuit(const SHandleCommand_Pursuit &data)
{
	return new HandleCommandPursuit(data);
}

/////////////////////////////////////////HandleCommandFactory/////////////////////////////////////////
HandleCommandFactory::HandleCommandFactory()
{
}

HandleCommandFactory::~HandleCommandFactory()
{
}

// 从队列中移除指定技能
void HandleCommandFactory::removeCommandBySpellID(int nSpellID)
{
    AttackCmdQueue::getInstance().removeCommandBySpellID(nSpellID);
}