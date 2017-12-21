#pragma once

#include "IDBEngineService.h"
#include "IEventEngine.h"
#include "IEntity.h"
#include "Match_SchemeDef.h"
#include <unordered_map>

class CPlayerBankPart;

class CRankSeasonManager : public IDBRetSink, public IEventExecuteSink
{
public:
	// ����
	CRankSeasonManager();

	// ����
	virtual ~CRankSeasonManager();

	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	// ���ݿⷵ��
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	///////////////////////////////IEventExecuteSink/////////////////////////////////////
	// �¼��ص�
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	///////////////////////////////CRankSeasonManager/////////////////////////////////////
	// ���л�
	bool onSerialize(ibuffer& in);
	
	// �����л�
	bool deSerialize(obuf& out);

	// ��ʼ��
	bool init(CPlayerBankPart * pBankPart, __IEntity * pMaster, int nLoginMode);

    void restore();

	// ����
	void release();

	// ս��������������
	void setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo);

	// �ͻ�������������ϸ����
	void onMessageRequestSeasonDetail();

	// �ͻ�������������ϸ����
	void onMessageRequestSeasonRecord();

private:
	// �������ʱ��
	void checkSeasonTime();

	// ����������ʼ
	void dealSeasonBegin(int nBeginSeasonIndex);

	// ������������
	void dealSeasonEnd(int nEndSeasonIndex);

	// �������������ʼ�
	void sendSeasonPrizeMail(SSchemeMatchSeasonPrize * pPrize);

	// ����Ƿ������λ
	bool checkHavePlayRank();

	// ��ȡ�ʹ�õ�Ӣ��
	void getSeasonMostUseHero(WORD * arrHero, WORD nlen);

	// ��DB��ǰ��������
	void updateSeasonDetailToDB();

	// ��DB��ʷ��������
	void addSeasonRecordToDB(SRankSeasonRecord& record);

	// ȡDB��������
	void readSeasonInfoFromDB();

	// DB���ص�ǰ��������
	void onDBReturnSeasonDetail(int nQueueIndex, void * pOutData, int nOutLen);

	// DB������ʷ��������
	void onDBReturnSeasonRecord(int nQueueIndex, void * pOutData, int nOutLen);

private:
	__IEntity * m_pMaster;
	CPlayerBankPart * m_pBankPart;

	// ��ǰ��������
	SRankSeasonDetail m_seasonDetail;
	// ��ǰ����Ӣ������
	unordered_map<WORD, SRankSeasonDetailHero> m_mapSeasonDetailHero;
	// ������ʷ����
	vector<SRankSeasonRecord> m_vecSeasonRecord;
};