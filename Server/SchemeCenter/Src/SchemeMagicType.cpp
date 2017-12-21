/*******************************************************************
** 文件名:	SchemeMagicType.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/08/2015
** 版  本:	1.0
** 描  述:	魔法类型配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeMagicType.h"
#include "IServerGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeMagicType::CSchemeMagicType(void)
{	
	// 表初始化
	m_schemeMap.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeMagicType::~CSchemeMagicType(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMagicType::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( MAGIC_TYPE_SCHEME_FILENAME );
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
void CSchemeMagicType::Close(void)
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
bool CSchemeMagicType::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
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

		SMagicType scheme;
		// 能量值增加衰减方式
		scheme.nType = pCSVReader->GetInt(nRow, nCol++, 0);
		// 名字
		int nLen = sizeof(scheme.szName);
		pCSVReader->GetString(nRow, nCol++, scheme.szName, nLen);
		// 进度条类型
		scheme.nProgressBarType = pCSVReader->GetInt(nRow, nCol++, 0);
		// 上限值
		scheme.nLimitVale = pCSVReader->GetInt(nRow, nCol++, 0);
		// 初始值
		scheme.nInitialValue = pCSVReader->GetInt(nRow, nCol++, 0);
		// 击中增加值
		scheme.nHitValue = pCSVReader->GetInt(nRow, nCol++, 0);
		// 击杀怪物增加值
		scheme.nKillMonsterValue = pCSVReader->GetInt(nRow, nCol++, 0);
		// 击杀英雄增加值
		scheme.nKillRoleValue = pCSVReader->GetInt(nRow, nCol++, 0);
		// 助攻增加值
		scheme.nAssistValue = pCSVReader->GetInt(nRow, nCol++, 0);
		// 自动增长时间
		scheme.nAutoGrowTime = pCSVReader->GetInt(nRow, nCol++, 0);
		// 自动增加值
		scheme.nAutoGrowValue = pCSVReader->GetInt(nRow, nCol++, 0);
		// 自动衰减时间
		scheme.nAutoDecayTime = pCSVReader->GetInt(nRow, nCol++, 0);
		// 自动衰减值
		scheme.nAutoDecayValue = pCSVReader->GetInt(nRow, nCol++, 0);
		// 二次自动衰减时间
		scheme.nSecondaryAutoDecayTime = pCSVReader->GetInt(nRow, nCol++, 0);
		// 二次自动衰减值
		scheme.nSecondaryAutoDecayValue = pCSVReader->GetInt(nRow, nCol++, 0);

		// 基础属性表插入
		pair<SCHEME_MAP::iterator, bool> result = m_schemeMap.insert(SCHEME_MAP::value_type(scheme.nType, scheme));	
		if (!result.second)
		{
			ErrorLn("SchemeMagicType There is repeated,nType="<<scheme.nType);
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
bool CSchemeMagicType::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMagicType* pNewInstance = NewInstance();
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


/////////////////////////ISchemeMagicType/////////////////////////
/** 取得魔法类型配置信息
@param nType:	类型
@return 魔法类型配置信息
*/
SMagicType *CSchemeMagicType::getMagicType(int nType)
{
	SCHEME_MAP::iterator Iter = m_schemeMap.find(nType);
	if (Iter == m_schemeMap.end())
	{
		return NULL;
	}

	return &Iter->second;
}