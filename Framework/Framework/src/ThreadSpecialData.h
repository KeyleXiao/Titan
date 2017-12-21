/*******************************************************************
** 文件名:	TheadSpecialData.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	18/10/2014
** 版  本:	1.0
** 描  述:	线程私有存储数据

			
********************************************************************/

#pragma once

#include <map>

#include "Simple_Atom_Lock.h"

template
<
	typename DATA_TYPE,
	DATA_TYPE NULL_DATA=0
> 
class TheadSpecialData
{
public:
	typedef std::map<unsigned long,DATA_TYPE>  DATA_LIST;   

    TheadSpecialData() : m_mutex(0)
	{
		m_dwTLSIndex = ::TlsAlloc();
	}

	virtual ~TheadSpecialData()
	{
		if ( m_dwTLSIndex!=0 )
		{
			::TlsFree(m_dwTLSIndex);
			m_dwTLSIndex  =0;
		}
	}

	DATA_TYPE getData()
	{
		DATA_TYPE * pData = (DATA_TYPE *)TlsGetValue(m_dwTLSIndex);
		if ( pData==0 )
		{
			return NULL_DATA;
		}

		return *pData;
	}

	void setData(DATA_TYPE v)
	{
		DWORD dwThread = (DWORD)GetCurrentThreadId();

		Simple_Atom_Lock lock(&m_mutex);		
		m_dataList[dwThread] = v;

		DATA_LIST::iterator it = m_dataList.find(dwThread);
		if ( it!=m_dataList.end() )
		{
			TlsSetValue(m_dwTLSIndex,&(it->second));
		}
	}

	void getList(DATA_LIST & lst)
	{
		Simple_Atom_Lock lock(&m_mutex);
		lst = m_dataList;
	}

protected:
	DWORD       m_dwTLSIndex;			// 线程专有存储索引  
	DATA_LIST   m_dataList;             // 数据列表
	volatile	LONG      m_mutex;
};