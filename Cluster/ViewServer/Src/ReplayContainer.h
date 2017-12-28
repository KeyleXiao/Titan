#pragma once
#include "producer_consumer_queue.h"


class Replay;
struct IRunnableCallback;

class ReplayContainer : public Singleton<ReplayContainer>, public TimerHandler
{
	enum
	{
		ETimerID_HeartBeat = 0,
	};
	enum
	{
		ETimer_HeartBeat_Interval = 50,	// ����Timer���
	};
public:
	bool Init(TimerAxis* pTimerAxis);

	void Shutdown();

	Replay* Get(const ReplayID& rID);

	Replay* Insert(const ReplayID& rID);

	// �ɹ�ɾ�������޴�Ԫ�أ�������true
	bool	Remove(const ReplayID& rID);


	virtual void OnTimer(unsigned long dwTimerID) override;

protected:
private:
	typedef	std::map<ReplayID, Replay>	ReplayMap;

	ReplayMap		m_ReplayMap;

	TimerAxis*		m_pTimerAxis;		// ʱ����
};
extern ReplayContainer& gReplays;
extern producer_consumer_queue<IRunnableCallback*>&	gCbQueue;	// Callback��Ϣ����