#ifndef __POINTSHOPMANAGER_H__
#define __POINTSHOPMANAGER_H__

#include "IBankPart.h"

class CPlayerBankPart;

// 商城管理类
class CPointShopManager : public TimerHandler
{
	enum
	{
		REFLUSHCARDLISTMAXNUM = 6,					// 卡牌列表一天可手动刷新最大次数
	};

	enum
	{
		REFLUSHCARDLISTINTERVAL = 4 * 60 * 60,		// 卡牌列表刷新时间间隔
	};

	enum
	{
		ETIMERID_REFLUSH_CARDLIST,					// 卡牌列表刷新定时器
	};

public:
	/////////////////////////////////TimerHandler/////////////////////////////////////////
	virtual void OnTimer( unsigned long dwTimerID );

public:
	// 构造函数
	CPointShopManager();
	// 析构函数
	~CPointShopManager();

	// 初始化
	bool init(CPlayerBankPart *pBankPart, __IEntity *pMaster);

	void initReflushCardInfo();

    void restore();
	void release();

    // 新的一天
    void onNewDay();

	// 更新商城已购买数量数据到客户端
	void updatePointShopBuyNumToClient(int nSellID);

	// 更新商城卡牌列表信息到客户端
	void updatePointShopCardList();

	// 刷新商城卡牌列表
	void reflushPointShopCardList();	

	// 刷新商城已购买数量
	void reflushPointShopBuyNum();

	// 刷新商城已购买数量和重置手动刷新次数
	void reflushBuyNumAndResetOptCount();

	// 购买商城物品
	bool onMessagePointShopBuyGoods(void *data, size_t len);	

	// 客户端请求刷新卡牌列表
	void onMessageReflushCardList();

private:
	// 获取商城限制物品已购买数量
	int getPointShopLimitGoodsNum(WORD nSellID);
	// 设置商城限制物品已购买数量
	bool setPointShopLimitGoodsNum(WORD nSellID, WORD nNum);

private:
	__IEntity *m_pMaster;
	CPlayerBankPart *m_pBankPart;
};

#endif	// __POINTSHOPMANAGER_H__