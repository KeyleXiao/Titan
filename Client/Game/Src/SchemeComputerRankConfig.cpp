#include "StdAfx.h"
#include "SchemeComputerRankConfig.h"
#include "IClientGlobal.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeComputerRankConfig::CSchemeComputerRankConfig(void)
{

}

/** 
@param   
@param   
@return  
*/
CSchemeComputerRankConfig::~CSchemeComputerRankConfig(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeComputerRankConfig::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( COMPUTERRANKCONFIG_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("config file load failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeComputerRankConfig::Close(void)
{	
    // 清空
    m_mapComputerRankConfigManager.clear();
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
bool CSchemeComputerRankConfig::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type != READER_CSV)
	{
		return false;
	}

    m_mapComputerRankConfigManager.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow = 0; nRow < nRecordCount; ++nRow)
	{
		int nIndex = 0;
		SComputerRankConfigSchemeInfo item;

        //撮合类型ID
        item.ID             = pCSVReader->GetInt(nRow, nIndex++, 0);
        item.bySex          = (BYTE)pCSVReader->GetInt(nRow, nIndex++, 0);
        item.nMatchRankGrade= pCSVReader->GetInt(nRow, nIndex++, 0);
        item.fWinRate       = pCSVReader->GetFloat(nRow, nIndex++, 0.0f);
        item.nMatchNum      = pCSVReader->GetInt(nRow, nIndex++, 0);

        // 插入到列表中
        TMAP_COMPUTERRANKCONFIGSCHEME::iterator iter = m_mapComputerRankConfigManager.find(item.ID);
        if (iter == m_mapComputerRankConfigManager.end())
        {
            // 插入
            m_mapComputerRankConfigManager[item.ID] = item;
        }
        else
        {
            // 更新
            iter->second = item;
        }
	}
	
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
bool CSchemeComputerRankConfig::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}

SComputerRankConfigSchemeInfo* CSchemeComputerRankConfig::getComputerRankConfigShemeInfo()
{
    if(m_mapComputerRankConfigManager.size() == 0)
    {
        return NULL;
    }

    DWORD index = (mt_range_rand(1,time(NULL)))%m_mapComputerRankConfigManager.size();
    TMAP_COMPUTERRANKCONFIGSCHEME::iterator iter = m_mapComputerRankConfigManager.begin();
    for (;iter != m_mapComputerRankConfigManager.end(); ++iter)
    {
        --index;
        if( index <= 0)
        {
            break;
        }
    }

    return &(iter->second);
}
