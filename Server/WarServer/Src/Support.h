/*******************************************************************
** 文件名: Support.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/15
** 版  本: 1.0
** 描  述: 点赞
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#include "GameDef.h"
#include "WarDef.h"
#include "SchemeDef.h"
#include "IDBEngine.h"
#include "ISupportMgr.h"
#include <map>

class IWar;
struct SESVTPersonData;
struct SWPerson;
struct SCampTeamRecordInfo;

struct SSupportVoteInfo
{
	PDBID uPDBIU;
	int nVoteID;
	int  nVotePoint;
	int nLabelID;
	SSupportVoteInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};


class CSupport : public ISupportMgr, public IDBRetSink, public TimerHandler
{
public:
	CSupport(IWar* pWar);
	~CSupport();


public:
	virtual void OnTimer(unsigned long dwTimerID);

public:
	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

public:
	////////////////////////ISupportMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 客户端点赞操作
	virtual void onClientMsgPersonAddLabelRequest(UID uidActor, void* pData, size_t stLen );

	// 选出可参与点赞玩家
	virtual void filterPlayersForSupportCondition(bool bAwark);

	////////////////////////ISupportMgr end/////////////////////////

private:
	// 获得每个评分列表最大值列表
	void getEveryESVTMaxList(SESVTPersonData* personESVTList);
	// 获得每个玩家每个评分值子项
	int getEveryESVTMaxValue(const SWPerson* person, const SCampTeamRecordInfo* teamInfo, int nVoteID);
	// 对评分列表进行筛选处理 排除不合格项
	void filterEveryESVTMaxList(SESVTPersonData* personESVTList);
	// 将点赞信息更新到缓存
	void updateESVTMaxListToCache(SESVTPersonData* personESVTList);
	// 更新选出可参与点赞玩家到客户端
	void updateAllFilteredPlayersForSupportToClient();
	// 广播增加标签操作到所有玩家
	void broadcastAddLabelToAllPlayer(PDBID operatorPdbid, PDBID targetPdbid,int nLabelID);

private:
	// 给玩家添加标签
	void addLabelRequest(PDBID operatorPdbid, PDBID targetPdbid, int nLabelID);

	// 获取下个点赞的电脑玩家
	PDBID getNextSupportPlayer();
	// 统计所有电脑玩家
	void calcAllComputerPlayer();
	// 统计点赞台的所有真实玩家
	void calcAllRealPlayer();
	// 获取玩家身上的一个随机标签
	int getRandomLabel(PDBID pdbid);
	// 缓存所有玩家名字
	void cacheAllPlayerName();
	// 随机被点赞的玩家
	PDBID randomSupportedPlayer();


private:

	// 战场
	IWar*	m_pWar;
	
	// 点赞缓存
	std::map<PDBID, SSupportVoteInfo> m_mapSupportCacheEnd;      
	// 已经完成点赞操作的玩家
	std::vector<PDBID> m_AlreadySupportList;

	// 所有电脑玩家
	vector<PDBID> m_AllComputer;
	// 当前点赞电脑玩家下标
	int m_nCurSupportIndex;
	// 点赞台的真实玩家
	vector<PDBID> m_RPSupported;
	// 点赞台的电脑玩家
	vector<PDBID> m_CPSupported;
	// 所有玩家名字缓存
	map<PDBID, string> m_AllPlayerName;
	// 用于第一个电脑点赞的时间计数
	int m_nFirstSupportTime;
};