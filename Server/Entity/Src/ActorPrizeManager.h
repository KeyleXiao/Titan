#ifndef __ACTORPRIZEMANAGER_H__
#define __ACTORPRIZEMANAGER_H__

#include "IBankPart.h"
#include "Chat_ManagedDef.h"

class CPlayerBankPart;
#define DEFAULTBROADCASTFLAG -1

// ��ɫ����������
class CActorPrizeManager
{
public:
	// ���캯��
	CActorPrizeManager();
	// ��������
	~CActorPrizeManager();

	// ���л�
	bool onSerialize(ibuffer& in);

	// �����л�
	bool deSerialize(obuf& out);

	// ��ʼ��
	bool init(CPlayerBankPart *pBankPart, __IEntity *pMaster, int nLoginMode);

    void restore();

	// �ͷ�
	void release();

    // �µ�һ��
    void onNewDay();

	// �Ƿ���Ը���ɫ����
	bool canPrizeActor(int nPrizeID, int nNum);
	// ����ɫ����
	bool prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc, bool bBroadcast);
	// ����ɫ������
	bool groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc);

    // �Ƿ�������ĳ������ָ�������ı���
    bool canAddChest(int nChestType, int nNum);
	// ��ӱ���
	bool addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc);

	// �򿪱���
	bool onMessageOpenChest(void *data, size_t len);

	// �㲥��������
	bool broadcastChestResult();

	// ���±������ݵ��ͻ���
	void updateChestInfoToClient();

	// DB��������Ϣ���
	void onDBReturnReadChestInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods);

private:
	// ���ݹ㲥��־����ʾ��Ϣ
	void broadcastSystemMessageByFlag(EMChatTipID nTipID, int nBroadcastFlag, LPCSTR szPlusParam, int nDelayTime = 0);

	// ����������ƽ�����ͻ���
	void sendRandomCardInfoToClient(int nPrizeID, DWORD* dwCardID, int* nValue, int nCount);

	// ����������������
	bool addTaskSearchNum(int nNum);

private:
	__IEntity *m_pMaster;
	CPlayerBankPart *m_pBankPart;

	int m_nChestResultBroadcastFlag;
	string m_strChestResult;

	// ������Ϣ
	map<DWORD, DWORD> m_mapChestInfo;
};

#endif	// __ACTORPRIZEMANAGER_H__