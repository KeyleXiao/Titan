#ifndef TEST_CONCUR_H
#define TEST_CONCUR_H

#include "TestClient.hpp"

class CTestConcur : public CTestClient
{
	friend class CTestConcurRunGet;
	friend class CTestConcurRunSet;
public:
    CTestConcur();
    bool StartTest(const std::string &strHost);

private:
    void Test_GetS();
    void Test_Get();
    void Test_Set();

private:
    bool m_bExit;
    //std::mutex m_mutex;
	rkt::Mutex m_mutex;
};

class CTestConcurRunGet : public rkt::IRunnable
{
	CTestConcur * m_pTestConcur;
public:
	CTestConcurRunGet(CTestConcur * _pTestConcur):m_pTestConcur(_pTestConcur){ }
	~CTestConcurRunGet(){}

	virtual void run();
	virtual void release();
};

class CTestConcurRunSet : public rkt::IRunnable
{
	CTestConcur * m_pTestConcur;
public:
	CTestConcurRunSet(CTestConcur * _pTestConcur):m_pTestConcur(_pTestConcur){ }
	~CTestConcurRunSet(){}

	virtual void run();
	virtual void release();
};

#endif
