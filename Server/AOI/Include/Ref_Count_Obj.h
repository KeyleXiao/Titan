/*******************************************************************
** 文件名:	Ref_Count_Obj.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/15/2014
** 版  本:	1.0
** 描  述:	引用计数对象

			
********************************************************************/


#pragma once

/**
	用于多线程安全
	使用者需要手动调用AddRef,Release
	当所有线程都释放后，对象才会真正释放
*/
class Ref_Count_Obj
{
private:
	volatile LONG	m_dwRef;            // 这个变量的地址据说要32位对齐

public:
	Ref_Count_Obj() : m_dwRef(1)
	{
	}

	virtual ~Ref_Count_Obj()
	{
	}

	// 子对象可继承，用来把野指针清理掉，安全一点
	virtual void onDestroy()
	{
	};

	// 释放有两种,一种是告知释放该对象的引用计数，一种是告知对象已销毁，其他线程用的时候注意已无效
	void Release( bool bDestroy=false )
	{
		if ( bDestroy )
		{
			onDestroy();
		}

		if ( ::InterlockedDecrement(&m_dwRef)==0 )
		{
			delete this;
		}
	}

	// 增加引用计数
	void AddRef()
	{
		::InterlockedIncrement(&m_dwRef);
	}
};
