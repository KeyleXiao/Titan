#include "StdAfx.h"
#include "Product.h"


Product::Product(const char* apData, int nSize)
	: m_nSize(nSize)
{
	m_pData = new char[nSize];
	memcpy(m_pData, apData, nSize);
}

Product::Product(int nData)
{
	m_nData = nData;
	m_pData = 0;
	m_nSize = 0;
}
Product::Product()
{
	m_nData = 0;
	m_pData = 0;
	m_nSize = 0;
}


Product::~Product(void)
{
	if (m_pData)
	{
		delete []m_pData;
		m_pData = 0;
	}
}

int Product::GetSize()
{
	return m_nSize;
}

char* Product::GetData()
{
	return m_pData;
}

int Product::GetNumData()
{
	return m_nData;
}
