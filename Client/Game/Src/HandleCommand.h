#pragma once

#include "IHandleCommand.h"

/**
@name : 命令工厂接口
*/
class HandleCommandFactory: public IHandleCommandFactory
{
public:
	/////////////////////////////////////////IHandleCommandFactory/////////////////////////////////////////
	/**
	@name            : 增加攻击命令
	*/
	virtual void addAttackCommand(const SHandleCommand_Attack &data);
	
	/**
	@name            : 清除攻击命令
	*/
	virtual void clearAttackCommand(void);

	/**
	@name            : 创建攻击命令
	@return          : 返回攻击命令对象指针
	*/
	virtual IHandleCommand *CreateCommand_Attack(const SHandleCommand_Attack &data);

	/**
	@name            : 靠近目标命令
	@return          : 返回靠近目标命令对象指针
	*/
	virtual IHandleCommand *CreateCommand_Close(const SHandleCommand_Close &data);

	/**
	@name            : 靠近目标命令
	@return          : 返回靠近目标命令对象指针
	*/
	virtual IHandleCommand *CreateCommand_Pursuit(const SHandleCommand_Pursuit &data);

public:
	/////////////////////////////////////////HandleCommandFactory/////////////////////////////////////////
	// 构造函数
	HandleCommandFactory();
	// 析构函数
	~HandleCommandFactory();

    // 从队列中移除指定技能
    void removeCommandBySpellID(int nSpellID);
};