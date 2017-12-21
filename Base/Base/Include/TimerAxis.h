/*******************************************************************
** 文件名:	TimerAxis.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	01/21/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef  __TIMER_AXIS_H__
#define  __TIMER_AXIS_H__

#include "Common.h"

#ifdef SUPPORT_TIMER

#include <string>
#include <list>
#include <vector>
#include <map>
#include "TimerHandler.h"
#include "Singleton.h"
#include "StringHash.h"
#include "Api.h"
using namespace stdext;

// 定义数据类型
#ifdef WIN32
#	include <windows.h>
#else
	typedef unsigned long DWORD;
#endif

#ifdef RKT_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

namespace rkt {

	/*
	*  问题:
	*  1.OnCheck调用频率大于单位时间刻度时怎么处理?
	*  2.多线程支持
	*  3.长Timer怎么处理?
	*  4.需要支持调试功能
	*
	*  分析:
	*  1.时间轴的核心实际是一个高效的排序算法
	*  2.此算法对插入删除操作要求极高
	*  3.主要是插入排序,不是对一堆数字整体排序
	*  4.插入排序实际最主要的要求是如何尽快找到一个值大致所在的位置
	*  
	*  设计方案:
	*  1.把时间轴分成N个刻度,每个刻度内保存此段时间内需要调用的Timer
	*  2.长Timer折算成N个短Timer回调
	*  3.因为在单位时间刻度内只能也只需调用几次,所以同一刻度内的Timer不需排序
	*  4.完整时间轴由N个不同长度段的子时间轴组合而成,这样定时频率在1秒以下
	*    需要频繁调用的定时器可以放在一个每格长度1ms长度等于1秒极度细化的时间轴中
	*    以达到最高效率
	*  5.避免填加删除Timer时的查找操作可以大大提高效率.
	*/

	//////////////////////////////////////////////////////////////////////////
	/************************* TimerAxis Config *****************************/

	// 是否允许进行定时器统计
//#	define SUPPORT_TIMEAXIS_STAT

#define SUPPORT_TIMEAXIS_DEBUG_INFO	// 是否支持调试信息

	struct SAnalyseTimer
	{
	public:
		float fAvgTime;
		float fMaxTime;
		float fCount;
	};
	typedef hash_map<std::string, SAnalyseTimer>	TMAP_ANALYSETYPE;
		
	struct SStatTimer
	{
		int				nCount;
		std::string     debugInfo;

		SStatTimer(void){
			nCount = 0; debugInfo.clear();
		}
	};
	typedef StrHashMap<SStatTimer>	TMAP_STATTYPE;

	//////////////////////////////////////////////////////////////////////////
#	define INVALID_TIMER	        0xffffffff
#	define INFINITY_CALL            0xffffffff

	class RKT_EXPORT TimerAxis //: public SingletonEx<TimerAxis>
	{
		int CHECK_FREQUENCY	  ;	//精确到16ms推荐检查频率	:	16(ms) 
		int TIME_GRID		  ; //推荐时间刻度 :       64(ms)
		int TIME_AXIS_LENGTH  ;  //时间轴长度
	public:
		/**
		@purpose          : 设置一个定时器
		@param	 timerID  : 定时器ID
		@param   interval : 定时器调用间隔
		@param   handler  : 处理接口
		@param   callTimes: 调用次数,默认调用无穷次
		@param   debugInfo: 调试信息
		@return		      : 如果设置成功则返回true
		*/
		bool SetTimer( DWORD timerID,DWORD interval,ITimerHandler * handler,DWORD callTimes=INFINITY_CALL,const char * debugInfo=0 );

		/**
		@purpose          : 删除定时器
		@param	 timerID  : 定时器ID
		@param   handler  : 处理接口
		@return		      : 返回是否删除成功
		*/
		bool KillTimer( DWORD timerID,ITimerHandler * handler );

		/**  删除定时器对象内部的所有时钟 added by PeakGao 2011.5.3
		@param handler 定时器处理者	*/
		void KillTimer(ITimerHandler* handler);

		void CheckTimer(ulong timeout = 0);

		TMAP_STATTYPE * GetStateInfo(void);

		DWORD  GetTimerCount(){ return m_dwTimerCount; }

		void OutputAnalyseInfo();

		TimerAxis( int check_freq=2,int timer_grid=64,int axis_len=180000);

		virtual ~TimerAxis();

	protected:
		/// 取得当前时间,你可以修改成使用其他API
		inline DWORD GetTickCount() { return /*::GetTickCount()*/ getTickCountEx();}

	protected:
		struct Timer
		{
			DWORD           dwTimerID;
			DWORD           dwInterval;      // 
			DWORD           dwCallTimes;     // 总共需要回调多少次
			DWORD           dwLastCallTick;  // 最后一次调用的时间
			DWORD           dwGridIndex;     // 所在的时间刻度
			ITimerHandler * handler;
#ifdef  SUPPORT_TIMEAXIS_DEBUG_INFO
			std::string     debugInfo;
#endif
			std::list<Timer*>::iterator pos; // 在时间刻度中的iterator,加快搜索
		};

		typedef std::list<Timer>          TIMER_INFO;   // 存在ITimerHandler中的定时器临时信息
		typedef std::list<Timer *>        TIMER_LIST;   // 每一个时间刻度中存放的定时器列表
		typedef std::vector<TIMER_LIST>   TIMER_AXIS;	// 保存所有时间刻度信息的时间轴结构

		TIMER_AXIS         m_TimerAxis;
		DWORD              m_dwLastCheckTick;           // 最后一次检查的时间
		DWORD              m_dwCurRunTime;              // 当前运行的时间
		DWORD              m_dwInitializeTime;          // 时间轴初始时间
		DWORD              m_dwTimerCount;              // 定时器个数

#ifdef  SUPPORT_TIMEAXIS_STAT
		TMAP_STATTYPE	   m_mapStatType;				// 按定时器来统计数量
#endif
		TMAP_ANALYSETYPE   m_mapAnalyseType;            //定时器调用 耗时分析统计表

	};

} // namespace


#ifdef RKT_COMPILER_MSVC
#pragma warning(pop)
#endif

#endif // #ifdef SUPPORT_TIMER

#endif// __TIMER_AXIS_H__