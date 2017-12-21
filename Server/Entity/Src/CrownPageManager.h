/*******************************************************************
** 文件名:	CrownPageManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	12/1/2016
** 版  本:	1.0
** 描  述:	皇冠系统

********************************************************************/
#ifndef __CROWNPAGEMANAGER_H__
#define __CROWNPAGEMANAGER_H__

#include "IBankPart.h"
#include "IShareServer.h"
#include "EntityDef.h"
#include "IDBEngineService.h"
class CPlayerBankPart;



// 皇冠系统管理类
class CCrownPageManager: public IDBRetSink,public TimerHandler
{
    enum
    {
        ETIMERID_READ_CROWNDATA,
    };

    enum
    {
        READ_CROWNDATA_TIME = 500,
    };

public:
	// 构造函数
	CCrownPageManager();

	bool onSerialize(rkt::ibuffer& in);

	bool deSerialize(rkt::obuf& out);

	/** 创建
	*/
	bool onLoad( __IEntity* pEntity, CPlayerBankPart *pBankPart);

	/** 重置 
	*/
	void restore();

    // 释放
    void release(void);

	/////////////////////////////////////宝石逻辑/////////////////////////////////////
    // 是否宝石已满
    bool isFullForGemsone(void);
	// 判断下是否能添加皇冠宝石
	bool canAddCrownGemstone(int nGemstoneID,BYTE byGrade=1,int nCount = 1);
	// 添加皇冠宝石数据
	bool addCrownGemstone(int nGemstoneID,int nCount = 1, BYTE byGrade = 1, int nExp = 0, bool bAppraisal = false);
    // 是否能移除皇冠宝石
    bool canDeleteCrownGemstone(int nGemstoneDBID);
    // 移除皇冠宝石
    bool deleteCrownGemstone(int nGemstoneDBID);

    //宝石出售
    bool onMessageRequestSaleGemstone(LPCSTR pszMsg, size_t nLen);

    // 宝箱开宝石 TODO 现在功能关闭中 
    void onMessageOpenGemstoneChest(int nChestType);
	// 客户端选择原石（宝石抽奖） TODO 现在功能关闭中
	bool onMessageGemstoneSelectCobblestone(LPCSTR pszMsg, size_t nLen);
	// 客户端花费点券开窗 TODO 现在功能关闭中
	bool onMessageGemstoneOpenWindow(LPCSTR pszMsg, size_t nLen);
	// 客户端开窗后点选宝石 TODO 现在功能关闭中
	bool onMessageGemstoneWindowOpenToSelecet(LPCSTR pszMsg, size_t nLen);

    // 客户端鉴赏
    bool onMessageRequestAppraisalGemstone(LPCSTR pszMsg, size_t nLen);

	// 增加镶嵌宝石的经验在战斗后
	void addCrownInlayGemstoneExp( LONG lgValue );

private:
    // 请求所有宝石
    void readCrownGemstonesFromDatabase();
    // 请求宝石产出
    void readCrownGemstoneProductFromDatabase();
    // 数据库返回所有宝石	
    void onDBReturnCrownAllGemstoneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // 数据库返回产出	
    void onDBReturnCrownGemstoneProductInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // 保存单个宝石到数据库
    void saveCrownGemstoneToDatabase(GEMSTONE_ITEM* pItem);
    // 从数据库中移除单个皇冠宝石
    bool deleteCrownGemstoneFromDatabase(int nGemstoneDBID);

    // 创建新的宝石产出信息
    void newGemstoneProduceDistribution();
    // 更新产出信息到数据库
    void saveGemstoneDistributionToDatabase();
	// 宝石宝箱减少
	bool decreaseGemstoneChest();
    // 更新宝石颜值和威力
    void updateCrownGemstoneFaceScoreAndPropValue(GEMSTONE_ITEM* pItem);

    // 鉴赏操作
    bool doCrownAppraisalGemstone(LPCSTR pszMsg, size_t nLen);
    // 鉴赏宝石
    bool canAppraisalCrownGemstone(ISchemeCrownGemstoneInfo *pCrownInfoService, int nMainStoneDBID, int nAssistantStoneDBID, int& nCostMoney, int& nCostTicket);
    // 鉴赏宝石
    bool appraisalCrownGemstone(ISchemeCrownGemstoneInfo *pCrownInfoService, int nMainStoneDBID);
	// 鉴赏日志
	void writeGemstoneAppraisalLog( LPCSTR pszMsg, size_t nLen, bool bAppraisalSucess );
    // 频道广播
    bool broadCastGemstoneLevelUpInChatChannel(int nGemstoneGrade);

    // 增加宝石经验
    bool addCrownGemstoneExp(int nGemstoneDBID, int nExp);
    // 增加宝石经验
    bool canAddCrownGemstoneExp(int nGemstoneDBID, int nExp);

    // 更新宝石到客户端
    void sendCrownGemstoneToClient(GEMSTONE_ITEM* pItem);
    // 更新所有宝石到客户端
    void sendCrownGemstonesToClient();
    // 更新产出信息到客户端
    void sendGemstoneDistributionToClient();
    // 宝石鉴定结果到客户端
    void sendGemstoneAppraisalRetToClient(int nGemstoneDBID, int nCritNum, bool bAppraisalRet);

    // 获得宝石
    GEMSTONE_ITEM* getGemstonePossessItem(int nGemstoneDBID);
	// 获得最大宝石dbid
	int  getGemstoneDBMaxIndex();
    // 获得宝石拥有数量
    int  getGemstoneCount();
    // 获得宝石空位置
    int getGemstoneListEmptyIndex(void);
    // 宝石排序 
    void sortCrownGemstoneList();

	/////////////////////////////////////皇冠逻辑/////////////////////////////////////
public:
    // 能否增加页面
    bool canAddPage(DWORD nNum=1);
    // 创建新页面
    bool createNewPage(DWORD nNum=1);
	// 客户端请求 保存皇冠页
	bool onMessageRequestSaveCrownPage(LPCSTR pszMsg, size_t nLen);
	// 客户端请求 删除皇冠页
	bool onMessageRequestDeleteCrownPage(LPCSTR pszMsg, size_t nLen);
	// 客户端请求 切换皇冠页
	bool onMessageRequestSwitchCrownPage(int nPageDBID);
    // 客户端请求 请求别人所有皇冠页数据
    void onMessageRequestOtherPlayerCrownPages(void * data, size_t nLen);

	// 获取其他玩家当前皇冠页数据
	virtual bool getOtherCrownPageData(obuf & outData);
	// 获取其他玩家当前宝石数据
	bool getOtherCrownGemstoneData(obuf & outData);
	// 战场设置效果 timeToAddEffect:ETIME_TO_ADD_EFFECT
	void setCrownEffect(int timeToAddEffect);

private:
    // 自动创建默认页
    void autoCreateDefaultPage();
    // 创建一页
    int innerCreateOnePage();
    // 数据库中读取自己所有皇冠页数据,由定时器触发读取
    void readCrownPagesFromDatabase();
	// 数据库返回 所有自己的皇冠页数据
	void onReturnReadCrownPages(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // 数据库返回 更新皇冠页数据
    void onReturnUpdateCrownPage(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // 数据库返回 删除皇冠页数据
    void onReturnDeleteCrownPage(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

    // 设置效果
    bool deSerializeCrownPropertyList(rkt::obuf& out);
    // 发送自己皇冠镶嵌情况到客户端
    void sendCrownCrownPagesToClient();

	// 获得皇冠页
	CROWN_PAGE_ITEM* getGemstoneCrownPageItem(int nPageDBID);
	// 获得皇冠最大页dbid
	int  getCrownDBPageMaxIndex();
	// 获得皇冠页数量
	int  getCrownPageCount();
	// 获得皇冠页空位置
    int getCrownPageListEmptyIndex(void);
    // 皇冠页排序 
    void sortCrownPageList();

	///////////////////////////////// IDBRetSink ////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	/////////////////////////////////TimerHandler/////////////////////////////////////////
	virtual void OnTimer( unsigned long dwTimerID );

	// 目前没有数据库数据 先测试下
	//void doTest();


private:
    // 当前拥有页面数量
    BYTE                            m_nPageCount;
    // 皇冠页数据库中记录保存最大索引值
    int				                m_nDBPageMaxIndex;
    // 当前皇冠页
    int                             m_nCurrentCrownPage;

    // 宝石数据库中记录保存最大索引值
    int				                m_nDBGemstoneMaxIndex;
	// 每页镶嵌详细信息
	CROWN_PAGE_ITEM                 m_crownPageList[CROWN_PAGE_MAX_COUNT];    
	// 拥有宝石信息
	GEMSTONE_ITEM                   m_crownGemstoneList[CROWN_GEMSTONE_MAX_COUNT];

	// 宝石产出信息
	GEMSTONE_PRODUCE_DISTRIBUTION   m_gemstoneProduce;

	// 皇冠属性列表
	int			                    m_crownPropertyList[CROWN_PROPERTY_COUNT_MAX];
	// 宝箱id 产出相关
	int				                m_nGemstoneChestID;

	__IEntity *                     m_pMaster;

	CPlayerBankPart *               m_pBankPart;
};

#endif	// __CrownPageMANAGER_H__