#ifndef __POINTSHOPMANAGER_H__
#define __POINTSHOPMANAGER_H__

#include "IBankPart.h"

class CPlayerBankPart;

// �̳ǹ�����
class CPointShopManager : public TimerHandler
{
	enum
	{
		REFLUSHCARDLISTMAXNUM = 6,					// �����б�һ����ֶ�ˢ��������
	};

	enum
	{
		REFLUSHCARDLISTINTERVAL = 4 * 60 * 60,		// �����б�ˢ��ʱ����
	};

	enum
	{
		ETIMERID_REFLUSH_CARDLIST,					// �����б�ˢ�¶�ʱ��
	};

public:
	/////////////////////////////////TimerHandler/////////////////////////////////////////
	virtual void OnTimer( unsigned long dwTimerID );

public:
	// ���캯��
	CPointShopManager();
	// ��������
	~CPointShopManager();

	// ��ʼ��
	bool init(CPlayerBankPart *pBankPart, __IEntity *pMaster);

	void initReflushCardInfo();

    void restore();
	void release();

    // �µ�һ��
    void onNewDay();

	// �����̳��ѹ����������ݵ��ͻ���
	void updatePointShopBuyNumToClient(int nSellID);

	// �����̳ǿ����б���Ϣ���ͻ���
	void updatePointShopCardList();

	// ˢ���̳ǿ����б�
	void reflushPointShopCardList();	

	// ˢ���̳��ѹ�������
	void reflushPointShopBuyNum();

	// ˢ���̳��ѹ��������������ֶ�ˢ�´���
	void reflushBuyNumAndResetOptCount();

	// �����̳���Ʒ
	bool onMessagePointShopBuyGoods(void *data, size_t len);	

	// �ͻ�������ˢ�¿����б�
	void onMessageReflushCardList();

private:
	// ��ȡ�̳�������Ʒ�ѹ�������
	int getPointShopLimitGoodsNum(WORD nSellID);
	// �����̳�������Ʒ�ѹ�������
	bool setPointShopLimitGoodsNum(WORD nSellID, WORD nNum);

private:
	__IEntity *m_pMaster;
	CPlayerBankPart *m_pBankPart;
};

#endif	// __POINTSHOPMANAGER_H__