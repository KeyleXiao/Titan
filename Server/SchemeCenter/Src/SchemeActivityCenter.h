/*******************************************************************
** 文件名:	SchemeActivityCenter.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	王全申
** 日  期:	5/23/2017
** 版  本:	1.0
** 描  述:	活跃度配置中心
********************************************************************/

#ifndef __SCHEMEACTIVITYCENTER_H__
#define __SCHEMEACTIVITYCENTER_H__

#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"

class CSchemeActivityCenter : public ISchemeActivityCenter, public ISchemeUpdateSink, public CSchemeCache<CSchemeActivityCenter>
{
public:
	/// purpose: 构造函数
	CSchemeActivityCenter();

	/// purpose: 虚构函数
	virtual ~CSchemeActivityCenter();

public:
	/// purpose: 载入
	bool LoadScheme();
	/// purpose: 销毁
	void Close();

public:
    ///////////////////////////////ISchemeUpdateSink///////////////////	
    virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/** 取得不同类型的活跃值
	@param activityID:	活跃值类型ID
	@param subActivityID:	活跃值子类型ID
	@return 活跃值
	*/
	virtual int getActivity(EMActivityType activityID, int subActivityID);

private:
	// 加载活跃度配置脚本
	bool LoadActivityScheme(ICSVReader * pCSVReader, const char* szFileName);
	
private:
	std::vector<SActivityConfig> m_ActivityList;
};

#endif // __SCHEMEACTIVITYCENTER_H__