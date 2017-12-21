/*******************************************************************
** 文件名:	NetBuffer.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	02/08/2008
** 版  本:	1.0
** 描  述:	网络通讯用的buffer
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __NET_BUFFER_H__
#define __NET_BUFFER_H__

#include "AsynIoFrame.h"
using namespace rkt;

/**
@name : IO状态
@brief:
*/
enum IOState
{
	good    =  0,     // IO操作正常
	error       ,     // IO操作错误
	fail        ,     // IO操作异常
};

/**
@name : 实现网络通讯时收发包的流操作
@brief: 
*/
class NetIn
{
public:
	template<typename T>
	NetIn & operator >> (T & value )
	{
		Read((char *)&value,sizeof(value));
		return *this;
	}

	void Read(char * buffer,size_t len )
	{
		if ( m_nDataLen-m_nCurPos<len)
		{
			m_nState = error;
			return ;
		}

		memcpy(buffer,m_pData+m_nCurPos,len);
		m_nCurPos += len;
		return ;
	};

	bool InGood()
	{
		return m_nState == good;
	}

	const char * GetInBuffer() const { return m_pData+m_nCurPos; }

	size_t GetLeaveLen() const { return m_nDataLen-m_nCurPos; }

	NetIn() : m_pData(0),m_nDataLen(0),m_nCurPos(0),m_nState(good)
	{
	}

	NetIn(const char * data,size_t data_len ) : m_pData(data),m_nDataLen(data_len),m_nCurPos(0),m_nState(good)
	{
	}

protected:
	const char *    m_pData;
	size_t          m_nDataLen;
	size_t          m_nCurPos;
	size_t          m_nState;
};

/**
@name : 实现网络通讯时收发包的流操作
@brief: 另外发包时这个类试图通过网络的内存分配接口分配内存,
        这样就可以告诉网络层不用多余的进行一个内存拷贝
*/
class NetOut
{
public:
	template<typename T>
	NetOut & operator << (const T & value )
	{
		Write((const char *)&value,sizeof(value));
		return *this;
	}

	void Write(const char * data,size_t len )
	{
		if ( m_nDataLen-m_nCurPos<len)
		{
			ReAlloc(m_nDataLen+len*8);
		}

		memcpy(m_pData+m_nCurPos,data,len);
		m_nCurPos += len;
	}

	bool OutGood()
	{
		return m_nState == good;
	}

	// 把缓冲区分离出来，如果打算把内存交给通讯层去释放的话
	void DetachBuffer(char ** ppBuffer,size_t & len )
	{
		*ppBuffer = m_pData;
		len = m_nCurPos;
		m_pData = 0;
		m_nDataLen = 0;
		m_nCurPos  = 0;
	}

	char * GetOutBuffer() { return m_pData; }

	size_t GetBufferLen() const { return m_nCurPos; }

	NetOut(size_t data_len) : m_nDataLen(data_len),m_nCurPos(0),m_nState(good)
	{
		m_pData = (char *)AllocateAsynIoBuffer(m_nDataLen);
	}

	NetOut() : m_pData(0),m_nDataLen(0),m_nCurPos(0),m_nState(good)
	{
	}

	virtual ~NetOut()
	{
		if ( m_pData )
		{
			DeallocateAsynIoBuffer(m_pData);
			m_pData = 0;
		}
	}

protected:
	void ReAlloc(size_t data_len)
	{
		if ( data_len<=0 )
			data_len = 32;

		if ( data_len<m_nDataLen )
			return;

		m_nDataLen = data_len;
		void * pBuffer = AllocateAsynIoBuffer(m_nDataLen);
		assert(pBuffer);

		if ( m_pData )
		{
			if ( m_nCurPos ) 
				memcpy(pBuffer,m_pData,m_nCurPos);

			DeallocateAsynIoBuffer(m_pData);
		}

		m_pData = (char *)pBuffer;
	}

protected:
	char *          m_pData;
	size_t          m_nDataLen;
	size_t          m_nCurPos;
	size_t          m_nState;
};

/**
@name : 实现网络通讯时收发包的流操作
@brief:
*/
class NetBuffer : public NetIn,public NetOut
{
public:
	NetBuffer(const char * data,size_t data_len) : NetIn(data,data_len){}

	NetBuffer(size_t data_len) : NetOut(data_len){}

	NetBuffer(){}

	virtual ~ NetBuffer() {}
};

#endif//__NET_BUFFER_H__