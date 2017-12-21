#ifndef __REDIS_CONNECTOR_H__
#define __REDIS_CONNECTOR_H__

#include "IRedisConnector.h"
#include "redis.h"
#include "Setting.h"
using namespace Redis;

class RedisConnector : public IRedisConnector
{
public:
	/////////////////// RedisConnector ///////////////////////////////////////////
	// ����
	RedisConnector();

	// ����
	~RedisConnector();

	/////////////////// IRedisConnector ///////////////////////////////////////////
	// ����
	virtual bool			create(const char* ip = "127.0.0.1", WORD port = 6379);

	// �ͷ�
	virtual void			release();

	// �ر�
	virtual void			stop();

	// ��ȡValue������
	virtual IValueOp*		valueOp() { return m_pValueOp; }
	// ��ȡList������
	virtual IListOp*		listOp() { return m_pListOp; }
	//// ��ȡSet������
	//virtual ISetOp*			setOp();
	//// ��ȡHash������
	//virtual IHashOp*		hashOp();
	// ��ȡ����ָ�������
	virtual IKeysOp*		keysOp() { return m_pKeysOp; }
	// Serverָ�������
	virtual	IServerOp*		serverOp() { return m_pServerOp; }

	//virtual IPipeline*		pipeline() { return m_pPipelineOp; }

	/* interfaces for hash */
	// ��ӣ����ԣ�
	virtual bool	setHash(const string& strKey, const string& strField, const string& strValue);

	// ��ӣ���ԣ�
	virtual bool	mSetVecHash(const string& strKey, const vector<string>& vecField, const vector<string>& vecValue);

	// ��ӣ���ԣ�
	virtual bool	mSetMapHash(const string& strKey, const map<string, string>& mapFv);

	// ��ӣ�long:��Field��ӣ���Field�Ӻͣ�
	virtual bool	incrbyHash(const string& strKey, const string& strField, long nIncr, long * pnValue);

	// ��ӣ�float:��Field��ӣ���Field�Ӻͣ�
	virtual bool	incrbyfloatHash(const string& strKey, const string& strField, double dIncr, double * pdValue);

	// ɾ��
	virtual bool	delHash(const string& strKey, const string& strField);

	// ��ȡ�����ԣ�
	virtual bool	getHash(const string& strKey, const string& strField, string * pstrValue);

	// ��ȡ����ԣ�vector��
	virtual bool	mGetVecHash(const string& strKey, const vector<string>& vecField, vector<string> * pvecValue);

	// ��ȡ����ԣ�map��
	virtual bool	mGetMapHash(const string& strKey, const vector<string>& vecField, map<string, string> * pmapFv);

	// ��ȡ������Keys��
	virtual bool	keysHash(const string& strKey, vector<string>* pvecField);

	// ��ȡ������Values��
	virtual bool	valuesHash(const string& strKey, vector<string>* pvecValue);

	// ��ȡ�����м�ֵ�ԣ�
	virtual bool	getAllHash(const string& strKey, map<string, string>* pmapFv);

	// ����
	virtual bool	existHash(const string& strKey, const string& strField, bool* pbValue);

	// ����
	virtual bool	lenHash(const string& strKey, long* pnValue);


	/* interfaces for set */
	// ���
	virtual bool	addSet(const string& strKey, const string& strValue);

	// ɾ����������
	virtual bool	remSet(const string& strKey, const string& strValue);

	// ɾ���������
	virtual bool	remSet(const string& strKey, const vector<string>& vecValue);

	// ɾ���������
	virtual bool	popSet(const string& strKey, string* pstrValue);

	// ��ȡ�����У�
	virtual bool	membersSet(const string& strKey, vector<string>* pvecValue);

	//// ��ȡ�������
	//virtual bool	randMemberSet(const string& strKey, long nCount, vector<string>* pvecValue);

	// ����
	virtual bool	isMemberSet(const string& strKey, const string& strValue, bool* pbValue);

	// ����
	virtual bool	cardSet(const string& strKey, long* pnValue);


	///* interfaces for sorted set */
	//// ���
	//virtual bool	addZset(const string& strKey, double dScore, const string& strElem);
	//// ��ӣ���Elem��ӣ���Elem�Ӻͣ�
	//virtual bool	incrbyZset(const string& strKey, double dIncr, const string& strElem);
	//// ɾ����������
	//virtual bool	remZset(const string& strKey, const string& strElem);
	//// ɾ���������
	//virtual bool	remZset(const string& strKey, const vector<string>& vecElem);
	//// ɾ����Ԫ�����䣩
	//virtual bool	remRangeByLexZset(const string& strKey, const string& strMin, const string& strMax);
	//// ɾ�����������䣩
	//virtual bool	remRangeByRankZset(const string& strKey, long nStart, long nStop);
	//// ɾ�����������䣩
	//virtual bool	remRangeByScoreZset(const string& strKey, double dMin, double dMax);
	//// ��ȡ���������䣬����vec��
	//virtual bool	rangeZset(const string& strKey, long nStart, long nStop, vector<string>* pvecValue);
	//// ��ȡ���������䣬����map��
	//virtual bool	rangeZset(const string& strKey, long nStart, long nStop, map<string, string>* pmapValue);
	//// ��ȡ��Ԫ�����䣬����vec��
	//virtual bool	rangeByLexZset(const string& strKey, const string& strMin, const string& strMax, vector<string>* pvecValue);
	//// ��ȡ���������䣬����vec��
	//virtual bool	rangeByScoreZset(const string& strKey, double dMin, double dMax, vector<string>* pvecValue);
	//// ��ȡ���������䣬����map��
	//virtual bool	rangeByScoreZset(const string& strKey, double dMin, double dMax, map<string, string>* pmapValue);
	//// ��ȡ���������䣬����vec��
	//virtual bool	revRangeZset(const string& strKey, long nStart, long nStop, vector<string>* pvecValue);
	//// ��ȡ���������䣬����vec��
	//virtual bool	revRangeByScoreZset(const string& strKey, double dMax, double dMin, vector<string>* pvecValue);
	//// ��ȡ���������䣬����map��
	//virtual bool	revRangeByScoreZset(const string& strKey, double dMax, double dMin, map<string, string>* pmapValue);
	//// ������ȫ����
	//virtual bool	cardZset(const string& strKey, long* pnValue);
	//// �������������䣩
	//virtual bool	countZset(const string& strKey, double dMin, double dMax, long* pnValue);
	//// ������Ԫ�����䣩
	//virtual bool	countByLexZset(const string& strKey, const string& strMin, const string& strMax, long* pnValue);
	//// ���Σ�����
	//virtual bool	rankZset(const string& strKey, const string& strElem, long* pnValue);
	//// ���Σ�����
	//virtual bool	revRankZset(const string& strKey, const string& strElem, long* pnValue);
	//// ����
	//virtual bool	scoreZset(const string& strKey, const string& strElem, double* pdValue);

protected:
	/* interfaces for Pipeline */
	// ��ִ��Pipline����֮ǰ��׼������
	virtual IPipeline*	pipeBegin();
	// ��ִ��Pipline����֮��Ĺ���
	virtual bool		pipeEnd(IPipeline* pPipe);


private:
	CRedisClient m_redis;

	// Value������
	IValueOp*		m_pValueOp;
	// List������
	IListOp*		m_pListOp;
	//// Set������
	//ISetOp*			m_pSetOp;
	//// Hash������
	//IHashOp*		m_pHashOp;
	// ����ָ�������
	IKeysOp*		m_pKeysOp;
	// Serverָ�������
	IServerOp*		m_pServerOp;

	CSetting		m_Setting;
};


#endif	//__REDIS_CONNECTOR_H__