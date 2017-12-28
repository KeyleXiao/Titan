#pragma once


struct IRunnableCallback;
class RedisThread : public Singleton<RedisThread>
{
public:
	RedisThread();
	virtual ~RedisThread();

	bool spawn(IRunnableCallback* task, int priority = 0);

	void wait();
	void start();
	void pause();
	void terminate();

	void sleep(unsigned long timeout);

private:
	rkt::Thread*		m_pRedisThread;
};
extern RedisThread& gRedisThread;	// 负责读取Redis的线程