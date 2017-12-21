/*******************************************************************
** 文件名:	SchemeTask.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/13/2017
** 版  本:	1.0
** 描  述:	任务配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeTask.h"
#include "IServerGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeTask::CSchemeTask(void)
{	
	// 表初始化
	m_schemeMap.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeTask::~CSchemeTask(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeTask::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( TASK_SCHEME_FILENAME );
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
void CSchemeTask::Close(void)
{
	// 表清除
	m_schemeMap.clear();
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
bool CSchemeTask::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

    // 表清除
    m_schemeMap.clear();

    // 奖励ID列
    const DWORD PRIZE_ID_COLUMN = 31;
    ISchemeActorPrize* pSchemeActorPrize = gServerGlobal->getSchemeCenter()->getSchemeActorPrize();

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    char szBuff[1024] = {0};
    int nLen = 1024;

    char szPrizeBuf[256] = { 0 };
    int nArrayPrizeID[256] = { 0 };

    STask data;
    for (int i=0; i<nRecordCount; ++i)
    {
        TASK_VECTOR vTask;

        ///////////////////////////////////////////////////////
        ZeroMemory(szPrizeBuf, sizeof(szPrizeBuf) );
        ZeroMemory(nArrayPrizeID, sizeof(nArrayPrizeID) );

        int nPrizeSize = sizeof(szPrizeBuf);
        pCSVReader->GetString(i, PRIZE_ID_COLUMN, szPrizeBuf, nPrizeSize);
        int nArrayCount = parseIntArray(szPrizeBuf, nArrayPrizeID, sizeof(nArrayPrizeID)/sizeof(int), ';');
        for (int n = 0; n < nArrayCount; ++n)
        {
            int & nPrizeID = nArrayPrizeID[n];      
            if (nPrizeID != 0 && !pSchemeActorPrize->isExists(nPrizeID))
            {
                ErrorLn(__FUNCTION__" prize id is invalid ! prizeID=" << nPrizeID << ",check file=" << szFileName << ",row=" << (i + 1));
                return false;
            }
        }

        ///////////////////////////////////////////////////////
        for (DWORD j=0; j<pCSVReader->GetFieldCount(); ++j)
        {
            data.nType = pCSVReader->GetFieldType(j);
            
            nLen = sizeof(szBuff);
            pCSVReader->GetString(i, j, szBuff, nLen);
            data.strValue = szBuff;

            vTask.push_back(data);
        }

        m_schemeMap.push_back(vTask);
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
bool CSchemeTask::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeTask* pNewInstance = NewInstance();
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


/////////////////////////ISchemeTask/////////////////////////
/** 取得任务配置信息
@return 任务配置信息
*/
SCHEME_TASK *CSchemeTask::getTaskScheme()
{
	return &m_schemeMap;
}