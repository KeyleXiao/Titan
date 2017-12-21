/*******************************************************************
** 文件名:	SchemeMatchRankLimit.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/13/2015
** 版  本:	1.0
** 描  述:	撮合限制设置
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "MatchSchemeDef.h"
// 定义
class CSchemeMatchRankLimit : public ISchemeMatchRankLimitInfo, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchRankLimit>
{
public:

    /** 取得撮合类型Rank开放时间限定
    */
    virtual bool matchTypeRankIsOpen(int eMatchTypeID, int nDay, int nHour, int nMinute);

    /** 取得撮合类型Rank开放时间限定
    */
    virtual int getTimeOpenFailedDesID(int eMatchTypeID);

    /** 取得撮合类型英雄限定配置
    */
    virtual SMatchRankLimitHeroSchemeInfo* getRankLimitHeroShemeInfo(int eMatchTypeID);

    /** 取得撮合英雄限定相关
    */
    virtual int getMatchHeroFailedDesID(int eMatchTypeID, bool isSelf);

    /** 取得英雄等级失败信息
    */
    virtual int getMatchPlayerLevelFailedDesID(int eMatchTypeID, bool isSelf);

	/** 取得惩罚类型配置信息
	*/
	virtual SSchemePunish* getPunishInfo(BYTE byPunishID);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMatchRankLimit//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeMatchRankLimit(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeMatchRankLimit(void);

private:
    bool    LoadTimeLimitScheme(ICSVReader * pCSVReader);

    bool    LoadHeroLimitScheme(ICSVReader * pCSVReader);

	bool    LoadPunishScheme(ICSVReader * pCSVReader);

private:
   
    // 类型索引 <nDay, list<SMatchRankLimitTimeSchemeInfo>>
    typedef map<int, list<SMatchRankLimitTimeSchemeInfo>> TMAP_WEEK_TIMELIMITE;

    // <nMatchTypeID, TMAP_WEEK_TIMELIMITE>
    typedef map<EMMatchType, TMAP_WEEK_TIMELIMITE> TMAP_WEEK_TIMELIMITE_MGR;

    TMAP_WEEK_TIMELIMITE_MGR m_mapMatchRankLimitTimeMgr;

    std::map<EMMatchType, SMatchRankLimitHeroSchemeInfo> m_mapMatchRankLimitHero;

	std::map<BYTE, SSchemePunish>	    m_SchemePunish;

};