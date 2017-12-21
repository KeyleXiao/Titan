#ifndef	__VALUEGOODSHELPER_H__
#define __VALUEGOODSHELPER_H__

#include "IBankPart.h"

class CPlayerBankPart;

class CValueGoodsHelper : public IDBRetSink
{
public:
	CValueGoodsHelper();

	~CValueGoodsHelper();

	// IDBRetSink
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	// ��ʼ��
	bool init(CPlayerBankPart *pBankPart, __IEntity *pMaster);

    void restore();
	// �ͷ�
	void release();

	// DB���ֵ��Ʒ��Ϣ
	bool saveValueGoodsInfoToDB(BYTE byType, DWORD dwGoodsID, DWORD dwGoodsCount);

	// DB����ֵ��Ʒ��Ϣ
	bool readValueGoodsInfoFromDB(BYTE byType);

	// DB����ֵ��Ʒ��Ϣ���
	void onDBReturnReadValueGoodsInfo(int nQueueIndex, void * pOutData, int nOutLen);

private:
	__IEntity * m_pMaster;
	CPlayerBankPart * m_pBankPart;
};


#endif
