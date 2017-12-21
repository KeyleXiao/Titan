/*******************************************************************
** 文件名:	TimerHandler.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/22/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __TIMER_HANDLER_H__
#define __TIMER_HANDLER_H__

#include "Common.h"

#ifdef SUPPORT_TIMER

#include "Trace.h"

namespace rkt {

	//////////////////////////////////////////////////////////////////////////
	/*********************** 定时器处理接口 *********************************/
	// 使用方法:
	//           class A : public TimerHandler
	//           {
	//                  virtual void OnTimer( unsigned long dwTimerID )
	//                  { do something .... }
	//           }
	//
	//          A a;
	//          TimeAxis.SetTimer(1000,&a);
	//          TimeAxis.KillTimer(&a);
	//////////////////////////////////////////////////////////////////////////

//#if 0
	// ---------------------------------------------------------------------------
	// Name	: 定时器触发后的处理接口
	// Desc : （应用层使用时尽量从TimerHandler继承）
	// ---------------------------------------------------------------------------
	struct ITimerHandler
	{
		/**
		@purpose          : 定时器触发后回调,你可以在这里编写处理代码
		@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
		@return		      : empty
		*/
		virtual void OnTimer( unsigned long dwTimerID ) = 0;

		/**
		@purpose          : 取得存放定时器内部数据的指针
		@note             : 这个函数和应用层无关,参照TimerHandler基类的实现即可
		@note             : 时间轴具体实现中需要提高定时器的插入删除效率，所以让应用层记录一个指针
		@return		      : 返回指向临时信息的指针,该指针必须初始化成0
		*/
		virtual void ** GetTimerInfoPtr() = 0;
	};

	// 魔数校验，避免传入错误的指针
#	define  MAGIC_VALUE_TIMER_HANDLER    (unsigned long)0x20161102

	// ---------------------------------------------------------------------------
	// Name	: 定时器处理类基类
	// Desc : 主要是封装一下GetTimerInfoPtr的实现
	// ---------------------------------------------------------------------------
	class RKT_EXPORT TimerHandler : public ITimerHandler
	{
	public:
		virtual void ** GetTimerInfoPtr() 
		{
			if(_magic_value!=MAGIC_VALUE_TIMER_HANDLER) 
			{
				ErrorLn("Invalude TimerHandler call GetTimerInfoPtr!!!!");
				__debugbreak();
				return 0;
			}

			return &_InfoPtr; 
		}  

		TimerHandler() : _InfoPtr(0),_magic_value(MAGIC_VALUE_TIMER_HANDLER)
		{
		}

		virtual ~TimerHandler()
		{
			if( _InfoPtr!=0 )
			{
				__debugbreak();	
			}
		}

	protected:
		void * _InfoPtr; // 指向临时数据的指针
		unsigned long _magic_value; // 魔数校验，避免传入错误的指针
	};

} // namespace

#endif // #ifdef SUPPORT_TIMER

#endif//__TIMER_HANDLER_H__