#include "stdafx.h"
#include <stdio.h>
#include "TestBase.hpp"
#include "TestGeneric.hpp"
#include "TestString.hpp"
#include "TestList.hpp"
#include "TestSet.hpp"
#include "TestHash.hpp"
#include "TestZset.hpp"
#include "TestConcur.hpp"

#include "IRedisConnector.h"

API_EXPORT  int RKT_CDECL test()
{
    std::string strHost = "127.0.0.1";
	const int Fail = 0;
	const int Success = 1;

    {
        CTestBase testBase;
        if (!testBase.StartTest(strHost))
            return Fail;
		TraceLn("CTestBase " << "Test Success!");

		CTestGeneric testKeys;
		if (!testKeys.StartTest(strHost))
			return Fail;
		TraceLn("CTestGeneric " << "Test Success!");

        CTestString testStr;
        if (!testStr.StartTest(strHost))
			return Fail;
		TraceLn("CTestString " << "Test Success!");

		CTestList testList;
		if (!testList.StartTest(strHost))
			return Fail;
		TraceLn("CTestList " << "Test Success!");

        CTestSet testSet;
        if (!testSet.StartTest(strHost))
			return Fail;
		TraceLn("CTestSet " << "Test Success!");

        CTestHash testHash;
        if (!testHash.StartTest(strHost))
			return Fail;
		TraceLn("CTestHash " << "Test Success!");

        CTestZset testZset;
        if (!testZset.StartTest(strHost))
			return Fail;
		TraceLn("CTestZset " << "Test Success!");

		// �������ʱ�䳬����������1Сʱδ���������Ҫ�Ų�(����ܿ��ܽ���������������Դ������ϵͳ������δ����) TODO
		//CTestConcur testConcur;
		//if (!testConcur.StartTest(strHost))
		//	return Fail;
		//TraceLn("CTestConcur " << "Test Success!");
    }
    return Success;
}

