/*******************************************************************
** 文件名:	Broadcast_Package.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/16/2014
** 版  本:	1.0
** 描  述:	要广播的数据包

			
********************************************************************/

#pragma once

#include "Ref_Count_Obj.h"

/**
	因为一个数据包要广播给很多个客户端，这样在很多个客户端的缓冲队列里都要排队，
	用引用计数的方式就可以共享，可以减少很多内存
*/
class Broadcast_Package : public Ref_Count_Obj
{
public:
	void *      pBuffer;
	int			nLen;

	Broadcast_Package() : pBuffer(0),nLen(0)
	{
	}

	virtual ~Broadcast_Package()
	{
		if ( pBuffer)
		{
			free(pBuffer);   // 这个是在一个线程分配在另外一个线程释放的，底层的内存分配器必须支持这种机制
			pBuffer = 0;
			nLen = 0;
		}
	}

	void setContext( void * pContext,int len)
	{
		if ( pContext!=0 && len>0 )
		{
			nLen   = len;
			pBuffer= malloc(len);				// 这个是在一个线程分配在另外一个线程释放的，底层的内存分配器必须支持这种机制
			memcpy(pBuffer,pContext,len);
		}
	}
};