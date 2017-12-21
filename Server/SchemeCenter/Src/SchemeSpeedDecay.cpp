/*******************************************************************
** 文件名:	SchemeSpeedDecay.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/03/2016
** 版  本:	1.0
** 描  述:	速度衰减
********************************************************************/

#include "StdAfx.h"
#include "SchemeSpeedDecay.h"
#include "IServerGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeSpeedDecay::CSchemeSpeedDecay(void)
{
	m_schemeVector.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeSpeedDecay::~CSchemeSpeedDecay(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeSpeedDecay::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( SPEED_DECAY_SCHEME_FILENAME );
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
void CSchemeSpeedDecay::Close(void)
{
	m_schemeVector.clear();
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
bool CSchemeSpeedDecay::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

    m_schemeVector.clear();

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        // 当前列数
        int nCol = 0;

        SSpeedDecay scheme;
        // 速度下限
        scheme.nSpeedLower = pCSVReader->GetInt(nRow, nCol++, 0);

        // 速度上限
        scheme.nSpeedUpper = pCSVReader->GetInt(nRow, nCol++, 0);

        // 速度衰减系数
        scheme.fDecayFactor = pCSVReader->GetFloat(nRow, nCol++, 0);

        // 速度值
        scheme.nValue = pCSVReader->GetInt(nRow, nCol++, 0);

        m_schemeVector.push_back(scheme);
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
bool CSchemeSpeedDecay::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeSpeedDecay* pNewInstance = NewInstance();
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


/////////////////////////ISchemeSpeedDecay/////////////////////////
/** 取得速度衰减信息
@param nType:	类型
@return 速度衰减信息
*/
SSpeedDecay *CSchemeSpeedDecay::getSpeedDecay(int nSpeed)
{
    int nCount = m_schemeVector.size();
    for (int i=0; i<nCount; ++i)
    {
        if (nSpeed > m_schemeVector[i].nSpeedLower && nSpeed <= m_schemeVector[i].nSpeedUpper)
        {
            return &m_schemeVector[i];
        }
    }

    return NULL;
}