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

	// 初始化
	bool init(CPlayerBankPart *pBankPart, __IEntity *pMaster);

    void restore();
	// 释放
	void release();

	// DB存价值物品信息
	bool saveValueGoodsInfoToDB(BYTE byType, DWORD dwGoodsID, DWORD dwGoodsCount);

	// DB读价值物品信息
	bool readValueGoodsInfoFromDB(BYTE byType);

	// DB读价值物品信息结果
	void onDBReturnReadValueGoodsInfo(int nQueueIndex, void * pOutData, int nOutLen);

private:
	__IEntity * m_pMaster;
	CPlayerBankPart * m_pBankPart;
};


#endif
