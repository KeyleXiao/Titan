/*******************************************************************
** 文件名:	ClanDataCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/7/2016
** 版  本:	1.0
** 描  述:	帮会数据中心
********************************************************************/

#ifndef __SCHEMECLANDATACENTER_H__
#define __SCHEMECLANDATACENTER_H__

#include "ClanDef.h"
#include "ISchemeCenter.h"

class CSchemeClanDataCenter : public ISchemeClanDataCenter, public ISchemeUpdateSink
{
public:
	/// purpose: 构造函数
	CSchemeClanDataCenter();

	/// purpose: 虚构函数
	virtual ~CSchemeClanDataCenter();

public:
	/// purpose: 载入
	bool LoadScheme();
	/// purpose: 释放
	void Close();

public:
    /////////////////////////ISchemeUpdateSink/////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/// purpose: 取得帮会每级信息
	SClanLevel* getLevelInfo(int nClanLevel);

	/// purpose: 取得帮会常量信息
	SClanConfig* getConfig();

private:
	// 加载帮会捐献脚本
	bool LoadDonateScheme(ICSVReader * pCSVReader,const char* szFileName);

	// 加载帮会升级脚本
	bool LoadLevelScheme(ICSVReader * pCSVReader,const char* szFileName);

	// 加载帮会升级脚本
	bool LoadConfigScheme(ICSVReader * pCSVReader,const char* szFileName);

private:
	// 帮会升级
	SClanLevel m_LevelInfoList[CLAN_MAX_LEVEL + 1];
	// 帮会常量
	SClanConfig m_Config;
};

#endif // __SCHEMECLANDATACENTER_H__