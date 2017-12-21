
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

#include "ISchemeCenter.h"



class SchemeCenter : public ISchemeCenter
{
public:
	virtual bool load();
	virtual void release();

	virtual MAP_SCHEME * getMapScheme( int mapID )
	{
		static MAP_SCHEME s_mapinfo;
		sprintf(s_mapinfo.szName,"%s","金陵城" );
		s_mapinfo.nMapID   = 1;
		s_mapinfo.nWidth   = 1024;
		s_mapinfo.nHeight  = 1024;
		s_mapinfo.nOffsetX = 0;           // 地图坐标偏移
		s_mapinfo.nOffsetY = 0;           // 地图坐标偏移
		s_mapinfo.nOffsetZ = 0;           // 地图坐标偏移
		s_mapinfo.uPathFindServiceID = 0; // 无障碍，两点直达

		return &s_mapinfo;
	}

	virtual ISchemeMatchRoom *		        getSchemeMatchRoom();
	virtual ISchemeMatchRoomRule *	        getSchemeMatchRoomRule();
    // 取得系统提示配置信息
	virtual ISchemeSystemMessage*           getSchemeSystemMessage();

    virtual ISchemeKinDataCenter *	        getSchemeKinDataCenter();

	/** 取得地图信息
	@return  
	*/
	ISchemeMapInfo*                         getSchemeMapInfo();

    // 取得帮会数据中心
    virtual ISchemeClanDataCenter *	        getSchemeClanDataCenter();

    // 取得帮会数据中心
    virtual ISchemeAIIDTable *	            getSchemeAIIDTable();

	/** 取得人物模型对象
	@return  
	*/
	virtual ISchemePersonModel*		        getSchemePersonModel();
	
	virtual ISchemeTeamDataCenter *	        getSchemeTeamDataCenter();

    virtual ISchemeMatchComputerName *	    getSchemeMatchComputerName();

    virtual ISchemeMatchPlan *	            getSchemeMatchPlan();

    virtual ISchemeLegendCup *	            getSchemeLegendCup();

	// 取得师徒数据中心
	virtual ISchemeMentorshipDataCenter *	getSchemeMentorshipDataCenter();

	virtual ISchemeMailDataCenter *		    getSchemeMailDataCenter();

    // 取得撮合限定配置
    virtual ISchemeMatchRankLimitInfo*      getSchemeMatchRankLimit();

    // 取得匹配Rank配置
    virtual ISchemeMatchRankConfigInfo*     getSchemeMatchRankConfig();

    /** 取得实体通用配置
    @return  
    */
    virtual ISchemeEntity*                  getSchemeEntity();

	// 获取排位赛季时间配置
	virtual ISchemeMatchSeasonTime*			getSchemeMatchSeasonTime();
};
