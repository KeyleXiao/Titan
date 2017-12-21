/*******************************************************************
** 文件名:	SchemeLegendCup.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	战队相关配置
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "LegendCupSchemeDef.h"
#include <map>
using namespace stdext;

// 定义
class CSchemeLegendCup : public ISchemeLegendCup, public ISchemeUpdateSink, public CSchemeCache<CSchemeLegendCup>
{
public:
	/////////////////////////ISchemeLegendCup/////////////////////////

    virtual SSchemeLegendCup* getConfig(int nConfigID);

    virtual SSchemeLegendCupDetail* getDetailInfo(int nDetailID, int nRoundID);

    virtual int getRoundIDByRegisterCount(int nDetailID,  int nKinCount);

    virtual SSchemeLegendCupPrize* getPrizeInfo(int nPrizeID, int nRankID);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeLegendCup//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool											LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void											Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeLegendCup(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeLegendCup(void);

private:

    /// purpose:  加载杯赛脚本
    bool LoadConfigScheme(ICSVReader * pCSVReader);

    /// purpose:  加载杯赛详细配置脚本
    bool LoadDetailScheme(ICSVReader * pCSVReader);

    /// purpose:  加载杯赛结束奖励
    bool LoadPrizeScheme(ICSVReader * pCSVReader);

private:
    map<int, SSchemeLegendCup> m_Config;
    map<int, map<int, SSchemeLegendCupDetail>> m_ConfigDetail;
    map<int, map<int, SSchemeLegendCupPrize>> m_ConfigPrize;
};