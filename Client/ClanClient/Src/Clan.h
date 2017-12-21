/*******************************************************************
** 文件名:	Clan.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/7/2016
** 版  本:	1.0
** 描  述:	帮会对象类
********************************************************************/

#ifndef __CLAN_H__
#define __CLAN_H__

#include "IClanClient.h"
#include "ClanDef.h"
#include "GameViewDef.h"

// 帮会对象类
class CClan : public IMessageHandler
{
public:
	// 构造函数
	CClan();

	// 析构函数
	virtual ~CClan();
	
	// 创建
	bool create(void);
	// 释放
	void release(void);
	// 清空所有数据
	void reset(void);
	// 是否有效
	bool isVaild(void);

	// 取得帮会的整型数据
	int getNumProp(DWORD dwPropID);
	// 设置帮会的整型数据
	bool setNumProp(DWORD dwPropID, int nValue);

	// 取得帮会的字符型数据
	string getStringProp(DWORD dwPropID);
	// 设置帮会的字符型数据
	bool setStringProp(DWORD dwPropID, string strValue);

	// 取得成员的个数
	int getMemberCount();
	// 取得成员的数据
	SClientClanMember* getMemberByID(DWORD dwPDBID);
	// 删除成员
	void removeMember(DWORD dwPDBID);
	// 排序
	void sortMember(int nSortType);

	// 取得申请成员的个数
	int getApplyMemberCount();
	// 取得申请成员的数据
	SApplyMember* getApplyMemberByIndex(int nIndex);
	// 增加申请成员
	void addApplyMember(SApplyMember* pApplyMember);
	// 删除申请成员
	void removeApplyMember(DWORD dwKinID);

	// 获得帮会事件的个数
	int getClanEventCount();
	// 获得帮会事件的Txt信息
	string getClanEventContext(int index);

    ////////////////////////IMessageHandler////////////////////
	// 消息处理
	void onMessage(SNetMsgHead *pHead, void *pData, size_t nLen);

private:
	// 更新帮会数字属性
	void onUpdateClanIntProp(LPCSTR pszMsg, int nLen);
	// 更新帮会字符串属性
	void onUpdateClanStrProp(LPCSTR pszMsg, int nLen);

	// 更新帮会基本数据
	void onUpdateClanBaseData(LPCSTR pszMsg, int nLen);
    // 设置帮会基本数据
    void onSetClanBaseData(LPCSTR pszMsg, int nLen);

	// 更新帮会成员数据
	void onUpdateMemberInfo(LPCSTR pszMsg, int nLen);
	// 移除帮会成员数据
	void onRemoveMemberInfo(LPCSTR pszMsg, int nLen);

	// 更新客户端资源信息
	void onClanUpdateClient(LPCSTR pszMsg, int nLen);
	
	// 更新帮会事件
	void onClanUpdateEvent(LPCSTR pszMsg, int nLen);

    // 请求卡牌列表
    void onClanRequestCardList(LPCSTR pszMsg, int nLen);
    // 更新请求卡牌事件
    void onClanUpdateRequestCard(LPCSTR pszMsg, int nLen);
    // 移除请求卡牌事件
	void onClanRemoveRequestCard(LPCSTR pszMsg, int nLen);
	// 请求捐赠列表事件
	void onClanRequestDonateLog(LPCSTR pszMsg, int nLen);
	// 更新捐赠列表事件
	void onClanUpdateDonateLog(LPCSTR pszMsg, int nLen);
	// 增加请求提示事件
	void onClanAddRequestTip(LPCSTR pszMsg, int nLen);
	// 捐卡信息消息
	void onSendRequestCardInfo(LPCSTR pszMsg, int nLen);

    // 盟主创建杯赛dida
    void onClanCreateLegendCupDida(LPCSTR pszMsg, int nLen);

private:
	// 填充显示层帮会成员信息
	void fillClanMemberCmd(cmd_clan_memberlist &sCmd, SClientClanMember *pMemberInfo);

	// 设置自动标示到显示层
	void onUpdatePropToView(DWORD dwPropID);
private:
	// 帮会数据
	char m_szManifesto[CLAN_MANIFESTO_SIZE];		// 帮会的宣言
	char m_szNotice[CLAN_NOTICE_SIZE];				// 帮会的每日公告
	char m_szName[CLAN_NAME_SIZE];					// 帮会名
	char m_szShaikhName[ACTORNAME_MAXSIZE];			// 帮主名字

	bool m_ClanVaild;								// 帮会对象是否有效	
	DWORD m_dwID;									// 帮会ID
	int m_nState;									// 帮会的状态：临时，正式，解散中
	int	m_nEstablishTime;							// 帮会创立时间
	int m_nShaikhID;								// 帮会族长PDBID
	int m_nLevel;									// 帮会等级
	int m_nConstructDegree;							// 建设度
	int m_nGold;									// 黄金
	int m_nActivity;								// 活跃度
	int m_nWeekActivity;                            // 周活跃度
	int m_nShaikhState;                             // 盟主状态
	int m_nPlace;									// 排名
	int nDismissApplyTime;							// 战队解散时间
	int m_nKickTime;								// 踢人时间
	int m_nKickNum;									// 踢人次数
	int m_nLegendCupCount;							// 杯赛举办次数
	int m_nAutoAcceptFlag;							// 自动接受设置

	// 成员数据
	vector<SClientClanMember>	m_MemberVector;
	
	// 申请中的成员列表
	vector<SApplyMember>	m_ApplyMemberVector;

	// 帮会事件列表
	vector<SClanEvent> m_ClanEventVector;

	// 整型数据的地址数组
	int* m_propAddress[emClanProp_NumPropCount];
};

extern CClan* g_pClan;

#endif // __CLAN_H__