/*******************************************************************
** 文件名:	EventRefBase.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-8-5
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once


// 引用计数对象接口
struct IRefObj
{
	/** 增加引用计数
	*/
	virtual void AddRef() = 0;

	/** 减少引用计数
	*/
	virtual void Release() = 0;

    virtual void* get(){return 0;}
};

template<typename T>
class RefObj : public IRefObj
{
public:
    static IRefObj* createRefObj(T& data){return new RefObj(data);}

    virtual void AddRef(){::InterlockedIncrement(&_count);}
    virtual void Release(){::InterlockedDecrement(&_count); if(_count == 0) delete this;}
    
    virtual void* get(){return &m_data;}
private:
    RefObj();
    RefObj(const RefObj& rhs);
    RefObj(T& data):_count(1){m_data = data;}
    ~RefObj(){m_data = T(0);}

    volatile LONG _count;
    T m_data;
};


// 事件基础类
struct event_base
{
	// 引用计数对象
	IRefObj*   ptrRef;

	event_base() : ptrRef(0) {}
};
