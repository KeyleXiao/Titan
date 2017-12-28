#ifndef __INTERFACE_REDIS_CONNECTOR_H__
#define __INTERFACE_REDIS_CONNECTOR_H__

#pragma pack(1)
#pragma warning(disable:4200)

#include <string>
#include <type_traits>
#include "buffer.h"
#include "ISerializableData.h"
#include "IListOp.h"
#include "IValueOp.h"
#include "ISetOp.h"
#include "IHashOp.h"
#include "IKeysOp.h"
#include "IPipeline.h"
#include "IPipelineValueOp.h"
#include "IPipelineListOp.h"
#include "IServerOp.h"

using std::string;

namespace Redis
{
	/* Redis连接器
	*/
	struct IRedisConnector 
	{
		// 创建
		virtual bool		create(const char* ip = "127.0.0.1", WORD port = 6379) = 0;
		// 释放
		virtual void		release() = 0;
		// 关闭
		virtual void		stop() = 0;

		//virtual IPipeline*		pipeline() = 0;

		// 获取Value操作器
		virtual IValueOp*			valueOp() = 0;
		// 获取List操作器
		virtual IListOp*		listOp() = 0;
		//// 获取Set操作器
		//virtual ISetOp*			setOp() = 0;
		//// 获取Hash操作器
		//virtual IHashOp*		hashOp() = 0;
		// 获取其他指令操作器
		virtual IKeysOp*			keysOp() = 0;
		// Server指令操作器
		virtual	IServerOp*			serverOp() = 0;

		//************************************
		//Example:	
		//pRedisConnector->pipeline([&](Redis::IPipeline* pPipe)
		//{
		//	pPipe->valueOp()->setString("key1", "Value1");
		//	pPipe->valueOp()->set("key2", 3.1415926);
		//	...
		//});
		//
		// Returns:   bool	是否执行成功
		// Qualifier: Pipeline中的语句全部执行完之后，才能一起拿到结果。
		// Parameter: Func func	Lambda表达式
		//************************************
		template<typename Func>
		bool	pipeline(Func func)
		{
			IPipeline* pPipe = pipeBegin();
			if (pPipe==nullptr)
				return false;

			func(pPipe);

			if (!pPipe->pipeEnd())
				return false;

			return pipeEnd(pPipe);
		}
	protected:
		// 在执行Pipline操作之前的准备工作
		virtual IPipeline*	pipeBegin() = 0;
		// 在执行Pipline操作之后的工作
		virtual bool		pipeEnd(IPipeline* pPipe) = 0;

		///* interfaces for sorted set */
		//// 添加
		//virtual bool	addZset(const string& strKey, double dScore, const string& strElem) = 0;
		//// 添加（无Elem添加，有Elem加和）
		//virtual bool	incrbyZset(const string& strKey, double dIncr, const string& strElem) = 0;
		//// 删除（单个）
		//virtual bool	remZset(const string& strKey, const string& strElem) = 0;
		//// 删除（多个）
		//virtual bool	remZset(const string& strKey, const vector<string>& vecElem) = 0;
		//// 删除（元素区间）
		//virtual bool	remRangeByLexZset(const string& strKey, const string& strMin, const string& strMax) = 0;
		//// 删除（名次区间）
		//virtual bool	remRangeByRankZset(const string& strKey, long nStart, long nStop) = 0;
		//// 删除（分数区间）
		//virtual bool	remRangeByScoreZset(const string& strKey, double dMin, double dMax) = 0;
		//// 获取（名次区间，升序，vec）
		//virtual bool	rangeZset(const string& strKey, long nStart, long nStop, vector<string>* pvecValue) = 0;
		//// 获取（名次区间，升序，map）
		//virtual bool	rangeZset(const string& strKey, long nStart, long nStop, map<string, string>* pmapValue) = 0;
		//// 获取（元素区间，升序，vec）
		//virtual bool	rangeByLexZset(const string& strKey, const string& strMin, const string& strMax, vector<string>* pvecValue) = 0;
		//// 获取（分数区间，升序，vec）
		//virtual bool	rangeByScoreZset(const string& strKey, double dMin, double dMax, vector<string>* pvecValue) = 0;
		//// 获取（分数区间，升序，map）
		//virtual bool	rangeByScoreZset(const string& strKey, double dMin, double dMax, map<string, string>* pmapValue) = 0;
		//// 获取（名次区间，降序，vec）
		//virtual bool	revRangeZset(const string& strKey, long nStart, long nStop, vector<string>* pvecValue) = 0;
		//// 获取（分数区间，降序，vec）
		//virtual bool	revRangeByScoreZset(const string& strKey, double dMax, double dMin, vector<string>* pvecValue) = 0;
		//// 获取（分数区间，降序，map）
		//virtual bool	revRangeByScoreZset(const string& strKey, double dMax, double dMin, map<string, string>* pmapValue) = 0;
		//// 数量（全部）
		//virtual bool	cardZset(const string& strKey, long* pnValue) = 0;
		//// 数量（分数区间）
		//virtual bool	countZset(const string& strKey, double dMin, double dMax, long* pnValue) = 0;
		//// 数量（元素区间）
		//virtual bool	countByLexZset(const string& strKey, const string& strMin, const string& strMax, long* pnValue) = 0;
		//// 名次（升序）
		//virtual bool	rankZset(const string& strKey, const string& strElem, long* pnValue) = 0;
		//// 名次（降序）
		//virtual bool	revRankZset(const string& strKey, const string& strElem, long* pnValue) = 0;
		//// 分数
		//virtual bool	scoreZset(const string& strKey, const string& strElem, double* pdValue) = 0;
	};


};

/// 静态库版本
#if defined(_LIB) || defined(CENTER_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(GuardConnector))
extern "C" Redis::IRedisConnector * CreateRedisConnector();
#	define	CreateRedisConnectorProc	CreateRedisConnector
#else 
/// 动态库版本
typedef Redis::IRedisConnector* (RKT_CDECL*procCreateRedisConnector)();
#	define	CreateRedisConnectorProc	DllApi<procCreateRedisConnector>::load(MAKE_DLL_NAME(GuardConnector), "CreateRedisConnector")
typedef int (RKT_CDECL*procInt)();
#	define	RunTest		DllApi<procInt>::load(MAKE_DLL_NAME(GuardConnector), "test")
#endif


#pragma pack()

#endif	//__INTERFACE_REDIS_CONNECTOR_H__