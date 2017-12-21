#pragma once

#include <list>
#include <string>
#include <map>
#include "api.h"
#include "Simple_Atom_Lock.h"
using namespace std;
using namespace stdext;
using namespace rkt;

// 发送最大层数
#define FIRE_MAX_LAYERNUM			20

// 引用最大层数
#define REF_MAX_LAYERNUM			5

// 是否支持性能分析
#define  SUPPORT_PROFILE

// 是否支持多线程
//#define  TEVENT_MULTITHREAD

// 事件引擎模板
/**
	备注：事件分实体事件和全局事件，实体事件通过IEntityEvent.h调用，全局事件通过IEeventEngine调用
	实体事件是不跨线程的，全局事件是跨线程的
	全局事件的源ID和类型，指发出事件的对象标识，可以是一个实体的UID，也可以是一个场景的场景ID
	如果传入特定的源ID或者类型ID，则只关心这类对象的事件，比如只关心怪物的事件，或者只关心人物的事件
*/
template< typename TSink,                 // 订阅接口类型
          typename TSinkCaller,           
		  typename TEventKey=int,         // 事件ID类型
		  TEventKey START_KEY=0,          // 起始ID
		  int EVENT_COUNT=256             // 事件个数
>
class TEventEngine
{	
private:	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	struct SSubscibeInfo
	{
		TSink *		pSink;
#ifdef SUPPORT_PROFILE
		char		szDesc[64];	// 发布版不用太大,超占内内存
#else
		//char		szDesc[4];
#endif
		BYTE		nCallCount; // 避免事件嵌套时逻辑出现问题
		bool		bRemoveFlag;

		SSubscibeInfo(TSink * pPrameSink, LPCSTR pDesc)
		{
			pSink = pPrameSink;		
			nCallCount = 0;
			bRemoveFlag = false;
#ifdef SUPPORT_PROFILE
			if(pDesc != NULL)
			{
				sstrcpyn(szDesc, pDesc, sizeof(szDesc));
			}
			else
			{
				szDesc[0] = '\0';
			}
#endif
		}

		void Add(void)
		{
			if ( nCallCount<255 )
				nCallCount++;
		}

		void Sub(void)
		{
			if ( nCallCount>0 )
				nCallCount--;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////
	// 事件订阅者列表
	typedef std::list< SSubscibeInfo >							TLIST_SUBSCIBEINFO;

	// 所有事件的事件订阅槽
	TLIST_SUBSCIBEINFO			m_EventsSlot[EVENT_COUNT];       

#	ifdef TEVENT_MULTITHREAD
	// 每个槽位的线程安全锁
	volatile	LONG            m_SlotMutexs[EVENT_COUNT]; 
#	endif

	// 回调对象
	TSinkCaller  	*			m_pSinkCaller;

	// 订阅的事件响应时又可能触发事件，如果出现回绕就会变成死循环
	int							m_nFireLayerNum;	


	TEventEngine()
	{
		m_nFireLayerNum = 0;
		m_pSinkCaller = 0;
	}

public:

	TEventEngine( TSinkCaller * pCaller )
	{
		m_pSinkCaller = pCaller;
		m_nFireLayerNum = 0;
#		ifdef TEVENT_MULTITHREAD
		for ( int i=0;i<EVENT_COUNT;++i )
		{
			m_SlotMutexs[i]=0;
		}
#		endif
	}

	virtual ~TEventEngine()
	{
		for ( int i=0;i<EVENT_COUNT;++i )
		{
			m_EventsSlot[i].clear();
		}
	}

	inline int Key2Index( TEventKey eventkey )
	{
		int index = (int)(eventkey - START_KEY);
		if ( index<0 || index>=EVENT_COUNT )
		{
			ErrorLn("invalid event key=" << eventkey);
			__debugbreak;
			return -1;
		}

		return index;
	}

	inline TEventKey Index2Key( int index )
	{
		return START_KEY + index;
	}
	/** 
	@param   
	@param   
	@return  
	*/
	bool Subscibe(TSink * pSink, TEventKey eventkey, LPCSTR pszDesc)	
	{
		if(pSink == NULL)
		{
			return false;
		}

		int index = Key2Index( eventkey );
		if ( index<0 ) return false;

		// 订阅者信息
		SSubscibeInfo subscibeinfo(pSink, pszDesc);

		// 加局部锁	
#		ifdef TEVENT_MULTITHREAD
		Simple_Atom_Lock lock(&m_SlotMutexs[index]);
#		endif

		// 不能重复订阅
		TLIST_SUBSCIBEINFO * plistSubscibeInfo = &m_EventsSlot[index];
		TLIST_SUBSCIBEINFO::iterator itList = plistSubscibeInfo->begin();
		for( ; itList != plistSubscibeInfo->end(); ++itList)
		{
			SSubscibeInfo * pSubscibeInfo = &(*itList);
			if(pSubscibeInfo->pSink == pSink && !pSubscibeInfo->bRemoveFlag)
			{
				if(pszDesc == NULL)
				{
					WarningLn("Repeat Subscribe, EventID="<<eventkey);
				}
				else
				{
					WarningLn("Repeat Subscribe, EventID="<<eventkey<<" Desc="<<pszDesc);
				}

				return true;
			}
		}		

		plistSubscibeInfo->push_front(subscibeinfo);

		return true;		
	}

	/** 
	@param   
	@param   
	@return  
	*/
	bool UnSubscibe(TSink * pSink,TEventKey eventkey)
	{
		if(pSink == NULL)
		{
			return false;
		}	

		int index = Key2Index( eventkey );
		if ( index<0 ) return false;

		// 加局部锁	
#		ifdef TEVENT_MULTITHREAD
		Simple_Atom_Lock lock(&m_SlotMutexs[index]);
#		endif

		TLIST_SUBSCIBEINFO * plistSubscibeInfo = &m_EventsSlot[index];

		TLIST_SUBSCIBEINFO::iterator itList = plistSubscibeInfo->begin();
		for( ; itList != plistSubscibeInfo->end(); ++itList)
		{
			SSubscibeInfo * pSubscibeInfo = &(*itList);
			if(pSubscibeInfo->pSink == pSink)
			{
				if(pSubscibeInfo->nCallCount == 0)			// 正在调用不要删除
				{
					plistSubscibeInfo->erase(itList);
				}
				else
				{
					pSubscibeInfo->bRemoveFlag = true;
				}

				break;
			}
		}				

		return true;	
	}

	/** 输出事件订阅的统计信息
	@param   
	@param   
	@return  
	*/
	void OutputSubscibeEventStatInfo(bool bExecuteCenter)
	{
		for ( int i=0;i<EVENT_COUNT;++i )
		{
			// 加局部锁	
#			ifdef TEVENT_MULTITHREAD
			Simple_Atom_Lock lock(&m_SlotMutexs[i]);
#			endif

			TraceLn("EventID:"<<Index2Key(i)<<", Size:"<<m_EventsSlot[i].size());
		}	
	}


	bool Fire(TEventKey eventkey, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
	{
		int index = Key2Index( eventkey );
		if ( index<0 ) return false;

		m_nFireLayerNum++;

		if(m_nFireLayerNum >= FIRE_MAX_LAYERNUM)
		{
			crash_ErrorLn("Event engine error, endless loop, EventID:"<<eventkey);
			debugBreak;		
			m_nFireLayerNum--;
			return false;
		}

		// 加局部锁	
#		ifdef TEVENT_MULTITHREAD
		Simple_Atom_Lock lock(&m_SlotMutexs[index]);
#		endif

		TLIST_SUBSCIBEINFO * plistSubscibeInfo = &m_EventsSlot[index];
			
		typename TLIST_SUBSCIBEINFO::iterator itList = plistSubscibeInfo->begin();
		for( ; itList != plistSubscibeInfo->end(); )
		{
			SSubscibeInfo * pSubscibeInfo = &(*itList);

			if(pSubscibeInfo->nCallCount >= REF_MAX_LAYERNUM)
			{
				crash_ErrorLn("Fatal error, a event is endless loop, EventID:"<<eventkey/*<<" Desc:"<<pSubscibeInfo->szDesc*/);
				m_nFireLayerNum--;
				return false;
			}

			if(!pSubscibeInfo->bRemoveFlag)
			{					
				bool bResult = false;

				int dwFireEventTick = getTickCount();

				pSubscibeInfo->Add();						
				bResult = (*m_pSinkCaller)(pSubscibeInfo->pSink, eventkey, bSrcType, dwSrcID, pszContext, nLen);
				pSubscibeInfo->Sub();

				// 这样计算耗时可能不准，因为事件回调里有可能继续发事件，就会清掉dwFireEventTick
				int nCost = getTickCount() - dwFireEventTick;
				if(nCost >= 30)
				{							
					WarningLn("Low performance event call, Cost:"<<nCost<<" EventID:"<<eventkey/*<<" Desc:"<<pSubscibeInfo->szDesc*/);
				}

				if(pSubscibeInfo->bRemoveFlag && pSubscibeInfo->nCallCount == 0)
				{
					itList = plistSubscibeInfo->erase(itList);

					if(plistSubscibeInfo->empty())
					{
						m_nFireLayerNum--;
						return bResult;
					}
				}					
				else
				{
					++itList;
				}

				if(!bResult) 
				{
					m_nFireLayerNum--;
					return false;
				}
			}
			else
			{
				if(pSubscibeInfo->nCallCount == 0)				
				{
					itList = plistSubscibeInfo->erase(itList);

					if(plistSubscibeInfo->empty())
					{
						break;
					}
				}
				else
				{
					++itList;
				}
			}
	}

	m_nFireLayerNum--;

	return true;
	};
};
#undef SUPPORT_PROFILE
