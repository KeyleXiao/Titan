/*******************************************************************
** 文件名:	SchemePlayerKill.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/03/2016
** 版  本:	1.0
** 描  述:	PK
********************************************************************/

#include "StdAfx.h"
#include "SchemePlayerKill.h"
#include "IServerGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemePlayerKill::CSchemePlayerKill(void)
{
    memset(&m_scheme, 0, sizeof(m_scheme));
}

/** 
@param   
@param   
@return  
*/
CSchemePlayerKill::~CSchemePlayerKill(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemePlayerKill::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( PK_SCHEME_FILENAME );
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
void CSchemePlayerKill::Close(void)
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
bool CSchemePlayerKill::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

    memset(&m_scheme, 0, sizeof(m_scheme));

    // 读取
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        // 当前列数
        int nCol = 0;
        // 发起切磋冷却时间
        m_scheme.nCoolTime = pCSVReader->GetInt(nRow, nCol++, 0);
        // 发起切磋范围要求
        m_scheme.nDistance = pCSVReader->GetInt(nRow, nCol++, 0);
        // 请求切磋有效时间
        m_scheme.nRequestValidTime = pCSVReader->GetInt(nRow, nCol++, 0);
        // 请求切磋有效数量
        m_scheme.nRequestValidCount = pCSVReader->GetInt(nRow, nCol++, 0);
        // 切磋范围
        m_scheme.nFightDistance = pCSVReader->GetInt(nRow, nCol++, 0);
        // 准备时间
        m_scheme.nPrepareTime = pCSVReader->GetInt(nRow, nCol++, 0);
        // 切磋时间
        m_scheme.nFightTime = pCSVReader->GetInt(nRow, nCol++, 0);
        // 怪物ID
        m_scheme.nMonsterID = pCSVReader->GetInt(nRow, nCol++, 0);
        // 开始PK增加BuffID
        m_scheme.nStartPKBuffID = pCSVReader->GetInt(nRow, nCol++, 0);
        // 开始PK增加BuffLevel
        m_scheme.nStartPKBuffLevel = pCSVReader->GetInt(nRow, nCol++, 0);
        // 结束PK增加BuffID
        m_scheme.nEndPKBuffID = pCSVReader->GetInt(nRow, nCol++, 0);
        // 结束PK增加BuffLevel
        m_scheme.nEndPKBuffLevel = pCSVReader->GetInt(nRow, nCol++, 0);
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
bool CSchemePlayerKill::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemePlayerKill* pNewInstance = NewInstance();
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


/////////////////////////ISchemePlayerKill/////////////////////////
/** 取得PK信息
@return PK信息
*/
SPlayerKill *CSchemePlayerKill::getPlayerKill(void)
{
    return &m_scheme;
}