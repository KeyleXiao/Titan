#pragma once
#include "IGameDatabaseDef.h"
#include <map>
#include "IEntity.h"
#include "TimerHandler.h"
#include "IDBEngine.h"
#include "IBankPart.h"
#include "SchemeDef.h"
#include "EntityDef.h"

class CPlayerBankPart;

class CBenefitCardManager: public IDBRetSink
{
public:
	CBenefitCardManager();
	~CBenefitCardManager();

	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

public:
	bool onSerialize(rkt::ibuffer& in);

	bool deSerialize(rkt::obuf& out);

	bool init( __IEntity* pEntity,CPlayerBankPart* pPlayerBank);

	void restore();
    void release(void);

public:
	// �Ƿ����������濨
	bool canAddBenefitCard(int nBenefitCardID, int nNum);
	// ������濨
	bool addBenefitCard(int nBenefitCardID, int nNum);
private:
	// ��ȡ��ҵ����濨��Ϣ
	void readAllBenefitCard();
	
private:
	// ���ݿⷵ�ؽ����������濨
	void onDBReturnBenefitCard_Add(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ���ݿⷵ�ؽ������ȡ����������濨��Ϣ
	void onDBReturnBenefitCard_Read(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ���ݿⷵ�ؽ������������������濨��Ϣ
	void onDBReturnBenefitCard_Update(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// ���µ����ݿ�
	bool updateBenefitCardToDB(BenefitCardInfo& info);
	// ��ӵ����ݿ�
	bool addBenefitCardToDB(BenefitCardInfo& info);
	//////////////////////////////////////

public:
	// �������濨����
	void calcBenefitCardMultiple(int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple);

	// ���ܺ���
private:
	// ��ȡ���濨��Ϣ��������
	bool getBenefitCardByType( int type, int subType, BenefitCardInfo& cardInfo);
	// �������濨��Ϣ�������ͣ�û��������
	void setBenefitCard(const BenefitCardInfo& cardInfo);
	// �ж�һ�ſ��Ƿ�����ʹ����
	bool isValidBenefitCard(BenefitCardInfo& card);
	// ��ȡĳһ���͵Ŀ�������ϵ��
	int calcBenefitCardMultiple(int type, BENEFITCARD_VECTOR& usedList);
	
private:
	// �������濨��Ϣ���ͻ���,nListType��1��ʾ���ʣ��Ŀ���2��ʾ���ִ���Ч���Ŀ�
	void sendBenefitCardToClient(BENEFITCARD_VECTOR& inCardList, int nListType);
	// ����ʣ��Ŀ����ͻ���
	void sendReaminBenefitCardToClient();

private:
	// For Debug
	void printBenefitCardList(BENEFITCARD_VECTOR& cardList, string extraLog);
	void printAllBenefitCard();

	__IEntity *		                            m_pMaster;

	CPlayerBankPart*                            m_pBankPart;

	// ��ҵ�ǰ��õ����濨
	BENEFITCARD_VECTOR m_BenefitCardInfoList;
};
