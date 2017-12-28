/*******************************************************************
** 文件名:	Clan.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/1/2016	10:37
** 版  本:	1.0
** 描  述:	帮会对象类
            1、处理帮会对象相关的消息
            2、管理帮会对象的相关数据
            3、帮会称号，帮会联盟也在这个类进行管理
********************************************************************/

#ifndef __CLAN_H__
#define __CLAN_H__

class CClanService;
struct ISharePerson;
struct SKinInfo;
struct SKinMember;

#include "IClanService.h"
#include "ClanState.h"
#include "ClanPropLock.h"
#include "ClanMessageDef.h"
#include "Chat_ManagedDef.h"

struct SSharePersonProperty;
enum ELogoutMode;

// 玩家请求支援卡牌信息
struct SRequestCardItem
{
	DWORD dwPDBID;			            // 请求支援玩家
	int nIndex;							// 请求流水号
	char szName[ACTORNAME_MAXSIZE];     // 请求支援玩家名字
	int nLevel;							// 玩家等级
	int nCardID;                        // 卡牌ID
	int nCurCount;                      // 当前卡牌数量
	int nMaxCount;                      // 最大卡牌数量
	vector<PDBID> vecIDList;			// 捐赠者列表

	SRequestCardItem()
	{
		dwPDBID = 0;
		nIndex = 0;
		memset(szName, 0, sizeof(szName));
		nLevel = 0;
		nCardID = 0;
		nCurCount = 0;
		nMaxCount = 0;
	}
};

// 捐赠记录信息
struct SDonateCardLogInfo
{
	PDBID nGaveActorID;							// 赠与玩家ID
	char szGaveActorName[ACTORNAME_MAXSIZE];	// 赠与玩家名字
	PDBID nTakeActorID;							// 被赠与玩家ID
	char szTakeActorName[ACTORNAME_MAXSIZE];	// 被赠与玩家名字
	int nCardID;								// 卡牌ID
	int nCardNum;								// 卡牌数量

	SDonateCardLogInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

typedef list<SRequestCardItem> REQUEST_CARD_LIST;
typedef list<SRequestCardItem>::iterator REQUEST_CARD_LIST_ITER;

class CClan : public TimerHandler
{
private:
	enum EMTimerID
	{
		TimerID_SaveDB = 0,			              // 存盘定时器
	};

public:
	// 构造函数
	CClan();

	// 虚构函数
	virtual ~CClan();

    //////////////////////////TimerHandler///////////////////////////
    virtual void OnTimer(unsigned long dwTimerID);

public:
	// 创建，启动社会服时，从数据库中读取帮会数据来创建
	bool onDBCreate(CClanService *pClanService, LPCSTR pszContext, int nLen);
	// 设置帮会成员数据
	bool setDBMemberContext(LPCSTR pszContext, int nLen);
	// 创建，由场景服发来创建帮会对象（场景服中在中心库及区库中创建本帮会成功后发来社会服创建）
	bool create(CClanService *pClanService, LPCSTR pszContext, int nLen);
	// 初始化帮会数据
	bool init();
	// 销毁
	void release();

	// 取得帮会名字
	LPCSTR getName();
	// 取得帮主名字
	LPCSTR getShaikhName();
	// 取得帮会宣言
	LPCSTR getManifesto();
	// 取得帮会战队个数
	int getKinCount();
	// 获取帮会成员列表
	int getKinList(DWORD *pKinList, int nLen);
	// 取得帮会战队成员个数
	int getMemberCount();
	// 取得某个玩家的帮会身份
	int getActorIdentity(DWORD dwPDBID);
	// 取得世界排名
	int getWorldPlace();
	// 获得历史活跃度
	int getHistoryActivity();

    // 取得帮会所有战力值
    int getTotalFightScore();

	// 取得帮会的整型数据-TClanProp
	int getNumProp(DWORD dwPropID);
	// 设置帮会的整型数据-TClanProp
	bool setNumProp(DWORD dwPropID, int nValue, bool bSaveDB, bool bSyncToZone);

	// 设置当前状态
	bool setCurState(int nState);
	// 设置当前排名
	void setPlace(int nPlace, bool bSaveDB);
	// 设置当前世界排名
	void setWorldPlace(int nPlace, int nHistoryActivity);
	// 设置帮主
	void setShaikh(int nPDBID, char* szText = NULL);
	// 设置帮主名字
	void setShaikhName(LPCSTR szName);

	// 接收成员
	void acceptMember(DWORD dwKinID);
	// 删除成员
	void delMember(DWORD dwKinID, DWORD dwKinShaikhID, bool bSaveDB);

	// 添加申请中成员, 不处理全局成员列表和数据
	void addApplyMember(DWORD dwKinID);
	// 删除申请中成员, 不处理全局成员列表和数据
	void removeApplyMember(DWORD dwKinID);

	// 解散帮会
	void dismiss(TClanDismissType nReason);

	/*
	* @param dismissActivity : 解散的周活跃度
	* @param dismissWeeks    : 连续低于解散活跃度的周数
	*/
	void autoDismiss(int dismissActivity, int dismissWeeks);

	// 帮会数据存数据库
	void updateDataToDB();
	// 同步帮会数据到场景服
	void updateDataToZone(CSID ServerID = INVALID_SERVER_ID);
	// 同步帮会数据到客服端（只同步最重要的几个数据）
	void updateDataToClient(DWORD dwPDBID, int nZoneID = -1);
	// 广播帮会数据到客服端（只同步最重要的几个数据）
	void broadcastDataToClient();
	// 把更新消息广播给观察者
	void broadcastMsgToObserve(int nMsgID, LPCSTR pszData, size_t nLen);
	// 联盟杯赛提示
	void broadcastClanLegendCupInfoToClient(DWORD dwPDBID = INVALID_PDBID);
	// 联盟杯赛提示
	void broadcastClanCreateLegendCupToClient(LONGLONG llLegendCupID, int nMaxKinCount, LPCSTR pCupName);

    // 联盟杯赛提示
    void recCreateLegendCupEvent( LPCSTR pCupName);

	// 是否有在线成员
	bool hasOnLineMember();
	// 是否读取完成
	bool isReadFinish();
	// 设置读取完成
	void setReadFinish(bool bFinish);
	// 成员登录调用
	void onMemberLogin(SSharePersonProperty& shareProperty);

	// 成员注销调用
	void onMemberLogout(ISharePerson * pSharePerson, ELogoutMode nLineType);

	// 帮会事件处理函数--添加帮会事件
	void addClanEvent(const SClanEvent &sClanEven);
	// 获取帮会事件
	void onClientUpdateClanEvent(DWORD dwPDBID);

    // 更新帮会基本数据
	void updateClientClanBaseData(DWORD dwPDBID);
	// 填充ClientMember数据
	void fillClientMemberInfo(SClientClanMember &clientMember, SKinInfo &KinInfo, SKinMember *pKinMember);

    // 帮会服务器模块消息码
    void onClanHandleMessage(DWORD dwServerID, DWORD dwMsgID, LPCSTR pszData, size_t nLen);

    // 帮会客户端消息码
    void onClanClientMsg(DWORD dwClientID, DWORD dwMsgID, LPCSTR pszData, size_t nLen);
    
    // 更新帮会排行榜数据
    void updateClanOrderInfo();

	// 重置周活跃度
	void ResetWeekActivity();

	// 任命盟主
	void appointShaikh(DWORD dwPDBID, DWORD dwNewShaikhPDBID);

    // 检测队长状态
    void CheckShaikhState();

    // 重置联盟杯赛dida
    void resetClanLegendDidaSend();

	// 刷新杯赛创建次数
	void refreshCreateLegendCupCount();

private:
	// 场景服请求卡牌支援
	void onSceneRequestCard(DWORD dwSeverID, LPCSTR pszData, size_t nLen);
	// 场景服捐赠卡牌
	void onSceneDonateCard(DWORD dwSeverID, LPCSTR pszData, size_t nLen);
	// 场景服更新属性
	void onSceneUpdateProp(DWORD dwSeverID, LPCSTR pszData, size_t nLen);

	// 解散帮会消息处理
	void onClientDismiss(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 取消解散帮会消息处理
	void onClientCanelDismiss(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 剔除成员
	void onClientKick(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 帮主提出战队成员
	void onClientKickKinMember(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 申请加入
	void onClientApply(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 接收成员
	void onClientAdmit(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 临时处理自动接收成员，非正式功能
	void onForceAdmit(DWORD dwKinID);
	// 退出帮会
	void onClientQuit(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 请求帮会基本数据
	void onClientClanBaseData(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 修改宣言，公告
	void onClientSetStringProp(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 退出观察
	void onClientExitObserve(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 请求卡牌列表客户端消息码
	void onClientRequestCardList(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 请求卡牌客户端消息码
	void onClientRequestCard(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 捐赠卡牌客户端消息码
	void onClientDonateCard(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
    // 捐赠卡牌客户端消息码
    void onClientAppoint(DWORD dwPDBID, LPCSTR pszData, size_t nLen);
	// 修改整形属性数据
	void onClientSetIntProp(DWORD dwPDBID, LPCSTR pszData, size_t nLen);

    // 发送邮件给当前联盟所有玩家
    void sendEMailToAll(int mailID, LPCSTR szParam = NULL);

private:
	// 广播一个属性到客户端
	void broadcastPropToClient(int nPropID);

	// 覆盖修改 非负数
	void cover_NoNegative(DWORD dwPropID, int nValue);
	// 增量修改 非负数
	void increment_NoNegative(DWORD dwPropID, int nValue);

	// 添加成员
	bool addMember(int nPDBID);

	// 更新卡牌列表到客户端
	void updateCardListToClient(DWORD dwPDBID);
    // 更新请求卡牌到客户端
    void updateRequestCardToClient(DWORD dwPDBID, const SRequestCardItem &requestCardInfo);
    // 移除请求卡牌到客户端
	void removeRequestCardToClient(DWORD dwPDBID, DWORD dwRemoveDBID);
	// 增加请求提示计数
	void addRequestTipToClient(SSharePersonProperty &shareProperty, int nCardID);

	// 发送数据给所有在线成员
	void sendDataToAllClanMember(char *pData, size_t nLen);

	// 查找请求卡牌信息
	REQUEST_CARD_LIST_ITER findRequestCard(PDBID targetID);

    // 查找对某个人捐赠卡牌次数
    int findRequestCardDonateCount(DWORD dwRequestPDBID, DWORD dwDonatePDBID);

	// 通知解散帮会
	void notifyDismiss(TClanDismissType nReason);

    // 帮主改变需要更新基础数据下去
    void sendClanBaseData();

    // 发送请求捐卡提示
    void sendRequestTip(DWORD dwPDBID);

	// 盟主进入离线状态
	void enterOfflineState();

	// 卸任时间到执行卸任动作
	void executeOutgoing();

	// 挑选新的盟主
	bool chooseNewShaikh(SKinMember & kinMember);

    // 联盟杯赛信息广播
    void clanLegendCupInfoBroadcast(DWORD dwActorID);

    // 联盟检测杯赛发布
    void clanCheckLegendCupInfoPublic(DWORD dwActorID);
private:
    CClanService *m_pClanService;					// 帮会服务  
	bool m_bReadFinish;								// 帮会和战队的数据是否都读取完毕

	// 帮会数据
	DWORD m_dwReadFlag;								// 已经从数据库读出数据的标志
	DWORD m_dwID;									// 帮会ID
	char m_szName[CLAN_NAME_SIZE];					// 帮会名
	int m_nState;									// 帮会的状态：临时，正式，解散中
	int m_nFlag;									// 帮会旗帜
	int m_nLevel;									// 帮会等级
	int	m_nEstablishTime;							// 帮会创立时间

	char m_szManifesto[CLAN_MANIFESTO_SIZE];		// 帮会的宣言
	char m_szNotice[CLAN_NOTICE_SIZE];				// 帮会的每日公告
	int m_nDismissApplyTime;						// 发起解散的时间
	int m_nConstructDegree;							// 建设度
	int m_nGold;									// 黄金
	int m_nActivity;								// 活跃度
	int m_nWeekActivity;							// 周活跃度,一周清一次
	int m_nWeekCount;							    // 连续{m_nWeekCount}周低于解散活跃度
	int	m_nPlace;									// 排名
	int	m_nWorldPlace;								// 世界排名
	int	m_nHistoryActivity;							// 历史活跃度
	int m_nWorldID;									// 游戏世界ID
	int	m_nKickTime;								// 上一次踢人的时间
	int	m_nKickNum;									// 上一次踢人的次数
	int m_nLegendCupCount;							// 可举行杯赛次数
	int m_nAutoAcceptFlag;							// 自动接受标识

	int m_nShaikhID;								// 帮会族长PDBID
	Clan_ManagerDef::EMClanShaikhState m_ShaikhState;// 盟主状态
	char m_szShaikhName[ACTORNAME_MAXSIZE];			// 帮主名字
	list<int> m_nMemberIDList;						// 帮会成员列表
	int m_nMemberCount;								// 成员个数

	list<DWORD> m_ApplyKinList;						// 申请中成员个数

	int	m_nMemberLastLogoutTime;					// 成员的最后上线时间

	// 整型数据的地址数组
	int* m_propAddress[emClanProp_NumPropCount];

	// 帮会事件列表
	list<SClanEvent> m_EvenList;
	
	// 观察者列表
	list<DWORD> m_ObserveList;

	// 帮会状态
	CClanNormalState m_NormalState;					// 正式状态
	CClanDismissingState m_DismissingState;			// 解散状态
	CClanState* m_StateCtrl[Clan_ManagerDef::emClanState_Num];		// 状态指针数组

	// 是否存数据库
	bool m_bSaveDB;

	// 发送数据的临时Buff
	obuf m_ob;
	obuf8192 m_obdata;

	bool m_bIsInit;

	// 请求卡牌表
    REQUEST_CARD_LIST   m_RequestCardList;
	// 请求卡牌流水号
	int m_nRequestCardIndex;
	// 捐赠卡牌记录
	list<SDonateCardLogInfo> m_DonateCardLogInfo;
    bool m_bClanLegendDidaSend;

};

#endif // __CLAN_H__