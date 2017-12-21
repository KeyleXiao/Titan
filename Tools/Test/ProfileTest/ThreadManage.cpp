#include "StdAfx.h"
#include "ThreadManage.h"
#include "Thread.h"


ThreadManage::ThreadManage(void)
{

}


ThreadManage::~ThreadManage(void)
{
}

rkt::ThreadPool* ThreadManage::getThreadPool()
{
	return &m_threadPool;
}

void ThreadManage::StartThread()
{
	m_threadPool.start();
}

void ThreadManage::TerminateThread()
{
	m_threadPool.clear();
}

