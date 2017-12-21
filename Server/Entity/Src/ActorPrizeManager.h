#ifndef __ACTORPRIZEMANAGER_H__
#define __ACTORPRIZEMANAGER_H__

#include "IBankPart.h"
#include "Chat_ManagedDef.h"

class CPlayerBankPart;
#define DEFAULTBROADCASTFLAG -1

// 角色奖励管理类
class CActorPrizeManager
{
public:
	// 构造函数
	CActorPrizeManager();
	// 析构函数
	~CActorPrizeManager();

	// 序列化
	bool onSerialize(ibuffer& in);

	// 反序列化
	bool deSerialize(obuf& out);

	// 初始化
	bool init(CPlayerBankPart *pBankPart, __IEntity *pMaster, int nLoginMode);

    void restore();

	// 释放
	void release();

    // 新的一天
    void onNewDay();

	// 是否可以给角色奖励
	bool canPrizeActor(int nPrizeID, int nNum);
	// 给角色奖励
	bool prizeActor(int nPrizeID, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc, bool bBroadcast);
	// 给角色奖励组
	bool groupPrizeActor(int nPrizeGroupID, int *pOutPrizeID, int &nOutPrizeCount, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc);

    // 是否能增加某个类型指定数量的宝箱
    bool canAddChest(int nChestType, int nNum);
	// 添加宝箱
	bool addPrizeChest(int nChestType, int nNum, WORD wApproach, DWORD dwTargetID, LPCSTR szLogDesc);

	// 打开宝箱
	bool onMessageOpenChest(void *data, size_t len);

	// 广播宝箱内容
	bool broadcastChestResult();

	// 更新宝箱数据到客户端
	void updateChestInfoToClient();

	// DB读宝箱信息结果
	void onDBReturnReadChestInfo(vector<DBREQ_TABLE_VALUE_GOODS> * pvecGoods);

private:
	// 根据广播标志发提示消息
	void broadcastSystemMessageByFlag(EMChatTipID nTipID, int nBroadcastFlag, LPCSTR szPlusParam, int nDelayTime = 0);

	// 发送随机卡牌结果到客户端
	void sendRandomCardInfoToClient(int nPrizeID, DWORD* dwCardID, int* nValue, int nCount);

	// 奖励任务搜索次数
	bool addTaskSearchNum(int nNum);

private:
	__IEntity *m_pMaster;
	CPlayerBankPart *m_pBankPart;

	int m_nChestResultBroadcastFlag;
	string m_strChestResult;

	// 宝箱信息
	map<DWORD, DWORD> m_mapChestInfo;
};

#endif	// __ACTORPRIZEMANAGER_H__