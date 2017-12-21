/*******************************************************************
** 文件名:	SchemeMatchComputerName.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	电脑名字相关配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeMatchComputerName.h"
#include "ISocialGlobal.h"
#include "TeamDef.h"

/** 
@param   
@param   
@return  
*/
CSchemeMatchComputerName::CSchemeMatchComputerName(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeMatchComputerName::~CSchemeMatchComputerName(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMatchComputerName::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gSocialGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( MATCHCOMPUTERNAME_SCHEME_FILENAME );
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
void CSchemeMatchComputerName::Close(void)
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
bool CSchemeMatchComputerName::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    bool bRet = false;
    string strConfigPath = SCP_PATH_FORMAT( MATCHCOMPUTERNAME_SCHEME_FILENAME );

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
bool CSchemeMatchComputerName::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMatchComputerName* pNewInstance = NewInstance();
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


bool CSchemeMatchComputerName::LoadConfigScheme(ICSVReader * pCSVReader)
{
    // 要将以前的数据清掉
    m_mapNameSchemeInfo.clear();

    // 加载每一条记录
    int nNameLen = 0;
    int nCount = pCSVReader->GetRecordCount();
    for(int i = 0; i < nCount; i++)
    {	
        SMatchComputerName DataInfo;
        DataInfo.nID			= pCSVReader->GetInt(i, 0, 0);
        // 找到记录
        map<int,SMatchComputerName>::iterator iter = m_mapNameSchemeInfo.find(DataInfo.nID);
        if(DataInfo.nID == 0 || iter!= m_mapNameSchemeInfo.end())
        {
            // 不要跳过，要策划来改，因为已经给予的称号或者即将要给的，都有问题
            ErrorLn(_GT("加载[命令]配置文件失败,配置文件有重复ID,或无效ID,行=")<<i<<",ID="<< DataInfo.nID << endl);
            return false;
        }

        nNameLen =sizeof(DataInfo.szName);
        pCSVReader->GetString(i, 1, DataInfo.szName,nNameLen );	// 名称

        // 加入到Map中
        m_mapNameSchemeInfo.insert(pair<int,SMatchComputerName>(DataInfo.nID,DataInfo));
    }
    return true;
}

std::string CSchemeMatchComputerName::GetComputerNameByIndex(int nIndex)
{
    map<int,SMatchComputerName>::iterator iter =m_mapNameSchemeInfo.find(nIndex);
    if(iter != m_mapNameSchemeInfo.end())
    {
        return iter->second.szName;
    }
    return "";
}

int CSchemeMatchComputerName::GetConfigNameCount()
{
    return m_mapNameSchemeInfo.size();
}

