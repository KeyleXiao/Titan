#pragma once

#include "IDBEngineService.h"
#include "IEventEngine.h"
#include "IEntity.h"
#include "Match_SchemeDef.h"
#include <unordered_map>

class CPlayerBankPart;

class CRankSeasonManager : public IDBRetSink, public IEventExecuteSink
{
public:
	// 构造
	CRankSeasonManager();

	// 析构
	virtual ~CRankSeasonManager();

	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	// 数据库返回
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	///////////////////////////////IEventExecuteSink/////////////////////////////////////
	// 事件回调
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	///////////////////////////////CRankSeasonManager/////////////////////////////////////
	// 序列化
	bool onSerialize(ibuffer& in);
	
	// 反序列化
	bool deSerialize(obuf& out);

	// 初始化
	bool init(CPlayerBankPart * pBankPart, __IEntity * pMaster, int nLoginMode);

    void restore();

	// 清理
	void release();

	// 战场结束设置数据
	void setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo);

	// 客户端请求赛季详细数据
	void onMessageRequestSeasonDetail();

	// 客户端请求赛季详细数据
	void onMessageRequestSeasonRecord();

private:
	// 检测赛季时间
	void checkSeasonTime();

	// 处理赛季开始
	void dealSeasonBegin(int nBeginSeasonIndex);

	// 处理赛季结束
	void dealSeasonEnd(int nEndSeasonIndex);

	// 发送赛季奖励邮件
	void sendSeasonPrizeMail(SSchemeMatchSeasonPrize * pPrize);

	// 检测是否玩过排位
	bool checkHavePlayRank();

	// 获取最常使用的英雄
	void getSeasonMostUseHero(WORD * arrHero, WORD nlen);

	// 存DB当前赛季数据
	void updateSeasonDetailToDB();

	// 存DB历史赛季数据
	void addSeasonRecordToDB(SRankSeasonRecord& record);

	// 取DB赛季数据
	void readSeasonInfoFromDB();

	// DB返回当前赛季数据
	void onDBReturnSeasonDetail(int nQueueIndex, void * pOutData, int nOutLen);

	// DB返回历史赛季数据
	void onDBReturnSeasonRecord(int nQueueIndex, void * pOutData, int nOutLen);

private:
	__IEntity * m_pMaster;
	CPlayerBankPart * m_pBankPart;

	// 当前赛季数据
	SRankSeasonDetail m_seasonDetail;
	// 当前赛季英雄数据
	unordered_map<WORD, SRankSeasonDetailHero> m_mapSeasonDetailHero;
	// 赛季历史数据
	vector<SRankSeasonRecord> m_vecSeasonRecord;
};