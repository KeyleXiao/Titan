/*******************************************************************
** 文件名:	SchemeMonster.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈俊涛
** 日  期:	3/13/2015
** 版  本:	1.0
** 描  述:	战场怪物信息配置
********************************************************************/
#pragma once
#include <map>
#include <vector>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeWarDef.h"

using namespace std;

// 刷新类型集合
typedef vector<EWarMonsterExec> EWarMonsTypeVect;

// 战场信息集合
typedef vector<SWarInfoScheme> SWarInfoSchemeVect;
typedef map<EWarMonsterExec, SWarInfoSchemeVect> SWarInfoSchemeMap;

struct SWarConfig
{
	// 配置信息
	SWarScheme sWarScheme;
	SWarInfoSchemeMap sWarInfoSchemeMap;
	EWarMonsTypeVect eWarMonsTypeVect;
};

typedef map<int, SWarConfig> WarConfigMap;
// 定义
class CSchemeWarManager : public ISchemeWarManager, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarManager>
{
public:
	// 通过静态ID和类型，获得当前怪物的的列表
	SWarInfoSchemeVect* GetWarInfoSchemeMap(int nStaticID, EWarMonsterExec eType);
	// 通过静态ID获得当前战场信息
	SWarScheme GetWarScheme(int nStaticID);
	// 获得刷新类型
	EWarMonsTypeVect* GetWarEMonstTypeVect(int nStaticID);

	/////////////////////////ISchemeUpdateSink/////////////////////////
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeWarManager//////////////////////////
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
	CSchemeWarManager(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeWarManager(void);

	/** 添加战场描述信息
	@param nWarStaticID: 战场ID  
	@param eType： 刷新的类型
	@param sWarInfo：怪物的信息
	@return 是否添加成功
	*/
	bool AddWarInfo(int nWarStaticID, EWarMonsterExec eType, SWarInfoScheme sWarInfo);
private:
	WarConfigMap m_WarConfigMap;
};