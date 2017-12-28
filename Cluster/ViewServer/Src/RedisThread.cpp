#include "stdafx.h"
#include "RedisThread.h"
#include "IReplayRunnable.h"


RedisThread& gRedisThread = RedisThread::getInstance();
RedisThread::RedisThread()
{
	m_pRedisThread = new rkt::Thread;
}

RedisThread::~RedisThread()
{
	safeDelete(m_pRedisThread);
}

bool RedisThread::spawn(IRunnableCallback* task, int priority)
{
	task->OnSpawn();
	return m_pRedisThread->spawn(task, priority);
}

void RedisThread::wait()
{
	m_pRedisThread->wait();
}
void RedisThread::start()
{
	m_pRedisThread->start();
}
void RedisThread::pause()
{
	m_pRedisThread->pause();
}
void RedisThread::terminate()
{
	m_pRedisThread->terminate();
}

void RedisThread::sleep(unsigned long timeout)
{
	m_pRedisThread->sleep(timeout);
}