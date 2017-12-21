// ===========================================================================
//	filename: 	share_ptr.h
//
//	author:		CHENTAO
//
//	created:	2007/01/26
//	
//	purpose:	一个简单的share_ptr
//
// ===========================================================================

#ifndef  __share_ptr_h__
#define  __share_ptr_h__

//#define  SHARE_PTR_DEBUG

#	ifdef SHARE_PTR_DEBUG
#include "IStackWalk.h"
RKT_API void * g_share_ptr_debug;          // 设置了这个变量会打印该变量所有引用计数的增减情况
#	endif

#include "delay_cleaner.h"

// ---------------------------------------------------------------------------
// Name	: 拷贝智能指针
// Desc : stl的auto_ptr不能放到stl容器中
//		  这里是简单实现的引用计数型智能指针
//		  智能指针在Boost库和Loki库中有良好的实现,但用一个智能指针要包含太多
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 这个主要用于管理多个模块引用一个对象时的生存期问题
// 特别用于多线程编程时，一个线程创建的对象给另外一个线程用，这其中的生存期管理
// ---------------------------------------------------------------------------

template<typename T, bool is_interface = false>
class share_ptr
{
	typedef		volatile LONG	ref_count;

	// 此函数不能被外界调用
    template<bool b>
    void release_impl()
    {
        if (_count==0 )
            return ;

        if ( ::InterlockedDecrement(_count) == 0 )
        {
			ref_count * temp_count = _count;
			_count = 0;
			if(*temp_count==0)// 如果不等于0，说明又有人成功引用了
			{
				delay_cleaner<ref_count>::getInstance().clean( temp_count );         // 延迟回收引用计数，否则多线程容易崩溃
				if ( _ptr ) { delete _ptr; _ptr = 0; } // 对象会在任意一个线程被释放，所以释构函数中不要有太多逻辑，可以在destroy()函数中提前清理掉
			}
        }
    }

    template<>
    void release_impl<true>()
    {
        if (_count==0 )
            return ;

        if ( ::InterlockedDecrement(_count) == 0 )
        {
			ref_count * temp_count = _count;
			_count = 0;
			if(*temp_count==0)// 如果不等于0，说明又有人成功引用了
			{
				delay_cleaner<ref_count>::getInstance().clean( temp_count );         // 延迟回收引用计数，否则多线程容易崩溃
				if ( _ptr ) { _ptr->release(); _ptr = 0; }// 对象会在任意一个线程被释放，所以释构函数中不要有太多逻辑，可以在destroy()函数中提前清理掉
			}
		}
    }

	void release()
	{
        release_impl<is_interface>();
	}

public:
	share_ptr() : _count(0),_ptr(0){}

#	ifndef SHARE_PTR_DEBUG
	explicit share_ptr(T * const p) 
	{  
		if ( p ) {	_count = new ref_count(1); _ptr = p; }             // 初始化不是原子的，但问题不大，只需保证初始化结束后才能多线程调用
		else {	_count = 0; _ptr = 0; }
	}

	share_ptr( const share_ptr & p )
	{
		_count = p._count;
		if(_count==0)
        {
            _ptr = 0;
			return;
        }
		
		if ( ::InterlockedIncrement(_count)<=1 )    // 递增引用计数还小于=1，那表示原指针已经释放了._count是延迟释放的，所以很安全
		{
			_count = 0;
            _ptr = 0;
			return;
		}

		_ptr = p._ptr;
		if ( p._count==0 ) {_ptr=0;_count=0;}   // 保险一点
	}

	share_ptr & operator =( const share_ptr & p )
	{
		if ( this==&p )      
			return *this;

		release();

		_count = p._count;
		if ( _count==0) 
        {
            _ptr = 0;
			return *this;
        }

		if ( ::InterlockedIncrement(_count)<=1 )    // 递增引用计数还小于=1，那表示原指针已经释放了._count是延迟释放的，所以很安全
		{
			_count = 0;
            _ptr = 0;
			return *this;
		}

		_ptr = p._ptr;
		if ( p._count==0 ) {_ptr=0;_count=0;}   // 保险一点
		return *this;
	}

#	else

	////////////////////////////* 调试版本 *////////////////////////////////////////////////
	// 打印调试信息
#	define DEBUG_INFO( op,count )	if ( g_share_ptr_debug==_ptr && _ptr )  \
		ErrorLn("share ptr " << op <<" ["<<getStackWalk()->getCallStack(4)<< "] count=" << count << " obj=" << (INT_PTR)this );

	explicit share_ptr(T * const p)
	{  
		if ( p ) {	_count = new ref_count(1); _ptr = p; }
		else {	_count = 0; _ptr = 0; }

		DEBUG_INFO("new",*_count);
	}


	share_ptr( const share_ptr & p ) : _count(p._count),_ptr(p._ptr)
	{
		if(_count){	
			unsigned long c = ::InterlockedIncrement(_count);

			DEBUG_INFO("addref",c);
		}
	}

	share_ptr & operator =( const share_ptr & p )
	{
		if (this == &p)
		{
			return *this;
		}

		release();

		_count = p._count;
		_ptr = p._ptr;
		if ( _count)
		{
			unsigned long c = ::InterlockedIncrement(_count);

			if ( g_share_ptr_debug==_ptr &&_ptr)
			ErrorLn("share ptr equal ["<<getStackWalk()->getCallStack(4) << "] count=" << c << " obj=" << (INT_PTR)this);
		}
		return *this;
	}

	void release()
	{
		if (_count==0 )
			return ;
		
		if ( g_share_ptr_debug==_ptr &&_ptr)
			ErrorLn("share ptr release count=" << (*_count-1) << " obj=" << (INT_PTR)this);

		if ( ::InterlockedDecrement(_count) == 0 )
		{
			if ( _count ) { delete _count; _count = 0; }
			if ( _ptr ) { delete _ptr; _ptr = 0; }
		}
	}
	////////////////////////////* 调试版本 *////////////////////////////////////////////////
#endif//SHARE_PTR_DEBUG

	T * operator ->() const { return _ptr; }

	operator T*() const { return _ptr; }

	T & operator *() const{ return *_ptr; }

	bool operator==(const share_ptr & val )
	{
		return _ptr==val._ptr;
	}

	bool operator==(const T * val )
	{
		return _ptr==val;
	}

	bool operator!=(const share_ptr & val )
	{
		return _ptr!=val._ptr;
	}

	bool operator!=(const T * val )
	{
		return _ptr!=val;
	}

	virtual ~share_ptr()
	{
		release();
	}

private:
	ref_count * _count;
	T	*		_ptr;

public:

	// 智能指针序列化辅助类
	// 
	struct T_BAG
	{
		typedef share_ptr<T, is_interface> PTR;

		ref_count * _count;
		T	*		_ptr;

		T_BAG(){}

		T_BAG(const PTR & ptr )
		{
			_count = ptr._count;
			_ptr   = ptr._ptr;

			if ( _count ) InterlockedIncrement(_count);
		}

		PTR get()
		{
			PTR ptr;
			ptr._count = _count;
			ptr._ptr   = _ptr;

			return ptr;
		}
	};

};

#endif// __share_ptr_h__
