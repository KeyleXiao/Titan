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
	// 是否可以添加增益卡
	bool canAddBenefitCard(int nBenefitCardID, int nNum);
	// 添加增益卡
	bool addBenefitCard(int nBenefitCardID, int nNum);
private:
	// 读取玩家的增益卡信息
	void readAllBenefitCard();
	
private:
	// 数据库返回结果：添加增益卡
	void onDBReturnBenefitCard_Add(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 数据库返回结果：读取玩家所有增益卡信息
	void onDBReturnBenefitCard_Read(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 数据库返回结果：更新玩家所有增益卡信息
	void onDBReturnBenefitCard_Update(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	// 更新到数据库
	bool updateBenefitCardToDB(BenefitCardInfo& info);
	// 添加到数据库
	bool addBenefitCardToDB(BenefitCardInfo& info);
	//////////////////////////////////////

public:
	// 计算增益卡倍数
	void calcBenefitCardMultiple(int *nGoldMultiple, int *nActorExpMultiple, int *nHeroExpMultiple);

	// 功能函数
private:
	// 获取增益卡信息根据类型
	bool getBenefitCardByType( int type, int subType, BenefitCardInfo& cardInfo);
	// 设置增益卡信息根据类型，没有则新增
	void setBenefitCard(const BenefitCardInfo& cardInfo);
	// 判断一张卡是否正在使用期
	bool isValidBenefitCard(BenefitCardInfo& card);
	// 获取某一类型的卡的增益系数
	int calcBenefitCardMultiple(int type, BENEFITCARD_VECTOR& usedList);
	
private:
	// 发送增益卡信息到客户端,nListType：1表示玩家剩余的卡，2表示本局触发效果的卡
	void sendBenefitCardToClient(BENEFITCARD_VECTOR& inCardList, int nListType);
	// 发送剩余的卡到客户端
	void sendReaminBenefitCardToClient();

private:
	// For Debug
	void printBenefitCardList(BENEFITCARD_VECTOR& cardList, string extraLog);
	void printAllBenefitCard();

	__IEntity *		                            m_pMaster;

	CPlayerBankPart*                            m_pBankPart;

	// 玩家当前获得的增益卡
	BENEFITCARD_VECTOR m_BenefitCardInfoList;
};
