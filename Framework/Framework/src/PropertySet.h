/*******************************************************************
** 文件名:	PropertySet.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	5/30/2016
** 版  本:	1.0
** 描  述:	多线程安全的属性集访问对象

			
********************************************************************/

#pragma once

#include "IPropertySet.h"
#include "net/reader_writer_lock.h"

// 多线程安全的属性集
class PropertySet : public IPropertySet
{
	char *                  m_data;             // 属性集内容
	size_t                  m_size;			    // 属性集大小
	TinyReaderWriterLock    m_lock;             // 读写锁
	bool                    m_bAlloc;           // 是否是内部分配的内存

public:
	PropertySet() : m_data(0),m_size(0),m_bAlloc(false){}

	virtual ~PropertySet(){}

	bool create( size_t bytes,void * pDataChunk )
	{
		if ( pDataChunk != 0 )
		{
			m_data = (char *)pDataChunk;
			m_bAlloc = false;
		}else
		{
			m_data = (char *)malloc( bytes );
			if ( m_data==0 )
				return false;
			m_bAlloc = true;
		}

		m_size = bytes;
		return true;
	}

	// 多线程安全获取属性[会独立加锁]
	// @param offset : 字节偏移数
	// @param bytes  : 字节数
	// @param data   : [输出]数据内容
	// @param len    : [输出]数据长度
	virtual bool get_Property(size_t offset, size_t bytes, void* data, size_t len)
	{
		if ( m_data==0 )
			return false;

		if ( offset+bytes > m_size )
			return false;

		if ( data==0 || len<bytes )
			return false;

		m_lock.EnterRead();

		memcpy( data,m_data+offset,bytes );
	
		m_lock.LeaveRead();
		return true;
	}

	// 多线程安全设置属性[会独立加锁]
	// @param offset : 字节偏移数
	// @param bytes  : 字节数
	// @param data   : [输出]数据内容
	// @param len    : [输出]数据长度
	virtual bool set_Property(size_t offset,size_t bytes, void* data,bool camp=false)
	{
		if ( m_data==0 )
			return false;

		if ( offset+bytes > m_size )
			return false;

		if ( data==0 )
			return false;

		bool bReturn = true;

		m_lock.EnterWrite();

		if ( camp == true )
		{
			// 如果新老属性相等就直接返回
			bReturn = memcmp(m_data+offset,data,bytes)!=0;
		}

		if ( bReturn ) memcpy( m_data+offset,data,bytes );
	
		m_lock.LeaveWrite();

		return bReturn;
	}

	virtual void release()
	{
		if ( m_data!=0 && m_bAlloc )
		{
			free(m_data);
			m_data = 0;
		}

		delete this;
	}
};
