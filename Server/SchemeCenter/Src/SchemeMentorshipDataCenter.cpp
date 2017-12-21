#include "StdAfx.h"
#include "SchemeMentorshipDataCenter.h"
#include "ISchemeEngine.h"
#include "IServerGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeMentorshipDataCenter::CSchemeMentorshipDataCenter(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeMentorshipDataCenter::~CSchemeMentorshipDataCenter(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMentorshipDataCenter::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( MENTORSHIP_CONFIG_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
        return false;
    }

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeMentorshipDataCenter::Close(void)
{
}

/** CSV配置载入时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/ 
bool CSchemeMentorshipDataCenter::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strConfigPath = SCP_PATH_FORMAT( MENTORSHIP_CONFIG_FILENAME );

    if(rkt::StringHelper::casecmp(szFileName, strConfigPath.c_str()) == 0)
    {
        bRet = LoadConfigScheme(reader.pCSVReader);
    }

    if(!bRet)
    {
        return false;
    }	

    AfterSchemeLoaded();

	return true;
}

/** 配置动态更新时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeMentorshipDataCenter::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMentorshipDataCenter* pNewInstance = NewInstance();
	if(pNewInstance == NULL)
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
}

/// purpose:  加载战队常量脚本
bool CSchemeMentorshipDataCenter::LoadConfigScheme(ICSVReader * pCSVReader)
{
	if (pCSVReader == NULL)
	{
		return false;
	}

    string strLoadFileName = MENTORSHIP_CONFIG_FILENAME;

    memset(&m_Config, 0, sizeof(m_Config));

	char szTemp[512] = {0};
	int nLine = 0;
	// 导师需要的等级
	m_Config.nMasterNeedLevel = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nMasterNeedLevel <= 0 || m_Config.nMasterNeedLevel > PERSON_MAX_LEVEL)
	{
		return false;
	}
	// 学徒需要等级
	m_Config.nPrenticeNeedLevel = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nPrenticeNeedLevel <= 0 || m_Config.nPrenticeNeedLevel > PERSON_MAX_LEVEL)
	{
		return false;
	}
	// 正式师傅个数
	m_Config.nFormalMasterCount = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nFormalMasterCount < 0)
	{
		return false;
	}
	// 见习师傅个数
	m_Config.nInternMasterCount = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nInternMasterCount < 0)
	{
		return false;
	}
	// 正式学徒个数
	m_Config.nFormalPrenticeCount = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nFormalPrenticeCount < 0)
	{
		return false;
	}
	// 见习学徒个数
	m_Config.nInternPrenticeCount = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nInternPrenticeCount < 0)
	{
		return false;
	}
	// 学徒首胜奖励次数
	m_Config.nPFWinAwardCount = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nPFWinAwardCount < 0)
	{
		return false;
	}
	// 学徒首胜奖励拜师时间限制
	m_Config.nPFWinAwardTimeLen = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nPFWinAwardTimeLen < 0)
	{
		return false;
	}
	// 导师首胜奖励次数
	m_Config.nMFWinAwardCount = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nMFWinAwardCount < 0)
	{
		return false;
	}
	// 导师首胜奖励拜师时间限制
	m_Config.nMFWinAwardTimeLen = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nMFWinAwardTimeLen < 0)
	{
		return false;
	}
	// 解散需要时长
	m_Config.nServerTimeLen = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nServerTimeLen < 0)
	{
		return false;
	}
	// 胜利获取师徒值
	m_Config.nWinShipVal = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nWinShipVal < 0)
	{
		return false;
	}
	// 失败获取师徒值
	m_Config.nFailShipVal = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nFailShipVal < 0)
	{
		return false;
	}
	// 周获取师徒最大值
	m_Config.nWeekShipVal = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nWeekShipVal < 0)
	{
		return false;
	}
	// 经验加成百分比
	m_Config.nExpPercent = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nExpPercent < 0)
	{
		return false;
	}
	// 经验加成百分比
	m_Config.nHeroExpPercent = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nHeroExpPercent < 0)
	{
		return false;
	}
	// 金币加成百分比
	m_Config.nGoldPercent = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nGoldPercent < 0)
	{
		return false;
	}
	// 导师首胜奖励ID
	m_Config.nMFWinPrizeID = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nMFWinPrizeID < 0)
	{
		return false;
	}
	// 学徒首胜奖励ID
	m_Config.nPFWinPrizeID = pCSVReader->GetInt(nLine++, 1, 0);
	if (m_Config.nPFWinPrizeID < 0)
	{
		return false;
	}

    return true;
}

SMentorshipConfig* CSchemeMentorshipDataCenter::getConfig()
{
    return &m_Config;
}

