// ProfileTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "SharePtrTester.h"


int _tmain(int argc, _TCHAR* argv[])
{
    SharePtrTester test;
    test.Create();
    test.Start();
    //test.Release();
    
    system("pause");

	return 0;
}

