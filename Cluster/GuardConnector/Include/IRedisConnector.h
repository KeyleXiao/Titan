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
	/* Redis������
	*/
	struct IRedisConnector 
	{
		// ����
		virtual bool		create(const char* ip = "127.0.0.1", WORD port = 6379) = 0;
		// �ͷ�
		virtual void		release() = 0;
		// �ر�
		virtual void		stop() = 0;

		//virtual IPipeline*		pipeline() = 0;

		// ��ȡValue������
		virtual IValueOp*			valueOp() = 0;
		// ��ȡList������
		virtual IListOp*		listOp() = 0;
		//// ��ȡSet������
		//virtual ISetOp*			setOp() = 0;
		//// ��ȡHash������
		//virtual IHashOp*		hashOp() = 0;
		// ��ȡ����ָ�������
		virtual IKeysOp*			keysOp() = 0;
		// Serverָ�������
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
		// Returns:   bool	�Ƿ�ִ�гɹ�
		// Qualifier: Pipeline�е����ȫ��ִ����֮�󣬲���һ���õ������
		// Parameter: Func func	Lambda���ʽ
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
		// ��ִ��Pipline����֮ǰ��׼������
		virtual IPipeline*	pipeBegin() = 0;
		// ��ִ��Pipline����֮��Ĺ���
		virtual bool		pipeEnd(IPipeline* pPipe) = 0;

		///* interfaces for sorted set */
		//// ���
		//virtual bool	addZset(const string& strKey, double dScore, const string& strElem) = 0;
		//// ��ӣ���Elem��ӣ���Elem�Ӻͣ�
		//virtual bool	incrbyZset(const string& strKey, double dIncr, const string& strElem) = 0;
		//// ɾ����������
		//virtual bool	remZset(const string& strKey, const string& strElem) = 0;
		//// ɾ���������
		//virtual bool	remZset(const string& strKey, const vector<string>& vecElem) = 0;
		//// ɾ����Ԫ�����䣩
		//virtual bool	remRangeByLexZset(const string& strKey, const string& strMin, const string& strMax) = 0;
		//// ɾ�����������䣩
		//virtual bool	remRangeByRankZset(const string& strKey, long nStart, long nStop) = 0;
		//// ɾ�����������䣩
		//virtual bool	remRangeByScoreZset(const string& strKey, double dMin, double dMax) = 0;
		//// ��ȡ���������䣬����vec��
		//virtual bool	rangeZset(const string& strKey, long nStart, long nStop, vector<string>* pvecValue) = 0;
		//// ��ȡ���������䣬����map��
		//virtual bool	rangeZset(const string& strKey, long nStart, long nStop, map<string, string>* pmapValue) = 0;
		//// ��ȡ��Ԫ�����䣬����vec��
		//virtual bool	rangeByLexZset(const string& strKey, const string& strMin, const string& strMax, vector<string>* pvecValue) = 0;
		//// ��ȡ���������䣬����vec��
		//virtual bool	rangeByScoreZset(const string& strKey, double dMin, double dMax, vector<string>* pvecValue) = 0;
		//// ��ȡ���������䣬����map��
		//virtual bool	rangeByScoreZset(const string& strKey, double dMin, double dMax, map<string, string>* pmapValue) = 0;
		//// ��ȡ���������䣬����vec��
		//virtual bool	revRangeZset(const string& strKey, long nStart, long nStop, vector<string>* pvecValue) = 0;
		//// ��ȡ���������䣬����vec��
		//virtual bool	revRangeByScoreZset(const string& strKey, double dMax, double dMin, vector<string>* pvecValue) = 0;
		//// ��ȡ���������䣬����map��
		//virtual bool	revRangeByScoreZset(const string& strKey, double dMax, double dMin, map<string, string>* pmapValue) = 0;
		//// ������ȫ����
		//virtual bool	cardZset(const string& strKey, long* pnValue) = 0;
		//// �������������䣩
		//virtual bool	countZset(const string& strKey, double dMin, double dMax, long* pnValue) = 0;
		//// ������Ԫ�����䣩
		//virtual bool	countByLexZset(const string& strKey, const string& strMin, const string& strMax, long* pnValue) = 0;
		//// ���Σ�����
		//virtual bool	rankZset(const string& strKey, const string& strElem, long* pnValue) = 0;
		//// ���Σ�����
		//virtual bool	revRankZset(const string& strKey, const string& strElem, long* pnValue) = 0;
		//// ����
		//virtual bool	scoreZset(const string& strKey, const string& strElem, double* pdValue) = 0;
	};


};

/// ��̬��汾
#if defined(_LIB) || defined(CENTER_STATIC_LIB)
#	pragma comment(lib, MAKE_LIB_NAME(GuardConnector))
extern "C" Redis::IRedisConnector * CreateRedisConnector();
#	define	CreateRedisConnectorProc	CreateRedisConnector
#else 
/// ��̬��汾
typedef Redis::IRedisConnector* (RKT_CDECL*procCreateRedisConnector)();
#	define	CreateRedisConnectorProc	DllApi<procCreateRedisConnector>::load(MAKE_DLL_NAME(GuardConnector), "CreateRedisConnector")
typedef int (RKT_CDECL*procInt)();
#	define	RunTest		DllApi<procInt>::load(MAKE_DLL_NAME(GuardConnector), "test")
#endif


#pragma pack()

#endif	//__INTERFACE_REDIS_CONNECTOR_H__