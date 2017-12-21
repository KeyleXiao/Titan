/*******************************************************************
** 文件名:	SchemeGuideStep.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/4/2017
** 版  本:	1.0
** 描  述:	新手引导步骤配置 GuideStep.csv

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeGuideStep.h"
#include "IServerGlobal.h"
#include "SpellDef.h"
#include <string>
using namespace std;


#define GUIDESTEP_SCHEME_FILENAME			    "Scp\\GuideStep.csv"

/** 
@param   
@param   
@return  
*/
CSchemeGuideStep::CSchemeGuideStep(void)
{	

}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeGuideStep::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载csv脚本
    string stringPath = SCP_PATH_FORMAT( GUIDESTEP_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeGuideStep::Close(void)
{	
	// 清空
	m_map.clear();
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
bool CSchemeGuideStep::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;

	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_map.clear();

    int nPID = 0;
    int nStepID = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		// 场外引导步骤ID
		nPID = pCSVReader->GetInt(nRow, COL_PID, 0);

        // 查询ID
        nStepID = pCSVReader->GetInt(nRow, COL_STEP, 0);

        if(m_map.find(nStepID) != m_map.end())
        {
            ErrorLn(__FUNCTION__": step exists, please check config["<< szFileName <<"], row="<< (nRow+1) <<", step="<< nStepID );
            return false;
        }

        m_map[nStepID] = nPID;
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
bool CSchemeGuideStep::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeGuideStep* pNewInstance = NewInstance();
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


//////////////////////////////////////////////////////////////////////////
// 取得指定ID对应的步骤数字
int CSchemeGuideStep:: getStepByID( int nPID )
{
    for (TMAP_PID2STEPID::iterator iter = m_map.begin(); iter != m_map.end(); ++iter)
    {
        if(iter->second == nPID)
        {
            return iter->first;
        }
    }
    return 0;
}
