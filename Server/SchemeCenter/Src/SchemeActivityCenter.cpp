#include "stdafx.h"
#include "SchemeActivityCenter.h"
#include "IServerGlobal.h"

/// purpose: 构造函数
CSchemeActivityCenter::CSchemeActivityCenter()
{

}

/// purpose: 虚构函数
CSchemeActivityCenter::~CSchemeActivityCenter()
{

}

/// purpose: 载入
bool CSchemeActivityCenter::LoadScheme()
{
	ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	string strPath = ACTIVITY_SCHEME_FILENAME;
	bool bResult = pSchemeEngine->LoadScheme(strPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("加载配置文件失败。文件名 = " << strPath.c_str() << endl);
		return false;
	}

	return true;
}

/// purpose: 销毁
void CSchemeActivityCenter::Close()
{
	m_ActivityList.clear();
}

///////////////////////////////ISchemeUpdateSink///////////////////	
bool CSchemeActivityCenter::OnSchemeLoad(SCRIPT_READER reader, const char* szFileName)
{
    ICSVReader *pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;

	if (rkt::StringHelper::casecmp(szFileName, ACTIVITY_SCHEME_FILENAME) == 0)
    {
        bRet = LoadActivityScheme(pCSVReader,szFileName);
    }

    if(!bRet)
    {
        return false;
    }	

    AfterSchemeLoaded();
	
	return true;
}

bool CSchemeActivityCenter::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    CSchemeActivityCenter* pNewInstance = NewInstance();
    if(pNewInstance == NULL)
    {
        return false;
    }

	if (rkt::StringHelper::casecmp(szFileName, ACTIVITY_SCHEME_FILENAME) == 0)
	{
		return false;
	}

    if(pNewInstance->OnSchemeLoad(reader, szFileName))
    {
        return true;
    }
    else
    {
        safeDelete(pNewInstance);
        return false;
    }
    
    return false;
}

class Finder
{
public:
	Finder(EMActivityType activityID, int subActivityID)
	{
		this->activityID = activityID;
		this->subActivityID = subActivityID;
	}

	bool operator()(const SActivityConfig & activity)
	{
		if (activity.activityID == this->activityID && activity.subActivityID == this->subActivityID)
			return true;

		return false;
	}

private:
	EMActivityType activityID;
	int subActivityID;
};

/// purpose: 获取活跃值
int CSchemeActivityCenter::getActivity(EMActivityType activityID, int subActivityID)
{
	vector<SActivityConfig>::iterator it = find_if(m_ActivityList.begin(), m_ActivityList.end(), Finder(activityID, subActivityID));
	if (it != m_ActivityList.end())
	{
		return it->value;
	}
	else
	{
		return 0;
	}
}

// 加载帮会升级脚本
bool CSchemeActivityCenter::LoadActivityScheme(ICSVReader * pCSVReader,const char* szFileName)
{
	int nRecordCount = (int)pCSVReader->GetRecordCount();
	SActivityConfig activityConfig;
	m_ActivityList.clear();
	for (int i = 0; i < nRecordCount; i ++)
	{
		// 活跃类型ID
		activityConfig.activityID = pCSVReader->GetInt(i, 1, 0);
		// 子活跃类型ID
		activityConfig.subActivityID = pCSVReader->GetInt(i, 2, 0);
		// 活跃值
		activityConfig.value = pCSVReader->GetInt(i, 3, 0);
		activityConfig.value = activityConfig.value < 0 ? 0 : activityConfig.value;
		m_ActivityList.push_back(activityConfig);
	}

	return true;
}
