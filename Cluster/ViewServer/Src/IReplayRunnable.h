#pragma once
#include "IRunnableCallback.h"
#include "ReplayContainer.h"



template<typename T>
struct IReplayRunnable : public IRunnableCallback
{
	T&	m_tHolder;

	IReplayRunnable(T& tHolder) : m_tHolder(tHolder)
	{
	}

	virtual void run() override
	{
		Run();
		IRunnableCallback* pCB = this;
		gCbQueue.push(pCB);// Ͷ���߼��������
	}

	virtual void release() override
	{
		delete this;
	}

	virtual	void callback() override
	{
		Callback(m_tHolder);
		m_tHolder.DecRef();
	}

protected:
	// ��Redis�߳�����
	virtual	void Run() = 0;
	// ���߼��߳�����
	virtual void Callback(T& tHolder) = 0;

	virtual void _OnSpawn() override
	{
		m_tHolder.IncRef();
	}

};