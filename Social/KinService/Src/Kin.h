//==========================================================================
/**
** 文件名:	e:\Rocket\Server\KinService\Src\KinInviteManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2009/3/9
** 版  本:	1.0
** 描述  :  1、处理战队对象相关的消息
            2、管理战队对象的相关数据
            3、战队称号，战队联盟也在这个类进行管理
*/
//==========================================================================


#ifndef __Kin_H__
#define __Kin_H__

class KinService;
#include "KinService.h"
#include "KinState.h"
#include "NameLenDef.h"
#include "IKin.h"
#include "Kin_ManagedDef.h"
#include "EventDef.h"
#include "KinDef.h"

#include <unordered_map>
using namespace Kin_ManagerDef;

struct SKinApply;
struct SSharePersonProperty;
struct ISharePerson;

enum
{
	Task_KinCar		=	0,
	Task_KinTaskMax	=	10,
};


class CKin : public IKin
{
public:
	/// purpose: 构造函数
	CKin();

	/// purpose: 虚构函数
	virtual ~CKin();

public:
	/////////////////////////////IKin/////////////////////////////////////////////
    virtual void release();

    /// purpose: 取得战队名字
    virtual LPCSTR getName();

    /// purpose: 取得战队的整型数据
    virtual int getNumProp(DWORD dwPropID);

    /// purpose: 删除成员
    //@ param  : nPDBID 玩家PDBID
    //@ param  : szName 玩家名字
    virtual void delMember(DWORD dwPDBID);

    /// purpose: 取得战队成员个数
    virtual int getMemberCount();

    /////////////////////////////IKin end/////////////////////////////////////////////
    virtual bool create(KinService *pKinService, LPCSTR pszContext, size_t nLen);

    /// purpose: 初始化战队数据
	bool Init();

    /// purpose: 添加成员
    bool addMember(DWORD dwPDBID, int nIdentity);

    /// purpose: 添加申请中成员
    //@ note   : 不处理全局成员列表和数据
    void addApplyMember(DWORD dwPDBID);

    /// purpose: 删除申请中成员
    //@ note   : 不处理全局成员列表和数据
    void removeApplyMember(DWORD dwPDBID);

    /// purpose: 接收成员
    //@ param  : nPDBID 玩家PDBID
    //@ param  : szName 玩家名字
    //@ param  : nLevel 仅申请进入战队时的等级
    virtual void acceptMember(SKinApply* pKinApply);

    /// purpose: 消息处理
    virtual void onMessage(DWORD dwMsgID, SKinMsgHead * pKinMsgHead, LPCSTR pszMsg, size_t nLen);

    // 获取总战力
    virtual DWORD getTotalFightScore();

     /** 获取战队总战力
	@param dwKinID		:战队ID 
	*/
    virtual SAutoAccept* getAutoAccept();

    /// purpose: 数据创建
    //@ pszContext 创建现场（DBREQ_TABLE_KIN结构）
    bool createFromDB(KinService *pKinService, LPCSTR pszContext, int nLen);

    // 修改战队名字
    bool rename(char* szKinName, size_t nNameLen, int nRenameTime);

public:

    /// purpose: 设置战队成员数据
    bool setDBMemberContext(LPCSTR pszContext, int nLen);

    /// purpose: 设置战队基本数据
    bool setDBBaseContext(LPCSTR pszContext, int nLen);

	/// purpose: 设置战队杯赛奖励数据
	bool setDBLegendAwardContext(LPCSTR pszContext, int nLen);

	/// purpose: 设置战队战绩数据
	bool setDBLegendWarContext(LPCSTR pszContext, int nLen);

    /// purpose: 是否已经读取扩展数据
    bool isReadPlusData();

    /// purpose: 设置已经读取扩展数据
    void setReadPlusData();

    /// purpose: 同步战队数据到场景服
    void updateDataToScene(CSID ServerID = INVALID_SERVER_ID);

    /// purpose: 同步战队数据到客服端（只同步最重要的几个数据）
    void updateDataToClient(DWORD dwPDBID);

    /// purpose: 广播战队数据到客服端（只同步最重要的几个数据）
    void broadcastDataToClient();

    /// purpose: 删除一个副族长
    bool removeElder(int nElderID);

    /// purpose: 设置战队的整型数据
    bool setNumProp(DWORD dwPropID, int nValue, bool bSaveDB, bool syncToZone);

    /// purpose: 覆盖修改 非负数
    void cover_NoNegative(DWORD dwPropID, int nValue);

    /// purpose: 增量修改 非负数
    void increment_NoNegative(DWORD dwPropID, int nValue);

    /// purpose: 活跃度修改 非负数
    void increment_Activity(DWORD dwPropID, int nValue);

    /// purpose: 取得战队族长PDBID
    LPCSTR getShaikhName();

	/// purpose: 修改战队族长名字
	void setShaikhName(LPCSTR szName);

    /// purpose: 取得战队族长名字
    int getShaikhPDBID();

    /// purpose: 战队数据存数据库
    void updateDataToDB(bool bSoon = false);

    /// purpose: 能否解散战队
    bool canDisMissKin(DWORD dwActorID);

    /// purpose: 解散战队
    void dismiss(TKinDismissType nReason, LPCSTR pszReason);

	/*
	* @param dismissActivity : 解散的周活跃度
	* @param dismissWeeks    : 连续低于解散活跃度的周数
	*/
	void autoDismiss(int dismissActivity, int dismissWeeks);

    /// purpose: 添加观察者
    bool addObserve(DWORD dwPDBID);

    /// purpose: 删除观察者
    void removeObserve(DWORD dwPDBID, bool bCoerceRemove);

    /// purpose: 查找观察者
    SKinObserve* findObserve(DWORD dwPDBID);

    /// purpose: 战队消息广播
    //bool broadcastSystemTips(TipsType type, InfoPos pos, const char* szText);

    /// purpose: 把属性改变广播给观察者

    void broadcastDataToObserve( LPCSTR pszData, size_t nLen);

    /// purpose: 把属性改变广播给观察者
    //@ param  : nEventID 事件ID
    void broadcastToObserve(int nEventID, LPCSTR pszData, size_t nLen);

    /// purpose: 广播消息到在线人员
    void broadcastKinMsgToPerson( int nSystemMessageID,LPCSTR szParamList);

    /// purpose: 获得成员列表
    DWORD getMemberList(UID* pReturnArray, DWORD dwArrayMaxSize);

    /// purpose: 同步战队数据到客服端（只同步最重要的几个数据）
    void sendDataToLoginClient(DWORD dwPDBID);

    /// purpose: 广播消息有权限的在线人员
    void broadcastToHaveRightMember(TKinPopedom ePopedom,int nEventID, LPCSTR pszData, size_t nLen);

    /// purpose: 广播给观察者基础数据
    void broadcastKinBaseInfoToOb();

    // 战队排行榜信息更新
    void updateKinOrderInfo();

	// 战队杯赛奖励增加
	void addKinLegendAward(SKinCupAwardNode* pAwardNode);

	// 战队杯赛战绩增加
	void addKinLegendWarResult(SKinWarRecordNode* pRecordNode);

	// 战队杯赛荣誉增加
	void addKinLegendGlory(int nNum, int nGloryType);

	// 取战队杯赛荣誉
	int getKinLegendGlory(int nGloryType);

	// 成员登录调用
	void onMemberLogin(SSharePersonProperty& shareProperty);

	// 成员注销调用
	void onMemberLogout(ISharePerson * pSharePerson, ELogoutMode nLineType);

	// 重置周活跃度
	void ResetWeekActivity();

    // 检测队长状态
    void CheckShaikhState();

private:

    /// purpose: 进入观察											-- 来自客户端
    void onClientEnterObserve(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 退出观察											-- 来自客户端
    void onClientExitObserve(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 解散战队消息处理									-- 来自客户端
    void onClientDismiss(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 请求战队基本数据									-- 来自客户端
    void onClientKinBaseData(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 申请加入											-- 来自客户端
    void onClientApply(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 接收成员											-- 来自客户端
    void onClientAdmit(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 剔除成员											-- 来自客户端
	void onClientKick(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 退出战队											-- 来自客户端
    void onClientQuit(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 修改公告											-- 来自客户端
    void onClientMotify(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 编辑自动接收											-- 来自客户端
    void OnClientEditAutoAccept(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 请求自动接收											-- 来自客户端
    void OnClientRequestAutoAccept(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 请求战队事件											-- 来自客户端
    void OnClientRequestKinEventList(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 请求战队荣誉数据										-- 来自客户端
	void OnClientRequestKinGloryData(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 请求战队总战绩数据										-- 来自客户端
	void OnClientRequestWarRecordTotal(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 请求战队单场战绩数据									-- 来自客户端
	void OnClientRequestWarRecordSingle(ClientID clientID, LPCSTR pszData, size_t nLen);

    /// purpose: 任命职位									            -- 来自客户端
    void OnClientAppoint(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 请求查看杯赛战队信息									-- 来自客户端
	void OnClientLegendCupKinInfo(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 请求查看杯赛战队荣誉									-- 来自客户端
	void OnClientLegendCupKinGlory(ClientID clientID, LPCSTR pszData, size_t nLen);

	/// purpose: 响应场景服更新战队属性 
	void OnSceneUpdateProp(DWORD dwSeverID, LPCSTR pszData, size_t nLen);

    /// purpose: 设置某个玩家的职位
    void setMemberIdentity(int nPDBID, int nIdentity, char* szReason = NULL);

    /// purpose: 自动接收成员
    bool autoAccept(DWORD dwPDBID);

    /// purpose: 增加一个战队事件
    void AddKinEvent(SKinEvent* pKinEvent);

    // purpose: 下发给观察者战队基础数据
    void sendObClientKinBaseData();

	// 战队解散发送邮件通知玩家
	void sendMail(TKinDismissType nReason);

	// 队长进入离线状态
	void enterOfflineState();

	// 队长进入卸任状态
	void enterOutgoingState();

	// 卸任时间到执行卸任动作
	void executeOutgoing();

	// 挑选新的队长
    SKinMember* chooseNewShaikh();

	// 获取战队成员的当天活跃度
	int getMemberActivity(DWORD pdbid);

    // 战队排行榜信息更新
    DWORD getOnlineMemberCount();

    /**
	@name				: 重新计算战队成员的战力值排名
	@param				: 是否将最新的战力排名信息保存数据库
	@return
	*/
	void reCalcBattleValOrder(bool bSaveDB);

	// 发送给客户端战队荣誉公共处理函数
	void send2ClientKinGloryData(int nKeyAction, int nClientID);

	// 填充客户端战队成员数据
	void fillClientMemberInfo(SClientKinMember& clientMember, const SKinMember* pMember, const SSharePersonProperty& memberShareProperty);

private:
    KinService *m_pKinService;                      // 所属战队服务         
	// 战队数据
	DWORD   m_dwReadFlag;							// 已经从数据库读出数据的标志
	DWORD   m_dwID;									// 战队ID
	char    m_szName[ACTORNAME_MAXSIZE];			// 战队名
	int     m_nState;								// 战队的状态 TKinState ：临时，正式，解散中
	int     m_nLevel;								// 战队等级
	int	    m_nEstablishTime;						// 战队创立时间
	
	char	m_szNotice[KIN_NOTICE_LEN];				// 战队的每日公告
	int		m_nDismissApplyTime;					// 发起解散的时间
	int		m_nMoney;								// 战队资金
	int 	m_nActivity;							// 活跃度
	int     m_nWeekActivity;						// 周活跃度,一周清一次
	int     m_nWeekCount;						    // 连续{m_nWeekCount}周低于解散活跃度
	int 	m_nFightCount;							// 正在对战的数量
	int		m_nMemberLastLogoutTime;				// 成员的最后上线时间
	int		m_nPlace;								// todo  计算排名
	int		m_nHistoryActivity;						// 历史活跃度
	int		m_nLastRenameTime;						// 上次战队改名时间
	int 	m_nWorldID;								// 游戏世界ID
	int		m_nShaikhID;							// 战队族长PDBID
	char	m_szShaikhName[ACTORNAME_MAXSIZE];		// 战队族长名字
	Kin_ManagerDef::EMKinShaikhState m_ShaikhState;// 队长状态
	int		m_nElderIDList[KIN_ELDER_LEN];			// 战队副族长PDBID列表
	int		m_nElderNum;							// 战队副族长的个数
	list<DWORD> m_nMemberIDList;					// 战队成员列表
	int m_nMemberCount;								// 成员个数
	list<DWORD> m_nApplyIDList;						// 申请中的战队成员列表

    char	m_szSlogan[KIN_SLOGAN_LEN];				// 宣传口号
	SAutoAccept m_AutoSetting;						// 自动接收数据
	int     m_nSendSloganTime;                          // 上次发送宣传口号的时间
	int*    m_propAddress[emKinProp_NumPropCount];     // 整型数据的地址数组
	bool    m_bIsInit;                                 // 是否初始化
    bool    m_bIsReadPlusData;                         // 读取附加数据标示
    map<DWORD, SKinObserve> m_mapObserveList;        // 战队观察者列表
    // 战队事件列表
    list<SKinEvent> m_EventList;

	// 杯赛
	char	m_szLabel[KIN_LABEL_SIZE];					// 战队标签
	int		m_nGlory[emLegendGlory_Max];				// 战队荣耀ELegendGlory
	vector<SKinCupAwardNode> m_cupAwardVec;				// 杯赛奖励列表
	vector<SKinWarRecordNode> m_warRecordBaseVec;		// 战队历史战绩头vector 
};

#endif // __Kin_H__