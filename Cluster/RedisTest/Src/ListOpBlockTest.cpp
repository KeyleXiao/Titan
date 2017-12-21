#include "stdafx.h"
#include "ListOpBlockTest.h"


ListOpBlockTest::ListOpBlockTest(Redis::IListOp* pValueOp)
	: m_pValueOp(pValueOp)
{}

const char* ListOpBlockTest::GetName()
{
	return "ListOpBlockTest";
}

bool ListOpBlockTest::Run()
{
	string key("keyListOpBlockTest");


	return true;
}


//void RedisMng::TempTest()
//{
//	//////////////////////////////////////////////////////////////////////////
//	// 阻塞List
//	{
//		long length = 0;
//		string keyBlock("keyBlock");
//
//		m_pRedis->listOp()->lRem(keyBlock, -2, "ValueB1", &length);
//
//		m_pRedis->listOp()->lInsertAfter(keyBlock, "ValueB1", "hahaha");
//		m_pRedis->listOp()->lInsertBefore(keyBlock, "hahaha", "heihei");
//
//		// 存
//		//m_pRedis->listOp()->lPush(keyBlock, "ValueB1", &length);
//		//m_pRedis->listOp()->lPush(keyBlock, "ValueB2", &length);
//		m_pRedis->listOp()->rPush(keyBlock, 1024, &length);
//		int iRet = 0;
//		m_pRedis->listOp()->rPop(keyBlock, iRet);
//		// 取
//		//std::vector<string> vecStrB;
//		//m_pRedis->listOp()->Brpop(keyBlock, &vecStrB);
//		string keyBlock99("keyBlock99");
//		string strB;
//		m_pRedis->listOp()->Brpop(keyBlock99, strB, 2);
//
//
//	}
//
//	// 阻塞List 多个key
//	{
//		//string keyBlock2("keyBlock2");
//		//string keyBlock3("keyBlock3");
//		//// 存
//		//m_pRedis->listOp()->lPush(keyBlock3, "ValueB1");
//		//m_pRedis->listOp()->lPush(keyBlock3, "ValueB2");
//		//// 取
//		//string strB;
//		//std::vector<std::string> vecKey;
//		//vecKey.push_back(keyBlock2);
//		//vecKey.push_back(keyBlock3);
//		//string strKeyR;
//		//m_pRedis->listOp()->Brpop(vecKey, strB, &strKeyR);
//		//m_pRedis->listOp()->Blpop(vecKey, strB);
//	}
//}