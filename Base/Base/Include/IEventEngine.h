#pragma once

namespace rkt {
	/*! \addtogroup EventEngine
	*  事件引擎
	*  @{
	*/

	/// 投票事件sink
	/// Desc : 投票事件其实是一个事件过滤器(用于在事件发生前对事件行为进行否决或者修改事件现场)
	//         比如实体需要执行移动命令前，各部件会进行一个投票，看实体当前能不能移动（比如被捆住）
	struct IEventVoteSink 
	{
		/** 
		@param   wEventID ：事件ID
		@param   bSrcType ：发送源类型
		@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
		@param   pszContext : 上下文
		@param   nLen : 上下文长度
		@return  如果返回false : 则中断执行，否则继续向下执行
		@note     
		@warning 
		@retval buffer 
		*/
		virtual bool		OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = 0;	
	};

	/// 执行事件sink
	/// Desc : 动作真正执行之后的事件
	struct IEventExecuteSink 
	{
		IEventExecuteSink() : _InfoPtr(0) { }
		virtual ~IEventExecuteSink() { }

		/** 
		@param   wEventID ：事件ID
		@param   bSrcType ：发送源类型
		@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
		@param   pszContext : 上下文
		@param   nLen : 上下文长度
		@return  
		@note     
		@warning 
		@retval buffer 
		*/
		virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = 0;

		// 这个应用层不用实现，事件引擎内部的优化逻辑
		virtual void **		__GetProxyInfoPtr() { return &_InfoPtr; }  

	private:
		void * _InfoPtr; // 指向临时数据的指针
	};

	/// 事件引擎
	struct IEventEngine
	{
		/** 释放
		@param   
		@param   
		@param   
		@return  
		@note     
		@warning 
		@retval buffer 
		*/
		virtual void		Release(void) = 0;

		/** 发送投票事件   
		@param   wEventID ：事件ID
		@param   bSrcType ：发送源类型
		@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
		@param   pszContext : 上下文
		@param   nLen : 上下文长度	  
		@return  
		@note     
		@warning 
		@retval buffer 
		*/
		virtual bool		FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = 0;

		/**
			备注：事件分实体事件和全局事件，实体事件通过IEntityEvent.h调用，全局事件通过IEeventEngine调用
			实体事件是不跨线程的，全局事件是跨线程的
			全局事件的源ID和类型，指发出事件的对象标识，可以是一个实体的UID，也可以是一个场景的场景ID
			如果传入特定的源ID或者类型ID，则只关心这类对象的事件，比如只关心怪物的事件，或者只关心人物的事件
		*/
		virtual void		FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen) = 0;

		/**
			备注：事件分实体事件和全局事件，实体事件通过IEntityEvent.h调用，全局事件通过IEeventEngine调用
			实体事件是不跨线程的，全局事件是跨线程的
			全局事件的源ID和类型，指发出事件的对象标识，可以是一个实体的UID，也可以是一个场景的场景ID
			如果传入特定的源ID或者类型ID，则只关心这类对象的事件，比如只关心怪物的事件，或者只关心人物的事件
		*/
		virtual bool		Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc) = 0;

		/**
			备注：事件分实体事件和全局事件，实体事件通过IEntityEvent.h调用，全局事件通过IEeventEngine调用
			全局事件的源ID和类型，指发出事件的对象标识，可以是一个实体的UID，也可以是一个场景的场景ID
			如果传入特定的源ID或者类型ID，则只关心这类对象的事件，比如只关心怪物的事件，或者只关心人物的事件
		*/
		virtual bool		Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc) = 0;

		/**
			备注：事件分实体事件和全局事件，实体事件通过IEntityEvent.h调用，全局事件通过IEeventEngine调用
			实体事件是不跨线程的，全局事件是跨线程的
			全局事件的源ID和类型，指发出事件的对象标识，可以是一个实体的UID，也可以是一个场景的场景ID
			如果传入特定的源ID或者类型ID，则只关心这类对象的事件，比如只关心怪物的事件，或者只关心人物的事件
		*/
		virtual bool		UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID) = 0;

		/**
			备注：事件分实体事件和全局事件，实体事件通过IEntityEvent.h调用，全局事件通过IEeventEngine调用
			实体事件是不跨线程的，全局事件是跨线程的
			全局事件的源ID和类型，指发出事件的对象标识，可以是一个实体的UID，也可以是一个场景的场景ID
			如果传入特定的源ID或者类型ID，则只关心这类对象的事件，比如只关心怪物的事件，或者只关心人物的事件
		*/
		virtual bool		UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID) = 0;

		/** 输出事件订阅的统计信息
		@param   
		@param   
		@return  
		*/
		virtual void		OutputSubscibeEventStatInfo(void) = 0;

		/** 输出耗时统计信息 
		@param   
		@param   
		@return  
		*/
		virtual void		OutputAnalyseInfo(void) = 0;
	};

	RKT_API IEventEngine * CreateEventEngine(void);

	#define	CreateEventEngineProc	CreateEventEngine


}//namespace rkt*/
