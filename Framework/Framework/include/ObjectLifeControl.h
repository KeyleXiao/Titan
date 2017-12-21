/*******************************************************************
** 文件名:	ObjectLifeControl.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-8-5
** 版  本:	1.0
** 描  述:	对象生命周期控制

********************************************************************/

#pragma once

#include "EventRefBase.h"

typedef void (* AddRefPtr)();
typedef void (* ReleasePtr)();
typedef void (* DestroyCallBackPtr)(void*);

// 对象周期持久类
template<class Interface>
class ObjectLifeControl : IRefObj
{
	// 对象列表
	struct LifeObject
	{
		AddRefPtr  ptrAddRef;
		ReleasePtr ptrReleaseRef;
		DestroyCallBackPtr  ptrCallBack;

		Interface*	pObj;
	};

public:
	ObjectLifeControl() : m_dwRef(0)
	{
		AddRef();
	}

	~ObjectLifeControl()
	{
		m_objList.clear();
	}

	/** 附加对象到管理中
	*/
	Interface * Attach(Interface *pObj, AddRefPtr ptrAdd, ReleasePtr ptrRelease, DestroyCallBackPtr ptrCallBack)
	{
		LifeObject o;
		o.pObj = pObj;
		o.ptrAddRef = ptrAdd;
		o.ptrReleaseRef = ptrRelease;
		o.ptrCallBack = ptrCallBack;

		m_objList.push_back(o);

		return pObj;
	}


	/** 增加引用计数
	*/
	virtual void AddRef()
	{
		::InterlockedIncrement(&m_dwRef);

		for ( vector<LifeObject>::iterator iter = m_objList.begin(); iter != m_objList.end(); ++iter )
		{
			if(iter->ptrAddRef)
			{
				iter->ptrAddRef();
			}
		}
	}

	/** 减少引用计数
	*/
	virtual void Release()
	{
		for ( vector<LifeObject>::iterator iter = m_objList.begin(); iter != m_objList.end(); ++iter )
		{
			if(iter->ptrReleaseRef)
			{
				iter->ptrReleaseRef();
			}
		}

		if ( ::InterlockedDecrement(&m_dwRef)==0 )
		{
			//ErrorLn("ObjectLifeControl:Release()");
			void * pObj = NULL;
			for ( vector<LifeObject>::iterator iter = m_objList.begin(); iter != m_objList.end(); ++iter )
			{
				pObj = iter->pObj;
				if(iter->ptrCallBack)
				{
					iter->ptrCallBack(pObj);
				}
				if (iter->pObj)
				{
					iter->pObj->release();
				}
			}

			delete this;
		}
	}


private:
	volatile LONG		m_dwRef;
	vector<LifeObject>	m_objList;
};
