#include "stdafx.h"
#include "ValueOpStringTest.h"



ValueOpStringTest::ValueOpStringTest(Redis::IValueOp* pValueOp)
	: m_pValueOp(pValueOp)
{}
bool ValueOpStringTest::Run()
{
	string keyString("keyString");
	string valueStringRet;
	{
		// char*
		char* psz = "valueString";
		m_pValueOp->set(keyString, psz);
		m_pValueOp->get(keyString, valueStringRet);
		if (valueStringRet.compare(psz) != 0)
		{
			ErrorLn("--------------- valueStringRet.compare(char*) != 0");
		}
	}
	{
		// char[]
		char sz[] = "valueString";
		m_pValueOp->set(keyString, sz);
		m_pValueOp->get(keyString, valueStringRet);
		if (valueStringRet.compare(sz) != 0)
		{
			ErrorLn("--------------- valueStringRet.compare(char[]) != 0");
		}
	}
	{
		// std::string
		string valueString("valueString");
		m_pValueOp->set(keyString, valueString);
		m_pValueOp->get(keyString, valueStringRet);
		if (valueStringRet.compare(valueString) != 0)
		{
			ErrorLn("--------------- valueStringRet.compare(std::string) != 0");
		}
	}

	return true;
}