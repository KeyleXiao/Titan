/*******************************************************************
** 文件名:	SchemeStaminaBuff.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	6/18/2015
** 版  本:	1.0
** 描  述:	体力buff配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeStaminaBuff.h"
#include "IServerGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeStaminaBuff::CSchemeStaminaBuff(void)
{	
	// 表初始化
	m_schemeMap.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeStaminaBuff::~CSchemeStaminaBuff(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeStaminaBuff::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( STAMINA_BUFF_SCHEME_FILENAME );
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
void CSchemeStaminaBuff::Close(void)
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
bool CSchemeStaminaBuff::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 表清除
	m_schemeMap.clear();

	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		// 当前列数
		int nCol = 0;

		SStaminaBuff scheme;
		// 职业
		scheme.nVocation = pCSVReader->GetInt(nRow, nCol++, 0);
		// buffID
		scheme.nBuffID = pCSVReader->GetInt(nRow, nCol++, 0);
		// buff等级
		scheme.nBuffLevel = pCSVReader->GetInt(nRow, nCol++, 0);

		// 基础属性表插入
		pair<SCHEME_MAP::iterator, bool> result = m_schemeMap.insert(SCHEME_MAP::value_type(scheme.nVocation, scheme));	
		if (!result.second)
		{
			ErrorLn("SchemeStaminaBuff There is repeated,nVocation="<<scheme.nVocation);
			return false;
		}
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
bool CSchemeStaminaBuff::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeStaminaBuff* pNewInstance = NewInstance();
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


/////////////////////////ISchemeStaminaBuff/////////////////////////
/** 取得体力BUFF配置信息
@param nVocation:	职业ID
@return 体力BUFF配置信息
*/
SStaminaBuff *CSchemeStaminaBuff::getStaminaBuff(int nVocation)
{
	SCHEME_MAP::iterator Iter = m_schemeMap.find(nVocation);
	if (Iter == m_schemeMap.end())
	{
		return NULL;
	}

	return &Iter->second;
}