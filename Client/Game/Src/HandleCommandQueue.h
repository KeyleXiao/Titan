#ifndef __HandleCommandQueue_H__
#define __HandleCommandQueue_H__

#include "IHandleCommand.h"

class HandleCommandQueue
{
public:
	void pushBack(IHandleCommand* pCommand);
	void pushFront(IHandleCommand* pCommand);
	void update();
	void clearAll();
	void clearBefore(IHandleCommand *pCmd);
	void clearAfter(IHandleCommand* pCmd);
	bool isEmpty() {return m_vCommands.empty();}

	// 移除某项指令
	void RemoveCommand(IHandleCommand *pCommand);

    // 从队列中移除指定技能
    void removeCommandBySpellID(int nSpellID);

    // 获取命令数量
    size_t count() { return m_vCommands.size(); }

public:
    HandleCommandQueue();

    ~HandleCommandQueue();

private:
	std::list<IHandleCommand*>		m_vCommands;
};

#endif // __HandleCommandQueue_H__