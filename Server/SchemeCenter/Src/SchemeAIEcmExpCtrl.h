/*******************************************************************
** 文件名:	CSchemeAIEcmExpCtrl.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/10/28
** 版  本:	1.0
** 描  述:	
            经济控制脚本
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include <map>

using namespace rkt;

// 系统提示配置
class CSchemeAIEcmExpCtrl: public ISchemeAIEcmExpCtrl, public ISchemeUpdateSink, public CSchemeCache<CSchemeAIEcmExpCtrl>
{
	// AI经济控制<configWarID,<lineID,配置列表>>
	typedef std::map< int,map<int, list<SAIEcmCtrlConfig_S>> > ECM_CTRL_MAP;
	typedef ECM_CTRL_MAP::iterator ECM_CTRL_MAP_ITER;

	// AI经验控制<configWarID,<lineID,配置列表>>
	typedef std::map< int,map<int, list<SAIExpCtrlConfig_S>> > EXP_CTRL_MAP;
	typedef EXP_CTRL_MAP::iterator EXP_CTRL_MAP_ITER;

    // 配置文件对应的列
    enum SCHEME_COLUMN
    {
        COLNUM_CONFIG_WAR_ID = 0,// 战场ID（用于配置文件）
		COLNUM_MATCHSCORE_RANGE,
        COLNUM_ID,				// 经济差类型ID
		COLNUM_A_RATIO,			// a系数
        COLNUM_B_RATIO,			// b系数
        COLNUM_SOLDIER,			// 士兵加成
        COLNUM_MONSTER,			// 野怪加成
        COLNUM_SALARY,			// 工资加成
        COLNUM_HERO,			// 英雄加成
    };

public:
	CSchemeAIEcmExpCtrl(void);
	virtual ~CSchemeAIEcmExpCtrl(void);

    /////////////////////////////////CSchemeChatCD/////////////////////////////////////////
	// 载入脚本
	bool LoadScheme(void);

	// 关闭
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// 配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

private:
	// AI经济控制
	bool LoadAIEcmCtrlScheme(SCRIPT_READER reader);
	// AI经验控制
	bool LoadAIExpCtrlScheme(SCRIPT_READER reader);

	//////////////////////////////////////////ISchemeAIEcmExpCtrl///////////////////////
	// 获取AI经济控制配置
	virtual SAIEcmCtrlConfig_S* getAIEcmCtrl(int nConfigWarID, int nMatchScore, int nLineID);

	// 获取AI经验控制配置
	virtual SAIExpCtrlConfig_S* getAIExpCtrl(int nConfigWarID, int nMatchScore, int nLineID);
public:

private:
	ECM_CTRL_MAP     m_mapEcmCtrlScheme;
	EXP_CTRL_MAP     m_mapExpCtrlScheme;
};