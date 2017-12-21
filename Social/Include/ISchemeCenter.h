/*******************************************************************
** 文件名:	ISchemeCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	9/1/2014
** 版  本:	1.0
** 描  述:	
            游戏脚本配置中心
********************************************************************/

#pragma once

#include "Vector3.h"
struct SMonsterScheme;

class	ISchemeMatchRoom;
class	ISchemeMatchRoomRule;
struct  ISchemeKinDataCenter;
class   ISchemeTeamDataCenter;
class   ISchemeMatchComputerName;
class   ISchemeMatchPlan;
struct  ISchemeMentorshipDataCenter;

struct SMapAreaInfo;
struct SMapLandSchemeInfo;
class CSchemeMapInfo;
struct SMapSchemeInfo;
struct STankSchemeInfo;
struct SchemeWarPersonConfigInfo;

struct SClanLevel;
struct SClanConfig;
struct SClanCardSchemeInfo;
struct SKinLevel;
struct SKinConfig;
struct SMentorshipConfig;

struct SSchemeLegendCup;
struct SSchemeLegendCupDetail;
struct SSchemeLegendCupPrize;

struct PersonSkinModelItem;

struct SMailConfig;
struct SMatchRankLimitTimeSchemeInfo;
struct SMatchRankLimitHeroSchemeInfo;
struct SMatchRankConfigSchemeInfo;
struct SSchemePunish;


// 刷新类型集合
enum EWarMonsterExec;
struct SWarInfoScheme;
typedef vector<EWarMonsterExec> EWarMonsTypeVect;
struct SWarScheme;
typedef vector<SWarInfoScheme> SWarInfoSchemeVect;
struct SWarPointScheme;
typedef vector<SWarPointScheme> WarPointVect;

class ISchemeMatchSeasonTime;

// 地图配置信息
struct MAP_SCHEME
{
	char    szName[64];			// 地图名字
	int     nMapID;				// 地图ID
	int     nWidth;				// 地图宽度
	int     nHeight;			// 地图高度
	int     nOffsetX;           // 地图坐标偏移
	int     nOffsetY;           // 地图坐标偏移
	int     nOffsetZ;           // 地图坐标偏移
	unsigned int uPathFindServiceID;	// 使用的寻路服务ID
};



// 效果配置信息
struct EFFECT_SCHEME
{
	int		id;					// 效果ID
	int		type;				// 效果类型
	int		params[4];			// 参数
};

// 地图信息
class ISchemeMapInfo
{
public:
	/** 取得地图配置信息
	@param   
	@param   
	@return  
	*/
	virtual SMapSchemeInfo * GetMapSchemeInfo(long lMapID) = 0;

	/** 取得寻路id=0地图配置信息
	@param   
	@param   
	@return  
	*/
	virtual void GetMapSchemeInfoNoPathID(vector<SMapSchemeInfo*>& vecRet) = 0;

	/** 格式化地图特殊配置信息
	@param   
	@param   
	@return  
	*/
	virtual bool FormatMapSchemeInfo(void) = 0;
};


struct SSchemeSystemMessage;
struct ISchemeSystemMessage
{
    // 取得配置信息
    virtual SSchemeSystemMessage* getSystemMessageInfo(int nId) = 0;
};

struct ISchemeClanDataCenter
{
    /// purpose: 取得帮会每级信息
    virtual SClanLevel* getLevelInfo(int nClanLevel) = 0;

    /// purpose: 取得帮会常量信息
    virtual SClanConfig* getConfig() = 0;

    /// purpose: 帮会卡牌信息
    virtual SClanCardSchemeInfo* getClanCardInfo(int nLevel) = 0;
};

// 比赛大厅房间配置接口
struct ISchemeKinDataCenter
{
public:

    /// purpose: 取得战队常量信息
    virtual SKinConfig* getConfig() = NULL;

    virtual SKinLevel* getLevelInfo(int nKinLevel) = NULL;
};


struct ISchemeMentorshipDataCenter
{
public:

	/// purpose: 取得帮会常量信息
	virtual SMentorshipConfig* getConfig() = 0;
};

// AI配置ID表
struct ISchemeAIIDTable
{
    // 获取默认英雄AI配置
    virtual int getHeroAIDefault(int nBattleFieldID, int nVocation, int nRank = 0) =0;

    // 校验AI配置
    virtual bool isHeroAIValid(int nBattleFieldID, int nVocation, int nAIID) =0;

    // 获取NPC AI配置ID
    virtual int getNpcAISchemeID(int nBattleFieldID, int nNpcID) = 0;

    // 是否有AI配置
    virtual bool isHaveAIConfig(int nBattleFieldID) = 0;

    // 获取NPC AI配置ID
    virtual const char* getHeroAIDes(int nBattleFieldID, int nVocation, int nAIID) = 0;
};

struct ISchemePersonModel
{
	
	/** 取得英雄名称
	@param wVocation: 英雄ID  
	@return  
	*/
	virtual const char *							getHeroName(int nHeroID) = 0;

	/** 取得人物英雄指定的皮肤模型项
	@param wVocation:	英雄ID
	@param nSkinID:		皮肤ID
	@return  
	*/
	virtual const PersonSkinModelItem*				getPersonSkinModelItem(int nHeroID,int nSkinID=0) = 0;
};

struct ISchemeLegendCup
{
public:

    /// purpose: 取得战队常量信息
    virtual SSchemeLegendCup* getConfig(int nConfigID) = NULL;

    virtual SSchemeLegendCupDetail* getDetailInfo(int nConfigID, int nRoundID) = NULL;

    virtual int getRoundIDByRegisterCount(int nConfigID,  int nKinCount) = NULL;

    virtual SSchemeLegendCupPrize* getPrizeInfo(int nPrizeID, int nRankID) = NULL;
};

struct ISchemeMailDataCenter
{
public:

	/// purpose: 取得邮件常量信息
	virtual SMailConfig* getConfig(int nMailFillID) = 0;
};

struct ISchemeMatchRankLimitInfo
{
    /** 取得撮合类型Rank开放时间限定
    */
    virtual bool matchTypeRankIsOpen(int eMatchTypeID, int nDay, int nHour, int nMinute) = 0;

    /** 获取开放失败描述
    */
    virtual int getTimeOpenFailedDesID(int eMatchTypeID) = 0;


    /** 取得撮合类型英雄限定配置
    */
    virtual SMatchRankLimitHeroSchemeInfo* getRankLimitHeroShemeInfo(int eMatchTypeID) = 0;

    /** 取得撮合英雄限定相关
    */
    virtual int getMatchHeroFailedDesID(int eMatchTypeID, bool isSelf) = 0;

    /** 取得惩罚类型配置信息
    */
    virtual SSchemePunish* getPunishInfo(BYTE byPunishID) = 0;


    /** 是否有时间限制
    */
    virtual bool haveTimeLimitInfo(int nMatchTypeID) = 0;

    /** 取得开始or结束触发器列表
    */
    virtual int getMatchTimeLimitTriggerList(int nMatchTypeID, bool bBegin, int* nTriggerArray, int nMaxCount) = 0;

    /** 获取时间限制配置
    */
    virtual SMatchRankLimitTimeSchemeInfo* getMatchTimeLimitShemeInfo(int eMatchTypeID, bool bBegin, int nTriggerID) = 0;
};

// 实体通用配置
struct ISchemeMatchRankConfigInfo
{
    /** 取得撮合匹配配置
    */
    virtual const SMatchRankConfigSchemeInfo* getMatchRankConfigShemeInfo(int eMatchTypeID, int nGradeId) = 0;


    /** 根据得分取得撮合配置信息
    */
    virtual SMatchRankConfigSchemeInfo* getShemeInfoByTypeAndScore(int eMatchTypeID, int nScore) = 0;

    /** 取得王者分段最低分
    */
    virtual int getMatchTypeKingRankBaseScore(int eMatchTypeID) = 0;

    /** 通过排名获得等级ID
    */
    virtual int getRankKingGradeByOrderID(int eMatchTypeID, int nOrderID) = 0;
};

struct SEntityXml_Server;
// 实体通用配置
struct ISchemeEntity
{
    virtual SEntityXml_Server *getEntityXml(void) = 0;
};

/**
	1.实现配置脚本的快速加载
	2.实现配置脚本的热部属（即在不重启服务器的情况修改游戏配置）
	3.根据发布环境，支持多种脚本加载方式（例如服务器的脚本从数据库中读取，本地不留源文件)
*/
struct ISchemeCenter
{
	virtual bool load() = 0;

	virtual void release() = 0;

	/** 取得地图信息
	@return  
	*/
	virtual ISchemeMapInfo* getSchemeMapInfo() = 0;

	virtual MAP_SCHEME * getMapScheme( int mapID ) = 0;

	virtual ISchemeMatchRoom * getSchemeMatchRoom() = 0;

	virtual ISchemeMatchRoomRule * getSchemeMatchRoomRule() = 0;

    // 取得系统提示配置信息
    virtual ISchemeSystemMessage* getSchemeSystemMessage() = 0;

    virtual ISchemeKinDataCenter *	getSchemeKinDataCenter() = 0;

    // 取得帮会数据中心
    virtual ISchemeClanDataCenter *	getSchemeClanDataCenter() = 0;

    // 取得AI配置相关
    virtual ISchemeAIIDTable *	getSchemeAIIDTable() = 0;

	/** 取得人物模型对象
	@return  
	*/
	virtual ISchemePersonModel* getSchemePersonModel() = 0;
	
	    // 取得组队配置相关
    virtual ISchemeTeamDataCenter *	getSchemeTeamDataCenter() = 0;


    virtual ISchemeMatchComputerName *	getSchemeMatchComputerName() = 0;

    virtual ISchemeMatchPlan *	getSchemeMatchPlan() = 0;

    virtual ISchemeLegendCup *	getSchemeLegendCup() = 0;

	// 取得师徒数据中心
	virtual ISchemeMentorshipDataCenter *	getSchemeMentorshipDataCenter() = 0;

	virtual ISchemeMailDataCenter *		getSchemeMailDataCenter() = 0;

    // 取得撮合限定配置
    virtual ISchemeMatchRankLimitInfo* getSchemeMatchRankLimit() = 0;

    // 取得匹配Rank配置
    virtual ISchemeMatchRankConfigInfo*     getSchemeMatchRankConfig() = 0;

    /** 取得实体通用配置
    @return  
    */
    virtual ISchemeEntity* getSchemeEntity() = 0;

	// 获取排位赛季时间配置
	virtual ISchemeMatchSeasonTime* getSchemeMatchSeasonTime() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(SCHEMECENTER_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(SchemeCenter))
extern "C" ISchemeCenter * createSchemeCenter();
#	define	createSchemeCenterProc	createSchemeCenter
#else													/// 动态库版本
typedef ISchemeCenter * (RKT_CDECL * procCreateSchemeCenter)();
#	define	createSchemeCenterProc	DllApi<procCreateSchemeCenter>::load(MAKE_DLL_NAME(SchemeCenterService), "createSchemeCenter")
#endif