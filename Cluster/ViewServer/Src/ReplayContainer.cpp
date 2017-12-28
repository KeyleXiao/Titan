#include "stdafx.h"
#include "ReplayContainer.h"
#include "Replay.h"
#include "TimerAxis.h"
#include "RedisThread.h"

////���̺߳���  
//unsigned int __stdcall ThreadFun(PVOID pM)
//{
//	printf("�߳�ID��Ϊ%4d�����߳�˵��Hello World\n", GetCurrentThreadId());
//	return 0;
//}

//struct MyStruct : public IRunnable
//{
//	virtual void run() override
//	{
//		ErrorLn("=================================run()");
//	}
//
//
//	virtual void release() override
//	{
//		ErrorLn("=================================release()");
//	}
//	volatile bool bflag;
//};


ReplayContainer& gReplays = ReplayContainer::getInstance();

producer_consumer_queue<IRunnableCallback*> _gCbQueue;
producer_consumer_queue<IRunnableCallback*>& gCbQueue = _gCbQueue;

bool ReplayContainer::Init(TimerAxis* pTimerAxis)
{
	if (pTimerAxis == nullptr)
		return false;

	m_pTimerAxis = pTimerAxis;

	m_pTimerAxis->SetTimer(ETimerID_HeartBeat, ETimer_HeartBeat_Interval, this, INFINITY_CALL, "ReplayContainer::ETimerID_HeartBeat");

	return true;
}

void ReplayContainer::Shutdown()
{
	m_pTimerAxis->KillTimer(ETimerID_HeartBeat, this);
}

Replay* ReplayContainer::Get(const ReplayID& rID)
{
	auto it = m_ReplayMap.find(rID);
	if (it != m_ReplayMap.end())
		return &it->second;
	else
		return nullptr;
}

Replay* ReplayContainer::Insert(const ReplayID& rID)
{
	m_ReplayMap[rID].m_rID = rID;
	return &m_ReplayMap[rID];
}

bool ReplayContainer::Remove(const ReplayID& rID)
{
	auto it = m_ReplayMap.find(rID);
	if (it == m_ReplayMap.end())
		return true;

	if (!it->second.IsRefEmpty())
		return false;

	m_ReplayMap.erase(it);
	return true;
}

void ReplayContainer::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case ETimerID_HeartBeat:
	{
		// 1.����ص�
		std::vector<IRunnableCallback*> vecIRC;
		IRunnableCallback* pCB = nullptr;
		while (gCbQueue.pop(pCB))
		{
			pCB->callback();
			pCB->SetDone();
			if (pCB->NeedSpawnAgain())
				vecIRC.push_back(pCB);
			else
			{
				if (pCB->NeedRecovery())
					safeDelete(pCB);
			}
		}

		// 2.���������ս�ڵ������
		for each (auto entry in m_ReplayMap)
		{
			entry.second.HeartBeat();
		}

		// 3.�ٴ�Ͷ�ݡ�����1����ֱ��Ͷ�ݣ���Ϊ�˷�ֹ��ѭ��
		for (size_t i = 0; i < vecIRC.size(); i++)
		{
			gRedisThread.spawn(vecIRC.at(i));
		}
	}
	break;
	default:
		break;
	}
}
