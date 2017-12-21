/*******************************************************************
** 文件名:	SchemeSpellUpgrade.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/30/2015
** 版  本:	1.0
** 描  述:	技能升级配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeSpellUpgrade.h"
#include "IClientGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeSpellUpgrade::CSchemeSpellUpgrade(void)
{	
	// 表初始化
	m_schemeMap.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeSpellUpgrade::~CSchemeSpellUpgrade(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeSpellUpgrade::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( SPELL_UPGRADE_SCHEME_FILENAME );
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
void CSchemeSpellUpgrade::Close(void)
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
bool CSchemeSpellUpgrade::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
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

		SSpellUpgrade scheme;
		// 职业ID
		scheme.nVocation = pCSVReader->GetInt(nRow, nCol++, 0);
		// 槽索引
		scheme.nSlotIndex = pCSVReader->GetInt(nRow, nCol++, 0);
		// 槽等级
		scheme.nSlotLevel = pCSVReader->GetInt(nRow, nCol++, 0);
		// 技能ID
		scheme.nSpellID = pCSVReader->GetInt(nRow, nCol++, 0);
		// 英雄等级
		scheme.nLevel = pCSVReader->GetInt(nRow, nCol++, 0);

		SCHEME_MAP::iterator IterMap = m_schemeMap.find(scheme.nVocation);
		if (IterMap == m_schemeMap.end())
		{
			SCHEME_VECTOR schemeVector;
			schemeVector.push_back(scheme);

			m_schemeMap.insert(SCHEME_MAP::value_type(scheme.nVocation, schemeVector));
		}
		else
		{
			SCHEME_VECTOR &schemeVector = IterMap->second;

			// 是否插入
			bool bInsert = true;
			for (SCHEME_VECTOR::iterator IterVector=schemeVector.begin(); IterVector!=schemeVector.end(); ++IterVector)
			{
				if (scheme.nVocation == IterVector->nVocation && scheme.nSlotIndex == IterVector->nSlotIndex && scheme.nSlotLevel == IterVector->nSlotLevel)
				{
					bInsert = false;

					ErrorLn("SchemeSpellUpgrade There is repeated,szFileName="<<szFileName<<",nRow="<<nRow);
					break;
				}
			}

			if (bInsert)
			{
				schemeVector.push_back(scheme);
			}
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
bool CSchemeSpellUpgrade::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}


/////////////////////////ISchemeSpellUpgrade/////////////////////////
/** 取得技能升级配置信息
@param nVocation:	职业
@param vSpellUpgrade:	技能升级容器数据
@return 技能升级配置信息
*/
void CSchemeSpellUpgrade::getSpellUpgrade(int nVocation, vector<SSpellUpgrade> &vSpellUpgrade)
{
	// 清空表
	vSpellUpgrade.clear();

	SCHEME_MAP::iterator IterMap = m_schemeMap.find(nVocation);
	if (IterMap == m_schemeMap.end())
	{
		return;
	}

	SCHEME_VECTOR &schemeVector = IterMap->second;
	// 设置技能升级容器数据
	vSpellUpgrade = schemeVector;
}