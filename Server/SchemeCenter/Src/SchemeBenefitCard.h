/*******************************************************************
** 文件名:	ISchemeCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	11/17/2016
** 版  本:	1.0
** 描  述:	增益卡脚本配置
********************************************************************/


#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "SchemeDef.h"

// 宝箱配置表
class CSchemeBenefitCard : public ISchemeBenefitCard, public ISchemeUpdateSink, public CSchemeCache<CSchemeBenefitCard>
{
public:
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemeBenefitCard
	// 获得增益卡配置信息
	virtual SBenefitCardConfig_S* getBenefitCardConfig(int nChestID);

public:
	bool LoadScheme();
	void Close();

private:
	// 加载增益卡脚本
	bool LoadBenefitCardScheme(ICSVReader *pCSVReader,const char *szFileName);

private:
	map<int, SBenefitCardConfig_S> m_mapBenefitCardConfig;
};