/*******************************************************************
** 文件名:	Coroutine.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/12/2014
** 版  本:	1.0
** 描  述:	协程类

			
********************************************************************/

#pragma once

/**
	协程（Coroutine）是一种模型：
	本质就是一段代码在独立的运行堆栈上运行，如果任务无法立即完成，则挂起，下次接着上次运行的堆栈继续往下运行
	1.结合多线程的模型，可以即获得多线程的并发效率，又能兼顾到单线程的习惯逻辑
	2.即每一个服务自己维护一个Coroutine,阻塞了就通知工作线程切换到下一个Coroutine执行，这样就有点类似Erlang的用户进程
	  切换起来性能非常高
	3.C++实现Coroutine的方式：参考(http://blog.codingnow.com/2005/10/fiber.html)
	       1).Windows: Fiber
		   2).Linux: setcontext (https://github.com/cloudwu/coroutine/)
	4.其他编程语言,Lua,C#等基本都有Coroutine(http://blog.codingnow.com/2012/06/continuation_in_lua_52.html)
	5.最开始的思路是用C++写服务，用Lua_state保存运行环境，相当于用lua虚拟机实现协程效果了
*/

typedef void (WINAPI * coroutine_func)(void *ud);

struct ICoTask
{
    virtual void work_routine() = 0;
};

enum 
{
    CO_STATUS_EMPTY   = 0,
    CO_STATUS_CREATED = 1,
    CO_STATUS_RUNNING = 2,
    CO_STATUS_YIELD   = 3,
};

struct ICoroutine
{
public:
	// 创建
	virtual bool create(coroutine_func entry,INT_PTR param,int stack_size) = 0;

    virtual bool create(int stack_size) =0;

    virtual void set_task(ICoTask* pTask) = 0;

    virtual bool is_idle() = 0;

	// 继续执行
	virtual bool resume() = 0;

	// 当前状态
	virtual int status() = 0;

	// 挂起当前协程
	virtual void yield() = 0;

	// 获取创建参数
	virtual INT_PTR get_param() = 0;

	// 当前正在运行的纤程(注意，这个取到的是纤程的参数，不是纤程本身）
	virtual void * running() = 0;

    virtual int get_stack_size() = 0;

	// 销毁
	virtual void release() = 0;
};
