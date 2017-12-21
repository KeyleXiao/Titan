/*******************************************************************
** 文件名:	PackagePtr.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	1/5/2015
** 版  本:	1.0
** 描  述:	

			跨线程传递的数据包结构
********************************************************************/


#include "share_ptr.h"
#include "buffer.h"
#include <string>

#pragma once

// 定义成智能指针，解决多线程对象生存期问题
typedef share_ptr<std::string>     PACKAGE_PTR;

using rkt::ibuffer;
using rkt::obuffer;


// 智能指针特化，这样他们就可以输入到流中
template <class buffer_allocator,class PTR, bool bInterface>
obuffer<buffer_allocator>& _cdecl operator<<(obuffer<buffer_allocator>& ob, const share_ptr<PTR,bInterface>& val)
{
	share_ptr<PTR,bInterface>::T_BAG packer(val);
	ob << packer;

	return ob;
}

template <class PTR,bool bInterface>
ibuffer& _cdecl operator>>(ibuffer& ib, share_ptr<PTR, bInterface>& val)
{
	share_ptr<PTR,bInterface>::T_BAG packer;
	ib >> packer;
	val = packer.get();

	return ib;
}

