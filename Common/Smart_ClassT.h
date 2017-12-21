/*******************************************************************
** 文件名:	Smart_classT.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	4/23/2015
** 版  本:	1.0
** 描  述:	

            定义一组模板类，用于完成一些比手工调用更智能的操作，比如:
            1. 静态成员函数的自动调用
            2. 指针的自动析构，这点类似smart_ptr
            3. 文件句柄等资源的自动释放
********************************************************************/

#pragma once

// 用于类的静态初始化工作(在程序启动时即初始化, 类 只要实现init函数即可)
template < typename T >
struct static_init
{
    static_init()
    {
        T::init();
    }
};

#define STATIC_INITIALIZE( T ) static static_init<T> _static##__FILE__##__LINE__
/* */


// 自动释放器
#define AUTO_DESTRUCTOR( opname )               \
    template < typename T >                     \
    class auto_##opname                         \
    {                                           \
        T * _ptr;                               \
    public:                                     \
        explicit auto_##opname( T * p )         \
            : _ptr( p )                         \
        {                                       \
        }                                       \
        ~auto_##opname()                        \
        {                                       \
            if (_ptr) _ptr->opname();           \
        }                                       \
                                                \
        void detach() { _ptr = 0; }             \
    }
/* */

// 析构时自动close
AUTO_DESTRUCTOR(close);

// 析构时自动release
AUTO_DESTRUCTOR(release);

// 析构时自动Release
AUTO_DESTRUCTOR(Release);

/* 自动析构器，这个稍微复杂一点，需要一个模板参数用于区分是数组指针还是对象指针 */
template < typename T,bool ARRAY=false >
struct auto_delete
{
    T * _ptr;

    explicit auto_delete( T * p ) : _ptr( p ) 
    {
    }

    ~auto_delete()
    {
        if ( _ptr )
        {
            if ( ARRAY )
                delete [] _ptr;
            else
                delete _ptr;
        }
    }

    void detach()
    {
        _ptr = 0;
    }
};
