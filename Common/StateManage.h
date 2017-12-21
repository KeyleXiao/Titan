/*******************************************************************
** 文件名:	E:\speed\Server\MatchServer\Src\StateManage.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	4/24/2015	11:25
** 版  本:	1.0
** 描  述:	
            状态管理
********************************************************************/

#pragma once


template<typename TState>
class StateManage
{
public:
	// 调用者new出状态 
	virtual void	newState(TState* pState);
	// 切换状态
	virtual void	toState(uint32 u32ID);

    // 恢复至状态
    virtual void	restoreState(uint32 u32ID);

	uint32 const	getPreStateID(){return m_u32PreState;};
	TState*	getState(){return m_pCurrent;};

    virtual void    release();

public:
	// 前一个状态
	uint32			m_u32PreState;	
	// 当前状态
	TState*			m_pCurrent;	
	// 所有的状态
	vector<TState*>	m_States;


public:
    
	StateManage(uint32 u32StateCount);
	virtual ~StateManage();
};

template<typename TState>
void StateManage<TState>::release()
{
    for (size_t i = 0; i < m_States.size(); ++i)
    {
       m_States[i]->release();

       delete m_States[i];
       m_States[i] = nullptr;
    }
}

template<typename TState>
StateManage<TState>::~StateManage()
{
}

template<typename TState>
StateManage<TState>::StateManage( uint32 u32StateCount )
	:m_pCurrent(nullptr)
	,m_u32PreState(UINT_MAX)
{
	m_States.resize(u32StateCount, nullptr);
}

template<typename TState>
void StateManage<TState>::toState( uint32 u32ID )
{
	TraceLn("StateManage::toState u32ID=" << u32ID);
	if (u32ID >= m_States.size())
	{
		ErrorLn("StateManage::toState u32ID >= m_States.size()=" << m_States.size());
		return;
	}
	if (nullptr == m_States[u32ID])
	{
		ErrorLn("StateManage::toState not id=" << u32ID);
		return;
	}

	if (nullptr != m_pCurrent)
	{
		m_u32PreState = m_pCurrent->getID();
		m_pCurrent->onLeave();
	}
	m_pCurrent = m_States[u32ID];
	m_pCurrent->onEnter();
}

template<typename TState>
void StateManage<TState>::restoreState( uint32 u32ID )
{
    TraceLn("StateManage::toState u32ID=" << u32ID);
    if (u32ID >= m_States.size())
    {
        ErrorLn(__FUNCTION__": u32ID >= m_States.size()=" << m_States.size());
        return;
    }
    if (nullptr == m_States[u32ID])
    {
        ErrorLn(__FUNCTION__": not id=" << u32ID);
        return;
    }

    m_pCurrent = m_States[u32ID];
    m_pCurrent->restoreState();
}

template<typename TState>
void StateManage<TState>::newState( TState* pState )
{
	TraceLn("StateManage::newState pState->getID()=" << pState->getID());
	if (pState->getID() >= m_States.size())
	{
		ErrorLn("StateManage::newState pState->getID() >= m_States.size()=" << m_States.size());
		return;
	}
	if (nullptr != m_States[ pState->getID() ])
	{
		ErrorLn("StateManage::newState have id=" << pState->getID());
		return;
	}

	m_States[ pState->getID() ] = pState;
}
