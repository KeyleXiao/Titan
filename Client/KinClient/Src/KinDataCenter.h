//==========================================================================
/**
* @file	  : KinDataCenter.h
* @author : xiaobao
* created : 2009-3-7
* purpose : 家族数据中心
* note    : 负责读取脚本和提供接口，目前的脚本有：升级脚本，常量配置脚本,家族技能脚本，成员族内等级脚本
*/
//==========================================================================

#ifndef __KinDataCenter_H__
#define __KinDataCenter_H__

#include "KinDef.h"

// 家族常量脚本
#define KIN_CONFIG_FILENAME			"Scp\\KinConfig.csv"

// 家族升级脚本
#define KIN_LEVEL_FILENAME			"Scp\\KinLevel.csv"

class CKinDataCenter :	public ISchemeUpdateSink
{
public:
	/////////////////////////////// 初始化 ////////////////////////////////////
	/// purpose: 构造函数
	CKinDataCenter();

	/// purpose: 虚构函数
	virtual ~CKinDataCenter();

	/// purpose: 创建
	bool Create();

	/// purpose: 销毁
	void Close();

    ///////////////////////////////ISchemeUpdateSink///////////////////	
    virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	/////////////////////////////// CKinDataCenter ////////////////////////////////////

	/// purpose: 取得家族常量信息
	SKinConfig* GetConfig();

    SKinLevel* GetLevelInfo(int nKinLevel);

private:

	/// purpose:  加载家族常量脚本
	bool LoadConfigScheme(ICSVReader * pCSVReader);

    /// purpose:  加载家族升级脚本
    bool LoadLevelScheme(ICSVReader * pCSVReader);

private:

	// 家族常量
	SKinConfig m_Config;

    // 家族升级
    SKinLevel m_LevelInfoList[KIN_MAX_LEVEL + 1];
};

// 全局家族数据中心对象声明
extern CKinDataCenter g_KinDataCenter;

#endif // __KinDataCenter_H__