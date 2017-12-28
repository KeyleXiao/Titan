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
		ETimer_HeartBeat_Interval = 50,	// 心跳Timer间隔
	};
public:
	bool Init(TimerAxis* pTimerAxis);

	void Shutdown();

	Replay* Get(const ReplayID& rID);

	Replay* Insert(const ReplayID& rID);

	// 成功删除或者无此元素，都返回true
	bool	Remove(const ReplayID& rID);


	virtual void OnTimer(unsigned long dwTimerID) override;

protected:
private:
	typedef	std::map<ReplayID, Replay>	ReplayMap;

	ReplayMap		m_ReplayMap;

	TimerAxis*		m_pTimerAxis;		// 时间轴
};
extern ReplayContainer& gReplays;
extern producer_consumer_queue<IRunnableCallback*>&	gCbQueue;	// Callback消息队列