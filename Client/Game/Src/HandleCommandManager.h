#ifndef _COMMANDMANAGER_H
#define _COMMANDMANAGER_H

#include "HandleCommand.h"
#include "HandleCommandQueue.h"

class HandleCommandManager: public IHandleCommandManager,public TimerHandler, public IEventExecuteSink
{
	// 操作命令逻辑定时器
	#define HANDLE_COMMAND_LOGIC_TIMER	0
	// 操作命令逻辑定时器时间
	#define HANDLE_COMMAND_LOGIC_TIME	16
public:
	/////////////////////////////////////////IHandleCommandManager/////////////////////////////////////////
	/** 获取命令工厂接口
	*/
	virtual IHandleCommandFactory *getHandleCommandFactory();

	/** 创建其它要素，设置时钟
	*/
	virtual bool create();

	/** 显式关闭，特别是时钟，因为始终与系统有关
		系，在析构关闭时钟很危险
	*/
	virtual void close();

	virtual void update();

	virtual bool isIdle() { return m_commandQueue.isEmpty(); }

	/** 添加一个命令到队列尾部
	*/
	virtual void appendCommandTail(IHandleCommand *pCmd);

	/**  插入到表头部
	*/
	virtual void insertCommandFront(IHandleCommand *pCmd);

	/** 清除队列
	*/
	virtual void clear();	

	/** 清除除了本指令以外的其他指令
	*/
	virtual void clearBefore(IHandleCommand *pCmd);

	/** 清除除了本指令之后的其他指令
	*/
	virtual void clearAfter(IHandleCommand *pCmd);

	/** 移除某项指令
	*/
	virtual void RemoveCommand(IHandleCommand *pCommand);

    // 获取队列命令数量
    virtual int getCommandCount(void);

	/////////////////////////////////////////TimerHandler/////////////////////////////////////////
	// 定时器到了
	virtual void OnTimer(unsigned long dwTimerID);

    /////////////////////////////////////////TimerHandler/////////////////////////////////////////
    // 事件 
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

public:
	HandleCommandManager();
	~HandleCommandManager();

private:
	/** 消息队列
	*/
	HandleCommandQueue			m_commandQueue;

	// 命令工厂
	HandleCommandFactory		m_HandleCommandFactory;
};

#endif