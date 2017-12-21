/*******************************************************************
** 文件名:	Clan.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/6/2016
** 版  本:	1.0
** 描  述:	帮会对象类
********************************************************************/

#ifndef __CLAN_H__
#define __CLAN_H__

#include "IClanScene.h"

// 帮会对象实现类
class CClan
{
public:
	// 构造函数
	CClan(void);

	// 析构函数
	~CClan(void);

public:
	// 创建
	bool create(LPCSTR pszData, int nLen);

	// 关闭
	void release(void);

	// 取得帮会的整型数据
	int getNumProp(DWORD dwPropID);

	// 设置帮会的整型数据（覆盖型）
	bool coverNumProp(DWORD dwPropID, int nValue);

	// 取得帮会名字
	LPCSTR getName();

	// 取得帮主名字
	LPCSTR getShaikhName();

	// 更新数据
	bool updateData(LPCSTR pszData, int nLen);

	// 添加一个成员
	void addMember(DWORD dwMemberID);

	// 删除一个成员
	void removeMember(DWORD dwMemberID);

	// 检查杯赛
	bool checkCreateLegendCup(int nLegendCupConfigID);

private:
	// 覆盖修改 非负数
	void cover_NoNegative(DWORD dwPropID, int nValue);

	// 增量修改 非负数
	void increment_NoNegative(DWORD dwPropID, int nValue);

private:
	DWORD m_dwReadFlag;								// 已经从数据库读出数据的标志
	DWORD m_dwID;									// 帮会ID
	char m_szName[CLAN_NAME_SIZE];					// 帮会名
	DWORD m_dwCountryID;							// 国家ID
	int m_nState;									// 帮会的状态：临时，正式，解散中
	int m_nLevel;									// 帮会等级
	int m_nConstructDegree;							// 建设度
	int m_nGold;									// 黄金
	int m_nStuffValue;								// 材料
	int m_nNimbus;									// 灵气
	int m_nFightCount;								// 正在对战的帮会个数
	int m_nShaikhID;								// 帮主PDBID
	char m_szShaikhName[ACTORNAME_MAXSIZE];			// 帮会帮主名
	int m_nActivity;								// 活跃度
	int m_nWeekActivity;                            // 周活跃度，一周清一次
	int m_nPlace;									// 排名
	int m_nWorldID;									// 游戏世界ID
	int m_nLegendCupCount;							// 可举行杯赛次数

	list<DWORD> m_MemberList;						// 在线成员列表

	// 整型数据的地址数组
	int* m_propAddress[emClanProp_NumPropCount];
};

#endif // __CLAN_H__