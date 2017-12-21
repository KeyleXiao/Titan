#include "StdAfx.h"
#include "SchemeMailDataCenter.h"
#include "ISocialGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeMailDataCenter::CSchemeMailDataCenter(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeMailDataCenter::~CSchemeMailDataCenter(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMailDataCenter::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gSocialGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( MAIL_CONFIG_FILENAME );
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
void CSchemeMailDataCenter::Close(void)
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
bool CSchemeMailDataCenter::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strConfigPath = SCP_PATH_FORMAT( MAIL_CONFIG_FILENAME );

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
bool CSchemeMailDataCenter::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMailDataCenter* pNewInstance = NewInstance();
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
bool CSchemeMailDataCenter::LoadConfigScheme(ICSVReader * pCSVReader)
{
	if (pCSVReader == NULL)
	{
		return false;
	}

    string strLoadFileName = MAIL_CONFIG_FILENAME;

	m_mapMailScheme.clear();

	int nLen = 0;
	char szTitle[MAIL_TITLE_MAXSIZE] = {0};
	char szContext[MAIL_CONTENT_MAXSIZE] = {0};
	char  szSenderName[ACTORNAME_MAXSIZE] = {0};
	char szArrayPrize[256] = {0};

	std::vector<std::string> vecDataList;

	int nRecordCount = pCSVReader->GetRecordCount();
	for ( int nRow = 0; nRow < nRecordCount; ++nRow )
	{
		SMailConfig node;

		node.nMailFillID = pCSVReader->GetInt(nRow, COLUNM_MAIL_ID, 0);
		node.nMailLifeTime = pCSVReader->GetInt(nRow, COLUNM_MAIL_LIFETIME, 0);

		nLen = sizeof(szTitle);
		pCSVReader->GetString(nRow, COLUNM_MAIL_TITLE, szTitle, nLen);
		sstrcpyn(node.szTitle, szTitle, sizeof(node.szTitle));

		nLen = sizeof(szContext);
		pCSVReader->GetString(nRow, COLUNM_MAIL_CONTENT, szContext, nLen);
		sstrcpyn(node.szContext, szContext, sizeof(node.szContext));

		nLen = sizeof(szSenderName);
		pCSVReader->GetString(nRow, COLUNM_MAIL_SENDERNAME, szSenderName, nLen);
		sstrcpyn(node.szSenderName, szSenderName, sizeof(node.szSenderName));

		nLen = sizeof(szArrayPrize);
		pCSVReader->GetString(nRow, COLUNM_MAIL_ARRAYPRIZE, szArrayPrize, nLen);
		vecDataList.clear();

		StringHelper::split(vecDataList, szArrayPrize, ';');
		int nIndex = 0;
		for(std::vector<std::string>::iterator it = vecDataList.begin(); it != vecDataList.end() && nIndex < MAIL_GOODS_MAX_NUM; ++it)
		{
			int nPrizeID = StringHelper::toInt(*it);
			if(nPrizeID > 0)
			{
				node.arrayPrizeID[nIndex++] = nPrizeID;
			}
		}
		node.nPrizeIDCount = nIndex;

		m_mapMailScheme[node.nMailFillID] = node;
	}

    return true;
}

SMailConfig* CSchemeMailDataCenter::getConfig(int nMailFillID)
{
    std::map<int, SMailConfig>::iterator it = m_mapMailScheme.find(nMailFillID);
	if (it == m_mapMailScheme.end())
	{
		WarningLn(__FUNCTION__": not found item, check CSchemeMailDataCenter file config! id=" << nMailFillID);
		return NULL;
	}

	return &(it->second);
}

